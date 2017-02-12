#pragma once
#include "3rd\markup\markup.h"
// #include "UI\UISDK\Inc\Util\ibuffer.h"
#include "..\Ole\richeditolemgr.h"

// re内容xml编码，用于复制或者提取内容

namespace UI
{

// ole 剪粘板格式
UINT  g_cfRichEditUnicodeXmlFormat = 
    ::RegisterClipboardFormat(_T("UI_RichEdit_Unicode_Xml"));; 

class REXmlEncode
{
public:
    REXmlEncode(WindowlessRichEdit* pRE)
    {
        m_pRE = pRE;
    }

    void  BeginEncode()
    {
        const CHARFORMAT2* pcf = NULL;
        m_pRE->TxGetCharFormat((const CHARFORMATW**)&pcf);

        m_markup.AddElem(RE_CF_ROOT);
        m_markup.AddAttrib(RE_CF_VERSION, L"1.0");

        // 保存字体信息
        if (pcf->dwEffects & CFE_BOLD)
            m_markup.AddAttrib(RE_CF_FONT_BOLD, 1);
        if (pcf->dwEffects & CFE_ITALIC)
            m_markup.AddAttrib(RE_CF_FONT_ITALIC, 1);
        if (pcf->dwEffects & CFE_UNDERLINE)
            m_markup.AddAttrib(RE_CF_FONT_UNDERLINE, 1);

        m_markup.AddAttrib(RE_CF_FONT_COLOR, pcf->crTextColor);
        m_markup.AddAttrib(RE_CF_FONT_FACE, pcf->szFaceName);
        m_markup.AddAttrib(RE_CF_FONT_HEIGHT, (int)pcf->yHeight);
        m_markup.IntoElem();

    }
    void  EndEncode()
    {

    }

    // markup会负责对特殊符号编码
    void  AddText(LPCTSTR szText, int nLength)
    {
        m_markup.AddElem(RE_CF_OLE_TEXT, szText);
        m_markup.AddAttrib(RE_CF_OLE_TEXT_LENGTH, nLength);
    }

    // markup不负责szDoc的特殊符号编码
    void  AddSubDoc(LPCTSTR szDoc)
    {
        m_markup.AddSubDoc(szDoc);
    }

    void  AddREOle(REOleBase* pOle)
    {
        CComBSTR bstr;
        if (SUCCEEDED(pOle->GetClipboardData(g_cfRichEditUnicodeXmlFormat, &bstr)))
        {
            if (bstr && bstr.Length())
            {
                AddSubDoc(bstr);
            }
        }
    }

    int  GetContentLength()
    {
        return m_markup.GetDoc().length();
    }
    LPCTSTR  GetContentBuffer()
    {
        return m_markup.GetDoc().c_str();
    }

//     void  GetContentBuffer(IBuffer** ppBuffer)
//     {
//         if (!ppBuffer)
//             return;
// 
//         IBuffer* pBuffer = NULL;
//         UI::CreateTCHARBuffer(&pBuffer);
// 
//         int nSize = GetContentLength() + 1;
//         TCHAR* szBuffer = new TCHAR[nSize];
//         _tcscpy(szBuffer, GetContentBuffer());
// 
//         pBuffer->SetBuffer(szBuffer, nSize);
//         *ppBuffer = pBuffer;
//     }

    void  Add2DataObject(IDataObject* pDataObject)
    {
        if (!pDataObject)
            return;

        FORMATETC  format = {0};
        format.dwAspect = DVASPECT_CONTENT;
        format.cfFormat = g_cfRichEditUnicodeXmlFormat;
        format.tymed = TYMED_HGLOBAL;

        int nSize = GetContentLength() + 1;
        HGLOBAL hGlobal = GlobalAlloc(0, sizeof(WCHAR)*nSize);
        LPWSTR lpstr = (LPWSTR)::GlobalLock(hGlobal);
        _tcscpy(lpstr, GetContentBuffer());
        ::GlobalUnlock(hGlobal);

        STGMEDIUM medium = {0};
        medium.tymed = TYMED_HGLOBAL;
        medium.hGlobal = hGlobal;
        if (FAILED(pDataObject->SetData(&format, &medium, TRUE)))
        {
            GlobalFree(hGlobal);
        }
    }

private:
    CMarkup  m_markup;
    WindowlessRichEdit*  m_pRE;
};


}