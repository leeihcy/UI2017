#include "StdAfx.h"
#include "windowlessrichedit.h"
#include "helper\retextenum.h"
#include "helper\rexmlencode.h"
#include "Helper\rehtmlencode.h"
#include "Helper\reunicodeencode.h"
#include "OLE\comreole.h"
#include "richedittable.h"
#include "3rd\markup\markup.h"
#include "richedit.h"
#include "..\UISDK\Inc\Interface\iscrollbarmanager.h"
#include "..\UISDK\Inc\Interface\iwindow.h"
#include "..\UISDK\Inc\Util\struct.h"

//TEMP
#include "textfilter\uintfilter.h"
#include "TextFilter\digitfilter.h"
#include "TextFilter\phonenumberfilter.h"
#include "TextFilter\accountfilter.h"



// 备注：在CreateTextSerives之后，调用QueryInterface获取ITextServices接口时，总是返回E_NOINTERFACE
//       问题原来是从riched20.lib中的IID_ITextServices，是错误的。
const IID IID_ITextServices_Fix = { 0x8d33f740, 0xcf58, 0x11ce, {0xa8, 0x9d, 0x00, 0xaa, 0x00, 0x6c, 0xad, 0xc5} };
const IID IID_ITextHost = { 0x03bc25ff, 0x69bb, 0x25ff, {0x14, 0x0c, 0xbb, 0x69, 0xff, 0x25, 0x24, 0x0c} };

// RichEdit QueryInterface ITextDocument2失败问题，guid不对
extern "C" const GUID __declspec(selectany) IID_ITextDocument2 =
{ 0x01c25500, 0x4268, 0x11d1, { 0x88, 0x3a, 0x3c, 0x8b, 0x00, 0xc1, 0x00, 0x00 } };


HMODULE WindowlessRichEdit::s_RichEditDll = NULL;
LONG    WindowlessRichEdit::s_refDll = 0;
UINT    WindowlessRichEdit::s_cfRichTextFormat = 0;
UINT    WindowlessRichEdit::s_cfRichTextAndObjects = 0;
pfuncCreateTextServices  WindowlessRichEdit::s_funcCreateTextServices = NULL;

WindowlessRichEdit::WindowlessRichEdit()
{
	m_pRichEdit = NULL;
	m_pOleMgr = new REOleManager(this);
	m_nLastTimerKickCount = 0;
    m_bDuringTxDraw = false;

    m_nAutoResizeMaxSize = 0;
    m_bFocus = false;
    m_bScrollBarVisible = false;
    
    m_pTextFilter = NULL;

	this->InitRichEidtDll();
}

void WindowlessRichEdit::BindControl(RichEdit* p)
{
	m_pRichEdit = p;
	m_pIRichEdit = p->GetIRichEdit();

    if (m_pOleMgr && m_pIRichEdit)
	    m_pOleMgr->SetUIApplication(m_pIRichEdit->GetUIApplication());
}
WindowlessRichEdit::~WindowlessRichEdit(void)
{
	// 释放所有的ole对象
	this->ClearOleObjects();

	// Revoke our drop target
	RevokeDragDrop();

    __super::Destroy();  
	SAFE_DELETE(m_pOleMgr);
    if (m_pTextFilter)
    {
        m_pTextFilter->Release(false);
        m_pTextFilter = NULL;
    }

    m_pRichEdit = NULL;
	this->ReleaseRichEidtDll();
}

// S_FALSE: Message was not processed. Typically indicates that the caller should process the message itself, potentially by calling DefWindowProc. 
// 如果 pInvokeResult 返回 S_FALSE，表示要交给窗口继续去处理这个消息
LRESULT  ITextHostImpl::RESendMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT* pInvokeResult)
{
    if (!m_spTextServices)
        return 0;

	LRESULT lRet = 0;
    LRESULT lResult = m_spTextServices->TxSendMessage(uMsg, wParam, lParam, &lRet);
    if (pInvokeResult)
        *pInvokeResult = lResult;

	return lRet;
}

//
// 有几个注意事项非常重要正确启用以这种功能的工作的： 
//
//  框架窗口将工具栏和状态栏的父项的需要创建具有 WS_CLIPCHILDREN 样式。如果不存在此样式应
//  用程序将在对象处于活动的 RichEdit 控件中时表现绘画的一些问题。 
//
//  应该用 WS_CLIPSIBLING 样式创建 RichEdit 控件本身。 此处太，如果不存在样式 RichEdit 控
//  件将出现绘画问题时对象处于活动的过程中创建子窗口。 
//
//  销毁 RichEdit 控制时，您的应用程序应停用任何处于活动对象和调用 IOleObject->Close() 
//  RichEdit 控件中的所有嵌入的对象上。如果这不进行某些对象应用程序可能不关闭，从而导致它
//  们保留在内存，即使 RichEdit 控件已被破坏。下面是演示如何处理结算的 OLE 对象的代码段：
//
void WindowlessRichEdit::ClearOleObjects()
{
	if (m_spOle)
	{
		HRESULT hr = 0;

		// 
		// Start by getting the total number of objects in the control.
		// 
		int objectCount = m_spOle->GetObjectCount();

		// 
		// Loop through each object in the control and if active
		// deactivate, and if open, close.
		// 
		for (int i = 0; i < objectCount; i++)
		{
			REOBJECT reObj;
			ZeroMemory(&reObj, sizeof(REOBJECT));
			reObj.cbStruct  = sizeof(REOBJECT);

			// 
			// Get the Nth object
			// 
			hr = m_spOle->GetObject(i,&reObj,REO_GETOBJ_POLEOBJ);
			if(SUCCEEDED(hr))
			{
				// 
				// If active, deactivate.
				// 
				if (reObj.dwFlags && REO_INPLACEACTIVE)
					m_spOle->InPlaceDeactivate();

				// 
				// If the object is open, close it.
				// 
				if(reObj.dwFlags&&REO_OPEN)
					hr = reObj.poleobj->Close(OLECLOSE_NOSAVE);

				reObj.poleobj->Release();  // release GetObject
			}
		}
	}
}

bool WindowlessRichEdit::Create(HWND hWndParent)
{
	if (NULL == m_pRichEdit)
		return false;
	if (NULL == s_funcCreateTextServices)
		return false;

	LRESULT lr = 0;

	HDC hdc = GetDC(hWndParent);
	m_nxPerInch = GetDeviceCaps(hdc, LOGPIXELSX);  // TODO: 修改了分辨率的话，这个值就有问题了？
	m_nyPerInch = GetDeviceCaps(hdc, LOGPIXELSY); 
	ReleaseDC(hWndParent, hdc);

	if (!(m_dwStyle & (ES_AUTOHSCROLL | WS_HSCROLL)))
	{
		m_fWordWrap = true;  // 注：初始化m_fWordWrap不能放在CreateTextService后面，否则没作用
	}

	//////////////////////////////////////////////////////////////////////////
	// 创建Text Service

	IUnknown* pUnknown = NULL;
	HRESULT hr = s_funcCreateTextServices(NULL, static_cast<ITextHost*>(this), &pUnknown);
	if (FAILED(hr))
		return false;

	hr = pUnknown->QueryInterface(IID_ITextServices_Fix, (void**)&m_spTextServices);
 	pUnknown->Release();

	if (FAILED(hr) || NULL == m_spTextServices)
		return false;
	m_hParentWnd = hWndParent;
 
	// 避免英文字体与中文字体不统一的问题
    SetDualFont(false);

 	if (!(m_dwStyle & ES_READONLY))
 	{
 		// This isn't a read only window so we need a drop target.
 		RegisterDragDrop();
 	}

 	IRichEditOle* pOle = NULL;
 	RESendMessage(EM_GETOLEINTERFACE,0, (LPARAM)&pOle);
 	if (pOle)
 	{
 		m_spOle = pOle;
        GUID guid = __uuidof(ITextDocument2);
        pOle->QueryInterface(guid, (void**)&m_spTextDoc);
 
        SAFE_RELEASE(pOle);
 	}
 	RESendMessage(EM_SETOLECALLBACK, 0, (LPARAM)static_cast<IRichEditOleCallback*>(this));
    ModifyEventMask(ENM_CHANGE, 0);

//   // 4.1以上的RE可以支持TSF(Text Service Framework)
//    RESendMessage(EM_SETEDITSTYLE, SES_USECTF, SES_USECTF);

    m_spTextServices->OnTxInPlaceActivate(NULL);

    // 给子类初始化
    this->OnCreate();

	return true;
}

LPCTSTR WindowlessRichEdit::GetLibraryName()
{
	return _T("RICHED20.DLL");

// #if (_RICHEDIT_VER >= 0x0200)
//     return _T("msftedit.dll");  // 在win10上使用这个dll，ctrl+a变成了切换大小写
// #elif (_RICHEDIT_VER > 0x100)
// 	return _T("RICHED20.DLL");
// #else
// 	return _T("RICHED32.DLL");
// #endif
}

HFONT Hook_CreateFontIndirectW(
        CONST LOGFONT* lplf   // characteristics
    )
{
    const_cast<LOGFONT*>(lplf)->lfQuality = CLEARTYPE_QUALITY;
    return CreateFontIndirectW(lplf);
}

void WindowlessRichEdit::InitRichEidtDll()
{
	if (NULL == s_RichEditDll)  
	{
		s_RichEditDll = ::LoadLibrary(GetLibraryName());
		s_cfRichTextFormat = ::RegisterClipboardFormat(_T("Rich Text Format"));
		s_cfRichTextAndObjects = ::RegisterClipboardFormat(_T("RichEdit Text and Objects"));
		s_funcCreateTextServices = (pfuncCreateTextServices)GetProcAddress(s_RichEditDll, "CreateTextServices");

#if 0
        // 强制richedit创建cleartype的字体，而不是依赖于操作系统的设置
        // 否则re的字体和客户端的字体就会不一致
        PROC  pfnOrig = GetProcAddress(
                GetModuleHandle(TEXT("GDI32.dll")), 
                "CreateFontIndirectW");

        if (pfnOrig)
        {
            Util::ReplaceIATEntryInOneModule(
                    "GDI32.dll", 
                    pfnOrig, 
                    (PROC)Hook_CreateFontIndirectW, 
                    s_RichEditDll);
        }
#endif
	}
	s_refDll++;
}
void WindowlessRichEdit::ReleaseRichEidtDll()
{
	s_refDll--;
	if (0 == s_refDll)
	{
#if 0
        // 取消hook
        PROC  pfnOrig = GetProcAddress(
            GetModuleHandle(TEXT("GDI32.dll")), 
            "CreateFontIndirectW");

        if (pfnOrig)
        {
            Util::ReplaceIATEntryInOneModule(
                "GDI32.dll", 
                pfnOrig, 
                pfnOrig, 
                s_RichEditDll);
        }
#endif
		FreeLibrary(s_RichEditDll);
		s_RichEditDll = NULL;
	}
}

void  WindowlessRichEdit::SetTextFilterUint(UINT nMax)
{
    if (m_pTextFilter)
    {
        if (m_pTextFilter->GetType() == TFT_UINT)
        {
            static_cast<UIntTextFilter*>(m_pTextFilter)->SetMinMax(nMax);
            return;
        }

        m_pTextFilter->Release(true);
        m_pTextFilter = NULL;
    }
    UIntTextFilter* p = UIntTextFilter::CreateInstance(m_pIRichEdit);
    p->SetMinMax(nMax);

    m_pTextFilter = static_cast<ITextFilter*>(p);
}
void  WindowlessRichEdit::SetTextFilterDigit()
{
    if (m_pTextFilter)
    {
        if (m_pTextFilter->GetType() == TFT_DIGIT)
            return;

        m_pTextFilter->Release(true);
        m_pTextFilter = NULL;
    }
   DigitTextFilter* p = DigitTextFilter::CreateInstance(m_pIRichEdit);
    m_pTextFilter = static_cast<ITextFilter*>(p);
}

void  WindowlessRichEdit::SetTextFilterPhoneNumber()
{
    if (m_pTextFilter)
    {
        if (m_pTextFilter->GetType() == TFT_PHONENUMBER)
            return;

        m_pTextFilter->Release(true);
        m_pTextFilter = NULL;
    }
    PhoneNumberFilter* p = PhoneNumberFilter::CreateInstance((UI::IRichEdit*)m_pIRichEdit);
    m_pTextFilter = static_cast<ITextFilter*>(p);
}

void  WindowlessRichEdit::SetTextFilterAccount()
{
    if (m_pTextFilter)
    {
        if (m_pTextFilter->GetType() == TFT_ACCOUNT)
            return;

        m_pTextFilter->Release(true);
        m_pTextFilter = NULL;
    }
    AccountFilter* p = AccountFilter::CreateInstance((UI::IRichEdit*)m_pIRichEdit);
    m_pTextFilter = static_cast<ITextFilter*>(p);
}

void  WindowlessRichEdit::PreDraw(HDC hDC, RECT* prcDraw)
{
	if (m_pOleMgr)
	{
		m_pOleMgr->OnPreDraw(hDC, prcDraw);
	}
}

#if 0
void  WindowlessRichEdit::DrawShadow(HDC hDC, CRect* prcClient)
{
    IUIApplication* pUIApp = m_pIRichEdit->GetUIApplication();

    HBITMAP hMemBmp = pUIApp->GetCacheBitmap(prcClient->Width(), prcClient->Height());
    HDC hMemDC = pUIApp->GetCacheDC();
    HBITMAP hOldBmp = (HBITMAP)::SelectObject(hMemDC, hMemBmp);

    // 默认所有像素都有255的alpha channel
    // TxDraw之后，所有alpha channel为0的地方都是有文字的地方
    // 然后将所有的alpha反转即可
    Util::FixAlphaData data = {0};
    data.hBitmap = hMemBmp;
    data.lprc = prcClient;
    data.eMode = Util::SET_ALPHA_255;
    data.bTopDownDib = TRUE;
    Util::FixBitmapAlpha(&data);

    m_spTextServices->TxDraw(DVASPECT_CONTENT, 0, NULL, NULL, hMemDC,
        NULL, (RECTL *)prcClient, NULL, NULL, NULL, NULL, 
        m_pIRichEdit->IsFocus()?TXTVIEW_ACTIVE:TXTVIEW_INACTIVE);

    data.eMode = Util::SET_ALPHA_INVERSE_0_255;
    Util::FixBitmapAlpha(&data);

    // 阴影颜色，简单起见，直接取一个值，懒得加接口了
    byte r = GetRValue(m_cf.crTextColor);
    byte g = GetGValue(m_cf.crTextColor);
    byte b = GetBValue(m_cf.crTextColor);
    pUIApp->ShadowBlur(hMemBmp, RGB(255-r,255-g,255-b), prcClient, 5);

    // 提交到 hDC 上
    BLENDFUNCTION bf = {AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};
    ::AlphaBlend(hDC, prcClient->left, prcClient->top, prcClient->Width(), prcClient->Height(),
        hMemDC, 0, 0, prcClient->Width(), prcClient->Height(), bf);

    SelectObject(hMemDC, hOldBmp);
    pUIApp->ReleaseCacheDC(hMemDC);
}
#endif

void  WindowlessRichEdit::RealDraw(HDC hDC, RECT* prcClient)
{
	// 注：使用TXTVIEW_INACTIVE绘制时，内部会强制保证第一行完整可见。因此与TXTVIEW_ACTIVE的视图表现不一样
	m_spTextServices->TxDraw(
		DVASPECT_CONTENT, 
		0,
		NULL,
		NULL, 
		hDC,
		NULL, 
		(RECTL *)prcClient, 
		NULL, 
		NULL, 
		NULL,
		NULL,
		TXTVIEW_ACTIVE);
}

void WindowlessRichEdit::Draw(HDC hDC, bool bDrawShadow)
{
	if(NULL == m_spTextServices || NULL == hDC)  // Remark:如果传递NULL hdc给txdraw，居然会造成内存泄露...
		return;

    if (m_bDuringTxDraw)
    {
        UIASSERT (0 && TEXT("TODO: "));
        TxInvalidateRect(NULL, TRUE);
        return;
    }

    m_bDuringTxDraw = true;
    {
	    CRect  rcClient;
	    this->TxGetClientRect(&rcClient);
        ::OffsetRect(&rcClient, -rcClient.left, -rcClient.top);

        this->PreDraw(hDC, &rcClient);
		{
			if (bDrawShadow)
			{
				UIASSERT(0);
				// DrawShadow(hDC, &rcClient);
			}
			RealDraw(hDC, &rcClient);
		}
        this->PostDraw(hDC, &rcClient); 
    }
    m_bDuringTxDraw = false;

//  ::SetRectEmpty(&m_rcInvalidate);  // 可能是其它控件局部刷新，不能清空re的脏区域
}

// handled表示richedit不处理这个消息
LRESULT WindowlessRichEdit::OnPreHandleMsg( HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam )
{
	SetMsgHandled(FALSE);
	if (NULL == m_spTextServices) 
	{
		SetMsgHandled(TRUE);
		return 0;
	}

	return 0;
}
LRESULT WindowlessRichEdit::OnPostHandleMsg( HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam )
{   
	return OnDefaultHandle(Msg, wParam, lParam);
}

LRESULT  WindowlessRichEdit::OnMouseMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if (m_pOleMgr)
    {
        BOOL bHandled = FALSE;
        LRESULT lRet = m_pOleMgr->OnWindowMessage(uMsg, wParam, lParam, bHandled);
        if (bHandled)
            return lRet;
    }

    if (uMsg == WM_MOUSEWHEEL)
    {
        if (!IsMultiLine() || !m_bScrollBarVisible)
        {
            SetMsgHandled(FALSE);
            return 0;
        }
    }

	return OnDefaultHandle(uMsg, wParam, lParam);
}

LRESULT WindowlessRichEdit::OnScroll(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if (!m_bFocus)
    {
		m_pIRichEdit->SetFocusInWindow();
    }
    return OnDefaultHandle(uMsg, wParam, lParam);
}

LRESULT  WindowlessRichEdit::OnDefaultHandle(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    LRESULT lInvokeResult = 0;
    LRESULT lRet = RESendMessage(uMsg, wParam, lParam, &lInvokeResult);

    if (S_FALSE == lInvokeResult)
    {
        SetMsgHandled(FALSE);
        return 0;
    }
    return lRet;
}

BOOL WindowlessRichEdit::OnSetCursor(HWND hWnd, UINT nHitTest, UINT message)
{
	SetMsgHandled(FALSE);
	POINT pt;
	GetCursorPos(&pt);
	::ScreenToClient(m_hParentWnd, &pt);

    if (m_pOleMgr)
    {
        BOOL bHandled = FALSE;
        LRESULT lRet = m_pOleMgr->OnWindowMessage(WM_SETCURSOR, 0, 
            MAKELPARAM(pt.x, pt.y), bHandled);
        if (bHandled)
        {
            SetMsgHandled(TRUE);
            return TRUE;
        }
    }

	RECT  rcClient;
	this->TxGetClientRect(&rcClient);

	HDC hdc = GetDC(m_hParentWnd);
	HRESULT hr = m_spTextServices->OnTxSetCursor(
		DVASPECT_CONTENT,	
		-1,
		NULL,
		NULL,
		hdc,
		NULL,
		&rcClient,
		pt.x, 
		pt.y);
	ReleaseDC(m_hParentWnd, hdc);

	if (SUCCEEDED(hr))
	{
		SetMsgHandled(TRUE);
	}
	return TRUE;
}
void WindowlessRichEdit::OnKillFocus(HWND wndFocus)
{
    KillFocus();
    SetMsgHandled(FALSE);
}

//
// TODO:
// 为了解决鼠标第一次点击RE拖动没反应的问题，
// 在LButtonDown中判断如果没有焦点则手动发送一次SETFOCUS
// 猜测可能是因为先来的WM_LBUTTONDOWN消息，发送给RE后，然后再触发WM_FUCOS消息发送给RE，导致拖拽中断。
void  WindowlessRichEdit::OnSetFocus(HWND wndOld)
{
    SetFocus();
    SetMsgHandled(FALSE);
}

// 【注】 itexthost并没有提供TxDestroyCaret的接口，这可能是它的bug。
//        RE内部在收到WM_KILLFOCUS消息时，会调用::DestroyCaret。
//
// 00 0034d570 703e7f8c USER32!DestroyCaret
// 01 0034d580 703e7f1d RICHED20!CTxtEdit::OnKillFocus+0x8a
// 02 0034d6f4 05bf0e7d RICHED20!CTxtEdit::TxSendMessage+0x1b5
//

void  WindowlessRichEdit::KillFocus()
{
    if (!m_bFocus)
        return;

    m_bFocus = false;
    m_pRichEdit->GetCaret()->DestroyCaret(m_pIRichEdit, false);

    m_spTextServices->OnTxUIDeactivate();
    RESendMessage(WM_KILLFOCUS);

    m_pIRichEdit->Invalidate();
}
void  WindowlessRichEdit::SetFocus()
{
    if (!m_bFocus)
    {
        m_bFocus = true;

        m_spTextServices->OnTxUIActivate();
        RESendMessage(WM_SETFOCUS);

		m_pIRichEdit->Invalidate();
    }
}
bool  WindowlessRichEdit::IsFocus()
{
    return m_bFocus;
}
bool  WindowlessRichEdit::IsScrollBarVisible()
{
    return m_bScrollBarVisible;
}

void  WindowlessRichEdit::OnSize(UINT nType, CSize size)
{
    SetMsgHandled(FALSE);
	if (!m_spTextServices)
		return;

	// 让windowless richedit重新设置光标的位置
	m_spTextServices->OnTxPropertyBitsChange(TXTBIT_CLIENTRECTCHANGE, TXTBIT_CLIENTRECTCHANGE); 

    if (m_pOleMgr)
    {
        BOOL bHandled = FALSE;
        m_pOleMgr->OnWindowMessage(WM_SIZE, nType, MAKELPARAM(size.cx, size.cy), bHandled);
    }
}


bool WindowlessRichEdit::HitTest(POINT pt)
{
	RECT  rcClient;
	this->TxGetClientRect(&rcClient);

	if (::PtInRect(&rcClient, pt))
	{
		return true;
	}

	return false;
}

LRESULT WindowlessRichEdit::OnKeyDown(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if (!IsFocus())
        return 0;

    if (m_pTextFilter)
    {
        TextFilterResult bHandled = TFR_NOT_HANDLED;
        m_pTextFilter->KeyFilter(wParam, bHandled);
        if (TFR_HANDLED == bHandled)
            return 0;
    }

    return OnDefaultHandle(uMsg, wParam, lParam);
}

LRESULT WindowlessRichEdit::OnChar(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if (!IsFocus())
        return 0;

    TCHAR c = wParam;
    if (m_pTextFilter)
    {
        if (wParam != VK_BACK) // 这个动作是中KEYDOWN中处理的
        {
            TextFilterResult bHandled = TFR_NOT_HANDLED;
            TCHAR cRepleace = m_pTextFilter->CharFilter(wParam, bHandled);
            if (TFR_HANDLED == bHandled)
                return 0;
            if (TFR_REPLACE_TEXT == bHandled)
                c = cRepleace;
        }
    }

  	return RESendMessage(uMsg, c, lParam);
}

bool WindowlessRichEdit::DoGifOleUpdateRequst()
{
	int nNow = GetTickCount();
	int nDiff = nNow - m_nLastTimerKickCount;
	if (nDiff < 40)
	{
		return false;
	}
	m_nLastTimerKickCount = nNow;
	return true;
}
LRESULT WindowlessRichEdit::OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	SetMsgHandled(FALSE);
	if (m_pRichEdit)
	{
        // 平滑滚动
		TimerItem*  pItem = (TimerItem*)lParam;
		if (pItem->wParam == 1)
		{
			RESendMessage(WM_TIMER, pItem->nId);
		}
	}
    
	return 0;
}


//@cmember Create a timer with the specified timeout
BOOL WindowlessRichEdit::TxSetTimer(UINT idTimer, UINT uTimeout)
{
    TimerItem item ;
    item.pNotify = m_pIRichEdit;
    item.nId = idTimer;
    item.wParam = 1;
    item.lParam = 0;
    return m_pIRichEdit->GetUIApplication()->SetTimer(uTimeout, &item);
       return ::SetTimer(m_hParentWnd, idTimer, uTimeout, NULL);
}

//@cmember Destroy a timer
void WindowlessRichEdit::TxKillTimer(UINT idTimer)
{
    m_pIRichEdit->GetUIApplication()->KillTimerById(idTimer, m_pIRichEdit);
}

//@cmember Get the view rectangle relative to the inset
// 其实就是文字与边框的padding
// 由于UI控件的padding属性是绘制
HRESULT WindowlessRichEdit::TxGetViewInset(LPRECT prc)
{
	SetRectEmpty(prc);

#if 0 // TODO: delete this
	CRegion4 rPadding;
	m_pRichEditBase->GetPaddingRegion(&rPadding);
 	::CopyRect(prc, &rPadding);

	// Convert the inset to himetric that must be returned to ITextServices

	prc->left = DXtoHimetricX(prc->left, m_nxPerInch);
	prc->top  = DYtoHimetricY(prc->top, m_nyPerInch);
	prc->right  = DXtoHimetricX(prc->right, m_nxPerInch);
	prc->bottom = DYtoHimetricY(prc->bottom, m_nyPerInch);
#endif

	return S_OK;
}

// @cmember Retrieves the coordinates of a window's client area
// 这里不能使用左上角（0，0），而是返回其在窗口上的真实坐标
// 1. 鼠标选取一段内容，拖拽时会生成一个光标指示，该光标位置依赖TxGetClientRect
// 2. 微软拼音输入法的位置，也依赖也TxGetClientRect
HRESULT WindowlessRichEdit::TxGetClientRect(LPRECT prc)
{
	//m_pIRichEdit->GetClientRectAsWin32(prc);
    m_pIRichEdit->GetClientRectInWindow(prc);
	
#if 0
	// 减去padding的大小，因为padding的已经在richedit中作为inset view rect来实现
	CRegion4 rPadding;
	m_pRichEditBase->GetPaddingRegion(&rPadding);
	prc->left -= rPadding.left;
	prc->top  -= rPadding.top;
	prc->right  += rPadding.right;
	prc->bottom += rPadding.bottom;
#endif

	return S_OK;
}



void  WindowlessRichEdit::CaretInvalidateRect(RECT* prc, int nCount)
{
    if (!m_pRichEdit)
        return;

    m_pRichEdit->InvalidateRect2(prc, nCount);
}

// prc为窗口坐标。与TxGetClientRect一致
void  WindowlessRichEdit::InvalidateRect(RECT* prcWnd)
{
    if (!m_pRichEdit)
        return;

    if (m_bDuringTxDraw)  // windows 10上开启autoresize后，末尾接空格时，出现无限循环。。。
        return;

    m_pRichEdit->InvalidateRect(prcWnd);
}

//@cmember InvalidateRect
// 注：prc可能只是一行的区域大小，仅刷新该区域的话能提高绘制效率！
void /*ITextHostImpl*/WindowlessRichEdit::TxInvalidateRect(LPCRECT prc, BOOL fMode)
{
    if (m_pRichEdit)
    {
        // 单行模式的拖拽选中会触发prc为空
        InvalidateRect((RECT*)prc);
    }

}

//@cmember Get the background (either opaque or transparent)
// 注：设置TXTBACK_TRANSPARENT后，richedit将变成背景透明
//     不设置或设置其它值后，将会有白色背景
HRESULT /*ITextHostImpl*/WindowlessRichEdit::TxGetBackStyle(TXTBACKSTYLE *pstyle)
{
	// 默认使用RichEdit backrender的背景，便于统一扩展
// 	if (m_pRichEdit->IsTransparent())
 		*pstyle = TXTBACK_TRANSPARENT;
// 	else
// 		*pstyle = TXTBACK_OPAQUE;

	return S_OK;
}



//@cmember Show the scroll bar
BOOL WindowlessRichEdit::TxShowScrollBar(INT fnBar, BOOL fShow) 
{
    IScrollBarManager*  pScrollBarMgr = m_pRichEdit->GetScrollMgr();
    if (NULL == pScrollBarMgr)
        return FALSE;

	if (FALSE == fShow)
	{
		if (fnBar == SB_HORZ || fnBar == SB_BOTH)
		{
            pScrollBarMgr->SetHScrollRange(0);
		}
		if (fnBar == SB_VERT || fnBar == SB_BOTH)
		{
			//--为了解决删除文字时，删除到滚动条消失时，最上面几行仍然被隐藏的问题。此时应该将第一行显示出来
			if (0 != pScrollBarMgr->GetVScrollPos())
			{
				RESendMessage(WM_VSCROLL,MAKEWPARAM(SB_TOP,0),0);
				pScrollBarMgr->SetVScrollPos(0);
			}
			//--

			pScrollBarMgr->SetVScrollRange(0);
		}
	}

    m_bScrollBarVisible = fShow ? true:false;
    InvalidateRect(NULL);
	return TRUE;
}

//@cmember Enable the scroll bar
BOOL WindowlessRichEdit::TxEnableScrollBar (INT fuSBFlags, INT fuArrowflags)
{
	InvalidateRect(NULL);
	return TRUE;
}

//@cmember Set the scroll range
// PS: 这里RICHEDIT好像有一个BUG.在改变RICHEDIT大小时，RICHEDIT发出来TxShowScrollbar，TxSetScrollPos
//     的通知，但是居然没有发出TxSetScrollRagne的通知，导致滚动条不知如何显示。
//     (QQ2012 Input Edit也有这个问题，但Output Edit能正常显示。难道和READONLY有关系？）
BOOL WindowlessRichEdit::TxSetScrollRange(
									 INT fnBar,
									 LONG nMinPos,
									 INT nMaxPos, 
									 BOOL fRedraw)   // 这个fRedraw好像都是false 
{
    IScrollBarManager*  pScrollBarMgr = m_pRichEdit->GetScrollMgr();
    if (NULL == pScrollBarMgr)
        return FALSE;

	if (SB_HORZ == fnBar)
	{
		pScrollBarMgr->SetHScrollRange(nMaxPos - nMinPos);
	}
	else if (SB_VERT == fnBar)
	{
		pScrollBarMgr->SetVScrollRange(nMaxPos - nMinPos);
	}

    InvalidateRect(NULL);
	return TRUE;
}


void WindowlessRichEdit::SyncScrollPos()
{
    if (!m_spTextServices)
        return;

    IScrollBarManager*  pScrollBarMgr = m_pRichEdit->GetScrollMgr();
    if (NULL == pScrollBarMgr)
        return;

    LONG lMin,lMax,lPos,lPage;	BOOL fEnabled;
    HRESULT hr = m_spTextServices->TxGetVScroll(&lMin,&lMax,&lPos,&lPage,&fEnabled);
// 
// 
//     UISCROLLINFO info;
//     info.nMask = UISIF_RANGE | UISIF_POS | UISIF_PAGE;
//     info.nPos = lPos;
//     info.nPage = lPage;
//     info.nRange = lMax - lMin;
//     pScrollBarMgr->SetVScrollInfo(&info);
    RESendMessage(WM_VSCROLL, MAKEWPARAM(SB_THUMBTRACK, lPos), 0);
    RESendMessage(WM_VSCROLL, SB_ENDSCROLL);
}

//
//@cmember Set the scroll position
// 在输入内容的最后面敲入一个回车，结果会响应两次TxSetScrollPos，并且第一次的nPos是旧值，并且两次fRedraw都为1
// 最后导致滚动条会闪烁一次，不知道为何要这样。
//
// 为了规避这个问题，在这里做了一些处理：
//  1. 在这里不去根据fRedraw参数来刷新滚动条，而是在txinvalidateect刷新控件时同时刷新滚动条
//  2. 在TxInvalidateRect时，不能立即去刷新，否则也将导致滚动条闪烁。在这里通过模拟windows系统的行为，post一个
//     UI_WM_REDRAWOBJECT消息给richedit控件，由此来实现延迟刷新。
//  3. 在RichEditBase::OnRedrawObject中，检测是否需要刷新滚动条
//
BOOL WindowlessRichEdit::TxSetScrollPos (INT fnBar, INT nPos, BOOL fRedraw)   
{
    IScrollBarManager*  pScrollBarMgr = m_pRichEdit->GetScrollMgr();
    if (NULL == pScrollBarMgr)
        return FALSE;

	if (SB_HORZ == fnBar)
	{
        pScrollBarMgr->SetHScrollPos(nPos);
	}
	else if (SB_VERT == fnBar)
	{
	    pScrollBarMgr->SetVScrollPos(nPos);
	}

    InvalidateRect(NULL);
	return FALSE;
}


//////////////////////////////////////////////////////////////////////////

HRESULT STDMETHODCALLTYPE WindowlessRichEdit::QueryInterface(REFIID riid,void **ppvObject)
{
	if (NULL == ppvObject)
		return E_INVALIDARG;

	if (::IsEqualIID(IID_IUnknown,riid) || ::IsEqualIID(IID_ITextHost,riid))
	{
		*ppvObject = static_cast<ITextHost*>(this);
	}
	else if (::IsEqualIID(riid, IID_IRichEditOleCallback))
	{
		*ppvObject = static_cast<IRichEditOleCallback*>(this);
	}
	else
		return E_NOINTERFACE;
	
	this->AddRef();
	return S_OK;
}
ULONG STDMETHODCALLTYPE WindowlessRichEdit::AddRef(void)
{
	return 1;
}
ULONG STDMETHODCALLTYPE WindowlessRichEdit::Release(void)
{
	return 0;
}

//////////////////////////////////////////////////////////////////////////
//
//               ITempHostImpl 
//
//////////////////////////////////////////////////////////////////////////

//@cmember Get the DC for the host
HDC ITextHostImpl::TxGetDC() 
{
    if (m_hDCCache)
        return m_hDCCache;

    m_hDCCache = ::GetDC(m_hParentWnd);
    return m_hDCCache;
}

//@cmember Release the DC gotten from the host
INT ITextHostImpl::TxReleaseDC(HDC hdc)
{
    return 1;
    return ::ReleaseDC (m_hParentWnd, hdc);
}

//@cmember Send a WM_PAINT to the window
void ITextHostImpl::TxViewChange(BOOL fUpdate)
{
	// TODO: 在update richedit控件时，可能触发TxViewChange，导致又进入WindowBase::OnPaint
	//       rendertarget被二次begindraw

//	::UpdateWindow (m_hParentWnd);
}

//
//@cmember Create the caret
// hbmp [in] 
//      Handle to the bitmap that defines the caret shape.
//      If this parameter is NULL, the caret is solid. 
//      If this parameter is (HBITMAP) 1, the caret is gray. 
//      If this parameter is a bitmap handle, the caret is the specified bitmap. 
//      The bitmap handle must have been created by the CreateBitmap, 
//      CreateDIBitmap, or LoadBitmap function. 
//
//      If hBitmap is a bitmap handle, CreateCaret ignores 
//      the nWidth and nHeight parameters; the bitmap 
//      defines its own width and height.
//
BOOL WindowlessRichEdit::TxCreateCaret(HBITMAP hbmp, INT xWidth, INT yHeight)
{
    // return ::CreateCaret(m_hParentWnd, hbmp, xWidth, yHeight);
    if (!m_bFocus)
        return FALSE;

    // 在只读时，如果没有文本就不显示光标，以免形成可以输入的假像
    if (m_dwStyle & ES_READONLY)  
    {
        if (0 == RESendMessage(WM_GETTEXTLENGTH, 0, 0))
            return TRUE;
    }

    return m_pRichEdit->GetCaret()->CreateCaret(m_pIRichEdit, hbmp, xWidth, yHeight, 
        CARET_TYPE_UNKNOWN, static_cast<ICaretUpdateCallback*>(this));
}

//@cmember Show the caret
BOOL WindowlessRichEdit::TxShowCaret(BOOL fShow) 
{
    // return fShow?::ShowCaret(m_hParentWnd) : ::HideCaret(m_hParentWnd);

    // 如果是TxDraw发出来的消息，则不能刷新
    // 在输入过程中，光标跟随文字一起后移，需要显示出来(刷新)
	if (fShow)
		m_pRichEdit->GetCaret()->ShowCaret(m_pIRichEdit, !m_bDuringTxDraw);
	else
		m_pRichEdit->GetCaret()->HideCaret(m_pIRichEdit, !m_bDuringTxDraw);

	return TRUE;
}

//@cmember Set the caret position
BOOL WindowlessRichEdit::TxSetCaretPos(INT x, INT y)
{
    // return ::SetCaretPos(x, y);

    // 有可能是鼠标点击，导致位置修改，这个时候要刷新光标。但如果是TxDraw产生的消息，则不能刷新
    POINT pt = {x, y};
	m_pRichEdit->GetCaret()->SetCaretPos(m_pIRichEdit, pt.x, pt.y, !m_bDuringTxDraw);
	return TRUE;
}

//@cmember Scroll the content of the specified window's client area
void ITextHostImpl::TxScrollWindowEx (
	INT dx,
	INT dy,
	LPCRECT lprcScroll,
	LPCRECT lprcClip,
	HRGN hrgnUpdate,
	LPRECT lprcUpdate,
	UINT fuScroll)
{
	::ScrollWindowEx(m_hParentWnd, dx, dy, lprcScroll, lprcClip, hrgnUpdate, lprcUpdate, fuScroll);
}

//@cmember Get mouse capture
void ITextHostImpl::TxSetCapture(BOOL fCapture)
{
	if (fCapture)
		::SetCapture(m_hParentWnd);
	else
		::ReleaseCapture();
}

//@cmember Set the focus to the text window
void WindowlessRichEdit::TxSetFocus() 
{
	//::SetFocus(m_hParentWnd);
    SetFocus();
}

//@cmember Establish a new cursor shape
// 该接口并不仅仅设置text的ibean光标，还包括sel bar的指针
// If the fText parameter is TRUE, the text services object 
// is trying to set the text cursor (the cursor appears as 
// an I-beam when it is over text that is not selected). In 
// this case, the host can set it to whatever the control 
// MousePointer property is. 
void ITextHostImpl::TxSetCursor(HCURSOR hcur, BOOL fText)
{
	::SetCursor(hcur);
}

//@cmember Converts screen coordinates of a specified point to the client coordinates
BOOL ITextHostImpl::TxScreenToClient (LPPOINT lppt)
{
	return ::ScreenToClient(m_hParentWnd, lppt);	
}

//@cmember Converts the client coordinates of a specified point to screen coordinates
BOOL ITextHostImpl::TxClientToScreen (LPPOINT lppt) 
{
	return ::ClientToScreen(m_hParentWnd, lppt);
}

//@cmember Request host to activate text services
HRESULT ITextHostImpl::TxActivate( LONG * plOldState )
{
	return S_OK;
}

//@cmember Request host to deactivate text services
HRESULT ITextHostImpl::TxDeactivate( LONG lNewState )
{
	return S_OK;
}

// 由派生类实现
//@cmember Retrieves the coordinates of a window's client area
// HRESULT ITextHostImpl::TxGetClientRect(LPRECT prc)
// {
// 	*prc = m_rectInWindow;
// 	return S_OK;
// }

//@cmember Get the default character format for the text
HRESULT ITextHostImpl::TxGetCharFormat(const CHARFORMATW **ppCF )
{
	*ppCF = &m_cf;
	return S_OK;
}

//@cmember Get the default paragraph format for the text
HRESULT ITextHostImpl::TxGetParaFormat(const PARAFORMAT **ppPF)
{
	*ppPF = &m_pf;
	return S_OK;
}

//@cmember Get the background color for the window
COLORREF ITextHostImpl::TxGetSysColor(int nIndex) 
{
	return ::GetSysColor(nIndex);
}

//@cmember Get the maximum length for the text
HRESULT ITextHostImpl::TxGetMaxLength(DWORD *plength)
{
	*plength = m_dwMaxLength;
	return S_OK;
}

//@cmember Get the bits representing requested scroll bars for the window
HRESULT ITextHostImpl::TxGetScrollBars(DWORD *pdwScrollBar)
{
	*pdwScrollBar =  m_dwStyle & (WS_VSCROLL | WS_HSCROLL
		 | ES_AUTOVSCROLL | ES_AUTOHSCROLL | ES_DISABLENOSCROLL);

	return S_OK;
}

//@cmember Get the character to display for password input
HRESULT ITextHostImpl::TxGetPasswordChar(TCHAR *pch)
{
#ifdef UNICODE
	*pch = m_chPasswordChar;
#else
	WideCharToMultiByte(CP_ACP, 0, &m_chPasswordChar, 1, pch, 1, NULL, NULL) ;
#endif
	return NOERROR;
}

//@cmember Get the accelerator character
HRESULT ITextHostImpl::TxGetAcceleratorPos(LONG *pcp) 
{
	*pcp = m_laccelpos;
	return S_OK;
}

//@cmember Get the native size
HRESULT ITextHostImpl::TxGetExtent(LPSIZEL lpExtent)
{
	return E_NOTIMPL;
}

//@cmember Notify host that default character format has changed
HRESULT ITextHostImpl::OnTxCharFormatChange (const CHARFORMATW * pcf)
{
	// TODO:
	return S_OK;
}

//@cmember Notify host that default paragraph format has changed
HRESULT ITextHostImpl::OnTxParaFormatChange (const PARAFORMAT * ppf)
{
	// TODO:
	return S_OK;
}

//@cmember Bulk access to bit properties
HRESULT ITextHostImpl::TxGetPropertyBits(DWORD dwMask, DWORD *pdwBits)
{
	DWORD dwProperties = 0;

	if (m_fRich)
	{
		dwProperties = TXTBIT_RICHTEXT;
	}

	if (m_dwStyle & ES_MULTILINE)
	{
		dwProperties |= TXTBIT_MULTILINE;
	}

	if (m_dwStyle & ES_READONLY)
	{
		dwProperties |= TXTBIT_READONLY;
	}

	if (m_dwStyle & ES_PASSWORD)
	{
		dwProperties |= TXTBIT_USEPASSWORD;
	}

	if (!(m_dwStyle & ES_NOHIDESEL))
	{
		dwProperties |= TXTBIT_HIDESELECTION;
	}

// 	if (fEnableAutoWordSel)
// 	{
// 		dwProperties |= TXTBIT_AUTOWORDSEL;
// 	}
// 
// 	if (fVertical)
// 	{
// 		dwProperties |= TXTBIT_VERTICAL;
// 	}
 
	if (m_fWordWrap)
	{
		dwProperties |= TXTBIT_WORDWRAP;
	}
 
// 	if (fAllowBeep)
// 	{
// 		dwProperties |= TXTBIT_ALLOWBEEP;
// 	}
// 
// 	if (fSaveSelection)
// 	{
// 		dwProperties |= TXTBIT_SAVESELECTION;
// 	}
    if (!m_fShowSelectionInactive)
    {
        dwProperties |= TXTBIT_HIDESELECTION;
    }

    if (m_fDisableDrag)
    {
        dwProperties |= TXTBIT_DISABLEDRAG;
    }
	*pdwBits = dwProperties & dwMask; 
	return NOERROR;
}

// Far East Methods for getting the Input Context
//#ifdef WIN95_IME
// 调用堆栈：
//  窗口接收到WM_IME_STARTCOMPOSITION消息后，转发给richedit
//  
// UICtrls.dll!UI::ITextHostImpl::TxImmGetContext()  行983	C++
// riched20.dll!CTxtEdit::GetImmContext()  + 0x2e 字节	
// riched20.dll!LocalGetImmContext()  + 0x1b 字节	
// riched20.dll!CIme::SetCompositionFont()  + 0x28 字节	
// riched20.dll!CIme_Lev2::StartComposition()  + 0x1a 字节	
// riched20.dll!StartCompositionGlue()  + 0x15d 字节	
// riched20.dll!CTextMsgFilter::HandleMessage()  + 0x3b6 字节	
// riched20.dll!CTxtEdit::TxSendMessage()  + 0x1d4f 字节	
HIMC ITextHostImpl::TxImmGetContext()
{
    return ImmGetContext(m_hParentWnd);
}
void ITextHostImpl::TxImmReleaseContext(HIMC himc)
{
    ImmReleaseContext(m_hParentWnd, himc);
}
//#endif

//@cmember Returns HIMETRIC size of the control bar.
// 注：如果不设置该变量，会导致cursor显示异常，显示成为一个行选择样式（箭头朝向右上角）
//	   像VS的一个功能
//
// 从google上找到的有关该设置的信息：
// Re: What ES_SELECTIONBAR option Does for a RichEdit 
//		ES_SELECTIONBAR is a new rich edit control style. it is really confusing since 
//		it has been defined with the same value as WS_MAXIMIZE, therefore it cannot be 
//		used as style passed to Create.
//
//		ES_SELECTIONBAR is equivalent to ECO_SELECTIONBAR that (I think) should replace 
//		confusing ES_SELECTIONBAR. 
//
//		ECO_SELECTIONBAR can be used as parameter when SetOption member is called or 
//		EM_SETOPTIONS message is sent.
//
//		When ECO_SELECTIONBAR for reach edit control is set, there is small left margin 
//		(wider than default) where cursor changes to right-up arrow allowing full line(s)
//		selection. 

HRESULT ITextHostImpl::TxGetSelectionBarWidth (LONG *lSelBarWidth)
{
	*lSelBarWidth = m_lSelBarWidth;
	return S_OK;
}

//////////////////////////////////////////////////////////////////////////
//
//               ITempHostImpl 其它设置方法
//
//////////////////////////////////////////////////////////////////////////

ITextHostImpl::ITextHostImpl()
{
	m_nxPerInch = m_nyPerInch = 96;
	m_hParentWnd = NULL;
	m_dwStyle = ES_MULTILINE|ES_NOHIDESEL|WS_VSCROLL | /*WS_HSCROLL |*/ ES_AUTOVSCROLL /*| ES_AUTOHSCROLL | ES_DISABLENOSCROLL*/;
	m_dwMaxLength = INFINITE;
	m_lSelBarWidth = 0;

	m_chPasswordChar = L'*';
	m_laccelpos = -1;
	m_fWordWrap = false;
	m_fRich = true;
	m_fRegisteredForDrop = false;
	m_fPassword = false;
    m_fShowSelectionInactive = false;
    m_fDisableDrag = false;
    m_hDCCache = NULL;

	memset(&m_cf, 0, sizeof(CHARFORMAT2W));
    m_cf.cbSize = sizeof(CHARFORMAT2W);
    m_cf.dwMask = CFM_ALL;
    
    _tcscpy(m_cf.szFaceName, _T("宋体"));
	InitDefaultParaFormat();

	if (!(m_dwStyle & ES_LEFT))
	{
		if (m_dwStyle & ES_CENTER)
			m_pf.wAlignment = PFA_CENTER;
		else if (m_dwStyle & ES_RIGHT)
			m_pf.wAlignment = PFA_RIGHT;
	}
}

ITextHostImpl::~ITextHostImpl()
{
    if (m_hDCCache)
    {
        ReleaseDC(m_hParentWnd, m_hDCCache);
        m_hDCCache = NULL;
    }
}
void  ITextHostImpl::Destroy()
{
	if (m_spOle)
	{
		m_spOle.Release();
	}
    if (m_spTextDoc)
    {
        m_spTextDoc.Release();
    }
	if (m_spTextServices)
	{
		m_spTextServices->OnTxInPlaceDeactivate();
		m_spTextServices.Release();
		m_spTextServices = NULL;
	}
}

bool ITextHostImpl::SetCharFormatByLogFont(LOGFONT* plf, COLORREF color)
{
    if (NULL == plf)
        return false;

    HWND hWnd = GetDesktopWindow();
    HDC  hDC = GetDC(hWnd);
    LONG yPixPerInch = GetDeviceCaps(hDC, LOGPIXELSY);
    m_cf.yHeight = abs(plf->lfHeight * 1440 / yPixPerInch);
    ReleaseDC(hWnd, hDC);

    if (plf->lfWeight >= FW_BOLD)
        m_cf.dwEffects |= CFE_BOLD;
    else
        m_cf.dwEffects &= ~CFE_BOLD;

    if (plf->lfItalic)
        m_cf.dwEffects |= CFE_ITALIC;
    else
        m_cf.dwEffects &= ~CFE_ITALIC;

    if (plf->lfUnderline)
        m_cf.dwEffects |= CFE_UNDERLINE;
    else
        m_cf.dwEffects &= ~CFE_UNDERLINE;

    if (plf->lfStrikeOut)
        m_cf.dwEffects |= CFE_STRIKEOUT;
    else
        m_cf.dwEffects &= ~CFE_STRIKEOUT;
        
    m_cf.crTextColor = color;
    m_cf.bCharSet = plf->lfCharSet;
    m_cf.bPitchAndFamily = plf->lfPitchAndFamily;
#ifdef UNICODE
    _tcscpy(m_cf.szFaceName, plf->lfFaceName);
#else
    //need to thunk pcf->szFaceName to a standard char string.in this case it's easy because our thunk is also our copy
    MultiByteToWideChar(CP_ACP, 0, plf->lfFaceName, LF_FACESIZE, m_cf.szFaceName, LF_FACESIZE) ;
#endif

    if (m_spTextServices)
    {
        m_spTextServices->OnTxPropertyBitsChange(TXTBIT_CHARFORMATCHANGE, 
            TXTBIT_CHARFORMATCHANGE);
    }

    return true;
}
bool ITextHostImpl::SetCharFormat(CHARFORMAT2* pcf)
{
	if (NULL == pcf)
		return false;

	// Set CHARFORMAT structure
    if (pcf->dwMask & CFM_SIZE)
        m_cf.yHeight = pcf->yHeight;

    if (pcf->dwMask & CFM_COLOR)
    {
        m_cf.crTextColor = pcf->crTextColor; 
    }
    if (pcf->dwMask & CFM_BACKCOLOR)
    {
        m_cf.crBackColor = pcf->crBackColor; 
    }

    if (pcf->dwMask & CFM_BOLD)
    {
        if (pcf->dwEffects & CFE_BOLD)
            m_cf.dwEffects |= CFE_BOLD;
        else
            m_cf.dwEffects &= ~CFE_BOLD;
    }
    if (pcf->dwMask & CFM_ITALIC)
    {
        if (pcf->dwEffects & CFE_ITALIC)
            m_cf.dwEffects |= CFE_ITALIC;
        else
            m_cf.dwEffects &= ~CFE_ITALIC;
    }
    if (pcf->dwMask & CFM_UNDERLINE)
    {
        if (pcf->dwEffects & CFM_UNDERLINE)
            m_cf.dwEffects |= CFM_UNDERLINE;
        else
            m_cf.dwEffects &= ~CFM_UNDERLINE;
    }

    if (pcf->dwMask & CFM_FACE)
    {
        _tcscpy(m_cf.szFaceName, pcf->szFaceName);
    }
// 	m_cf.dwEffects = CFM_EFFECTS | CFE_AUTOBACKCOLOR;
// 	m_cf.dwEffects &= ~(CFE_PROTECTED | CFE_LINK | CFE_AUTOCOLOR);

//  RESendMessage(EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM)&m_cf);

    if (m_spTextServices)
    {
        m_spTextServices->OnTxPropertyBitsChange(TXTBIT_CHARFORMATCHANGE, 
            TXTBIT_CHARFORMATCHANGE);
    }
	return true;
}

//
// SCF_ALL
// Applies the formatting to all text in the control. Not valid with SCF_SELECTION or SCF_WORD.
// SCF_SELECTION
// Applies the formatting to the current selection. If the selection is empty, the character formatting is applied to the insertion point, and the new character format is in effect only until the insertion point changes.
// SCF_WORD
// Applies the formatting to the selected word or words. If the selection is empty but the insertion point is inside a word, the formatting is applied to the word. The SCF_WORD value must be used in conjunction with the SCF_SELECTION value.
// SCF_ASSOCIATEFONT
// RichEdit 4.1: Associates a font to a given script, thus changing the default font for that script. To specify the font, use the following members of CHARFORMAT2: yHeight, bCharSet, bPitchAndFamily, szFaceName, and lcid.
// SCF_ASSOCIATEFONT2
// RichEdit 4.1: Associates a surrogate (plane-2) font to a given script, thus changing the default font for that script. To specify the font, use the following members of CHARFORMAT2: yHeight, bCharSet, bPitchAndFamily, szFaceName, and lcid.
// SCF_DEFAULT
// RichEdit 4.1: Sets the default font for the control. 
// SCF_NOKBUPDATE
// RichEdit 4.1: Prevents keyboard switching to match the font. For example, if an Arabic font is set, normally the autokeyboard feature for Bidi languages changes the keyboard to an Arabic keyboard.
// SCF_USEUIRULES
// RichEdit 4.1: Used with SCF_SELECTION. Indicates that format came from a toolbar or other UI tool, so UI formatting rules should be used instead of literal formatting.
//
void  ITextHostImpl::SetCharFormatEx(UINT nPart, CHARFORMAT2* pcf)
{
	RESendMessage(EM_SETCHARFORMAT, (WPARAM)nPart, (LPARAM)pcf);
}
void  ITextHostImpl::SetSelectionCharFormat(CHARFORMAT2* pcf)
{
    SetCharFormatEx(SCF_SELECTION, pcf);
}
void  ITextHostImpl::ResetSelectionCharFormat()
{
    SetSelectionCharFormat(&m_cf);
}
void  ITextHostImpl::ResetSelectionParaFormat()
{
    SetSelectionParaFormat(&m_pf);
}
void  ITextHostImpl::GetDefaultCharFormat(CHARFORMAT2* pcf)
{
    if (NULL == pcf)
        return;

    if (pcf->dwMask & CFM_SIZE)
        pcf->yHeight = m_cf.yHeight;

    if (m_cf.dwMask & CFM_COLOR)
        pcf->crTextColor = m_cf.crTextColor; 
    if (m_cf.dwMask & CFM_BACKCOLOR)
        pcf->crBackColor = m_cf.crBackColor; 

    if (pcf->dwMask & CFM_BOLD)
    {
        if (m_cf.dwEffects & CFE_BOLD)
            pcf->dwEffects |= CFE_BOLD;
    }
    if (pcf->dwMask & CFM_ITALIC)
    {
        if (m_cf.dwEffects & CFE_ITALIC)
            pcf->dwEffects |= CFE_ITALIC;
    }
    if (pcf->dwMask & CFM_UNDERLINE)
    {
        if (m_cf.dwEffects & CFM_UNDERLINE)
            pcf->dwEffects |= CFM_UNDERLINE;
    }

    if (pcf->dwMask & CFM_FACE)
    {
        _tcscpy(pcf->szFaceName, m_cf.szFaceName);
    }
}

void  ITextHostImpl::CopyDefaultCharFormat(CHARFORMAT2* pcf)
{
    if (!pcf)
        return;

    int nSize = min(pcf->cbSize, m_cf.cbSize);
    memcpy(pcf, &m_cf, nSize);
}

void  ITextHostImpl::CopyDefaultParaFormat(PARAFORMAT2* ppf)
{
    if (!ppf)
        return;

    int nSize = min(ppf->cbSize, m_pf.cbSize);
    memcpy(ppf, &m_pf, nSize);
}

void  ITextHostImpl::SetDefaultParaFormat(PARAFORMAT2* ppf)
{   
    if (!ppf)
        return;

    memcpy(&m_pf, ppf, sizeof(m_pf));

    if (m_spTextServices)
    {
        m_spTextServices->OnTxPropertyBitsChange(
            TXTBIT_PARAFORMATCHANGE, 
            TXTBIT_PARAFORMATCHANGE);
    }
}
void ITextHostImpl::InitDefaultParaFormat() 
{	
	memset(&m_pf, 0, sizeof(m_pf));

	m_pf.cbSize = sizeof(m_pf);
	m_pf.dwMask = (PFM_ALL2&~PFM_EFFECTS);  // 加了PFM_EFFECTS会导致设置para失败，这个effect是做什么的？
	m_pf.wAlignment = PFA_LEFT;

    m_pf.cTabCount = 1;
    m_pf.rgxTabs[0] = lDefaultTab;
}

void  ITextHostImpl::SetDefaultParaAlign(long eAlign/* = PFA_LEFT*/)
{
    if (m_pf.wAlignment == (WORD)eAlign)
        return;

    m_pf.wAlignment = (WORD)eAlign;
    if (m_spTextServices)
    {
        m_spTextServices->OnTxPropertyBitsChange(
            TXTBIT_PARAFORMATCHANGE, 
            TXTBIT_PARAFORMATCHANGE);
    }
}

void ITextHostImpl::SetSelectionParaFormat(PARAFORMAT2* ppf)
{
    // The EM_SETPARAFORMAT message sets the paragraph formatting
    // for the current selection in a rich edit control.

    RESendMessage(EM_SETPARAFORMAT, 0, (LPARAM)ppf);
}

WCHAR ITextHostImpl::SetPasswordChar(WCHAR ch_PasswordChar)
{
	WCHAR chOldPasswordChar = m_chPasswordChar;

	m_chPasswordChar = ch_PasswordChar;

	// notify text services of property change
	if (m_spTextServices)
	{
		m_spTextServices->OnTxPropertyBitsChange(TXTBIT_USEPASSWORD, 
			(m_chPasswordChar != 0) ? TXTBIT_USEPASSWORD : 0);
	}

	return chOldPasswordChar;
}

LONG ITextHostImpl::SetAccelPos(LONG l_accelpos)
{
	LONG laccelposOld = l_accelpos;

	m_laccelpos = l_accelpos;

	// notify text services of property change
	if (m_spTextServices)
	{
		m_spTextServices->OnTxPropertyBitsChange(TXTBIT_SHOWACCELERATOR, 0);
	}

	return laccelposOld;
}

void ITextHostImpl::SetWordWrap(bool fWordWrap)
{
    if (m_fWordWrap == (int)fWordWrap)
        return;

	m_fWordWrap = fWordWrap;
	if (m_spTextServices)
	{
		m_spTextServices->OnTxPropertyBitsChange(TXTBIT_WORDWRAP, fWordWrap ? TXTBIT_WORDWRAP : 0);
	}
}


void WindowlessRichEdit::SetReadOnly(bool fReadOnly)
{
    if (IsReadOnly() == fReadOnly)
        return;

	DWORD dwUpdatedBits = 0;

	if (fReadOnly)
	{
		m_dwStyle |= ES_READONLY;

		// Turn off Drag Drop 
		RevokeDragDrop();
		dwUpdatedBits |= TXTBIT_READONLY;
	}
	else
	{
		m_dwStyle &= ~ES_READONLY;

		// Turn drag drop back on
		RegisterDragDrop();	
	}

	// Notify control of property change
	if (m_spTextServices)
	{
		m_spTextServices->OnTxPropertyBitsChange(TXTBIT_READONLY, dwUpdatedBits);
	}
}

// 轻量级设置，用于气泡在调用SetIndents时，没有写权限的问题（readonly）
void  WindowlessRichEdit::SetReadOnlyLight(bool fReadOnly)
{
    DWORD dwUpdatedBits = 0;

	if (fReadOnly)
	{
		m_dwStyle |= ES_READONLY;
		dwUpdatedBits |= TXTBIT_READONLY;
	}
	else
	{
		m_dwStyle &= ~ES_READONLY;
	}

	// Notify control of property change
	if (m_spTextServices)
	{
		m_spTextServices->OnTxPropertyBitsChange(TXTBIT_READONLY, dwUpdatedBits);
	}
}

void  ITextHostImpl::SetMultiLine(bool b)
{
    if (IsMultiLine() == b)
        return;

    if (b)
    {
        m_dwStyle |= ES_MULTILINE; 
        m_dwStyle &= ~ES_AUTOHSCROLL;
    }
    else
    {
        m_dwStyle  &= ~ES_MULTILINE;  // 为单行时，需要能够在输满时自动滚动
        m_dwStyle |= ES_AUTOHSCROLL;
    }

    // Notify control of property change
    if (m_spTextServices)
    {
        m_spTextServices->OnTxPropertyBitsChange(TXTBIT_MULTILINE, b ? TXTBIT_MULTILINE:0);
    }
}
bool  ITextHostImpl::IsMultiLine()
{
    return (m_dwStyle & ES_MULTILINE)?true:false;
}

LRESULT ITextHostImpl::GetLangOptions()
{
    return RESendMessage(EM_GETLANGOPTIONS, 0, 0);
}

bool ITextHostImpl::IsDualFont()
{
    LRESULT lr = GetLangOptions();
    return lr & (IMF_AUTOFONT | IMF_AUTOFONTSIZEADJUST | IMF_DUALFONT) ? true:false;
}

// DWORD dwOption = RESendMessage(EM_GETLANGOPTIONS);
// dwOption |= IMF_AUTOKEYBOARD|IMF_DUALFONT;
// dwOption &= ~IMF_AUTOFONT;
// RESendMessage(EM_SETLANGOPTIONS, 0, (LPARAM)&dwOption);
void ITextHostImpl::SetDualFont(bool useDualFont)
{
    if (useDualFont)
    {
        RESendMessage(EM_SETLANGOPTIONS, 0, IMF_AUTOFONT | IMF_DUALFONT);
    }
    else
    {
        RESendMessage(EM_SETLANGOPTIONS, 0, IMF_UIFONTS);
    }
}

void ITextHostImpl::SetMaxLegnth(DWORD dw)
{
	m_dwMaxLength = dw;
	
	if (m_spTextServices)
	{
		m_spTextServices->OnTxPropertyBitsChange(TXTBIT_MAXLENGTHCHANGE, 
			TXTBIT_MAXLENGTHCHANGE);
	}
}


// HIMETRIC units per inch (used for conversion)
#define HIMETRIC_PER_INCH 2540
// Convert Himetric along the X axis to X pixels
LONG HimetricXtoDX(LONG xHimetric, LONG xPerInch) 
{	
	if (-1 == xPerInch)
	{
		HDC hDC = GetDC(NULL);
		xPerInch = GetDeviceCaps(hDC, LOGPIXELSX); 
		ReleaseDC(NULL, hDC);
	}
	return (LONG) MulDiv(xHimetric, xPerInch, HIMETRIC_PER_INCH);
}

// Convert Himetric along the Y axis to Y pixels
LONG HimetricYtoDY(LONG yHimetric, LONG yPerInch) 
{	
	if (-1 == yPerInch)
	{
		HDC hDC = GetDC(NULL);
		yPerInch = GetDeviceCaps(hDC, LOGPIXELSY); 
		ReleaseDC(NULL, hDC);
	}
	return (LONG) MulDiv(yHimetric, yPerInch, HIMETRIC_PER_INCH);
}
// Convert Pixels on the X axis to Himetric
LONG DXtoHimetricX(LONG dx, LONG xPerInch) 
{ 
	if (-1 == xPerInch)
	{
		HDC hDC = GetDC(NULL);
		xPerInch = GetDeviceCaps(hDC, LOGPIXELSX); 
		ReleaseDC(NULL, hDC);
	}
	return (LONG) MulDiv(dx, HIMETRIC_PER_INCH, xPerInch); 
}
// Convert Pixels on the Y axis to Himetric
LONG DYtoHimetricY(LONG dy, LONG yPerInch) 
{ 
	if (-1 == yPerInch)
	{
		HDC hDC = GetDC(NULL);
		yPerInch = GetDeviceCaps(hDC, LOGPIXELSY); 
		ReleaseDC(NULL, hDC);
	}
	return (LONG) MulDiv(dy, HIMETRIC_PER_INCH, yPerInch); 
}

LONG ITextHostImpl::GetSelBarWidth()
{
    if (m_lSelBarWidth)
	    return HimetricXtoDX(m_lSelBarWidth, m_nxPerInch);
    else
        return 0;
}

LONG ITextHostImpl::SetSelBarWidth(LONG l_SelBarWidth)
{
	LONG lOldSelBarWidth = m_lSelBarWidth;

	m_lSelBarWidth = DXtoHimetricX(l_SelBarWidth, m_nxPerInch);

	if (m_lSelBarWidth)
	{
		m_dwStyle |= ES_SELECTIONBAR;
	}
	else
	{
		m_dwStyle &= (~ES_SELECTIONBAR);
	}

	if (m_spTextServices)
	{
		m_spTextServices->OnTxPropertyBitsChange(TXTBIT_SELBARCHANGE, TXTBIT_SELBARCHANGE);
	}
	return lOldSelBarWidth;
} 

void WindowlessRichEdit::SetRichTextFlag(bool b)
{
	m_fRich = b;
    if (m_fRich)
        RegisterDragDrop();
    else
        RevokeDragDrop();

	if (m_spTextServices)
	{
		m_spTextServices->OnTxPropertyBitsChange(TXTBIT_RICHTEXT, 
			b ? TXTBIT_RICHTEXT : 0);
	}
}


void WindowlessRichEdit::RevokeDragDrop(void)
{
    if (m_pIRichEdit)
    {
        m_pRichEdit->RevokeDropEvent();
        m_fRegisteredForDrop = false;
    }

#if 0  // 独占窗口模式
	if (m_fRegisteredForDrop)
	{
		::RevokeDragDrop(m_hParentWnd);
		m_fRegisteredForDrop = FALSE;
	}
#endif

}
bool  WindowlessRichEdit::CanDragDrop()
{
    return m_fRegisteredForDrop?true:false;
}

//
// 备注：
// Applications that use the following functionality must call OleInitialize 
// before calling any other function in the COM library: 
//		Clipboard
//		Drag and drop
//		Object linking and embedding (OLE)
//		In-place activation
// 用oleInitialize初始化COM库，而不是使用CoInitialize
//
void WindowlessRichEdit::RegisterDragDrop(void)
{
    if (m_pIRichEdit)
    {
        m_pRichEdit->RegisterDropEvent();
        m_fRegisteredForDrop = true;
    }

#if 0  // 独占窗口模式
	IDropTarget *pdt = NULL;

	if(!m_fRegisteredForDrop && m_spTextServices->TxGetDropTarget(&pdt) == NOERROR)
	{
		HRESULT hr = ::RegisterDragDrop(m_hParentWnd, pdt);

		if(hr == NOERROR/*|| hr == 0x80040101*/)  // 只有把自己的droptarget注册上了才算成功，最后才能反注册
		{	
			m_fRegisteredForDrop = true;
		}
		else
		{
		}

		pdt->Release();
	}
#endif
}

void  ITextHostImpl::EnableUpdate(bool b)
{
    if (m_spTextDoc)
    {
        long lCount = 0;
        if (b)
        {
            m_spTextDoc->Unfreeze(&lCount);
        }
        else
        {
            m_spTextDoc->Freeze(&lCount);
        }
    }
}
void  ITextHostImpl::EnableDrag(bool b)
{
    m_fDisableDrag = b;
}

void ITextHostImpl::SetPasswordMode(bool b)
{
	m_fPassword = b;

	if (m_spTextServices)
	{
		m_spTextServices->OnTxPropertyBitsChange(TXTBIT_USEPASSWORD, 
			b? TXTBIT_USEPASSWORD:0);
	}
}

bool WindowlessRichEdit::SetText(LPCTSTR szText)
{
	if (NULL == m_spTextServices)
		return false;

	// For RichEdit 1.0, the max text length would be reset by a settext so 
	// we follow pattern here as well.

	RESendMessage(WM_SETTEXT, 0, (LPARAM)szText);

	// Update succeeded.
    DWORD cNewText = 0;
    if (szText)
    {
        cNewText = _tcslen(szText);
    }

	// If the new text is greater than the max set the max to the new
	// text length.
	if (cNewText > m_dwMaxLength)
	{
		m_dwMaxLength = cNewText;                
	}
	return true;
}

int  WindowlessRichEdit::GetText(TCHAR* szBuf, uint nLen)
{
    if (NULL == m_spTextServices)
        return 0;

    GETTEXTEX textex = {0};
    textex.cb = (nLen+1)*sizeof(TCHAR);  // nSize需要包括末尾的\0大小
    textex.flags = GT_DEFAULT;
    textex.codepage = 1200; // unicode

    return RESendMessage(EM_GETTEXTEX, (WPARAM)&textex, (LPARAM)szBuf);
}

// 使用riched20.dll，调用EM_GETTEXTEX可以拿到WCH_EMBED，但使用msftedit.dll调用EM_GETTEXTEX却拿不到
// 使用mstfedit.dll时，应该用range去获取text??
//
// 注：向re中插入a\r\nb之后，gettext只能拿到a\nb，这个在notepad中无法换行
//     采用range/gettextex都拿不到。目前没找到解决方案
void  WindowlessRichEdit::GetText(BSTR* pbstr)
{
    if (!pbstr)
        return;

    if (!m_spTextDoc)
        return;

    int nEnd = GetTextLengthW();

    CComPtr<ITextRange> range;
    HRESULT hr = m_spTextDoc->Range(0, nEnd, &range);
    hr = range->GetText(pbstr);
}

int  WindowlessRichEdit::GetTextRange(TCHAR*  szBuf, uint nStart, uint nSize)
{
	if (!szBuf || 0 == nSize)
		return -1;

	TEXTRANGE range;
	range.chrg.cpMin = nStart;
	range.chrg.cpMax = nStart + nSize;
	range.lpstrText  = szBuf;

	return (int)RESendMessage(EM_GETTEXTRANGE, 0, (LPARAM)&range);
}
int  WindowlessRichEdit::GetTextRange(BSTR* pbstr, uint nStart, uint nSize)
{
    if (!pbstr)
        return -1;

    if (!m_spTextDoc)
        return -1;

    CComPtr<ITextRange> range;
    HRESULT hr = m_spTextDoc->Range(nStart, nStart+nSize, &range);
    if (!range)
        return -1;

    hr = range->GetText(pbstr);
    if (FAILED(hr))
        return -1;

    return SysStringLen(*pbstr);
}

int   WindowlessRichEdit::GetTextLengthW()
{
    LRESULT lr = 0;
    if (!m_spTextServices)
        return 0;

    GETTEXTLENGTHEX data = {0};
    data.flags = GTL_DEFAULT;
    data.codepage = 1200; // unicode
    
	return RESendMessage(EM_GETTEXTLENGTHEX, (WPARAM)&data, (LPARAM)0);
}

// void  WindowlessRichEdit::GetTextW(IBuffer** ppBuffer)
// {
//     int nSize = GetTextLengthW();
//     if (0 == nSize)
//         return;
// 
//     TCHAR*  szBuffer = new TCHAR[nSize];
//     GetText(szBuffer, nSize);
// 
//     UI::CreateTCHARBuffer(ppBuffer);
//     (*ppBuffer)->SetBuffer((void*)szBuffer, nSize);
// }

// DWORD CALLBACK _EditStreamCallback(DWORD_PTR dwCookie,
//                          LPBYTE pbBuff,
//                          LONG cb,
//                          LONG *pcb)
// {
// 
//     fstream f;
//     f.open(L"C:\\hack.txt", ios_base::out|ios_base::app);
//     f.seekp(0, ios_base::end);
//     f.write((char*)pbBuff, cb);
//     f.close();
// 
//     *pcb = cb;
//     return 0;
// }


DWORD CALLBACK _EditStreamCallback(DWORD_PTR dwCookie,
                                   LPBYTE pbBuff,
                                   LONG cb,
                                   LONG *pcb)
{
    char** pp = (char**)dwCookie;

    if (!pp)
    {
        *pcb = 0;
        return 0;
    }

    if (!*pp)
    {
        *pp = (char*)malloc(cb+1);
        memcpy(*pp, pbBuff, cb);
        (*pp)[cb] = 0;
    }
    else
    {
        char* p = *pp;

        int nOldLength = strlen(p);
        int nNewLength = nOldLength + cb + 1;

        void* pTemp = realloc(p, nNewLength);
        if (NULL == pTemp)
        {
            *pcb = 0;
            return 0;
        }
        p = *pp = (char*)pTemp;

        memcpy(p+nOldLength, pbBuff, cb);
        p[nNewLength-1] = 0;
    }

    *pcb = cb;
    return 0;
}

void  WindowlessRichEdit::ReleaseRtfBuffer(const char* p)
{
    if (p)
        free((void*)p);
}

const char*  WindowlessRichEdit::GetRtfBuffer(int nFlags)
{
    char* p = NULL;

    EDITSTREAM  stream = {0};
    stream.pfnCallback = _EditStreamCallback;
    stream.dwCookie = (DWORD)&p;

    if (0 == nFlags)
        nFlags = SF_RTF;

    RESendMessage(EM_STREAMOUT, nFlags, (LPARAM)&stream);
    return p;
}

// bool  WindowlessRichEdit::GetEncodeTextW(IBuffer** ppBuffer)
// {
//     if (NULL == ppBuffer)
//         return false;
// 
//     CComBSTR  bstr;
//     GetText(&bstr);
// 
//     return GetEncodeTextW(bstr, 0, ppBuffer);
// }

// 获取内容的xml编码
// bool  WindowlessRichEdit::GetEncodeTextW(LPCTSTR szBuffer, int ncpStart, IBuffer** ppBuffer)
// {
//     if (!ppBuffer || !szBuffer)
//         return false;
// 
//     REXmlEncode encode(this);
//     encode.BeginEncode();
// 
//     RETextEnum _enum(this, szBuffer, ncpStart);
//     RETextEnum::ENUMRESULT  eResult = RETextEnum::FINISH;
// 
//     while (eResult = _enum.Next())
//     {
//         if (eResult == RETextEnum::TEXT)
//         {
//             encode.AddText(_enum.GetText(), _enum.GetTextLength());
//         }
//         else if (eResult == RETextEnum::OLE)
//         {
//             REOleBase*  pOle = _enum.GetOle();
//             encode.AddREOle(pOle);
//         }
//     }
//     encode.EndEncode();
// 
//     IBuffer* pBuffer = NULL;
//     encode.GetContentBuffer(&pBuffer);
//     *ppBuffer = pBuffer;
//     return true;
// }

bool  WindowlessRichEdit::ReplaceSelectionText(LPCTSTR szText, int nSize)
{
    // EM_REPLACESEL居然只接受以null字符结尾的字符串
    if (m_spTextServices)
    {
        if (-1 == nSize || szText[nSize] == _T('\0'))
        {
            RESendMessage(EM_REPLACESEL, TRUE, (LPARAM)szText);
        }
        else
        {
            TCHAR* szTemp = new TCHAR[nSize+1];
            memcpy(szTemp, szText, nSize*sizeof(TCHAR));
            szTemp[nSize] = _T('\0');

            RESendMessage(EM_REPLACESEL, TRUE, (LPARAM)szTemp);
            SAFE_ARRAY_DELETE(szTemp);
        }
    }
    return true;
}
bool  WindowlessRichEdit::AppendText(LPCTSTR szText, int nLength)
{
    SelectEnd();
    return ReplaceSelectionText((TCHAR*)szText, nLength);
}
bool  WindowlessRichEdit::InsertText(int nPos, LPCTSTR szText, int nLength)
{
    SetSel(nPos);
    return ReplaceSelectionText((TCHAR*)szText, nLength);
}
bool  WindowlessRichEdit::InsertText(LPCTSTR szText, int nLength)
{
    return ReplaceSelectionText((TCHAR*)szText, nLength);
}

//
//	如果InsertObject时不去提供/创建storage对象,那么复制对象后粘贴操作将失败!
//
bool WindowlessRichEdit::InsertOleObject(REOleBase* pItem)
{
	if (NULL == pItem)
		return false; 

	bool       bRet = false;
	HRESULT    hr = E_FAIL;
	IOleObject*     pOleObject = NULL;
	IStorage*       pStorage = NULL;
	ILockBytes*     pLockbytes = NULL;
	LPOLECLIENTSITE pClientSite = NULL;

	do 
	{
		SCODE sc = ::CreateILockBytesOnHGlobal(NULL, TRUE, &pLockbytes);
		if (sc != S_OK)
			break;

		sc = ::StgCreateDocfileOnILockBytes(pLockbytes, 
                STGM_SHARE_EXCLUSIVE|STGM_CREATE|STGM_READWRITE,
                0, &pStorage);
		if (sc != S_OK)
			break;

		hr = m_spOle->GetClientSite(&pClientSite);
		if (FAILED(hr))
			break;

		pItem->GetOleObject(&pOleObject);
		if (NULL == pOleObject)
			break;

		REOBJECT reObj;
		ZeroMemory(&reObj, sizeof(REOBJECT));
		reObj.cbStruct = sizeof(REOBJECT);
		reObj.poleobj = pOleObject;
		reObj.polesite = pClientSite;
		reObj.dvaspect = DVASPECT_CONTENT;
		reObj.dwFlags = REO_BELOWBASELINE;
		reObj.pstg = pStorage;
		reObj.dwUser = (DWORD)pItem;
		reObj.cp = REO_CP_SELECTION;

		SIZEL  sizel = {0,0};
		reObj.sizel.cx = 0;
		reObj.sizel.cy = 0;
    
        pItem->SetOleObjectManager(m_pOleMgr);
        pItem->OnPreInsert2RE();

		// The rich edit control automatically increments the reference count for the interfaces 
		// if it holds onto them, so the caller can safely release the interfaces if they are not 
		// needed. 
		HRESULT hr = m_spOle->InsertObject(&reObj);
		if (FAILED(hr))
			break;

		OleSetContainedObject(pOleObject, TRUE);

		this->m_pOleMgr->AddOle(pItem);
        pItem->OnInsert2RE();

		bRet = true;
	} while (0);
	
	
	SAFE_RELEASE(pClientSite);
	SAFE_RELEASE(pOleObject);
	SAFE_RELEASE(pStorage);
	SAFE_RELEASE(pLockbytes);
	
	return bRet;
}

bool WindowlessRichEdit::InsertComObject(REFCLSID clsid)
{
	ComREOle* pItem = new ComREOle;
	pItem->Attach(clsid);

	bool bRet = this->InsertOleObject(pItem);

	IOleObject* p = NULL;
	pItem->GetOleObject(&p, false);
	p->Release();
	
	return bRet;
}

void  WindowlessRichEdit::Paste()
{
	RESendMessage(WM_PASTE);
}

// 检测pDataObject中的各种格式，按优先级进行插入 
// Call this function to paste the OLE item in dataobj into this rich edit document/view.
HRESULT WindowlessRichEdit::DoPaste(LPDATAOBJECT pDataObject, BOOL fReally, CLIPFORMAT* pOutClipFormat)
{
	if (NULL == pDataObject)
		return E_INVALIDARG;

	// 我们自己实现的OLE对象，如动画
	do 
	{
		FORMATETC format = {0};
		format.cfFormat = g_cfRichEditUnicodeXmlFormat;
		format.tymed = TYMED_HGLOBAL;
		format.dwAspect = DVASPECT_CONTENT;
        format.lindex = -1; // 有时设置0会报无效的结构

		if (!fReally)
		{
			if (FAILED(pDataObject->QueryGetData(&format)))
				break;
			return S_OK;
		}

		STGMEDIUM medium = {0};
		medium.tymed = TYMED_HGLOBAL;
		if (FAILED(pDataObject->GetData(&format, &medium)))
			break;

		if (TYMED_HGLOBAL != medium.tymed)
		{
			::ReleaseStgMedium(&medium);
			break;
		}

		HGLOBAL hGlobal = medium.hGlobal;
		LPWSTR lpwstr = (LPWSTR)::GlobalLock(hGlobal);
		bool bRet = this->PasteEncodeText(lpwstr);  // TODO: 如何给子类调用
		GlobalUnlock(hGlobal);
        ::ReleaseStgMedium(&medium);

		if (false == bRet)
			break;
		
		return S_OK;
	}
	while(0);

	// bitmap 类型
	do 
	{
		FORMATETC format = {0};
		format.cfFormat = CF_BITMAP;
		format.tymed = TYMED_GDI;
		format.dwAspect = DVASPECT_CONTENT;
		format.lindex = -1; // 有时设置0会报无效的结构

		if (!fReally)
		{
			if (FAILED(pDataObject->QueryGetData(&format)))
				break;
			return S_OK;
		}
		
		STGMEDIUM medium = {0};
		medium.tymed = TYMED_HGLOBAL;
		if (FAILED(pDataObject->GetData(&format, &medium)))
			break;

		if (TYMED_GDI != medium.tymed)
		{
			::ReleaseStgMedium(&medium);
			break;
		}

		static_cast<WindowlessRichEditEx*>(this)->InsertPicture(medium.hBitmap);

		::ReleaseStgMedium(&medium);
		return S_OK;

	} while (0);

	// static object类型：如CF_DIB/CF_BITMAP
	if (fReally)
	{
		m_pRichEdit->QueryPaste.emit(m_pIRichEdit, pDataObject);
	}
	return E_FAIL;


#if 0
	LPOLEOBJECT     lpOleObject = NULL;
	IStorage*       pStorage = NULL;
	ILockBytes*     pLockbytes = NULL;
	LPOLECLIENTSITE pClientSite = NULL;

	bool  bRet = false;
	do 
	{
		SCODE sc = ::CreateILockBytesOnHGlobal(NULL, TRUE, &pLockbytes);
		if (sc != S_OK)
			break;

		sc = ::StgCreateDocfileOnILockBytes(pLockbytes, STGM_SHARE_EXCLUSIVE|STGM_CREATE|STGM_READWRITE, 0, &pStorage);
		if (sc != S_OK)
			break;

		HRESULT hr = m_spOle->GetClientSite(&pClientSite);
		if (FAILED(hr))
			break;

// 		sc = ::OleCreateFromData(pDataObject, IID_IUnknown, OLERENDER_DRAW, NULL, pClientSite, pStorage, (void**)&lpOleObject);
// 		if (FAILED(sc))
		{
			// CF_METAFILEPICT, CF_DIB, or CF_ BITMAP, and CF_ENHMF
			sc = ::OleCreateStaticFromData(pDataObject, IID_IUnknown, OLERENDER_DRAW, NULL, pClientSite, pStorage, (void**)&lpOleObject);
		}

		if (sc != S_OK)
			break;

		// all items are "contained" -- this makes our reference to this object
		//  weak -- which is needed for links to embedding silent update.
		OleSetContainedObject(lpOleObject, TRUE);


		REOBJECT reObj;
		memset(&reObj, 0, sizeof(reObj));
		reObj.cbStruct = sizeof(REOBJECT);
		reObj.cp = REO_CP_SELECTION;

		reObj.poleobj = lpOleObject;
		reObj.polesite = pClientSite;
		reObj.dvaspect = DVASPECT_CONTENT;
		reObj.dwFlags = REO_BELOWBASELINE;
		reObj.pstg = pStorage;
		reObj.dwUser = 0;
		reObj.cp = REO_CP_SELECTION;

		hr = m_spOle->InsertObject(&reObj);
		if (FAILED(hr))
			break;

		bRet = true;
	} while (0);

	SAFE_RELEASE(pLockbytes);
	SAFE_RELEASE(pStorage);
	SAFE_RELEASE(pClientSite);
	if (!bRet)
	{
		SAFE_RELEASE(lpOleObject);
	}
#endif
	//return bRet ? S_OK:E_FAIL;
}

// Creation IDataObject from IDataObject (used for drag-drop, paste)
IOleObject* WindowlessRichEdit::CreateOleObjectFromData(LPDATAOBJECT pDataObject, bool bOleCreateFromDataOrOleCreateStaticFromData, OLERENDER render, CLIPFORMAT cfFormat, LPFORMATETC lpFormatEtc)
{
	LPOLEOBJECT     lpOleObject = NULL;
	IStorage*       pStorage = NULL;
	ILockBytes*     pLockbytes = NULL;
	LPOLECLIENTSITE pClientSite = NULL;

	bool  bRet = false;
	do 
	{
		SCODE sc = ::CreateILockBytesOnHGlobal(NULL, TRUE, &pLockbytes);
		if (sc != S_OK)
			break;

		sc = ::StgCreateDocfileOnILockBytes(pLockbytes, STGM_SHARE_EXCLUSIVE|STGM_CREATE|STGM_READWRITE, 0, &pStorage);
		if (sc != S_OK)
			break;

		HRESULT hr = m_spOle->GetClientSite(&pClientSite);
		if (FAILED(hr))
			break;

		if (bOleCreateFromDataOrOleCreateStaticFromData)
			sc = ::OleCreateFromData(pDataObject, IID_IUnknown, render, NULL, pClientSite, pStorage, (void**)&lpOleObject);
		else
			sc = ::OleCreateStaticFromData(pDataObject, IID_IUnknown, render, NULL, pClientSite, pStorage, (void**)&lpOleObject);

		if (sc != S_OK)
			break;

		// all items are "contained" -- this makes our reference to this object
		//  weak -- which is needed for links to embedding silent update.
		OleSetContainedObject(lpOleObject, TRUE);

		bRet = true;
	} while (0);

	SAFE_RELEASE(pLockbytes);
	SAFE_RELEASE(pStorage);
	SAFE_RELEASE(pClientSite);
	if (!bRet)
	{
		SAFE_RELEASE(lpOleObject);
	}

	return lpOleObject;
}

/*
If the selection is empty, the return value is SEL_EMPTY.
If the selection is not empty, the return value is a set of flags containing one or more of the following values.
    SEL_TEXT Text. 
    SEL_OBJECT At least one Component Object Model (COM) object. 
    SEL_MULTICHAR More than one character of text. 
    SEL_MULTIOBJECT More than one COM object. 
*/
WORD  WindowlessRichEdit::GetSelectionType()
{
    return (WORD)RESendMessage(EM_SELECTIONTYPE);
}
// int WindowlessRichEdit::GetObjectTypeByOleObject(LPOLEOBJECT*  pOleObject)
// {
// 	if (NULL == pOleObject)
// 		return OT_STATIC;
// 
// 	// check for linked object
// 	LPOLELINK lpOleLink = NULL;
// 	m_lpObject->QueryInterface(IID_IOleLink, (void**)&lpOleLink);
// 	if (lpOleLink != NULL)
// 	{
// 		lpOleLink->Release();
// 		return OT_LINK;
// 	}
// 
// 	// check for static object
// 	DWORD dwStatus;
// 	if (pOleObject->GetMiscStatus(DVASPECT_CONTENT, &dwStatus) == S_OK
// 		&& (dwStatus & OLEMISC_STATIC) == 0)
// 	{
// 		return OT_EMBEDDED;
// 	}
// 
// 	// not not link, not embedding -- must be static
// 	return OT_STATIC;
// }

REOleBase* WindowlessRichEdit::GetSelectionOle()
{
	REOBJECT reObj;
	ZeroMemory(&reObj, sizeof(REOBJECT));
	reObj.cbStruct = sizeof(REOBJECT);

	HRESULT hr = m_spOle->GetObject(REO_IOB_SELECTION, &reObj, REO_GETOBJ_NO_INTERFACES);
    if (SUCCEEDED(hr) && reObj.dwUser)
    {
        return (REOleBase*)reObj.dwUser;
    }
	return NULL;
}
REOleBase*  WindowlessRichEdit::GetOleByCharPos(uint ncp)
{
    REOBJECT reObj;
    ZeroMemory(&reObj, sizeof(REOBJECT));
    reObj.cbStruct = sizeof(REOBJECT);
    reObj.cp = ncp;

    HRESULT hr = m_spOle->GetObject(REO_IOB_USE_CP, &reObj, REO_GETOBJ_NO_INTERFACES);
    if (SUCCEEDED(hr) && reObj.dwUser)
    {
        return (REOleBase*)reObj.dwUser;
    }
    return NULL;
}

// 获取一个Ole对象在richedit中的位置
int  WindowlessRichEdit::GetOleCharPos(IOleObject* pOleObj)
{
    if (!m_spOle)
        return -1;

    int  nOleCount = m_spOle->GetObjectCount();

    REOBJECT  reObj = {0};
    reObj.cbStruct = sizeof(reObj);
    
    for (int i = 0; i < nOleCount; i++)
    {
        m_spOle->GetObject(i, &reObj, REO_GETOBJ_POLEOBJ);
        if (pOleObj == reObj.poleobj)
        {
            SAFE_RELEASE(reObj.poleobj);
            return reObj.cp;
        }
        SAFE_RELEASE(reObj.poleobj);
    }
    return -1;
}
int  WindowlessRichEdit::GetOleCharPos(REOleBase* pOle)
{
    if (!pOle)
        return -1;

    IOleObject* pOleObject = NULL;
    pOle->GetOleObject(&pOleObject, false);
    if (!pOleObject)
        return -1;

    return GetOleCharPos(pOleObject);
}

// 获取一个对象的坐标
void  WindowlessRichEdit::GetCharRect(int ncp, RECT* prc)
{
    if (!m_spTextDoc)
        return;

    CComPtr<ITextRange>  spRangeLine;
    m_spTextDoc->Range(ncp, ncp, &spRangeLine);
    if (!spRangeLine)
        return;

    // http://technet.microsoft.com/zh-cn/hh768766(v=vs.90) 新类型定义
#define _tomClientCoord     256  // 默认获取到的是屏幕坐标， Use client coordinates instead of screen coordinates.
#define _tomAllowOffClient  512  // Allow points outside of the client area.

    long lTypeTopLeft     = _tomAllowOffClient|_tomClientCoord|tomStart|TA_TOP|TA_LEFT;
    long lTypeRightBottom = _tomAllowOffClient|_tomClientCoord|tomEnd|TA_BOTTOM|TA_RIGHT;
    spRangeLine->GetPoint(lTypeTopLeft,     &prc->left, &prc->top);
    spRangeLine->GetPoint(lTypeRightBottom, &prc->right,   &prc->bottom);
}

long  WindowlessRichEdit::GetEventMask()
{
    return RESendMessage(EM_GETEVENTMASK, 0, 0);
}

void  WindowlessRichEdit::SetEventMask(long n)
{
    RESendMessage(EM_SETEVENTMASK, 0, n);
}
void  WindowlessRichEdit::ModifyEventMask(long nAdd, long nRemove)
{
    long lOldMask = GetEventMask();
    long lNewMask = lOldMask | nAdd;
    lNewMask = lNewMask &~ nRemove;

    if (lNewMask != lOldMask)
    {
        SetEventMask(lNewMask);
    }
}

//
// 注：autoresize模式RE内部不会再通知滚动条显示了
//     另外对于单行模式，RE自适应宽度.多行模式,RE自适应高度
//
void  WindowlessRichEdit::SetAutoResize(bool b, int nMaxSize)
{
    if (b)
    {
        ModifyEventMask(ENM_REQUESTRESIZE, 0);
        m_nAutoResizeMaxSize = nMaxSize;
    }
    else
    {
        ModifyEventMask(0, ENM_REQUESTRESIZE);
        m_nAutoResizeMaxSize = 0;
    }
}


//@cmember Notify host of events
HRESULT WindowlessRichEdit::TxNotify(DWORD iNotify, void *pv)
{
    BOOL bHandled = FALSE;
    HRESULT hRes = m_pRichEdit->OnTxNotify(iNotify, pv, bHandled);
    if (bHandled)
        return hRes;

    return DefTxNotify(iNotify, pv);
}
HRESULT WindowlessRichEdit::DefTxNotify(DWORD iNotify, void* pv)
{
    switch (iNotify)
    {
    case EN_REQUESTRESIZE:
        {
            REQRESIZE* pData = (REQRESIZE*)pv;
            OnRequestResize(&pData->rc);
        }
        break;
    }

    return S_OK;
}


BOOL  IsWin8OrHigh()
{
    static long s_bool = -1;

    if (-1 == s_bool)
    {
        s_bool = 0;
        OSVERSIONINFOEX osVersionInfoEx;  
        osVersionInfoEx.dwOSVersionInfoSize = sizeof( OSVERSIONINFOEX );  
        GetVersionEx( (OSVERSIONINFO *)&osVersionInfoEx );

        // 大于6.1即认为是win8
        if (osVersionInfoEx.dwMajorVersion > 6)
            s_bool = 1;
        else if (osVersionInfoEx.dwMajorVersion == 6 && osVersionInfoEx.dwMinorVersion > 1 )
            s_bool = 1;
    }

    return s_bool;
}

void  WindowlessRichEdit::OnRequestResize(RECT* prc)
{
    int nDesiredWidth = prc->right - prc->left;
    int nDesiredHeight = prc->bottom - prc->top;

    REGION4 rcNonClient;
    m_pIRichEdit->GetNonClientRegion(&rcNonClient);

    nDesiredWidth += rcNonClient.left+rcNonClient.right;
    nDesiredHeight += rcNonClient.top+rcNonClient.bottom;

    // TODO:
    // 用于修复richedit autosize是win10下面，如果以空格结尾时，会出现死循环的问题.
    // 具体原因查不出来，只测出来将width-1可以避免。但会造成最后面的光标看不见的问题。
    // 先这么处理吧。
    if (IsWin8OrHigh())
        nDesiredWidth -= 1;

    CRect rcParent;
    m_pIRichEdit->GetParentRect(&rcParent);

    if (IsMultiLine())  // 宽度固定，高度自适应
    {
        if (m_nAutoResizeMaxSize > 0 && nDesiredHeight > m_nAutoResizeMaxSize)
            nDesiredHeight = m_nAutoResizeMaxSize;

        rcParent.bottom = rcParent.top + nDesiredHeight;
    }
    else  // 高度固定，宽度自适应
    {
        if (m_nAutoResizeMaxSize > 0 && nDesiredWidth > m_nAutoResizeMaxSize)
            nDesiredWidth = m_nAutoResizeMaxSize;

        rcParent.right = rcParent.left + nDesiredWidth;
    }

    //  这个消息就是从CTxEdit::TxDraw触发的，再刷新就死循环了
    m_pIRichEdit->SetObjectPos(&rcParent, SWP_NOREDRAW);
}

// This method must be implemented to allow cut, copy, paste, drag, 
// and drop operations of Component Object Model (COM) objects.
// 例如向richedit中随便拖入一个桌面上的图标，就会调用该函数
HRESULT __stdcall WindowlessRichEdit::GetNewStorage(LPSTORAGE FAR * lplpstg)
{
	if (NULL == lplpstg)
	{
		return E_INVALIDARG;
	}
	LPLOCKBYTES lpLockBytes = NULL;
	SCODE sc = ::CreateILockBytesOnHGlobal(NULL, TRUE, &lpLockBytes);
	if (sc != S_OK)
	{
		return E_OUTOFMEMORY;
	}

	sc = ::StgCreateDocfileOnILockBytes(lpLockBytes,
		STGM_SHARE_EXCLUSIVE|STGM_CREATE|STGM_READWRITE, 0, lplpstg);
	if (sc != S_OK)
	{
		return E_OUTOFMEMORY;
	}

	return S_OK;
}
HRESULT __stdcall WindowlessRichEdit::GetInPlaceContext(LPOLEINPLACEFRAME FAR * lplpFrame,
									LPOLEINPLACEUIWINDOW FAR * lplpDoc,
									LPOLEINPLACEFRAMEINFO lpFrameInfo)
{
	return E_NOTIMPL;
}
HRESULT __stdcall WindowlessRichEdit::ShowContainerUI(BOOL fShow)
{
	return E_NOTIMPL;
}
// 调用richedit ole的InsertObject函数时将触发这个回调函数
// 在从外部拖入一个文件到richedit时，先响应了GetNewStorage成功后，就会再调到这个接口函数
// 当返回S_OK时，这个对象将被插入，返回其它值时，对象将不会被插入
HRESULT __stdcall WindowlessRichEdit::QueryInsertObject(LPCLSID lpclsid, LPSTORAGE lpstg, LONG cp)
{
	return S_OK;
}

// 例如将richedit中的一个COM对象删除，则会调用一次该接口函数
// 例如将richedit中的一个COM对象用鼠标拖拽到另一个位置，则会调用一次该接口函数
// 该函数仅是一个通知，告诉我们有一个对象要被deleted from rich edit control;
// 这里不用调用release lpoleobj?
HRESULT __stdcall WindowlessRichEdit::DeleteObject(LPOLEOBJECT lpoleobj)
{
	return S_OK;
}

//  
//  在richedit中使用 CTRL+V粘贴、拖拽时被询问是否接受
//
// 1. 如何粘贴HTML格式？
//    在MSDN中搜索 HTML Clipboard Format
//
// 2. BOOL fReally是做什么的？
//   a. 从MSDN上看好像是用于EM_CANPASTE消息的，这个时候仅check，不去import
//   b. 在拖拽过程中直到鼠标放下前fReally都是0，放下鼠标左键时才为1
//
// 3. 如果是拖拽操作，则不能用IsClipboardFormatAvailable来检测
//
//   堆栈如下：
// >UIDLL.dll!UI::WindowlessRichEdit::QueryAcceptData(IDataObject *
//    lpdataobj=0x0030f518, unsigned short * lpcfFormat=0x002ed81c, 
//    unsigned long reco=1, int fReally=0, void * hMetaPict=0x00000000)  行1634	C++
// riched20.dll!CLightDTEngine::CanPaste()  + 0x69 字节	
// riched20.dll!CDropTarget::DragEnter()  + 0x5e 字节	
// ole32.dll!CPrivDragDrop::PrivDragDrop()  + 0x1c9 字节	
// ole32.dll!PrivDragDrop()  + 0xa3 字节	
// ole32.dll!CDragOperation::GetDropTarget()  + 0x5e 字节	
// ole32.dll!CDragOperation::UpdateTarget()  + 0x242 字节	
// ole32.dll!_DoDragDrop@16()  + 0xaa 字节	
// riched20.dll!CW32System::DoDragDrop()  + 0x33 字节	
// riched20.dll!CLightDTEngine::StartDrag()  + 0x16c 字节	
// riched20.dll!CTxtEdit::OnTxMouseMove()  + 0x1aa 字节	
// riched20.dll!CTxtEdit::TxSendMessage()  + 0x24d4 字节	
// UIDLL.dll!UI::WindowlessRichEdit::OnPostHandleMsg(HWND__ * 
//  hWnd=0x00120626, unsigned int Msg=512, unsigned int wParam=1, 
//  long lParam=28377121)  行270 + 0x39 字节	C++
//
HRESULT __stdcall WindowlessRichEdit::QueryAcceptData(
    LPDATAOBJECT lpdataobj, 
    CLIPFORMAT FAR * lpcfFormat, 
    DWORD reco,   // 是拖拽还是粘贴
    BOOL fReally, // 是查询还是执行
    HGLOBAL hMetaPict)
{
	if (!fReally) // not actually pasting TODO:
		return S_OK;

	// if direct pasting a particular native format allow it
	if (*lpcfFormat == CF_TEXT ||
        *lpcfFormat == CF_UNICODETEXT ||
		*lpcfFormat == WindowlessRichEdit::s_cfRichTextFormat ||
		*lpcfFormat == WindowlessRichEdit::s_cfRichTextAndObjects)
	{
		return S_OK;
	}

    FORMATETC format = {0};
    format.tymed = TYMED_HGLOBAL;
    format.dwAspect = DVASPECT_CONTENT;
    format.lindex = -1;

    HRESULT hr = S_OK;

	// if format is 0, then force particular formats if available
	if (*lpcfFormat == 0)
	{
#pragma region // 原文格式无法接受richedit format。这种情况下直接判断cf_text
        if (!m_fRich)
        {
            format.cfFormat = CF_UNICODETEXT;
            hr = lpdataobj->QueryGetData(&format);
            if (SUCCEEDED(hr))
            {
                // 文本过滤(只考虑UNICODETEXT先）
                if (m_pTextFilter)
                {
                    bool bHandled = false;
                    PasteTextFilter(lpdataobj, bHandled);
                    if (bHandled)
                        return S_FALSE;
                }

                *lpcfFormat = CF_UNICODETEXT;
                return S_OK;
            }

            format.cfFormat = CF_TEXT;
            hr = lpdataobj->QueryGetData(&format);
            if (SUCCEEDED(hr))
            {
                *lpcfFormat = CF_TEXT;
                return S_OK;
            }

			return S_FALSE;
        }
#pragma endregion

#if 0  // 不支持接受rtf格式，由业务逻辑自己去处理，如IM的输入区
        format.cfFormat = (CLIPFORMAT)WindowlessRichEdit::s_cfRichTextAndObjects;
		if (SUCCEEDED(lpdataobj->QueryGetData(&format))) // native avail, let richedit do as it wants
		{
			*lpcfFormat = (CLIPFORMAT)WindowlessRichEdit::s_cfRichTextAndObjects;
			return S_OK;
		}

        format.cfFormat = (CLIPFORMAT)WindowlessRichEdit::s_cfRichTextFormat;
        if (SUCCEEDED(lpdataobj->QueryGetData(&format)))
		{
			*lpcfFormat = (CLIPFORMAT)WindowlessRichEdit::s_cfRichTextFormat;
			return S_OK;
		}
#endif
	}

	// paste OLE formats
    // A return of a successful SCODE other than S_OK means that the callback either 
    // checked the data itself (if fReally is FALSE) or imported the data itself (if 
    // fReally is TRUE).
	if (SUCCEEDED(DoPaste(lpdataobj, fReally, lpcfFormat)))
		return S_FALSE;  

	// 最后检查一下文本格式
    format.cfFormat = (CLIPFORMAT)CF_UNICODETEXT;
	if (SUCCEEDED(lpdataobj->QueryGetData(&format)))
	{
		*lpcfFormat = CF_UNICODETEXT;
		return S_OK;
	}
    format.cfFormat = (CLIPFORMAT)CF_TEXT;
    if (SUCCEEDED(lpdataobj->QueryGetData(&format)))
    {
        *lpcfFormat = CF_TEXT;
        return S_OK;
    }

	return E_FAIL;
}

void WindowlessRichEdit::PasteTextFilter(LPDATAOBJECT pDataObject, bool& bHandled)
{
    bHandled = false;
    if (!m_pTextFilter)    
        return;

    FORMATETC format = {0};
    format.cfFormat = CF_UNICODETEXT;
    format.tymed = TYMED_HGLOBAL;
    format.dwAspect = DVASPECT_CONTENT;
    format.lindex = -1;

    STGMEDIUM medium = {0};
    medium.tymed = TYMED_HGLOBAL;
    if (FAILED(pDataObject->GetData(&format, &medium)))
        return;

    if (TYMED_HGLOBAL == medium.tymed)
    {
        HGLOBAL hGlobal = medium.hGlobal;
        LPWSTR lpwstr = (LPWSTR)::GlobalLock(hGlobal);

        TextFilterResult  handleresult = TFR_NOT_HANDLED;
        LPCTSTR szReplace = m_pTextFilter->StringFilter(lpwstr, handleresult);
        switch (handleresult)
        {
        case TFR_NOT_HANDLED:
            bHandled = false;
            break;

        case TFR_HANDLED:
            bHandled = true;
            break;

        case TFR_REPLACE_TEXT:
            if (szReplace)
                ReplaceSelectionText(szReplace, _tcslen(szReplace));
            break;
        }
        GlobalUnlock(hGlobal);
    }
    ::ReleaseStgMedium(&medium);
}

HRESULT __stdcall WindowlessRichEdit::ContextSensitiveHelp(BOOL fEnterMode)
{
	return E_NOTIMPL;
}
// 在richedit中使用 CTRL+C/拖拽 时被调用，获取要复制的数据
// This method returns success status. If the SCODE of the return value is E_NOTIMPL, the 
// rich edit control creates its own clipboard object. 
// If the SCODE of the return value is a failure other than E_NOTIMPL, the operation fails.
//
// 之后richedit内部将会调用OleSetClipboard, SetClipboardDataObject (将IDataObject AddRef)
//
// WCH_EMBEDDING
// 
// 注：如果SEL_OBJECT == wSelType时返回不是S_OK会造成内存pItem被AddRef，最后导致内存泄露.什么原理?/
// 貌似是因为为IDataObject放到clipboard中增加了一个引用计数。当再去复制一个其它内容时，引用计数就会正常
//
HRESULT __stdcall WindowlessRichEdit::GetClipboardData(
    CHARRANGE FAR * lpchrg, DWORD reco, LPDATAOBJECT FAR * lplpdataobj)
{
    // 获取选区文本
	int  nLength = lpchrg->cpMax - lpchrg->cpMin;
	int  nTextSize = nLength + 1;

	CComBSTR  szText;
	int  nRead = this->GetTextRange((BSTR*)&szText, lpchrg->cpMin, nLength);

	IDataObject* pDataobject = NULL;
	m_pOleMgr->CreateDataObject(&pDataobject);
	*lplpdataobj = static_cast<IDataObject*>(pDataobject);

    // 标识只选中了一个Ole对象
    bool bSelSingleOle = false;
	if (1 == nRead && szText[0] == WCH_EMBEDDING)  // 特殊处理，提供更丰富的剪贴格式
	{
        bSelSingleOle = true;
    }

    // 准备各种格式
    REXmlEncode  xml_encode(this);
    REHtmlEncode  html_encode(this);
    REUnicodeEncode  unicode_encode(this);

    xml_encode.BeginEncode();
    html_encode.BeginEncode();
    unicode_encode.BeginEncode();

    // 开始遍历
    int  ncpOffset = lpchrg->cpMin;
    RETextEnum _enum(this, szText, ncpOffset);
    RETextEnum::ENUMRESULT  eResult = RETextEnum::FINISH;

    while (eResult = _enum.Next())
    {
        if (eResult == RETextEnum::TEXT)
        {
            LPCTSTR  szText = _enum.GetText();
            int  nLength = _enum.GetTextLength();

            xml_encode.AddText(szText, nLength);
            html_encode.AddText(szText, nLength);
            unicode_encode.AddText(szText, nLength);
        }
        else if (eResult == RETextEnum::OLE)
        {
            REOleBase*  pOle = _enum.GetOle();
        
            xml_encode.AddREOle(pOle);
            html_encode.AddREOle(pOle);
            unicode_encode.AddREOle(pOle);

            if (bSelSingleOle)
            {
                pOle->GetClipboardDataEx(pDataobject);
            }
        }
    }

    // 提交
    xml_encode.EndEncode();
    html_encode.EndEncode();
    unicode_encode.EndEncode();

    xml_encode.Add2DataObject(pDataobject);
    html_encode.Add2DataObject(pDataobject);
    unicode_encode.Add2DataObject(pDataobject);

    return S_OK;
#if 0
		REOleBase* pItem = this->GetOleObjectByCharPos(lpchrg->cpMin);
		if (pItem)
		{
			if (SUCCEEDED(pItem->GetClipboardData(lpchrg, reco, *lplpdataobj)))
			{
				return S_OK;
			}
		}
	}
#endif
	// 调用默认的方法。内部堆栈为：
	// 0. COM组件的：     ATL::IOleObjectImpl<XXX>::GetClipboardData <-- 默认ATL内部没有实现
	// 1. riched20.dll的: CDataTransferObj::Create()
	// 2. riched20.dll的: CLightDTEngine::RangeToDataObject()
	// 3. riched20.dll的: CTxtEdit::GetClipboardData()
	// 4. UIDLL.dll的:    这里m_spOle->GetClipboardData
	m_spOle->GetClipboardData(lpchrg,reco,lplpdataobj);
	return S_OK;
}

// 在richedit中使用鼠标拖拽时被调用
HRESULT __stdcall WindowlessRichEdit::GetDragDropEffect(BOOL fDrag, DWORD grfKeyState, LPDWORD pdwEffect)
{
//     if (fDrag)
//         return S_OK;

    DWORD dwEffect;
    // check for force link
    if ((grfKeyState & (MK_CONTROL|MK_SHIFT)) == (MK_CONTROL|MK_SHIFT))
    {
        dwEffect = DROPEFFECT_LINK;
    }
    // check for force copy
    else if ((grfKeyState & MK_CONTROL) == MK_CONTROL)
    {
        dwEffect = DROPEFFECT_COPY;
    }
    // check for force move
    else if ((grfKeyState & MK_ALT) == MK_ALT)
    {
        dwEffect = DROPEFFECT_MOVE;
    }
    // default -- recommended action is move
    else
    {
        dwEffect = DROPEFFECT_MOVE;
    }

    if (dwEffect & *pdwEffect) // make sure allowed type
        *pdwEffect = dwEffect;

	return S_OK;
}

//#include "E:\\编程\\workingpath\\test\\richeditole\\richeditole_i.h"
#include "tom.h"

// 右击RichEdit时被调用，根据鼠标右键时，鼠标下面的对象的不同，得到的参数也不同，
// 例如在空白处右击，seltype=0, lpoleobj=NULL
// 例如在一个COM对象处右击，可能seltype=2, lpoleobj = xxx;
HRESULT __stdcall WindowlessRichEdit::GetContextMenu(WORD seltype, LPOLEOBJECT lpoleobj, CHARRANGE FAR * lpchrg, HMENU FAR * lphmenu)
{
// #ifdef _DEBUG
// 	HMENU& hMenu = *lphmenu;
// 	TCHAR szInfo[128] = _T("");
// 	_stprintf(szInfo, _T("GetContextMenu Args: seltype=%d, lpoleobj=%08x, lpchrg=%d,%d"),
// 		seltype, lpoleobj, lpchrg->cpMin, lpchrg->cpMax);
// 
// 	hMenu = CreatePopupMenu();
// 	BOOL bRet = ::AppendMenu(hMenu, MF_STRING, 10001, szInfo);
//     POINT pt;
//     GetCursorPos(&pt);
//     ::TrackPopupMenu(hMenu, TPM_RETURNCMD, pt.x, pt.y, 0, m_hParentWnd, NULL);
//     DestroyMenu(hMenu);
// #endif

    if (m_pRichEdit)
    {
        m_pRichEdit->OnContextMenu(seltype, lpoleobj, lpchrg);
    }
	return S_OK;
}

void  WindowlessRichEdit::SetSel(int nPos, int nLen)
{
    if (m_spTextServices)
    {
        CHARRANGE range = {nPos, nPos+nLen};
        RESendMessage(EM_EXSETSEL, NULL, (LPARAM)&range);
    }
}
void  WindowlessRichEdit::SetSel(int nPos)
{
    SetSel(nPos, 0);
}

void WindowlessRichEdit::SetSel2(int nStartChar, int nEndChar)
{
    SetSel(nStartChar, nEndChar - nStartChar);
}

void  WindowlessRichEdit::SelectAll()
{
    SetSel(0, -1);    
}

void  WindowlessRichEdit::SelectEnd()
{
    SetSel(-1, 0);
}
void WindowlessRichEdit::SetSel(CHARRANGE &cr)
{
    SetSel2(cr.cpMin, cr.cpMax);
}

void  WindowlessRichEdit::GetSel(int* pnPos, int* pnLen)
{
    if (m_spTextServices)
    {
        CHARRANGE range ={0, 0};
        RESendMessage(EM_EXGETSEL, NULL, (LPARAM)&range);

        if (pnPos)
            *pnPos = range.cpMin;
        if (pnLen)
            *pnLen = range.cpMax-range.cpMin;
    }
}

int WindowlessRichEdit::GetSelectionStart()
{
    int nPos = 0;
    GetSel(&nPos, 0);
    return nPos;
}

void WindowlessRichEdit::ScrollEnd()
{
    RESendMessage(WM_VSCROLL, (WPARAM)SB_BOTTOM, NULL);
}

// Gets the character index of the first character of a specified line in a multiline edit control. 
int  WindowlessRichEdit::LineIndex(int nLineNum)
{
    if (!m_spTextServices)
        return -1;

    return RESendMessage(EM_LINEINDEX, nLineNum, 0);
}

// Retrieves the client area coordinates of a specified character in an edit control.
// If the character is a line delimiter, the returned coordinates indicate a point just beyond the last visible character in the line. 
void  WindowlessRichEdit::PosFromChar(UINT ncp, POINT* pPos)
{
    if (!m_spTextServices || !pPos)
        return;

    POINTL pt = {0, 0};
    RESendMessage(EM_POSFROMCHAR, (WPARAM)&pt, ncp);
    pPos->x = pt.x;
    pPos->y = pt.y;

    // TODO: richedit 2.0是调用方式不一样
}

// Gets information about the character closest to a specified point in the client area of an edit control
int  WindowlessRichEdit::CharFromPos(POINT pt)
{
    if (!m_spTextServices)
        return -1;

    POINTL  ptl = {pt.x, pt.y};
    return (int) RESendMessage(EM_CHARFROMPOS, 0, (LPARAM)&ptl);
}

//
// Gets the index of the line that contains the specified character index in a multiline edit control. 
// A character index is the zero-based index of the character from the beginning of the edit control. 
//
// The return value is the zero-based line number of the line containing the character index specified by wParam. 
//
int   WindowlessRichEdit::LineFromChar(int ncp)
{
    if (!m_spTextServices)
        return -1;

    return (int)RESendMessage(EM_LINEFROMCHAR, ncp, 0);
}

// Retrieves the length, in characters, of a line in an edit control.
// nLineCP: 
//      The character index of a character in the line whose length is to be retrieved. 
//      If this parameter is greater than the number of characters in the control, the 
//      return value is zero. 
int WindowlessRichEdit::LineLength(int nLineCP /* = -1 */)
{
    if (!m_spTextServices)
        return -1;

    return (int) RESendMessage(EM_LINELENGTH, nLineCP, 0);
}

int   WindowlessRichEdit::GetLine(int nLineIndex, TCHAR* szBuffer, int nBufferSize)
{
    if (NULL == szBuffer)
        return -1;

    if (NULL == m_spTextServices)
        return -1;

    szBuffer[0] = (WORD)nBufferSize;
    return (int)RESendMessage(EM_GETLINE, nLineIndex, (LPARAM)szBuffer);
}

int   WindowlessRichEdit::GetLineCount()
{
    if (!m_spTextServices)
        return 0;

    return (int)RESendMessage(EM_GETLINECOUNT, 0, 0);
}
int   WindowlessRichEdit::GetFirstVisibleLine()
{
    if (!m_spTextServices)
        return 0;

    return (int)RESendMessage(EM_GETFIRSTVISIBLELINE, 0, 0);
}

void  WindowlessRichEdit::GetVisibleCharRange(int* pnFirstCp, int* pnLastCp)
{
    if (pnFirstCp)
    {
        int nFirstVisibleLine = GetFirstVisibleLine();
        *pnFirstCp = LineIndex(nFirstVisibleLine);
    }

    if (pnLastCp)
    {
        CRect  rcClient;
        TxGetClientRect(&rcClient);
        POINT  ptRightBottom = {rcClient.right, rcClient.bottom};
        *pnLastCp = CharFromPos(ptRightBottom);
    }
}

IRichEdit*   WindowlessRichEdit::GetControlPtr()
{
    return m_pIRichEdit;
}
ITextDocument2*  WindowlessRichEdit::GetDocPtr()
{
    return m_spTextDoc;
}

void  WindowlessRichEdit::SetUndoLimit(int nUndoSize)
{
    if (!m_spTextServices)
        return;

    RESendMessage(EM_SETUNDOLIMIT, (WPARAM)nUndoSize, 0);
}
void  WindowlessRichEdit::SetLimitText(int nLimit)
{
    if (!m_spTextServices)
        return;

    RESendMessage(EM_LIMITTEXT, nLimit, 0);
}

// 禁用undo操作
void  WindowlessRichEdit::DisableUndoOp()
{
    SetUndoLimit(0);
}

// 较完整的分隔符：
// const WCHAR g_szBreakChars[] = // Win2000 SourceCode Autocomp.cpp(private\shell\browseui)
BOOL IsDelimiter(TCHAR ch)
{
	return (ch == TEXT(' ')  ||
		ch == TEXT('\t') ||
		ch == TEXT('.')  ||
		ch == TEXT('/')  ||
		ch == TEXT('\\'));
}

// FastCharNext and FastCharPrev are like CharNext and CharPrev except that
// they don't check if you are at the beginning/end of the string.

#ifdef UNICODE
#define FastCharNext(pch) ((pch)+1)
#define FastCharPrev(pchStart, pch) ((pch)-1)
#else
#define FastCharNext        CharNext
#define FastCharPrev        CharPrev
#endif

// Win2000 SourceCode Comboex.c(private\shell\comctrl32) 
int FAR PASCAL ShellEditWordBreakProc(LPTSTR lpch,
							 int ichCurrent,
							 int cch,
							 int code
							 )
{
	LPTSTR lpchT = lpch + ichCurrent;
	int iIndex;
	BOOL fFoundNonDelimiter = FALSE;
	static BOOL fRight = FALSE;  // hack due to bug in USER

	switch (code) 
	{
	case WB_ISDELIMITER:
		fRight = TRUE;
		// Simple case - is the current character a delimiter?
		iIndex = (int)IsDelimiter(*lpchT);
		return iIndex;
		break;

	
	case WB_LEFT:
	case WB_LEFTBREAK:
	case WB_MOVEWORDLEFT:
		// Move to the left to find the first delimiter.  If we are
		// currently at a delimiter, then skip delimiters until we
		// find the first non-delimiter, then start from there.
		//
		// Special case for fRight - if we are currently at a delimiter
		// then just return the current word!
		while ((lpchT = CharPrev(lpch, lpchT)) != lpch)
		{
			if (IsDelimiter(*lpchT))
			{
				if (fRight || fFoundNonDelimiter)
					break;
			} 
			else
			{
				fFoundNonDelimiter = TRUE;
				fRight = FALSE;
			}
		}
		iIndex = (int) (lpchT - lpch);

		// We are currently pointing at the delimiter, next character
		// is the beginning of the next word.
		if (iIndex > 0 && iIndex < cch)
			iIndex++;

		return iIndex;
		break;

	case WB_RIGHT:
	case WB_RIGHTBREAK:
	case WB_MOVEWORDRIGHT:
		fRight = FALSE;

		// If we are not at a delimiter, then skip to the right until
		// we find the first delimiter.  If we started at a delimiter, or
		// we have just finished scanning to the first delimiter, then
		// skip all delimiters until we find the first non delimiter.
		//
		// Careful - the string passed in to us may not be NULL terminated!
		fFoundNonDelimiter = !IsDelimiter(*lpchT);
		if (lpchT != (lpch + cch)) 
		{
			while ((lpchT = FastCharNext(lpchT)) != (lpch + cch)) 
			{
				if (IsDelimiter(*lpchT)) 
				{
					fFoundNonDelimiter = FALSE;
				} 
				else 
				{
					if (!fFoundNonDelimiter)
						break;
				}
			}
		}
		// We are currently pointing at the next word.
		iIndex = (int) (lpchT - lpch);
		return iIndex;
		break;

	}
	return 0;
}

// 解决RE行尾输入空格时，无法换行的问题
// 注：该方法丢弃了RE的各种单词分隔行为，仅是不得已时使用
void  WindowlessRichEdit::EnableWordBreak(bool b)
{
	RESendMessage(EM_SETWORDBREAKPROC, 0, 
		b ? (LPARAM)ShellEditWordBreakProc:NULL);
}

LRESULT  WindowlessRichEdit::OnUIDropTargetEvent(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    DROPTARGETEVENT_TYPE  eType = (DROPTARGETEVENT_TYPE)wParam;
    DROPTARGETEVENT_DATA* pData = (DROPTARGETEVENT_DATA*)lParam;

    if (!CanDragDrop())
    {
        *(pData->pdwEffect) = DROPEFFECT_NONE;
        return 0;
    }

    LRESULT lRet = 0;
    CComPtr<IDropTarget> pdt = NULL;
    m_spTextServices->TxGetDropTarget(&pdt);

    if (pdt)
    {
        // 会触发WindowlessRichEdit::GetDragDropEffect
        switch (eType)
        {
        case _DragEnter:
            lRet = pdt->DragEnter(pData->pDataObj, pData->grfKeyState, pData->pt, pData->pdwEffect);
            break;
        case _DragOver:
            lRet = pdt->DragOver(pData->grfKeyState, pData->pt, pData->pdwEffect);
            break;
        case _Drop:
            lRet = pdt->Drop(pData->pDataObj, pData->grfKeyState, pData->pt, pData->pdwEffect);
            break;
        case _DragLeave:
            lRet = pdt->DragLeave();
            break;
        }
        return lRet;
    }
    
    return 0;
}


/*
HFONT  CRichEditWndlessBase::CreateFontByCharFormat()
{
    // 直接用整形得到的值与richedit自己算出来的不一样
    HDC hdc = GetDC(NULL);
    long nyPerInch = GetDeviceCaps(hdc, LOGPIXELSY);   // DPI
    ReleaseDC(NULL, hdc);

    float f = ((float)m_charFormat.yHeight*nyPerInch)/1440;
    int yHeight = -UI::round(f);

    HFONT hFont = CreateFont(
        yHeight,
        0, 0, 0,
        m_charFormat.dwEffects&CFE_BOLD ? FW_BOLD:FW_NORMAL,
        m_charFormat.dwEffects&CFE_ITALIC ? 1:0,
        m_charFormat.dwEffects&CFE_UNDERLINE ? 1:0,
        m_charFormat.dwEffects&CFE_STRIKEOUT ? 1:0,
        ANSI_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,
        DEFAULT_QUALITY,DEFAULT_PITCH | FF_SWISS,
        m_charFormat.szFaceName);

    return hFont;
}
*/