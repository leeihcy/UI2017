#pragma once
#include "richeditolemgr.h"
#include "Inc\Interface\iricheditole.h"

namespace UI
{

// 存储外部自使用COM实现的OLE对象
class ComREOle : public REOleBase
{
public:
    ComREOle();
    ~ComREOle();

public:
#pragma region // IUnknown
    virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void  **ppvObject){return E_NOTIMPL;}
    virtual ULONG   STDMETHODCALLTYPE AddRef(void);
    virtual ULONG   STDMETHODCALLTYPE Release(void);
#pragma endregion

    virtual REOLETYPE  GetOleType() override { return REOLE_COM; }
    virtual HRESULT  GetOleObject(IOleObject** ppOleObject, bool bAddRef=true);
    virtual HRESULT  GetClipboardData(UINT nClipFormat, __out BSTR* pbstrData);
    virtual HRESULT  GetClipboardDataEx(IDataObject* pDataObject);
    HRESULT  Attach(CLSID  clsid);

private:
    IOleObject*   m_pOleObject;
    CLSID         m_clsid;
};

}