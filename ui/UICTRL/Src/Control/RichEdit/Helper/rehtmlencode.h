#pragma once
#include "..\Ole\richeditolemgr.h"

// re内容html编码，用于复制


namespace UI
{

// ole 剪粘板格式
UINT  CF_HTML =
    ::RegisterClipboardFormat(_T("HTML Format"));

class REHtmlEncode
{
public:
    REHtmlEncode(WindowlessRichEdit* pRE)
    {
        m_pRE = pRE;
    }

    void  BeginEncode()
    {
        m_strText.append(TEXT("<DIV>\r\n"));
    }
    void  EndEncode()
    {
        m_strText.append(TEXT("\r\n</DIV>"));
    }

    void  AddText(LPCTSTR szText, int nLength)
    {
        if (szText)
            m_strText.append(szText);
    }

    void  AddREOle(REOleBase* pOle)
    {
        CComBSTR bstr;
        if (SUCCEEDED(pOle->GetClipboardData(CF_HTML, &bstr)))
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
        format.cfFormat = CF_HTML;
        format.tymed = TYMED_HGLOBAL;

        STGMEDIUM medium = {0};
        medium.tymed = TYMED_HGLOBAL;
        HGLOBAL hGlobal = GetHtmlData(CT2A(m_strText.c_str(), CP_UTF8));
        medium.hGlobal = hGlobal;
        if (FAILED(pDataObject->SetData(&format, &medium, TRUE)))
        {
            GlobalFree(hGlobal);
        }
    }

    // CopyHtml() - Copies given HTML to the clipboard.
    // The HTML/BODY blanket is provided, so you only need to
    // call it like CallHtml("<b>This is a test</b>");
    HGLOBAL GetHtmlData(char *html) 
    {
        // Create temporary buffer for HTML header...
        char *buf = new char [400 + strlen(html)];
        if(!buf) 
            return NULL;

        // Create a template string for the HTML header...
        strcpy(buf,
            "Version:0.9\r\n"
            "StartHTML:00000000\r\n"
            "EndHTML:00000000\r\n"
            "StartFragment:00000000\r\n"
            "EndFragment:00000000\r\n"
            "<html><body>\r\n"
            "<!--StartFragment -->\r\n");

        // Append the HTML...
        strcat(buf, html);
        strcat(buf, "\r\n");
        // Finish up the HTML format...
        strcat(buf,
            "<!--EndFragment-->\r\n"
            "</body>\r\n"
            "</html>");

        // Now go back, calculate all the lengths, and write out the
        // necessary header information. Note, wsprintf() truncates the
        // string when you overwrite it so you follow up with code to replace
        // the 0 appended at the end with a '\r'...
        char *ptr = strstr(buf, "StartHTML");
        sprintf(ptr+10, "%08u", strstr(buf, "<html>") - buf);
        *(ptr+10+8) = '\r';

        ptr = strstr(buf, "EndHTML");
        sprintf(ptr+8, "%08u", strlen(buf));
        *(ptr+8+8) = '\r';

        ptr = strstr(buf, "StartFragment");
        sprintf(ptr+14, "%08u", strstr(buf, "<!--StartFrag") - buf);
        *(ptr+14+8) = '\r';

        ptr = strstr(buf, "EndFragment");
        sprintf(ptr+12, "%08u", strstr(buf, "<!--EndFrag") - buf);
        *(ptr+12+8) = '\r';

        // Allocate global memory for transfer...
        HGLOBAL hText = GlobalAlloc(GMEM_MOVEABLE |GMEM_DDESHARE, strlen(buf)+4);
        {
            // Put your string in the global memory...
            char *ptr = (char *)GlobalLock(hText);
            strcpy(ptr, buf);
            GlobalUnlock(hText);
        }

        // Clean up...
        delete [] buf;
        return hText;
    } 

private:
    String  m_strText;
    WindowlessRichEdit*  m_pRE;
};


}