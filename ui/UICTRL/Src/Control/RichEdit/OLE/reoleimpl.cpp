#include "stdafx.h"
#include "reoleimpl.h"
#include "..\windowlessrichedit.h"
#include "..\UISDK\Inc\Util\struct.h"

REOleImpl::REOleImpl()
{
    m_pViewAdviseSink = NULL;
    m_pClientSite = NULL;

    m_pOleAdviseHolder = NULL;
	SetRectEmpty(&m_rcDrawSave);
    //	m_pDataAdviseHolder = NULL;
}
REOleImpl::~REOleImpl()
{
    SAFE_RELEASE(m_pViewAdviseSink);
    SAFE_RELEASE(m_pClientSite);
    SAFE_RELEASE(m_pOleAdviseHolder);
    //	SAFE_RELEASE(m_pDataAdviseHolder);
}

HRESULT STDMETHODCALLTYPE REOleImpl::QueryInterface(REFIID riid, void** ppvObject)
{
    if (::IsEqualIID(riid, IID_IUnknown) || ::IsEqualIID(riid, IID_IOleObject))
    {
        *ppvObject = static_cast<IOleObject*>(this);
    }
    else if (::IsEqualIID(riid, IID_IViewObject))
    {
        *ppvObject = static_cast<IViewObject*>(this);
    }
    else if (::IsEqualIID(riid, IID_IViewObject2))
    {
        *ppvObject = static_cast<IViewObject2*>(this);
    }
    else
    {
        return E_NOINTERFACE;
    }

    this->AddRef();
    return S_OK;
}



ULONG STDMETHODCALLTYPE REOleImpl::AddRef( void) 
{
    return AddRefImpl();
}

ULONG STDMETHODCALLTYPE REOleImpl::Release( void) 
{
    return ReleaseImpl();
}

HRESULT REOleImpl::GetOleObject(IOleObject** ppOleObject, bool bAddRef)
{
    if (NULL == ppOleObject)
        return E_INVALIDARG;

    *ppOleObject = static_cast<IOleObject*>(this);

    if (bAddRef)
        this->AddRef();

    return S_OK;
}

HRESULT STDMETHODCALLTYPE REOleImpl::SetClientSite(IOleClientSite *pClientSite)
{
    SAFE_RELEASE(m_pClientSite);
    m_pClientSite = pClientSite;
    if (m_pClientSite)
        m_pClientSite->AddRef();
    return S_OK;
}

HRESULT STDMETHODCALLTYPE REOleImpl::GetClientSite(IOleClientSite **ppClientSite) 
{
    *ppClientSite = m_pClientSite;
    if (m_pClientSite)
        m_pClientSite->AddRef();

    return S_OK;
}

LONG DXtoHimetricX(LONG dx, LONG xPerInch=-1);
LONG DYtoHimetricY(LONG dy, LONG yPerInch=-1);
HRESULT STDMETHODCALLTYPE REOleImpl::GetExtent(DWORD dwDrawAspect, SIZEL *psizel) 
{
    //	psizel->cx = psizel->cy = 2000;
    SIZE  size = {0,0};
    this->GetSize(&size);

    if (-1 == size.cx && -1 == size.cy)
    {
        psizel->cx = 1;
        psizel->cy = 1;
    }
    else
    {
        psizel->cx = DXtoHimetricX(size.cx);
        psizel->cy = DYtoHimetricY(size.cy);
    }

    return S_OK;
}

HRESULT STDMETHODCALLTYPE REOleImpl::Draw( 
    DWORD dwDrawAspect,
    LONG lindex,
    void *pvAspect,
    DVTARGETDEVICE *ptd,
    HDC hdcTargetDev,
    HDC hdcDraw,
    LPCRECTL lprcBounds,
    LPCRECTL lprcWBounds,
    BOOL ( STDMETHODCALLTYPE *pfnContinue )(ULONG_PTR dwContinue),
    ULONG_PTR dwContinue) 
{
	RECT  rc = {
		lprcBounds->left, 
		lprcBounds->top, 
		lprcBounds->right, 
		lprcBounds->bottom};

	// 优化：2016/12/14 例如光标刷新时，不去刷新其它ole
	CRect rcClip;
	GetClipBox(hdcDraw, &rcClip);
	if (!IntersectRect(&rcClip, &rcClip, &rc))
		return S_FALSE;

	__super::OnInternalDraw(hdcDraw, &rc);  // 基类的逻辑
    this->OnDraw(hdcDraw, &rc);  // 子类的逻辑

    // 	HBRUSH hBrush = CreateSolidBrush(rand());
    // 	::FillRect(hdcDraw, (RECT*)lprcBounds, hBrush);
    // 	DeleteObject(hBrush);

    return S_OK;
}

HRESULT STDMETHODCALLTYPE REOleImpl::GetExtent(DWORD dwDrawAspect, LONG lindex, DVTARGETDEVICE *ptd, LPSIZEL lpsizel) 
{
    return this->GetExtent(dwDrawAspect, lpsizel);
}

HRESULT STDMETHODCALLTYPE REOleImpl::Advise(IAdviseSink *pAdvSink,DWORD *pdwConnection)
{
    HRESULT hr = S_OK;
    if (m_pOleAdviseHolder == NULL)
        hr = CreateOleAdviseHolder(&m_pOleAdviseHolder);
    if (SUCCEEDED(hr))
        hr = m_pOleAdviseHolder->Advise(pAdvSink, pdwConnection);
    return hr;
}
HRESULT STDMETHODCALLTYPE REOleImpl::Unadvise(DWORD dwConnection)
{
    HRESULT hRes = E_FAIL;
    if (m_pOleAdviseHolder != NULL)
        hRes = m_pOleAdviseHolder->Unadvise(dwConnection);
    return hRes;
}
HRESULT STDMETHODCALLTYPE REOleImpl::SetAdvise( DWORD aspects, DWORD advf, IAdviseSink *pAdvSink) 
{
    SAFE_RELEASE(m_pViewAdviseSink);
    m_pViewAdviseSink = pAdvSink;
    if (m_pViewAdviseSink)
        m_pViewAdviseSink->AddRef();
    return S_OK;
}

HRESULT STDMETHODCALLTYPE REOleImpl::GetAdvise(DWORD *pAspects, DWORD *pAdvf, IAdviseSink **ppAdvSink)
{
    *ppAdvSink = m_pViewAdviseSink;
    if (m_pViewAdviseSink)
        m_pViewAdviseSink->AddRef();
    return S_OK;
}

// 通知re，自己的大小发生改变
void  REOleImpl::SelfSizeChanged()
{
    if (!m_pViewAdviseSink)
        return;

    m_pViewAdviseSink->OnViewChange(DVASPECT_CONTENT, -1);
    
    // 更新自己所在的气泡大小
    GetWindowlessRE()->OnOleSizeChanged(this);
}

// 通知re，自己需要被刷新
void  REOleImpl::InvalidateOle()
{
    InvalidateEx(&m_rcDrawSave);
}

void  REOleImpl::InvalidateEx(RECT* prcInControl)
{
    if (!IsVisibleInField())
        return;

    if (NULL == prcInControl)
        prcInControl = &m_rcDrawSave;

    WindowlessRichEdit*  pRE = GetWindowlessRE();
    if (!pRE)
        return;

    // 局部刷新
    RECT rcWnd;
    pRE->GetControlPtr()->ClientRect2WindowRect(prcInControl, &rcWnd);
    pRE->InvalidateRect(&rcWnd);
}

void  REOleImpl::ShowToolTip(LPCTSTR szContent)
{
    WindowlessRichEdit*  pRE = GetWindowlessRE();
    if (!pRE)
        return;

    IRichEdit* pRichEdit = pRE->GetControlPtr();
    if (!pRichEdit)
        return;

    IUIApplication* pUIApp = pRichEdit->GetUIApplication();

    if (!szContent)
    {
        pUIApp->HideToolTip();
    }
    else
    {
        TOOLTIPITEM  item = {0};
        item.szToolTipContent = szContent;
        pUIApp->ShowToolTip(&item);
    }
}

long  REOleImpl::DoNotify(UIMSG* msg)
{
    if (!msg)
        return 0;

    WindowlessRichEdit*  pRE = GetWindowlessRE();
    if (!pRE)
        return 0;

    IRichEdit* pRichEdit = pRE->GetControlPtr();
    if (!pRichEdit)
        return 0;

    msg->pMsgFrom = pRichEdit;
    return pRichEdit->DoNotify(msg);
}