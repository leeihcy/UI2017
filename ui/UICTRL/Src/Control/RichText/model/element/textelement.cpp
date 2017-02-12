#include "stdafx.h"
#include "textelement.h"
#include "..\layout_context.h"
#include "..\line.h"
#include "..\doc.h"
#include "..\..\uniscribe\uniscribehelper.h"
#include "..\font.h"


using namespace UI;
using namespace UI::RT;

TextElement::TextElement()
{
	m_pcf = NULL;
	// memset(&m_sa, 0, sizeof(m_sa));
	m_nABCWidth = 0;
    m_bNeedAnalyze = true;
}

TextElement::~TextElement()
{
	SAFE_DELETE(m_pcf);
}

void  TextElement::SetText(LPCTSTR  szText, int nLength)
{
	if (!szText || 0 == nLength)
		return;

	if (-1 == nLength)
	{
		m_strText = szText;
	}
	else
	{
		m_strText.append(szText, nLength);
	}

    m_bNeedAnalyze = true;
}

void  TextElement::InsertTextAt(uint pos, LPCTSTR szText, int nLength)
{
    UIASSERT(szText);
    if (!szText)
        return;
    UIASSERT(pos <= m_strText.length());
	if (pos > m_strText.length())
	{
		pos = m_strText.length();
	}

    if (nLength < 0)
        nLength = _tcslen(szText);

    m_strText.insert(pos, szText, nLength);

    analyze();
}

// 在该node中间需要插入一段新的文本，需要先将该结点分拆成两段
void  TextElement::SplitTwoElement(uint pos)
{
    if (pos >= m_strText.length())
        return; 

    TextElement* pNew = new TextElement();
    LPCTSTR secondText = m_strText.c_str() + pos;
    pNew->SetCharFormat(m_pcf);
    pNew->SetText(secondText, -1);

    String firstText(m_strText.c_str(), pos);
    SetText(firstText.c_str());

    pNew->InsertAfter(this);
}

// 获取最大索引
uint  TextElement::GetCharacters()
{
	return m_advances.size();
}

const CharFormat*  TextElement::GetCharFormat()
{
	if (m_pcf)
		return m_pcf;

    return Node::GetCharFormat();
}

const CharFormat*  TextElement::GetOriginCharFormat()
{
    return m_pcf;
}

void  TextElement::SetCharFormat(const CharFormat* pcf)
{
	if (!pcf)
	{
		SAFE_DELETE(m_pcf);
		return;
	}

	if (!m_pcf)
		m_pcf = new CharFormat;

	memcpy(m_pcf, pcf, sizeof(CharFormat));

    // 缓存该字体
    FontCache::Instance().CacheFont(m_pcf);

    m_bNeedAnalyze = true;
}


// 布局过程中，给定一个当前行的剩余宽度，求出还能放下多少字
bool  TextElement::GetRangeToByWidthFrom(uint nFrom, __inout uint* pWidth, __out uint* pnTo)
{
	uint nSize = m_advances.size();

	int nWidthReq = *pWidth;
	int nWidthRet = 0;

	int nTo = -1;
	for (uint i = nFrom; i < nSize; i++)
	{
		if (nWidthRet + m_advances[i] > nWidthReq)
			break;

		nWidthRet += m_advances[i];
		nTo = i;
	}

	// 一个都放不下
	if (-1 == nTo)
		return false;

	// 全部能放下
	*pWidth = nWidthRet;
	if (pnTo)
		*pnTo = nTo;

	return true;
}

// 由一个run发起来的鼠标点击检测.pos表示从nFrom开始的点击位置
CaretPos  TextElement::SelectionHitTest(uint nFrom, int posInElem)
{
    int nRet = 0;

    uint nWidth = 0;
    uint nWidthTrailLead = 0;
    uint nSize = m_advances.size();
    
    // 需要多运行一次，以检验最后一个字符后面的位置
    for (uint nIndex = nFrom; nIndex <= nSize; nIndex++)
    {
        if ((int)nWidth <= posInElem)
        {
            nRet = nIndex;
        }
        else
        {
            // 点击在该字符的后半部分
            if ((int)nWidthTrailLead <= posInElem)
            {
                nRet = nIndex;
            }
            break;
        }

        if (nIndex < nSize)
        {
            nWidthTrailLead = nWidth + (m_advances[nIndex] >> 1);
            nWidth += m_advances[nIndex];
        }
    }

    return CaretPos(nRet);
}

// 获取从nFrom开始，到文字结束之间的内容宽度
uint  TextElement::GetWidthByFrom(uint nFrom)
{
	if (0 == nFrom)
		return m_nABCWidth;

	uint nSize = (uint)m_advances.size();
	if (nFrom >= nSize)
		return 0;

	uint nHalfSize = nSize>>1;
	uint nRet = 0;

	if (nFrom < nHalfSize)
	{
		// 总大小减去 0~nFrom
		for (uint i = 0; i < nFrom; i++)
			nRet += m_advances[i];

		nRet = m_nABCWidth - nRet;
	}
	else
	{
		for (uint i = nFrom; i < nSize; i++)
			nRet += m_advances[i];
	}

	return nRet;
}

// 获取从开始，到nTo之间的内容宽度
uint  TextElement::GetWidthByTo(uint nTo)
{
    if (0 == nTo)
        return 0;

    uint nSize = (uint)m_advances.size();
    if (nTo >= nSize)
        return m_nABCWidth;

    uint nHalfSize = nSize >> 1;
    uint nRet = 0;

    if (nTo > nHalfSize)
    {
        // 总大小减去 0~nFrom
        for (uint i = nTo; i < nSize; i++)
            nRet += m_advances[i];

        nRet = m_nABCWidth - nRet;
    }
    else
    {
        for (uint i = 0; i < nTo; i++)
            nRet += m_advances[i];
    }

    return nRet;
}

// from to 代表光标的位置，不指定字符索引
uint  TextElement::GetWidthByFromTo(uint nFrom, uint nTo)
{
    if (nFrom > nTo)
    {
        swap(nFrom, nTo);
    }

    uint nSize = (uint)m_advances.size();
    if (nTo > nSize)
        nTo = nSize;

    uint nRet = 0;

//     uint nCount = nTo - nFrom;
//     uint nHalfSize = nSize >> 1;
//    
//     if (nCount > nHalfSize)
//     {
//     }
//     else
//     {
//         
//     }

    for (uint i = nFrom; i < nTo; i++)
        nRet += m_advances[i];

    return nRet;
}

uint  TextElement::GetWidth()
{
	return m_nABCWidth;
}

uint  TextElement::GetHeight()
{
	return GetCharFormat()->nFontHeight;
}


void  TextElement::Draw(HDC hDC, Run* pRun, RECT* prcRun)
{
    do 
    {
        if (GetSelectionState() == SelectionState_None)
            break;

        // 获取当前选区范围
        CaretPos runSelStart;
        CaretPos runSelEnd;

        if (!GetDocNode()->GetSelection().
                GetSelectionRange(pRun, runSelStart, runSelEnd))
        {
            break;
        }

        RECT rcSel = *prcRun;
        int nSelOffset = GetWidthByFromTo(pRun->GetStart(), runSelStart.pos);
        int nSelWidth = GetWidthByFromTo(runSelStart.pos, runSelEnd.pos);
        rcSel.left += nSelOffset;
        rcSel.right = rcSel.left + nSelWidth;
        if (rcSel.right > prcRun->right)
            rcSel.right = prcRun->right;
 
        if (!IsRectEmpty(&rcSel))
        {
            __super::DrawSelectionBackground(hDC, &rcSel);
        }
        
    } while (0);

    const CharFormat* pcf = this->GetCharFormat();
    LPCTSTR szText = m_strText.c_str() + pRun->GetStart();
    this->DrawText(hDC, prcRun, szText, pRun->GetLength(), pcf);
}

void  TextElement::ToString(
        CaretPos start, CaretPos end, int format, String& out)
{
    out = m_strText.substr(start.pos, end.pos - start.pos);
}


#if 0

TextUnit::TextUnit(Doc& pDoc):Unit(pDoc)
{
	m_pcf = NULL;
}
TextUnit::~TextUnit()
{
    Element* p = m_pFirstElement;
    Element* pNext = NULL;

    // 只textunit需要释放element,objectunit的element不是new出来的
    while (p)
    {
        pNext = p->GetNextElement();
        delete p;
        p = pNext;
    }
    m_pFirstElement = NULL;

	SAFE_DELETE(m_pcf);
}

void  TextUnit::SetCharFormat(RichTextCharFormat* pcf)
{
	if (!pcf)
	{
		SAFE_DELETE(m_pcf);
		return;
	}

	if (!m_pcf)
		m_pcf = new RichTextCharFormat;

	memcpy(m_pcf, pcf, sizeof(RichTextCharFormat));
}

const RichTextCharFormat*  TextUnit::GetCharFormat()
{
	if (m_pcf)
		return m_pcf;

	return m_pDoc.GetDefaultCharFormat();
}

    
//
// 外部添加了一段文字后，先将文字进行拆分为若干个Element。这个Element共用一种字体
// 例如"abc要不要123" -->  abc + 要不要 + 123
//
void  TextUnit::AppendTextNoCarriageReturn(
        LPCTSTR szText, 
        uint nLength, 
        RichTextCharFormat* pcf)
{
    if (!szText || !nLength)
        return;

    // 拆分
    vector<SCRIPT_ITEM>  vecScriptIetm;
    if (!ScriptItemizeWrap(szText, nLength, &vecScriptIetm) || 
        0 == vecScriptIetm.size())
        return;

    // 计算字符大小
    HDC hTempDC = CreateCompatibleDC(NULL);
    HFONT hOldFont = (HFONT)GetCurrentObject(hTempDC, OBJ_FONT);

    // Initialize to NULL, will be filled lazily.
    SCRIPT_CACHE script_cache = NULL;  

    SCRIPT_ITEM* script_item = &vecScriptIetm[0];
    for (uint i = 0; i < nLength; )
    {
        int nStart = i;
        int nEnd = (script_item + 1)->iCharPos;

        TextElement* pElement = new TextElement(*this);
        this->AppendElement(pElement);

        pElement->SetText(szText+nStart, nEnd-nStart);
		pElement->SetCharFormat(pcf);

        pElement->SetScriptAnalysis(&script_item->a);
        pElement->CalcGlyphSize(hTempDC, script_cache);

        script_item++;
        i = nEnd;
    }

    ScriptFreeCache(&script_cache);
    if (hOldFont)
        SelectObject(hTempDC, hOldFont);  // Put back the previous font.
    DeleteDC(hTempDC);
}

void  TextUnit::AppendCarriageReturn()
{
    __super::AppendElement(new CarriageReturnElement(*this));
}
#endif


void TextElement::LayoutMultiLine(MultiLineLayoutContext* context)
{
    if (m_bNeedAnalyze)
    {
        analyze();
    }
    context->BeginLayoutElement(this);
    {
        uint count = GetCharacters();
        for (uint i = 0; i < count; i++)
            context->PushChar(i, m_advances[i]);
    }
    context->EndLayoutElement(this);
}

void  TextElement::analyze()
{
    m_bNeedAnalyze = false;

    m_advances.clear();
    m_nABCWidth = 0;

    const CharFormat* pcf = this->GetCharFormat();
    UIASSERT(pcf);
    HFONT hfont = pcf->hFont;
    if (!hfont)
        return;

    LPCTSTR szText = m_strText.c_str();
    uint nLength = m_strText.length();

    // 拆分
    vector<SCRIPT_ITEM>  vecScriptIetm;
    if (!ScriptItemizeWrap(szText, nLength, &vecScriptIetm) ||
        0 == vecScriptIetm.size())
        return;

    // 计算字符大小
    HDC hTempDC = CreateCompatibleDC(NULL);
    HFONT hOldFont = (HFONT)SelectObject(hTempDC, hfont);

    // Initialize to NULL, will be filled lazily.
    SCRIPT_CACHE script_cache = NULL;

    SCRIPT_ITEM* script_item = &vecScriptIetm[0];
    for (uint i = 0; i < nLength;)
    {
        int nStart = i;
        int nEnd = (script_item + 1)->iCharPos;

        LPCTSTR text = m_strText.c_str() + nStart;
        uint length = nEnd - nStart;

        // pElement->SetScriptAnalysis(&script_item->a);
        // analyze_item(text, length, hTempDC, script_cache);
        
        std::vector<WORD> logs;
        std::vector<WORD> glyphs;
        std::vector<SCRIPT_VISATTR> visattr;
        ScriptShapeWrap(
            text,
            length,
            hfont,
            &script_cache,
            &script_item->a,
            &logs,
            &glyphs,
            &visattr);

        m_advances.insert(m_advances.end(), glyphs.size(), 0);
        vector<GOFFSET>   offsets;
        offsets.resize(glyphs.size());
        ABC abc;
       
        HRESULT hr = ScriptPlace(
            hTempDC,
            &script_cache,
            &glyphs[0], glyphs.size(),  // From previous call to ScriptShape
            &visattr[0],                // From previous call to ScriptShape
            &script_item->a,                      // From previous call to ScriptItemize
            &m_advances[nStart],               // Output: glyph advances
            &offsets[0],                // Output: glyph offsets
            &abc);                      // Output: size of run

        m_nABCWidth += abc.abcA + abc.abcB + abc.abcC;
        
        script_item++;
        i = nEnd;
    }

    ScriptFreeCache(&script_cache);
    if (hOldFont)
        SelectObject(hTempDC, hOldFont);  // Put back the previous font.
    DeleteDC(hTempDC);
}


void  TextElement::DrawText(
    HDC hDC, RECT* prc, LPCTSTR szText,
    ULONG nLen, const CharFormat* pcf)
{
    if (!pcf || !prc || !szText || !nLen)
        return;

    HFONT hfont = pcf->hFont;
    if (!hfont)
        return;

    COLORREF oldTextColor = 0, oldBackColor = 0;

    if (pcf->bTextColor)
    {
        oldTextColor = GetTextColor(hDC);
        SetTextColor(hDC, pcf->textColor & 0x00FFFFFF);
    }
    if (pcf->bBackColor)
    {
        oldBackColor = GetBkColor(hDC);
        SetBkMode(hDC, OPAQUE);
        ::SetBkColor(hDC, pcf->backColor & 0x00FFFFFF);
    }
    else
    {
        SetBkMode(hDC, TRANSPARENT);
    }

    int x = prc->left;
    int y = prc->bottom - pcf->nFontHeight;  // 文字默认画在baseline上

    // 单行的文本，需要支持末尾...功能，换成DrawText。但现在这个...还是问题，
    // 因为是基于run的，可能run只能显示下半个，结点...也显示不下了，只显示了半个字符。

    HFONT hOldFont = (HFONT)SelectObject(hDC, hfont);
    Doc* pDoc = GetDocNode();
    UIASSERT(pDoc);
    if (!pDoc)
        return;

    PageMode ePageMode = pDoc->GetPageMode();
    if (ePageMode == PageMode_singleline)
    {
        ::DrawTextW(hDC, szText, -1, prc, DT_TOP | DT_LEFT | DT_END_ELLIPSIS | DT_NOPREFIX);
    }
    else
    {
        ::TextOut(hDC, x, y, szText, nLen);
    }
    SelectObject(hDC, hOldFont);

    if (pcf->bTextColor)
    {
        SetTextColor(hDC, oldTextColor);
    }
    if (pcf->bBackColor)
    {
        SetBkMode(hDC, TRANSPARENT);
        ::SetBkColor(hDC, oldBackColor);
    }

	// 如果是一个链接，绘制下划线
	const Style& style = pDoc->GetStyle();
	if (style.draw_link_underline && 
		IsLink())
	{
		if (IsPress() ||/*&&*/ IsHover())
		{
			COLORREF color = RGB(0, 0, 0);
			if (pcf->bTextColor)
				color = pcf->textColor;

			int y = prc->bottom - pcf->nFontDescent + 1;

			HPEN hPen = CreatePen(PS_SOLID, 1, color);
			HPEN hOldPen = (HPEN)SelectObject(hDC, hPen);
			MoveToEx(hDC, prc->left, y, NULL);
			LineTo(hDC, prc->right, y);
			SelectObject(hDC, hOldPen);
			DeleteObject(hPen);
		}
	}

}

bool TextElement::IsLink()
{
	return __super::GetAncestorNodeByType(NodeTypeLink) ? true:false;
}