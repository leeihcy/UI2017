#pragma once
#include "richeditolemgr.h"

namespace UI
{

// 存储内部自己实现的ole对象的相关的信息
class REOleImpl : public REOleBase
	        ,public IOleObject
	        ,public IViewObject2
            
{
public:
	REOleImpl();
	virtual ~REOleImpl();


#pragma region // IUnknown
    virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void  **ppvObject);
    virtual ULONG   STDMETHODCALLTYPE AddRef(void);
    virtual ULONG   STDMETHODCALLTYPE Release(void);
#pragma endregion

#pragma region // ole object 
	virtual HRESULT STDMETHODCALLTYPE SetHostNames(LPCOLESTR szContainerApp,LPCOLESTR szContainerObj) 
	{ return S_OK; }

	// 将一个OLE对象用delete键给删除掉时，会调用。然后将对象用ctrl+z再还原，然后再删除时，又会调用一次
	// 该函数是否是可以用于停止播放一个GIF文件？那 ctrl+z 还原时，会调用什么接口函数？
	virtual HRESULT STDMETHODCALLTYPE Close(DWORD dwSaveOption) 
	{
		return S_OK;	
	}

	virtual HRESULT STDMETHODCALLTYPE SetMoniker(DWORD dwWhichMoniker, IMoniker *pmk)
	{ return E_NOTIMPL;	}
	virtual HRESULT STDMETHODCALLTYPE GetMoniker(DWORD dwAssign, DWORD dwWhichMoniker, IMoniker **ppmk) 
	{ return E_NOTIMPL;	}
	virtual HRESULT STDMETHODCALLTYPE InitFromData(IDataObject *pDataObject, BOOL fCreation, DWORD dwReserved)
	{ return E_NOTIMPL;	}
	virtual HRESULT STDMETHODCALLTYPE GetClipboardData(DWORD dwReserved, IDataObject **ppDataObject)
	{ return E_NOTIMPL; }
	virtual HRESULT STDMETHODCALLTYPE DoVerb(LONG iVerb, LPMSG lpmsg, IOleClientSite *pActiveSite,LONG lindex, HWND hwndParent, LPCRECT lprcPosRect) 
	{ return E_NOTIMPL; }
	virtual HRESULT STDMETHODCALLTYPE EnumVerbs(IEnumOLEVERB **ppEnumOleVerb) 
	{ return E_NOTIMPL; }
	virtual HRESULT STDMETHODCALLTYPE Update( void) 
	{ return E_NOTIMPL; }
	virtual HRESULT STDMETHODCALLTYPE IsUpToDate( void) 
	{ return E_NOTIMPL;	}
	virtual HRESULT STDMETHODCALLTYPE GetUserClassID(CLSID *pClsid) 
	{ return E_NOTIMPL;	}
	virtual HRESULT STDMETHODCALLTYPE GetUserType(DWORD dwFormOfType, LPOLESTR *pszUserType) 
	{ return E_NOTIMPL; }
	virtual HRESULT STDMETHODCALLTYPE SetExtent(DWORD dwDrawAspect, SIZEL *psizel) 
	{ return E_NOTIMPL; }
	virtual HRESULT STDMETHODCALLTYPE EnumAdvise(IEnumSTATDATA **ppenumAdvise) 
	{ return E_NOTIMPL; }
	virtual HRESULT STDMETHODCALLTYPE GetMiscStatus(DWORD dwAspect, DWORD *pdwStatus) 
	{ return E_NOTIMPL;	}
	virtual HRESULT STDMETHODCALLTYPE SetColorScheme(LOGPALETTE *pLogpal)
	{ return E_NOTIMPL;	}

	virtual HRESULT STDMETHODCALLTYPE Advise(IAdviseSink *pAdvSink,DWORD *pdwConnection);
	virtual HRESULT STDMETHODCALLTYPE Unadvise(DWORD dwConnection);
	virtual HRESULT STDMETHODCALLTYPE SetClientSite(IOleClientSite *pClientSite);
	virtual HRESULT STDMETHODCALLTYPE GetClientSite(IOleClientSite **ppClientSite);
	virtual HRESULT STDMETHODCALLTYPE GetExtent(DWORD dwDrawAspect, SIZEL *psizel);
#pragma  endregion

#pragma region // iviewobject
	virtual HRESULT STDMETHODCALLTYPE GetColorSet( 
                DWORD dwDrawAspect, LONG lindex, void *pvAspect, 
                DVTARGETDEVICE *ptd, HDC hicTargetDev, LOGPALETTE **ppColorSet) {
                return E_NOTIMPL; }
	virtual HRESULT STDMETHODCALLTYPE Freeze( 
                DWORD dwDrawAspect, LONG lindex, void *pvAspect, DWORD *pdwFreeze) {
                return S_OK; }
	virtual HRESULT STDMETHODCALLTYPE Unfreeze(DWORD dwFreeze) {
                return S_OK; }
	virtual HRESULT STDMETHODCALLTYPE SetAdvise( DWORD aspects, DWORD advf, IAdviseSink *pAdvSink);
	virtual HRESULT STDMETHODCALLTYPE GetAdvise(DWORD *pAspects, DWORD *pAdvf, IAdviseSink **ppAdvSink);
	virtual HRESULT STDMETHODCALLTYPE Draw(
                DWORD dwDrawAspect, LONG lindex,  void *pvAspect, 
                DVTARGETDEVICE *ptd, HDC hdcTargetDev, HDC hdcDraw, 
                LPCRECTL lprcBounds, LPCRECTL lprcWBounds,
                BOOL ( STDMETHODCALLTYPE *pfnContinue )(ULONG_PTR dwContinue), 
                ULONG_PTR dwContinue);
	virtual HRESULT STDMETHODCALLTYPE GetExtent(
                DWORD dwDrawAspect, LONG lindex, DVTARGETDEVICE *ptd, LPSIZEL lpsizel);
#pragma endregion

#pragma region  // 实现父类的虚函数
	virtual HRESULT  GetOleObject(IOleObject** ppOleObject, bool bAddRef=true) override;
#pragma  endregion

public:
    void  SelfSizeChanged();
	void  InvalidateOle();
    void  InvalidateEx(RECT* prcInControl);
    void  ShowToolTip(LPCTSTR szContent);
    long  DoNotify(UIMSG* msg);

protected:
//		IDataAdviseHolder* m_pDataAdviseHolder;
	IOleAdviseHolder*  m_pOleAdviseHolder;
	IAdviseSink*       m_pViewAdviseSink;
	IOleClientSite*    m_pClientSite;
};
}