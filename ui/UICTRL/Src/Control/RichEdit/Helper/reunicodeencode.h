#pragma once

namespace UI
{

class REUnicodeEncode
{
public:
    REUnicodeEncode(WindowlessRichEdit* pRE)
    {
        m_pRE = pRE;
    }

    void  BeginEncode()
    {

    }
    void  EndEncode()
    {

    }

    void  AddText(LPCTSTR szText, int nLength)
    {
        if (szText)
            m_strText.append(szText);
    }

    void  AddREOle(REOleBase* pOle)
    {
        CComBSTR bstr;
        if (SUCCEEDED(pOle->GetClipboardData(CF_UNICODETEXT, &bstr)))
        {
            if (bstr && bstr.Length())
            {
                m_strText.append(bstr);
            }
        }
    }


    void  Add2DataObject(IDataObject* pDataObject)
    {
        if (!pDataObject)
            return;

        FORMATETC  format = {0};
        format.dwAspect = DVASPECT_CONTENT;
        format.tymed = TYMED_HGLOBAL;
        STGMEDIUM medium = {0};
        medium.tymed = TYMED_HGLOBAL;

        {
            HGLOBAL hGlobalUnicode = GlobalAlloc(0, sizeof(WCHAR)*(m_strText.length()+1));
            LPWSTR lpstr = (LPWSTR)::GlobalLock(hGlobalUnicode);
            _tcscpy(lpstr, m_strText.c_str());
            ::GlobalUnlock(hGlobalUnicode);

            format.cfFormat = CF_UNICODETEXT;
            medium.hGlobal = hGlobalUnicode;
            if (FAILED(pDataObject->SetData(&format, &medium, TRUE)))
            {
                GlobalFree(hGlobalUnicode);
            }

        }
        {
            string strAnsii = CT2A(m_strText.c_str());
            HGLOBAL hGlobalAnsii = GlobalAlloc(0, strAnsii.length()+1);
            LPSTR lpstr = (LPSTR)::GlobalLock(hGlobalAnsii);
            strcpy(lpstr, strAnsii.c_str());
            ::GlobalUnlock(hGlobalAnsii);

            format.cfFormat = CF_TEXT;
            medium.hGlobal = hGlobalAnsii;
            if (FAILED(pDataObject->SetData(&format, &medium, TRUE)))
            {
                GlobalFree(hGlobalAnsii);
            }
        }
    }

private:
    String  m_strText;
    WindowlessRichEdit*  m_pRE;
};

}