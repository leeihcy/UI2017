#include "stdafx.h"
#include "popupcontrolwindow.h"
#include "Inc\Interface\imenu.h"
#include "Src\Control\Menu\menu.h"
#include "..\UISDK\Inc\Interface\iuiinterface.h"
#include "..\UISDK\Inc\Interface\ilayout.h"

namespace UI
{

PopupControlWindow::PopupControlWindow():ICustomWindow(CREATE_IMPL_TRUE)
{
	m_pBindObject = NULL;
	m_pContentObj = NULL;
	m_bExitLoop = false;
    m_bMouseIn = false;
	m_hWndClickFrom = NULL;
	m_rcClickFrom.SetRectEmpty();
}

PopupControlWindow::~PopupControlWindow()
{
}

//(WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE);
// 注：如果自己的父窗口不是桌面，则不会在任务栏上面显示图标
BOOL PopupControlWindow::PreCreateWindow(CREATESTRUCT* pcs)
{
    if (NULL == pcs->hwndParent)
    {
    	pcs->dwExStyle |= WS_EX_TOPMOST|WS_EX_NOACTIVATE|WS_EX_TOOLWINDOW;
    }
    else
    {
        pcs->dwExStyle |= WS_EX_NOACTIVATE;
    }
	pcs->lpszClass = WND_POPUP_CONTROL_NAME/*WND_POPUP_CONTROL_SHADOW_NAME*/;

	// 菜单等列表窗口可能会比较小，如果有WS_CAPTION将导致窗口有最小高度限制
	pcs->style = WS_POPUP;
    return TRUE;
}

// void  PopupControlWindow::Create(
// 	IObject*  pBindObj, 
// 	IObject* pContentObj, 
// 	LPCTSTR szId, 
// 	HWND hParentWnd)
// {
//     m_pBindObject = pBindObj;
// 	m_pContentObj = pContentObj;
//     __super::Create(_T(""), hParentWnd);  // 在创建完panel的layer之后再创建窗口，避免第一次响应_OnSize中没有layout
// 
//     IMapAttribute*  pMapAttrib = NULL;
//     pBindObj->GetMapAttribute(&pMapAttrib);
//     if (pMapAttrib)
//     {
// 		if (szId)
// 			this->SetID(szId);
// 
// 		SetAttributeByPrefix(XML_POPUPWINDOW_PREFIX, pMapAttrib, false, true);
// 		SAFE_RELEASE(pMapAttrib);
// 	}
// }
 
void  PopupControlWindow::SetBindObject(IObject* pBindObj)
{
	m_pBindObject = pBindObj;
}
void  PopupControlWindow::SetContentObject(IObject* pContentObj)
{
	m_pContentObj = pContentObj;
}

void  PopupControlWindow::SetPopupFromInfo(HWND hWnd, RECT* prcClickInWnd)
{
	m_hWndClickFrom = hWnd;
	if (prcClickInWnd)
		m_rcClickFrom.CopyRect(prcClickInWnd);
	else
		m_rcClickFrom.SetRectEmpty();
}

// 2014.4.18  libo
// 增加一个bDesignMode，用于支持在UIEditor当中显示菜单 
void  PopupControlWindow::Show(
        POINT pt, 
        SIZE size, 
        BOOL bDoModal, 
        long lShowType, 
        BOOL bDesignMode)
{
    if (NULL == m_pBindObject || NULL == m_pContentObj)
        return;

    m_bExitLoop = false;

    CRect rcWorkArea;
    MONITORINFO mi = {sizeof(MONITORINFO), 0};
    if (GetMonitorInfo(MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST), &mi))
    {
        rcWorkArea = mi.rcWork;
    }
    else
    {
        ::SystemParametersInfo(SPI_GETWORKAREA, 0, &rcWorkArea, 0);
    }

    if (size.cx > rcWorkArea.Width())
        size.cx = rcWorkArea.Width();
    if (size.cy > rcWorkArea.Height())
        size.cy = rcWorkArea.Height();

    if (pt.x < rcWorkArea.left)
        pt.x = rcWorkArea.left;
    if (pt.x + size.cx > rcWorkArea.right)
        pt.x = rcWorkArea.right - size.cx;
    if (pt.y < rcWorkArea.top)
        pt.y = rcWorkArea.top;

    if (pt.y + size.cy > rcWorkArea.bottom)
	{
		if (!m_rcClickFrom.IsRectEmpty())
		{
			// 将列表显示到COMBOBOX上面，而不是盖在COMBOBOX上
			POINT ptTop = {m_rcClickFrom.left, m_rcClickFrom.top};
			if (m_hWndClickFrom)
				::MapWindowPoints(m_hWndClickFrom, NULL, &ptTop, 1);
			pt.y = ptTop.y - size.cy;
		}
		else
		{
			pt.y = rcWorkArea.bottom - size.cy;
		}
	}

    // 注：如果该窗口上一次的大小和这一次显示的大小一致，会导致窗口收不到OnSize通知，导致内部控件无法布局
    HWND hPopupWnd = GetHWND();
    CRect rcOld, rcNew;

    ::GetClientRect(hPopupWnd, &rcOld);
    ::SetWindowPos(hPopupWnd, NULL, pt.x, pt.y, size.cx, size.cy, SWP_NOZORDER|SWP_NOACTIVATE);
    ::GetClientRect(hPopupWnd, &rcNew);

    if (rcNew.Width() == rcOld.Width() && rcNew.Height() == rcOld.Height())
    {
        if (GetLayout())
            GetLayout()->Arrange(NULL);
    }

    UISendMessage(m_pContentObj, UI_WM_INITPOPUPCONTROLWINDOW, 0,0,0, this);
    if (m_pBindObject && m_pBindObject != m_pContentObj)
    {
        UISendMessage(m_pBindObject, UI_WM_INITPOPUPCONTROLWINDOW, 0,0,0, this);
    }

	if (!bDesignMode)
	{
        if (lShowType != SW_HIDE)
        {
            // 解决showwindow将窗口直接显示成alpha 255的问题
            GetLayer()->SetOpacity(10, nullptr);
            GetWindowRender()->InvalidateNow();

            ::ShowWindow(hPopupWnd, lShowType);
            start_show_animate();
        }
        else
        {
            ::ShowWindow(hPopupWnd, lShowType);
        }

		IMessageFilterMgr* pMgr = GetUIApplication()->GetMessageFilterMgr();
		pMgr->AddMessageFilter(static_cast<IPreTranslateMessage*>(this));

		if (TRUE == bDoModal)
			GetUIApplication()->MsgHandleLoop(&m_bExitLoop);
	}
}

void  PopupControlWindow::Hide()
{
    m_bExitLoop = true;

    if (!IsWindowVisible())  // 已经隐藏了
        return ;

	HideWindow();
    if (m_pContentObj)
    {
        UISendMessage(m_pContentObj, UI_WM_UNINITPOPUPCONTROLWINDOW, 0,0,0, this);  // 将该消息放在这里发送，保证每一个子菜单窗口销毁时都能收到该消息
    }
	if (m_pBindObject && m_pBindObject != m_pContentObj)
	{
		UISendMessage(m_pBindObject, UI_WM_UNINITPOPUPCONTROLWINDOW, 0,0,0, this);
	}

    // 也许listbox弹出了提示条
    if (m_pContentObj)
        m_pContentObj->GetUIApplication()->HideToolTip();

    // 如下拉列表隐藏时，并不销毁，但此时要干掉pretranslate
    IMessageFilterMgr* pMgr = GetUIApplication()->GetMessageFilterMgr();
    if (pMgr)
        pMgr->RemoveMessageFilter(static_cast<IPreTranslateMessage*>(this));
}

void  PopupControlWindow::Destroy()
{
    ::DestroyWindow(GetHWND());
}

void PopupControlWindow::OnInitialize()
{
    __super::nvProcessMessage(GetCurMsg(), 0, 0);
    SetWindowResizeType(WRSB_NONE);
	EnableDwmTransition(false);

//     if (m_pContentObj)
//     {
// 	    m_pContentObj->AddHook(this, 0, 1);
//     }
}

void PopupControlWindow::OnDestroy()
{
	SetMsgHandled(FALSE);

    IMessageFilterMgr* pMgr = GetUIApplication()->GetMessageFilterMgr();
    if (pMgr)
        pMgr->RemoveMessageFilter(static_cast<IPreTranslateMessage*>(this));

    if (m_pBindObject && m_pBindObject != m_pContentObj)
    {
        UISendMessage(m_pBindObject, UI_WM_POPUPWINDOWDESTROY);
    }

	// 菜单在自己的析构函数中，从窗口的树结构中移除了。可以放心销毁窗口
}


// 注：为什么从alpha=10开始动画，而不是0开始？
//     例如从edit中右键弹菜单前，会先将光标重置为idc_arrow，但是由于一开始
//     窗口的透明度被设置为0，导致鼠标消息还是走到了edit中，光标又被修改为
//     idc_ibean。因此这里初始将透明度设置10.（测试设置为1不行）
void  PopupControlWindow::start_show_animate()
{
	LayerAnimateParam param = { 0 };
	param.bBlock = false;
	GetLayer()->SetOpacity(10, nullptr);
	GetLayer()->SetOpacity(255, &param);
}

// Popup控件不参与换肤，目前
void  PopupControlWindow::OnSkinChanging(BOOL* pbChange)
{
    *pbChange = FALSE;
}

// 在鼠标点击本窗口的时候，不要去抢占主窗口的焦点
// 但是这样同时导致了鼠标消息和键盘消息获取不进来了
// WM_NCHITTEST能收到
int PopupControlWindow::OnMouseActivate(HWND wndTopLevel, UINT nHitTest, UINT message)
{
	return MA_NOACTIVATE;
}
// 在某些时间要求菜单是抢焦点的，例如弹在一个跨进程的CEF上面。只能拿到自己的killfocus消息
void PopupControlWindow::OnKillFocus(IObject* pNewFocusObj)
{
    SetMsgHandled(FALSE);
    this->Hide();
}

// 这个消息是用于针对鼠标点击了桌面/其它顶层窗口的时候，
// 将窗口隐藏的通知。对于点击了自己的顶层窗口却处理不了
void PopupControlWindow::OnActivateApp(BOOL bActive, DWORD dwThreadID)
{
	SetMsgHandled(FALSE);
	if (FALSE == bActive)
	{
		this->Hide();
	}
}



// 注：
//	这里的GetMessage其实是只能拦截PostMessage发送过来的消息，
//  对于其它SendMessage的消息是不能得知的。
//

BOOL PopupControlWindow::PreTranslateMessage(MSG* pMsg)
{
	if ((WM_KEYFIRST <= pMsg->message && WM_KEYLAST >= pMsg->message) || WM_MOUSEWHEEL == pMsg->message)
	{
		if (WM_KEYDOWN == pMsg->message && VK_ESCAPE == pMsg->wParam)
		{
			this->Hide();
			return TRUE;
		}

		BOOL bHandle = FALSE;
		UISendMessage(this->m_pContentObj, pMsg->message, pMsg->wParam, pMsg->lParam, 0, 0, 0, &bHandle);
		//return bHandle;
        return TRUE;
	}
	
	if (WM_MOUSEMOVE == pMsg->message ||
		WM_NCMOUSEMOVE == pMsg->message )
	{
		if (pMsg->hwnd != GetHWND())
		{
            if (m_bMouseIn)
            {
                ::PostMessage(GetHWND(), WM_MOUSELEAVE, 0, 0);
                m_bMouseIn = false;
            }
			return TRUE;  // 窗口外的鼠标移动事件忽略
		}
		else
		{
            m_bMouseIn = true;
			return FALSE;
		}
	}
	else if (WM_LBUTTONDOWN == pMsg->message || WM_LBUTTONDBLCLK == pMsg->message)
	{
		// 单独将鼠标点击抽出来进一步判断。有可能该处是弹出菜单的地方，
		// 直接关闭菜单会导致菜单被关闭后又立刻显示出来菜单 

        //         RECT rcWindow;
        //         ::GetWindowRect(m_hWnd, &rcWindow);
        // 
        //         DWORD dwPos = GetMessagePos();
        //         POINT pt = {GET_X_LPARAM(dwPos), GET_Y_LPARAM(dwPos)};
        //         if (!PtInRect(&rcWindow, pt))  
        // 注：popupwindow可能是分层窗口，带有透明区域（如圆角），
        // 所以不能使用PtInRect进行判断
        if (pMsg->hwnd != GetHWND()) 
		{
			this->Hide();

			// 给原窗口发送一个鼠标移动消息，重置hover对象。
			// 否则会导致popupwnd消失后，原窗口鼠标直接点击无反应
			// 或者导致窗口接收到lbuttondown之前，还没有更新hover对象
			::SendMessage(pMsg->hwnd, WM_MOUSEMOVE, 0, pMsg->lParam);

			// 鼠标在弹出窗口外面点击了，关闭当前窗口。如果点在了弹出来的按钮上面，则直接关闭，不再转发消息
			if (m_hWndClickFrom && m_hWndClickFrom == pMsg->hwnd)
			{
				POINT ptWnd = {GET_X_LPARAM(pMsg->lParam), GET_Y_LPARAM(pMsg->lParam)};
				if (PtInRect(&m_rcClickFrom, ptWnd))
					return TRUE;
			}

			return FALSE/*TRUE*/;
		}
	}
	else if (
		WM_RBUTTONDOWN   == pMsg->message ||
		WM_RBUTTONDBLCLK == pMsg->message ||
		WM_MBUTTONDOWN   == pMsg->message ||
		WM_MBUTTONDBLCLK == pMsg->message ||
		WM_XBUTTONDOWN   == pMsg->message ||
		WM_XBUTTONDBLCLK == pMsg->message ||
		WM_NCLBUTTONDOWN == pMsg->message ||
		WM_NCRBUTTONDOWN == pMsg->message ||
        WM_NCRBUTTONDBLCLK == pMsg->message || 
        /*WM_GESTURE*/0x0119 == pMsg->message ||
        /*WM_TOUCH*/0x0240 == pMsg->message)
	{
        if (pMsg->hwnd != GetHWND())
        {
            this->Hide();

            // 给原窗口发送一个鼠标移动消息，重置hover对象。
            // 否则会导致popupwnd消失后，原窗口鼠标直接点击无反应
			// 或者导致窗口接收到lbuttondown之前，还没有更新hover对象
			::SendMessage(pMsg->hwnd, WM_MOUSEMOVE, 0, pMsg->lParam);

            return FALSE/*TRUE*/;
		}
	}
    else if (pMsg->message == WM_CLOSE && pMsg->hwnd != GetHWND())
    {
        // 关闭了父窗口，自己也得关掉。否则拦截器会变成野指针
        if (pMsg->hwnd == m_hWndClickFrom ||
            pMsg->hwnd == GetWindow(GetHWND(), GW_OWNER))
        {
            Hide();
        }
    }

	return FALSE;
}

PopupListBoxWindow::PopupListBoxWindow()
{
}

BOOL PopupListBoxWindow::PreTranslateMessage(MSG* pMsg)
{
	if (WM_KEYFIRST <= pMsg->message && WM_KEYLAST >= pMsg->message)
	{
		if (pMsg->message != WM_KEYDOWN)
			return FALSE;

		UINT nChar = pMsg->wParam;
		if (VK_ESCAPE == nChar)
		{
			this->Hide();
			return TRUE;
		}

		// 只处理一些基本按键，其它键盘消息还是传递给当前FOCUS COMBOBOX'S EDIT
		if (IsKeyDown(VK_SHIFT))  // SHIFT + xxx 由EDIT处理
			return FALSE;

		if (nChar == VK_UP    ||
			nChar == VK_DOWN  ||
			nChar == VK_HOME  ||
			nChar == VK_END   ||
			nChar == VK_PRIOR ||
			nChar == VK_NEXT  ||
            nChar == VK_RETURN)
		{
			BOOL bHandle = FALSE;
			UISendMessage(this->m_pContentObj, pMsg->message, pMsg->wParam, pMsg->lParam, 0, 0, 0, &bHandle);
			return TRUE;/*bHandle*/; // 有可能传给了其它控件
		}

		return FALSE;
	}

	return __super::PreTranslateMessage(pMsg);
}

void PopupListBoxWindow::OnInitialize()
{
	__super::OnInitialize();

	// 清除原窗口上面的hover、press对象
	HWND hWnd = GetActiveWindow();
	::PostMessage(hWnd,WM_MOUSELEAVE,0,0);
}

void PopupListBoxWindow::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	SetMsgHandled(FALSE);

	if (VK_DOWN == nChar || VK_UP == nChar) 
	{
		SetMsgHandled(TRUE);
		m_pContentObj->ProcessMessage(GetCurMsg(), 0);
	}
	return;
}

//////////////////////////////////////////////////////////////////////////
PopupMenuWindow::PopupMenuWindow()
{
	m_pMenu = NULL;
	m_ptLastMousePos.x = m_ptLastMousePos.y = 0;
    m_pLastHoverMenu = NULL;
}

// 菜单没有父窗口，直接置顶
BOOL PopupMenuWindow::PreCreateWindow(CREATESTRUCT* pcs)
{
	__super::PreCreateWindow(pcs);
	pcs->dwExStyle |= WS_EX_TOPMOST;  

	return TRUE;
}

void  PopupMenuWindow::SetContentObject(IObject* pContentObj)
{
	__super::SetContentObject(pContentObj);

    if (m_pContentObj)
    {
        IMenu* p = (IMenu*)m_pContentObj->QueryInterface(__uuidof(IMenu));
        if (p)
            m_pMenu = p->GetImpl();
    }

    if (m_pMenu)
    {
	    // 清除原窗口上面的hover、press对象.这样也能使原窗口releasecaption（在mouseleave中）
        // 因此可以直接在按钮的lbuttonup消息中弹出菜单了

        // 子菜单弹出来时,activeWindow是父菜单，因此不需要再发送mouseleave消息
	    if (m_pMenu->GetRootMenu() == m_pMenu) 
	    {
		    HWND hWnd = GetActiveWindow();
		    ::PostMessage(hWnd,WM_MOUSELEAVE,0,0);
	    }
    }
}


//
//	注：只有根菜单才会有MenuLoop，子菜单没有
//
//  REMARK: 模态菜单弹出期间，所在窗口直接被销毁导致菜单返回崩溃问题：
//  这种情况下可以采用让窗口在销毁之前先POSTMESSAGE出来，在这里被拦截
//  后提前关闭菜单。
//
BOOL PopupMenuWindow::PreTranslateMessage(MSG* pMsg)
{
    if (NULL == m_pMenu)
        return FALSE;

    HWND hWnd = GetHWND();

	if ((WM_KEYFIRST <= pMsg->message && WM_KEYLAST >= pMsg->message) 
        || WM_MOUSEWHEEL == pMsg->message  // 菜单不抢焦点，但得阻止内容控件(richedit)滚动
        )
	{
		if (WM_SYSKEYDOWN == pMsg->message && pMsg->wParam == VK_MENU)
		{
			this->Hide();   // 按下ALT键时，销毁整个菜单。(ESC只销毁last menu)
			return TRUE;
		}

		BOOL bHandle = FALSE;
        Menu* pMenu = m_pMenu->GetLastMenu();
        if (pMenu)
    		UISendMessage(pMenu->GetIMenu(), pMsg->message, pMsg->wParam, pMsg->lParam, 0, 0, 0, &bHandle);

		return bHandle;
	}

	if (WM_MOUSEMOVE == pMsg->message ||
		WM_NCMOUSEMOVE == pMsg->message )
	{
        // 屏蔽菜单外的鼠标消息（仿系统）,但当鼠标离开菜单的时候，得补发一个MOUSELEAVE事件...(TODO: 是不是我哪搞错了)
        Menu*  pMenuHover = m_pMenu->GetMenuByHWND(pMsg->hwnd);
		if (NULL == pMenuHover)
		{
            if (m_pLastHoverMenu)
            {
                ::PostMessage(m_pLastHoverMenu->GetIMenu()->GetHWND(), WM_MOUSELEAVE, 0, 0);
            }
            m_pLastHoverMenu = NULL;
			return TRUE;  // 窗口外的鼠标移动事件忽略
		}
		else
		{
            m_pLastHoverMenu = pMenuHover;
			POINT ptNow = {LOWORD(pMsg->lParam), HIWORD(pMsg->lParam)};
			if (ptNow.x == m_ptLastMousePos.x && ptNow.y == m_ptLastMousePos.y)  // 可能是由于子菜单的显示发出来的MOUSEMOVE，忽略
			{
				//UI_LOG_DEBUG(_T("point repeat"));
				return TRUE;
			}

			m_ptLastMousePos = ptNow;
			return FALSE;
		}
	}
	else if (
        WM_LBUTTONDOWN   == pMsg->message ||
        WM_LBUTTONUP     == pMsg->message ||
		WM_LBUTTONDBLCLK == pMsg->message ||
		WM_RBUTTONDOWN   == pMsg->message ||
        WM_RBUTTONUP     == pMsg->message ||  // TMD，不过滤这个消息，可能导致外部又去弹了一次菜单，但这一次的菜单状态还没干掉
		WM_RBUTTONDBLCLK == pMsg->message ||
		WM_MBUTTONDOWN   == pMsg->message ||
        WM_MBUTTONUP     == pMsg->message ||
		WM_MBUTTONDBLCLK == pMsg->message ||
		WM_XBUTTONDOWN   == pMsg->message ||
		WM_XBUTTONDBLCLK == pMsg->message ||
		WM_NCLBUTTONDOWN == pMsg->message ||
		WM_NCRBUTTONDOWN == pMsg->message ||
        WM_NCLBUTTONUP   == pMsg->message ||
        WM_NCRBUTTONUP   == pMsg->message ||
		WM_NCRBUTTONDBLCLK == pMsg->message)
	{
		RECT rcWindow;
		::GetWindowRect(GetHWND(), &rcWindow);
        Menu* pHitMenu = m_pMenu->GetMenuByPos(pMsg->pt);

        // 鼠标在弹出窗口外面点击了，关闭当前窗口
		if (NULL == pHitMenu)  
		{
			this->Hide();

			// 给原窗口发送一个鼠标移动消息，重置hover对象。
			// 否则会导致popupwnd消失后，原窗口鼠标直接点击无反应
			// 或者导致窗口接收到lbuttondown之前，还没有更新hover对象
			
			HWND hWnd = m_hWndClickFrom;  // GetActiveWindow(); 不能用activewindow，activewindow拿到的是顶级窗口，而原窗口可能是子窗口
			::SendMessage(hWnd, WM_MOUSEMOVE, 0, pMsg->lParam);
            
            // 给当前鼠标点击的窗口发送一个鼠标消息，用于解决在弹出菜单后，
            // 直接点击另一个窗口上的按钮会没有反应
            if (pMsg->hwnd != hWnd)
                ::SendMessage(pMsg->hwnd, WM_MOUSEMOVE, 0, pMsg->lParam);

			// 鼠标在弹出窗口外面点击了，关闭当前窗口。如果点在了弹出来的按钮上面，则直接关闭，不再转发消息
			if (m_hWndClickFrom && m_hWndClickFrom == pMsg->hwnd)
			{
				POINT ptWnd = {GET_X_LPARAM(pMsg->lParam), GET_Y_LPARAM(pMsg->lParam)};
				if (PtInRect(&m_rcClickFrom, ptWnd))
					return TRUE;
			}

            return FALSE;
		}
	}

	return FALSE;
}
}