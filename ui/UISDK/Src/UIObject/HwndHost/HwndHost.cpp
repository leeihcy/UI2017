#include "stdafx.h"
#include "HwndHost.h"
#include "Src\UIObject\Window\windowbase.h"
#include "hwndhost_desc.h"
#include "Inc\Interface\imapattr.h"

HwndHost::HwndHost(IHwndHost* p):Control(p)
{
    m_pIHwndHost = p;
	m_hWnd = NULL;
	m_oldWndProc = NULL;

	OBJSTYLE s = {0};
	s.default_reject_all_mouse_msg = 1;  // 默认不接收鼠标消息
	this->ModifyObjectStyle(&s, 0);

	this->SetDescription(HwndHostDescription::Get());
}
HwndHost::~HwndHost()
{
	if (m_hWnd)
	{
		Detach();
	}
	UIASSERT ( NULL == m_oldWndProc );
}

// 
//	子类化窗口，替换它的窗口过程
//
//	Return
//		成功返回false，失败返回false
//
bool HwndHost::SubclassWindow()
{
	UIASSERT(m_hWnd);
	UIASSERT(!m_oldWndProc);

	this->m_thunk.Init( &HwndHost::_WndProc, this );
	WNDPROC pProc = this->m_thunk.GetWNDPROC();
	this->m_oldWndProc = (WNDPROC)(LONG_PTR) ::SetWindowLong( m_hWnd, GWLP_WNDPROC, (LONG)(LONG_PTR)pProc);

	this->OnSubclassWindow();

	return true;
}

// 
//	取消子类化窗口，恢复它的窗口过程
//
//	Return
//		成功返回false，失败返回false
//
bool HwndHost::UnSubclassWindow()
{
	UIASSERT(m_hWnd);
	UIASSERT(m_oldWndProc);
	::SetWindowLong(m_hWnd, GWLP_WNDPROC, (LONG)(LONG_PTR)m_oldWndProc);
	m_oldWndProc = NULL;

	this->OnUnsubclassWindow();
	return true;
}

void HwndHost::OnSubclassWindow()
{
    UpdateTabStopStyle();
}

void HwndHost::UpdateTabStopStyle()
{
    long lStyle = ::GetWindowLong(m_hWnd, GWL_STYLE);
    if (lStyle & WS_TABSTOP)
    {
		m_objStyle.tabstop = 1;
    }
    else
    {
		m_objStyle.tabstop = 0;
    }
}
void HwndHost::OnUnsubclassWindow()
{
}

void  HwndHost::OnSerialize(SERIALIZEDATA* pData)
{
	__super::OnSerialize(pData);

    if (pData->IsLoad())
    {
	    HWND hParentWnd = this->GetHWND();

        int nControlID = 0;
        if (MAPATTR_RET_NOT_EXIST != pData->pMapAttrib->GetAttr_int(XML_HWNDHOST_CONTROL_ID, true, &nControlID))
	    {
		    HWND hWnd = ::GetDlgItem(hParentWnd, nControlID);
		    if (NULL == hWnd)
		    {
			    UI_LOG_WARN(_T("HwndHost::SetAttribute Failed. id=%s, GetDlgItem(%d)"), m_strId.c_str(), nControlID);
			    return ;
		    }

		    Attach(hWnd);
	    }
    }
}

void HwndHost::GetDesiredSize(SIZE* pSize)
{
	pSize->cx = pSize->cy = 0;
	if (m_hWnd)
	{
		CRect rc;
		::GetWindowRect(m_hWnd, &rc);
		pSize->cx = rc.Width();
		pSize->cy = rc.Height();
	}
}

HWND  HwndHost::Detach()
{
	UIASSERT(m_hWnd);
	if (!m_hWnd)
		return NULL;

	HWND hWnd = m_hWnd;
	UnSubclassWindow();
	m_hWnd = NULL;
	return hWnd;
}
void  HwndHost::Attach(HWND hWnd)
{
	UIASSERT(!m_hWnd);
	if (m_hWnd)
	{
		Detach();
	}
	m_hWnd = hWnd;
	if (m_hWnd)
	{
		this->SubclassWindow();
	}

	// 位置同步
	SyncPosition();
}

//
//	[static] LRESULT CALLBACK _WndProc( HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
//
//	被ATL的thunk替换过的窗口过程
//
//	Parameter：
//		hwnd
//			[in]	这里由于被替换过了，这里的hwnd是this指针
//
//		uMsg,wParam,lParam
//			[in]	消息信息
//
LRESULT  HwndHost::_WndProc( HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	HwndHost* pThis = (HwndHost*)hwnd;
	return pThis->WndProc( uMsg, wParam, lParam );
}

LRESULT HwndHost::DefWindowProc( UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	if( NULL != m_oldWndProc )
	{
		return ::CallWindowProc( m_oldWndProc, m_hWnd, uMsg, wParam, lParam );
	}

	return 0;
}

//
//	[private] LRESULT WndProc( UINT uMsg, WPARAM wParam, LPARAM lParam )
//
//	窗口被子类化过之后的窗口过程
//
LRESULT	HwndHost::WndProc( UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	switch(uMsg)
	{
	case WM_SETFOCUS:
		{
			this->SetFocus(true);
			WindowBase* pWindow = this->GetWindowObject();
			if (pWindow)
			{
				pWindow->GetMouseMgr()->SetFocusObjectDirect(this);
				::UISendMessage(this, WM_SETFOCUS, 
					(WPARAM)pWindow->GetMouseMgr()->GetOldFocusObject(),
					0 );
			}
		}
		break;

	case WM_KILLFOCUS:
		{
			this->SetFocus(false);
			WindowBase* pWindow = this->GetWindowObject();
			if (pWindow)
			{
				::UISendMessage(this, WM_KILLFOCUS, 
					(WPARAM)pWindow->GetMouseMgr()->GetFocusObject(),
					0 );
			}
		}
		break;

	case WM_NCDESTROY:
		{
			LRESULT lRet = DefWindowProc(uMsg,wParam,lParam);
			this->UnSubclassWindow();  // 会清空 oldwndproc，因此先调用默认过程
			m_hWnd = NULL;
			return lRet;
		}
		break;

    case WM_STYLECHANGED:
        {
            UpdateTabStopStyle();
        }
        break;

    case WM_WINDOWPOSCHANGED:
        {
            UIASSERT(m_pParent);

            if (m_pDescription && m_pDescription->GetMajorType() == OBJ_WINDOW)  // 直接放在了父窗口下面
            {
                ::GetWindowRect(m_hWnd, &m_rcParent);
                HWND hWndParent = ::GetParent(m_hWnd);
                ::MapWindowPoints(NULL, hWndParent, (LPPOINT)&m_rcParent, 2);
            }
            else   // 有可能是放在了一个panel下面
            {
                RECT  rcWindow;
                ::GetWindowRect(m_hWnd, &rcWindow);
                HWND hWndParent = GetHWND();
                ::MapWindowPoints(NULL, hWndParent, (LPPOINT)&rcWindow, 2);

                m_pParent->WindowRect2ObjectClientRect(&rcWindow, &m_rcParent);
            }
        }
        break;
	}
	return DefWindowProc(uMsg,wParam,lParam);
}

UINT HwndHost::OnHitTest(POINT* pt, POINT*  ptInChild)
{
	POINT point = *pt;
	MapWindowPoints(GetHWND(), NULL, &point, 1);
	UINT nRet = ::SendMessage(m_hWnd, WM_NCHITTEST, 0, MAKELPARAM(point.x, point.y));
	return nRet;
}

void HwndHost::OnVisibleChanged(BOOL bVisible, IObject* pObjChanged)
{
    SetMsgHandled(FALSE);

	if (false == this->IsSelfVisible())
		return;

	if (bVisible)
	{
		// 由于直接调用this->IsVisible是判断IsWindowVisible(m_hWnd)，导致得到的值不正确
		// 因此在这里采用判断父窗口的可见
		if (m_pParent && m_pParent->IsVisible())
		{
			ShowWindow(m_hWnd, SW_SHOWNOACTIVATE);
		}
	}
	else
	{
		//if (::IsWindowVisible(m_hWnd)) <-- 注：这里不能加上这个判断，因为当在OnInitDialog中隐藏parent时，HwndHost
		//                                       的IsWindowVisible会返回FALSE，导致判断错误
		{
			ShowWindow(m_hWnd, SW_HIDE);
		}
	}
}

void  HwndHost::SetObjectPos( int x, int y, int cx, int cy, int nFlag)
{
	m_rcParent.SetRect(x, y, x+cx, y+cy);

	SyncPosition();

	if (!(nFlag&SWP_NOUPDATELAYOUTPOS))
	{
		UpdateLayoutPos();
	}
}

void  HwndHost::virtualSetVisibleEx(VISIBILITY_TYPE eType)
{
	bool bVisibleCompatible = eType==VISIBILITY_VISIBLE ? true:false;
	::ShowWindow(m_hWnd, bVisibleCompatible?SW_SHOW:SW_HIDE);
}

void  HwndHost::virtualSetEnable(bool b)
{
	::EnableWindow(m_hWnd, b ? TRUE:FALSE);
}

bool  HwndHost::IsVisible()
{
	return ::IsWindowVisible(m_hWnd)?true:false;
}

bool  HwndHost::IsEnable()
{
	return ::IsWindowEnabled(m_hWnd)?true:false;
}

void UI::HwndHost::virtualOnMove()
{
	SyncPosition();
}

void UI::HwndHost::SyncPosition()
{
	if (!m_hWnd)
		return;

	// 转化为窗口坐标，而不是父对象坐标
	CRect rcWindow;
	this->GetWindowRect(&rcWindow);

	::SetWindowPos(m_hWnd, NULL, 
		rcWindow.left, rcWindow.top, rcWindow.Width(), rcWindow.Height(), 
		SWP_NOZORDER|SWP_NOACTIVATE);
}
