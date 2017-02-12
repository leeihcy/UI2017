#include "stdafx.h"
#include "comreole.h"

using namespace UI;

namespace UI
{
extern UINT  g_cfRichEditUnicodeXmlFormat;
extern UINT  CF_HTML;
}

ComREOle::ComREOle()
{
    m_pOleObject = NULL;
    memset(&m_clsid, 0, sizeof(m_clsid));
}
ComREOle::~ComREOle()
{  
    SAFE_RELEASE(m_pOleObject);
}

HRESULT  ComREOle::GetOleObject(IOleObject** ppOleObject, bool bAddRef)
{
    if (NULL == ppOleObject)
        return E_INVALIDARG;

    if (NULL == m_pOleObject)
        return E_FAIL;

    *ppOleObject = m_pOleObject;

    if (bAddRef)
        m_pOleObject->AddRef();

    return S_OK;
}

HRESULT  ComREOle::GetClipboardData(UINT nClipFormat, __out BSTR* pbstrData)
{
    if (nClipFormat == g_cfRichEditUnicodeXmlFormat)
    {
        WCHAR szText[128] = {0};
        WCHAR szGUID[64] = {0};

        StringFromCLSID(m_clsid, (LPOLESTR*)&szGUID);
        wsprintf(szText, L"<com version=\"1.0\" clsid=\"%s\"/>", szGUID);
        *pbstrData = SysAllocString(szText);
        return S_OK;
    }

    return E_FAIL;
}

HRESULT  ComREOle::GetClipboardDataEx(IDataObject* pDataObject)
{
    return E_FAIL;
}

HRESULT  ComREOle::Attach(CLSID  clsid)
{
    SAFE_RELEASE(m_pOleObject);
    m_clsid = clsid;
    return ::CoCreateInstance(clsid, NULL, CLSCTX_INPROC, IID_IOleObject, (void**)&m_pOleObject); 
}

ULONG STDMETHODCALLTYPE ComREOle::AddRef( void) 
{
    return AddRefImpl();
}

ULONG STDMETHODCALLTYPE ComREOle::Release( void) 
{
    return ReleaseImpl();
}

