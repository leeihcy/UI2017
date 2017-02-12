#include "stdafx.h"
#include "font.h"
using namespace UI;
using namespace UI::RT;

FontCache::FontCache()
{
    m_lRef = 0;
}

FontCache&  FontCache::Instance()
{
    static FontCache _s;
    return _s;
}

void  FontCache::AddRef()
{
    m_lRef ++;
}
void  FontCache::Release()
{
    m_lRef --;

    if (0 == m_lRef)
    {
        vector<FontInfo>::iterator iter = m_fontArray.begin();
        for (; iter != m_fontArray.end(); ++iter)
        {
            DeleteObject(iter->hFont);
        }
        m_fontArray.clear();
    }
}

int FontSize2Height(int nSize)
{
	long dpi = UI::GetDpi();
    int nHeight = 0;
    nHeight = -MulDiv(nSize, dpi, 72);
    return nHeight;
}

void  FontCache::CacheFont(const CharFormat* pcf)
{
    GetFont(const_cast<CharFormat*>(pcf));
}
bool  FontCache::GetFont(CharFormat* pcf)
{
    if (!pcf)
        return false;

    if (pcf->hFont)
        return true;

    uint nSize = m_fontArray.size();
    for (uint i = 0; i < nSize; i++)
    {
        HFONT  hFont = m_fontArray[i].hFont;
        if (!hFont)
            continue;

        LOGFONT lf = {0};
        ::GetObject(hFont, sizeof(lf), &lf);

        if (0 != _tcscmp(lf.lfFaceName, pcf->szFontName))
            continue;
        if (lf.lfWeight != (pcf->bBold?FW_BOLD:FW_NORMAL))
            continue;
        if (lf.lfItalic != (pcf->bItalic?1:0))
            continue;
        if (lf.lfUnderline != (pcf->bUnderline?1:0))
            continue;
		if (lf.lfStrikeOut != (pcf->bDeleteline?1:0))
			continue;
        if (lf.lfHeight != FontSize2Height(pcf->lFontSize))
            continue;

        pcf->hFont = hFont;
        pcf->nFontHeight = m_fontArray[i].nFontHeight;
        pcf->nFontDescent = m_fontArray[i].nFontDescent;
        return true;
    }

	pcf->textColor &= 0x00ffffff;  // 强制去掉高位，带alpha通道gdi不识别
	pcf->backColor &= 0x00ffffff;  // 强制去掉高位，带alpha通道gdi不识别

    // 创建该字体
    HFONT hFont = CreateFont(*pcf);
    uint nFontDescent = 0;
    uint nFontHeight = GetFontHeight(hFont, &nFontDescent);

    FontInfo info = {hFont, nFontHeight, nFontDescent};
    m_fontArray.push_back(info);

    pcf->hFont = hFont;
    pcf->nFontHeight = nFontHeight;
    pcf->nFontDescent = nFontDescent;
    return true;
}

// HFONT  FontCache::GetFont(uint nIndex)
// {
//     if (nIndex >= m_fontArray.size())
//         return NULL;
// 
//     return m_fontArray[nIndex];
// }
// 

HFONT  FontCache::CreateFont(CharFormat& cf)
{
    HFONT hFont = ::CreateFont(
        FontSize2Height(cf.lFontSize),  // nHeight
        0,                         // nWidth
        0,                         // nEscapement
        0,                         // nOrientation
        cf.bBold?FW_BOLD:FW_NORMAL, // nWeight
        cf.bItalic,                // bItalic
        cf.bUnderline,             // bUnderline
        cf.bDeleteline,            // cStrikeOut
        DEFAULT_CHARSET,           // nCharSet
        OUT_DEFAULT_PRECIS,        // nOutPrecision
        CLIP_DEFAULT_PRECIS,       // nClipPrecision
        CLEARTYPE_QUALITY,         // nQuality
        DEFAULT_PITCH | FF_SWISS,  // nPitchAndFamily
        cf.szFontName);            // lpszFacename

    return hFont;

}

uint  FontCache::GetFontHeight(HFONT hFont, uint* pnDescent)
{
    if (!hFont)
        return 0;

    HDC hDC = CreateCompatibleDC(NULL);
    HFONT hOldFont = (HFONT)SelectObject(hDC, hFont);

    TEXTMETRIC  tm;
    GetTextMetrics(hDC, &tm);

    SelectObject(hDC, hOldFont);
    DeleteDC(hDC);

    if (pnDescent)
        *pnDescent = tm.tmDescent;

    return tm.tmHeight;
}

namespace UI {
    namespace RT{

bool  IsSameFormat(
        const CharFormat* f1, const CharFormat* f2)
{
    if (f1 == f2)
        return true;

    if (!f1 || !f2)
        return false;

    if (0 == memcmp(f1, f2, sizeof(CharFormat)))
        return true;

    return false;
}
    }
}