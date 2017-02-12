#include "stdafx.h"
#include "listctrlmkmgr.h"
#include "Src\UIObject\ListCtrl\listctrlbase.h"
#include "Src\UIObject\ListCtrl\ListItemBase\listitembase.h"
#include "Src\Base\Application\uiapplication.h"
#include "Inc\Interface\ipanel.h"
#include "Src\UIObject\Panel\panel.h"
#include "Src\Helper\mousekeyboard\mkmgr.h"

ListCtrlMKMgrBase::ListCtrlMKMgrBase()
{
    m_pObjHover = NULL;
    m_pObjPress = NULL;
    m_pObjFocus = NULL;

    m_pItemHover = NULL;
    m_pItemPress = NULL;
    m_pItemFocus = NULL;
    m_pItemRPress = NULL;
    m_pItemMPress = NULL;
//     m_pItemSelected = NULL;

    m_pUIApplication = NULL;
    m_pListCtrlBase = NULL;
	m_bKeepHoverWhenPress = true;
    m_bMouseMoveReady = false;
}

ListCtrlMKMgrBase::~ListCtrlMKMgrBase()
{

}

BOOL  ListCtrlMKMgrBase::DoProcessMessage(UIMSG* pMsg)
{
    bool  bInterestMsg = true;
    switch(pMsg->message)
    {
    case WM_MOUSEMOVE:
        OnMouseMove(pMsg);
        break;

    case WM_MOUSELEAVE:
        OnMouseLeave(pMsg);
        break;

    case WM_LBUTTONDOWN:
        OnLButtonDown(pMsg);
        break;

    case WM_LBUTTONDBLCLK:
        OnLButtonDBClick(pMsg);
        break;

    case WM_LBUTTONUP:
        OnLButtonUp(pMsg);
        break;

    case WM_RBUTTONDOWN:
        OnRButtonDown(pMsg);
        break;

    case WM_RBUTTONUP:
        OnRButtonUp(pMsg);
        break;

    case WM_MBUTTONDBLCLK:
        OnMButtonDBClick(pMsg);
        break;

    case WM_MBUTTONDOWN:
        OnMButtonDown(pMsg);
        break;

    case WM_MBUTTONUP:
        OnMButtonUp(pMsg);
        break;

    case WM_KILLFOCUS:
        OnKillFocus(pMsg);
        break;

    case WM_SETFOCUS:
        OnSetFocus(pMsg);
        break;

    case WM_SETCURSOR:
        {
            if (OnSetCursor(pMsg))
                return true;
            else
                return false;
        }
        break;

	/*case UI_WM_GESTURE_PAN:
	case UI_WM_GESTURE_PRESSANDTAP:
		{
			BOOL bHandled = OnGestureMessage(pMsg);
			return bHandled;
		}
		break;*/

    case WM_CHAR:
    case WM_KEYDOWN:
    case WM_KEYUP:
	case WM_IME_STARTCOMPOSITION:   // EDIT输入法跟随
        if (OnKeyMsg(pMsg))
            return true;
        else
            return false;
        break;

    default:
        bInterestMsg = false;
        break;
    }


    if (bInterestMsg)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

BOOL  ListCtrlMKMgrBase::DoProcessItemMessage(UIMSG* pMsg, ListItemBase* pItem)
{
    if (NULL == pItem->GetRootPanel())
        return FALSE;

    bool bInterestMsg = true;
    switch (pMsg->message)
    {
    case WM_MOUSEMOVE:
        OnItemMouseMove(pMsg, pItem);
        break;

    case WM_MOUSELEAVE:
        OnItemMouseLeave(pMsg, pItem);
        break;

    case WM_LBUTTONDOWN:
        OnItemLButtonDown(pMsg, pItem);
        break;

    case WM_LBUTTONDBLCLK:
        {
            BOOL bHandled = FALSE;
            OnItemLButtonDBClick(pMsg, pItem, &bHandled);
            return bHandled;
        }
        break;

    case WM_LBUTTONUP:
        {
            BOOL b = GetHoverObject() || GetPressObject();
            OnItemLButtonUp(pMsg, pItem);
            if (b)
                return TRUE;
            else
                return FALSE;
        }
        break;

    case WM_KILLFOCUS:
        OnItemKillFocus(pMsg, pItem);
        break;

    case WM_SETCURSOR:
        {
            if (OnItemSetCursor(pMsg, pItem))
            {
                pMsg->lRet = 1;
                return TRUE;
            }
            else
            {
                pMsg->lRet = 0;
                return FALSE;
            }
        }
        break;

    case WM_KEYDOWN:
    case WM_CHAR:
    case WM_KEYUP:
	case WM_IME_STARTCOMPOSITION:  // EDIT输入法跟随
        if (OnItemKeyMsg(pMsg, pItem))
            return TRUE;
        else 
            return FALSE;

    default:
        bInterestMsg = FALSE;
    }

    if (!bInterestMsg)
        return FALSE;

    if ((pMsg->message >= WM_MOUSEFIRST && pMsg->message <= WM_MOUSELAST) ||
        pMsg->message == WM_MOUSELEAVE) 
    {
        if (!GetHoverObject() && !GetPressObject())
            return FALSE;
    }
    return TRUE;
}

void  ListCtrlMKMgrBase::SetListCtrlBase(UIApplication* pUIApp, ListCtrlBase*  pCtrl)
{
    m_pListCtrlBase = pCtrl;
    m_pUIApplication = pUIApp;
}


void  ListCtrlMKMgrBase::OnMouseMove(UIMSG* pMsg)
{
    m_bMouseMoveReady = true;

    POINT  ptWindow = { GET_X_LPARAM(pMsg->lParam), GET_Y_LPARAM(pMsg->lParam) };
    ListItemBase*  pNewHover = GetItemByPos(ptWindow);

    // m_bKeepHoverWhenPress为true时表示当鼠标已经按住了一个控件时，不改变hover item
    if ( (!m_bKeepHoverWhenPress || NULL == m_pItemPress) && pNewHover != m_pItemHover)  
    {
        ListItemBase* pSave = m_pItemHover;
        SetHoverItem(pNewHover);

        if (pSave != pNewHover)
        {
            if (pSave)
                UISendMessage(pSave->GetIListItemBase(), WM_MOUSELEAVE);
            
            if (pNewHover)
                UISendMessage(pNewHover->GetIListItemBase(), WM_MOUSEHOVER);
        }
    }

    if (m_pItemHover)
        UISendMessage(m_pItemHover->GetIListItemBase(), WM_MOUSEMOVE, pMsg->wParam, pMsg->lParam);
}

void  ListCtrlMKMgrBase::OnMouseLeave(UIMSG* pMsg)
{
    m_bMouseMoveReady = false;

    ListItemBase* pSave = m_pItemHover;
    SetPressItem(NULL);
    SetHoverItem(NULL);

    if (pSave)
    {
        UISendMessage(pSave->GetIListItemBase(), WM_MOUSELEAVE, pMsg->wParam, pMsg->lParam);
        this->m_pListCtrlBase->InvalidateItem(pSave);
    }

}


void  ListCtrlMKMgrBase::OnItemMouseMove(UIMSG* pMsg, ListItemBase* pItem)
{
    Panel* pRootPanel = pItem->GetRootPanel();
    if (NULL == pRootPanel)
        return;

    // 将鼠标转换为相对于ITEM
    POINT pt = { GET_X_LPARAM(pMsg->lParam), GET_Y_LPARAM(pMsg->lParam) };
    POINT ptInPanel = {0,0};
    m_pListCtrlBase->WindowPoint2ItemPoint(pItem, &pt, &ptInPanel);

    Object*   pOldObjHover = m_pObjHover;
    Object*   pOldObjPress = m_pObjPress;

    Object* pObj = pRootPanel->GetObjectByPos(&ptInPanel);
    _OnMouseMove(pObj, pMsg->wParam, pMsg->lParam, this);

    // 重置光标
    if (pOldObjPress==m_pObjPress && pOldObjHover!=m_pObjHover)  
    {
        HWND hWnd = m_pListCtrlBase->GetIListCtrlBase()->GetHWND();
        if (GetCapture() != hWnd)        
        {
            ::PostMessage(hWnd, WM_SETCURSOR, (WPARAM)hWnd, MAKELPARAM(HTCLIENT,1)); 
        }
    }
}

void  ListCtrlMKMgrBase::OnItemMouseLeave(UIMSG* pMsg, ListItemBase* pItem)
{
    Panel* pIRootPanel = pItem->GetRootPanel();
    if (NULL == pIRootPanel)
        return;

    _OnMouseLeave(this);
}

void  ListCtrlMKMgrBase::OnItemLButtonDown(UIMSG* pMsg, ListItemBase* pItem)
{
    BOOL bHandled = FALSE;
    _OnLButtonDown(pMsg->wParam, pMsg->lParam, &bHandled, this);
}
void  ListCtrlMKMgrBase::OnItemLButtonDBClick(UIMSG* pMsg, ListItemBase* pItem, BOOL* pbHandled)
{
    _OnLButtonDBClick(pMsg->wParam, pMsg->lParam, pbHandled, this);
}
void  ListCtrlMKMgrBase::OnItemLButtonUp(UIMSG* pMsg, ListItemBase* pItem)
{
    _OnLButtonUp(pMsg->wParam, pMsg->lParam, this);
}
bool  ListCtrlMKMgrBase::OnItemSetCursor(UIMSG* pMsg, ListItemBase* pItem)
{
    if (m_pObjPress)
    {
        ::UISendMessage(m_pObjPress, WM_SETCURSOR, pMsg->wParam, pMsg->lParam);
        return true;
    }
    else if (m_pObjHover)
    {
        ::UISendMessage(m_pObjHover, WM_SETCURSOR, pMsg->wParam, pMsg->lParam);
        return true;
    }
    return false;
}
void  ListCtrlMKMgrBase::OnItemKillFocus(UIMSG* pMsg, ListItemBase* pItem)
{
    if (m_pObjFocus)
    {
        SetFocusObject(NULL);
    }
}
bool  ListCtrlMKMgrBase::OnItemKeyMsg(UIMSG* pMsg, ListItemBase* pItem)
 {
    if (m_pObjFocus)
    {
        BOOL bHandled = FALSE;
        ::UISendMessage(m_pObjFocus->GetIObject(), pMsg->message, pMsg->wParam, pMsg->lParam,
            0,0,0, &bHandled);

        return bHandled ?  true:false;
    }
    return false;
}

// 处理press item，但不处理selection item
long  ListCtrlMKMgrBase::OnLButtonDown(UIMSG* pMsg)
{
    if (!m_bMouseMoveReady)
    {
        OnMouseMove(pMsg);
    }

    if (m_pObjPress)
        return IMKER_HANDLED_BY_INNERCONTROL;

    long  lRet = 0;
    if (m_pItemHover)
    {
        this->SetPressItem(m_pItemHover);

        lRet = UISendMessage(
            m_pItemHover->GetIListItemBase(),
            WM_LBUTTONDOWN, 
            pMsg->wParam, pMsg->lParam);

        if (lRet & (IMKER_HANDLED_BY_ITEM|IMKER_HANDLED_BY_INNERCONTROL))
            return lRet;
    }

    return lRet;
}

void  ListCtrlMKMgrBase::OnLButtonUp(UIMSG* pMsg)
{
    BOOL bHandled = FALSE;
    ListItemBase* pSave = m_pItemPress;
    if (m_pItemPress)
    {
        this->SetPressItem(NULL);
        UISendMessage(pSave->GetIListItemBase(), WM_LBUTTONUP, pMsg->wParam, pMsg->lParam,0, 0,0, &bHandled);
    }

    if (!bHandled)
    {
//         POINT  ptWindow = { GET_X_LPARAM(pMsg->lParam), GET_Y_LPARAM(pMsg->lParam) };
//         ListItemBase*  pNewHover = GetItemByPos(ptWindow);
//         if (pNewHover == pSave)
        {
//             LISTCTRL_CLICKNOTIFY_DATA data = {0};
//             if (pSave)
//                 data.pDowmItem = pSave->GetIListItemBase();
//             data.wParam = pMsg->wParam;
//             data.lParam = pMsg->lParam;
// 
//             UIMSG  msg;
//             msg.message = UI_MSG_NOTIFY;
//             msg.nCode   = UI_NM_CLICK;
//             msg.wParam  = (WPARAM)&data;
//             msg.pMsgFrom = m_pListCtrlBase->GetIListCtrlBase();
//             msg.pMsgFrom->DoNotify(&msg);

            m_pListCtrlBase->click.emit(
                m_pListCtrlBase->GetIListCtrlBase(),
                pSave ? pSave->GetIListItemBase() : NULL);
        }
    }
}

void  ListCtrlMKMgrBase::OnLButtonDBClick(UIMSG* pMsg)
{
    if (!m_bMouseMoveReady)
    {
        OnMouseMove(pMsg);
    }

    if (m_pObjPress)
        return;

    BOOL bHandled = FALSE;
    if (m_pItemHover)
    {
        this->SetPressItem(m_pItemHover);

        UISendMessage(m_pItemHover->GetIListItemBase(), WM_LBUTTONDBLCLK,
            pMsg->wParam, pMsg->lParam);
    }

    if (!bHandled)
    {
//         LISTCTRL_CLICKNOTIFY_DATA data = {0};
//         if (m_pItemPress)
//             data.pDowmItem = m_pItemPress->GetIListItemBase();
//         data.wParam = pMsg->wParam;
//         data.lParam = pMsg->lParam;
//
//         UIMSG  msg;
//         msg.message = UI_MSG_NOTIFY;
//         msg.nCode   = UI_NM_DBCLICK;
//         msg.wParam  = (WPARAM)&data;
//         msg.pMsgFrom = m_pListCtrlBase->GetIListCtrlBase();
//         msg.pMsgFrom->DoNotify(&msg);

        m_pListCtrlBase->dbclick.emit(
            m_pListCtrlBase->GetIListCtrlBase(),
            m_pItemPress ? m_pItemPress->GetIListItemBase() : NULL);
    }
}


void  ListCtrlMKMgrBase::OnRButtonDown(UIMSG* pMsg)
{
    if (!m_bMouseMoveReady)
    {
        OnMouseMove(pMsg);
    }

    if (m_pObjPress)
        return;

    if (!m_pItemHover)
        return;

    UISendMessage(m_pItemHover->GetIListItemBase(), WM_RBUTTONDOWN, pMsg->wParam, pMsg->lParam);
    m_pItemRPress = m_pItemHover;
}

void  ListCtrlMKMgrBase::OnRButtonUp(UIMSG* pMsg)
{
    BOOL bHandled = FALSE;

	ListItemBase* pSave = m_pItemRPress;
	m_pItemRPress = NULL;   // 必须在发通知前先重置。（通知中可能修改该项）

    if (pSave)
    {
        UISendMessage(pSave->GetIListItemBase(), WM_RBUTTONUP, pMsg->wParam, pMsg->lParam, 0, 0, 0, &bHandled);
    }
    if (!bHandled)
    {
//         LISTCTRL_CLICKNOTIFY_DATA data = {0};
//         if (pSave)
//             data.pDowmItem = pSave->GetIListItemBase();
//         data.wParam = pMsg->wParam;
//         data.lParam = pMsg->lParam;
// 
//         UIMSG  msg;
//         msg.message = UI_MSG_NOTIFY;
//         msg.nCode   = UI_NM_RCLICK;
// 		msg.wParam = (WPARAM)&data;
//         msg.pMsgFrom = m_pListCtrlBase->GetIListCtrlBase();
//         msg.pMsgFrom->DoNotify(&msg);

        m_pListCtrlBase->rclick.emit(
            m_pListCtrlBase->GetIListCtrlBase(),
            pSave ? pSave->GetIListItemBase() : NULL);
    }
}


void  ListCtrlMKMgrBase::OnMButtonDBClick(UIMSG* pMsg)
{
    OnMButtonDown(pMsg);
}
void  ListCtrlMKMgrBase::OnMButtonDown(UIMSG* pMsg)
{
    if (m_pObjPress)
        return;

    if (!m_pItemHover)
        return;

    UISendMessage(m_pItemHover->GetIListItemBase(), pMsg->message, pMsg->wParam, pMsg->lParam);
    m_pItemMPress = m_pItemHover;
}
void  ListCtrlMKMgrBase::OnMButtonUp(UIMSG* pMsg)
{
    BOOL bHandled = FALSE;

    ListItemBase* pSave = m_pItemMPress;
    if (m_pItemMPress)
    {
        m_pItemMPress = NULL;   // 必须在发通知前先重置。（通知中可能修改该项）
        UISendMessage(pSave->GetIListItemBase(), pMsg->message, pMsg->wParam, pMsg->lParam, 0, 0, 0, &bHandled);
    }
    if (!bHandled)
    {
        // 如果当前hover item改变了，由外部业务逻辑去判断是否处理

//         LISTCTRL_CLICKNOTIFY_DATA  data = {0};
//         if (pSave)
//             data.pDowmItem = pSave->GetIListItemBase();
//         data.wParam = pMsg->wParam;
//         data.lParam = pMsg->lParam;
// 
//         UIMSG  msg;
//         msg.message = UI_MSG_NOTIFY;
//         msg.nCode   = UI_NM_MCLICK;
//         msg.wParam  = (WPARAM)&data;
//         msg.pMsgFrom = m_pListCtrlBase->GetIListCtrlBase();
//         msg.pMsgFrom->DoNotify(&msg);

        m_pListCtrlBase->mclick.emit(
            m_pListCtrlBase->GetIListCtrlBase(),
            pSave ? pSave->GetIListItemBase() : NULL);
    }
}

void  ListCtrlMKMgrBase::OnSetFocus(UIMSG* pMsg)
{
    if (m_pItemFocus)
    {
        UISendMessage(m_pItemFocus->GetIListItemBase(), WM_SETFOCUS, pMsg->wParam, pMsg->lParam);
    }
}
void  ListCtrlMKMgrBase::OnKillFocus(UIMSG* pMsg)
{
    SetHoverObject(NULL);
    SetPressObject(NULL);
    SetHoverItem(NULL);
    SetPressItem(NULL);

    if (m_pItemFocus)
    {
        UISendMessage(m_pItemFocus->GetIListItemBase(),
            WM_KILLFOCUS, pMsg->wParam, pMsg->lParam);
    }
}
bool  ListCtrlMKMgrBase::OnSetCursor(UIMSG* pMsg)
{
    ListItemBase*  pItem = m_pItemPress;
    if (NULL == pItem)
        pItem = m_pItemHover;

    BOOL bHandled = FALSE;
    if (pItem)
    {
        long lRet = UISendMessage(
                        pItem->GetIListItemBase(), 
                        WM_SETCURSOR, 
                        pMsg->wParam, 
                        pMsg->lParam, 0, 0, 0, &bHandled);

        if (bHandled)
            pMsg->lRet = lRet;
    }
    return bHandled?true:false;
}

// BOOL  ListCtrlMKMgrBase::OnGestureMessage(UIMSG* pMsg)
// {
// 	return FALSE;
// }


bool  ListCtrlMKMgrBase::OnKeyMsg(UIMSG* pMsg)
{
    if (m_pItemFocus)
    {
        BOOL bHandled = FALSE;
        UISendMessage(
			m_pItemFocus->GetIListItemBase(),
			pMsg->message, 
			pMsg->wParam,
			pMsg->lParam,
            0,0,0, &bHandled);

        return bHandled ? true : false;
    }

    return false;
}

void ListCtrlMKMgrBase::OnRemoveAll()
{
    m_pItemHover = m_pItemPress = m_pItemFocus = m_pItemRPress = m_pItemMPress = NULL; 
    m_pObjPress = m_pObjHover = m_pObjFocus = NULL;

	m_bMouseMoveReady = FALSE;
    if (m_pListCtrlBase->GetIListCtrlBase()->IsHover())
    {
        m_pUIApplication->HideToolTip();
    }
}
void  ListCtrlMKMgrBase::OnDisableItem(ListItemBase* pItem)
{
	on_item_state_changed(pItem, ITEM_DISABLED, NULL);
}
void  ListCtrlMKMgrBase::OnHideItem(ListItemBase* pItem)
{
	on_item_state_changed(pItem, ITEM_HIDE, NULL);
}
void  ListCtrlMKMgrBase::OnRemoveItem(ListItemBase* pItem, bool* pbSelChanged)
{
	on_item_state_changed(pItem, ITEM_REMOVED, pbSelChanged);
}
void  ListCtrlMKMgrBase::on_item_state_changed(
		ListItemBase* pItem,
		ITEM_STATE_CHANGE_TYPE eType, 
		bool* pbSelChanged)
{
    if (m_pItemHover)
    {
        if (m_pItemHover == pItem || pItem->IsMyChildItem(m_pItemHover, true))
        {
            m_pUIApplication->HideToolTip();
            
			// todo: 如果是隐藏的话，这里并没有给item发送mouse leave通知，
			// 会造成消息不对称。
			if (eType == ITEM_REMOVED)
				m_pItemHover = NULL;
			else
				SetHoverItem(NULL);
        }
    }

    if (m_pItemPress)
    {
        if (m_pItemPress == pItem || pItem->IsMyChildItem(m_pItemPress, true))
        {
			if (eType == ITEM_REMOVED)
				m_pItemPress = NULL;
			else
				SetPressItem(NULL);
        }
    }
    if (m_pItemRPress)
    {
        if (m_pItemRPress == pItem || pItem->IsMyChildItem(m_pItemRPress, true))
        {
            m_pItemRPress = NULL;
        }
    }
    if (m_pItemMPress)
    {
        if (m_pItemMPress == pItem || pItem->IsMyChildItem(m_pItemMPress, true))
        {
            m_pItemMPress = NULL;
        }
    }
    if (m_pItemFocus)
    {
        if (m_pItemFocus == pItem || pItem->IsMyChildItem(m_pItemFocus, true))
        {
            ListItemBase*  pNextTreeItem = NULL;

            // 子结点将被删除，不能用于setfoucsitem。
            if (pItem->GetChildItem())
            {
                // 跳过子结点
                ListItemBase*  pSaveChildItem = pItem->GetChildItem();
                pItem->SetChildItem(NULL);
                pNextTreeItem = pItem->GetNextTreeItem();
                pItem->SetChildItem(pSaveChildItem);
            }
            else
            {
                pNextTreeItem = pItem->GetNextTreeItem();
            }

            ListItemBase*  pFocusMoveTo = NULL;
            if (pNextTreeItem)
            {
                pFocusMoveTo = m_pListCtrlBase->FindFocusableItemFrom(pNextTreeItem);
            }

            if (NULL == pFocusMoveTo)
            {
                // 注：在RemoceAllChildItem中不能走这个分支，child item之间的关系链没有被清空
                //     因此获取preitem肯定会崩溃
                pFocusMoveTo = pItem->GetPrevFocusableItem();  
            }

            m_pItemFocus = NULL; // 避免SetFocusItem中去刷新pItem，造成崩溃
            SetFocusItem(pFocusMoveTo);
        }
    }

    // 从selection列表中删除，放在最后通知sel changed
    ListItemBase* pSelItem = m_pListCtrlBase->GetFirstSelectItem();
    while (pSelItem)
    {
        if (pSelItem == pItem || pItem->IsMyChildItem(pSelItem, true))
        {
            if (pbSelChanged)
                *pbSelChanged = true;

            ListItemBase* pSave = pSelItem->GetNextSelection();
            m_pListCtrlBase->RemoveSelectItem(pSelItem, false);
            pSelItem = pSave;
        }
        else
        {
            pSelItem = pSelItem->GetNextSelection();
        }
    }

//     Object* pFocusObj = GetFocusObject();
//     if (NULL == pFocusObj)
//         return;
// 
    Panel* pPanel = pItem->GetRootPanel();
    if (NULL == pPanel)
        return;

    if (m_pObjFocus)
    {
        if (pPanel->IsMyChild(m_pObjFocus, true))
        {
            m_pObjFocus = NULL;
        }
    }
    if (m_pObjHover)
    {
        if (pPanel->IsMyChild(m_pObjHover, true))
            m_pObjHover = NULL;
    }
    if (m_pObjPress)
    {
        if (pPanel->IsMyChild(m_pObjPress, true))
            m_pObjPress = NULL;
    }
}
// void  ListCtrlMKMgrBase::OnRemoveObject(IObject* pObj)
// {
//     if (NULL == pObj)
//         return;
// 
//     Object* p = pObj->GetImpl();
//     Object* pFocusObj = GetFocusObject();
//     if (p == pFocusObj || p->IsMyChild(pFocusObj, true))
//     {
//         SetFocusObject(NULL);
//     }
// }

void  ListCtrlMKMgrBase::OnObjectVisibleChangeInd(Object* pObj, bool bVisible)
{
    if (NULL == pObj || bVisible)  // 只处理不可见的情况
        return;

    if (pObj == m_pObjFocus || pObj->IsMyChild(m_pObjFocus, true))
    {
        SetFocusObject(NULL);
    }
    if (m_pObjHover == pObj || pObj->IsMyChild(m_pObjHover, true))
    {
        SetHoverObject(NULL);
    }
    if (m_pObjPress == pObj || pObj->IsMyChild(m_pObjPress, true))
    {
        SetPressObject(NULL);
    }

}
void  ListCtrlMKMgrBase::OnObjectRemoveInd(Object* pObj)
{
    if (NULL == pObj)
        return;

    if (m_pObjHover == pObj || pObj->IsMyChild(m_pObjHover, true))
    {
        m_pObjHover = NULL;
    }
    if (m_pObjPress == pObj || pObj->IsMyChild(m_pObjPress, true))
    {
        m_pObjPress = NULL;
    }
    if (pObj == m_pObjFocus || pObj->IsMyChild(m_pObjFocus, true))
    {
        m_pObjFocus = NULL;
    }
}

ListItemBase*  ListCtrlMKMgrBase::GetItemByPos(POINT ptWindow)
{
    CRect rcClient;
    m_pListCtrlBase->GetClientRectInObject(&rcClient);

    // 1. 转换为内部坐标
    POINT pt = {0};
    m_pListCtrlBase->WindowPoint2ObjectPoint(&ptWindow, &pt, true);
    if (FALSE == rcClient.PtInRect(pt))
        return NULL;

	Object::ObjectPoint2ObjectClientPoint(m_pListCtrlBase, &pt, &pt);

    UIMSG  msg;
    msg.message = UI_LCM_HITTEST;
    msg.wParam = pt.x;
	msg.lParam = pt.y;
    msg.pMsgTo = m_pListCtrlBase->GetIListCtrlBase();

    IListItemBase*  pHoverItem = (IListItemBase*)UISendMessage(&msg);
    if (NULL == pHoverItem)
        return NULL;

    return pHoverItem->GetImpl();
}



void  ListCtrlMKMgrBase::SetHoverItem(ListItemBase* pItem)
{
    if (m_pItemHover == pItem)
        return;

    // 提示条逻辑 
    m_pUIApplication->HideToolTip();

    ListItemBase* pOldHoverItem = m_pItemHover;
    if (m_pItemHover)
    {
        m_pItemHover->SetHover(false);
        this->m_pListCtrlBase->InvalidateItem(m_pItemHover);
    }
    if (pItem)
    {
        pItem->SetHover(true);
        pItem->ShowSingleToolTip();
        this->m_pListCtrlBase->InvalidateItem(pItem);
    }

    m_pItemHover = pItem;
    FireHoverItemChanged(pOldHoverItem);
}


void  ListCtrlMKMgrBase::SetPressItem(ListItemBase* pItem)
{
    if (m_pItemPress == pItem)
        return;

    if (m_pItemPress)
    {
        m_pItemPress->SetPress(false);
        this->m_pListCtrlBase->InvalidateItem(m_pItemPress);
    }
    if (pItem)
    {
        pItem->SetPress(true);
        this->m_pListCtrlBase->InvalidateItem(pItem);
    }

    m_pItemPress = pItem;
}

void  ListCtrlMKMgrBase::SetFocusItem(ListItemBase* pItem)
{
    if (m_pItemFocus == pItem)
        return;

    if (pItem && !pItem->IsFocusable())
        return;

    if (m_pItemFocus)
    {
        this->m_pListCtrlBase->InvalidateItem(m_pItemFocus);
        m_pItemFocus->SetFocus(false);
    }

    m_pItemFocus = pItem; 

    if (m_pItemFocus)
    {
        this->m_pListCtrlBase->InvalidateItem(m_pItemFocus);
        m_pItemFocus->SetFocus(true);
    }

    // 更新foucs obj为m_pItemFocus下的对象
    if (m_pItemFocus)
    {
        Panel* pPanel = m_pItemFocus->GetRootPanel();
        if (NULL == pPanel)
        {
            SetFocusObject(NULL);
        }
        else
        {
            Object* pFocusObj = GetFocusObject();
            if (pFocusObj)
            {
                if (!pPanel->IsMyChild(pFocusObj, true))
                {
                    SetFocusObject(NULL);
                }
            }
        }
    }
    else
    {
        SetFocusObject(NULL);
    }

    if (m_pItemFocus)
        this->m_pListCtrlBase->InvalidateItem(m_pItemFocus);
}

void  ListCtrlMKMgrBase::SetHoverObject(Object* pNewHoverObj)
{
    _SetHoverObject(pNewHoverObj, this);
}
void  ListCtrlMKMgrBase::SetPressObject(Object* pNewPressObj)
{
    _SetPressObject(pNewPressObj, this);
}
void  ListCtrlMKMgrBase::SetFocusObject(Object* pObj)
{
    if (m_pObjFocus == pObj)  
        return;

    if (m_pObjFocus)
    {
        m_pObjFocus->SetFocus(false);
        ::UISendMessage(m_pObjFocus, WM_KILLFOCUS, (WPARAM)pObj, NULL );
    }
    if (pObj)
    {
        pObj->SetFocus(true);
        ::UISendMessage(pObj, WM_SETFOCUS, (WPARAM)m_pObjFocus, NULL );
    }
    m_pObjFocus = pObj;
    return;
}
 
void  ListCtrlMKMgrBase::FireHoverItemChanged(ListItemBase* pOldHoverItem)
{
    ListItemBase* pHover = m_pItemHover;
    IListCtrlBase* pIListCtrlBase = m_pListCtrlBase->GetIListCtrlBase();

    // 通知 ctrl
    UIMSG  msg;
    msg.message = UI_MSG_NOTIFY;
    msg.nCode = UI_LCN_HOVERITEMCHANGED;
    msg.wParam = (WPARAM)(pOldHoverItem?pOldHoverItem->GetIListItemBase():NULL);

    msg.lParam = (LPARAM)(pHover?pHover->GetIListItemBase():NULL);
    msg.pMsgFrom = pIListCtrlBase;

    // 先交给子类处理
    msg.pMsgTo = pIListCtrlBase;
    UISendMessage(&msg, 0, 0);

    // 再通知外部处理 
    msg.pMsgTo = NULL;
    msg.bHandled = FALSE;
    pIListCtrlBase->DoNotify(&msg);
}


//////////////////////////////////////////////////////////////////////////

BOOL  SingleSelListCtrlMKMgr::DoProcessMessage(UIMSG* pMsg)
{
    switch (pMsg->message)
    {
    case WM_LBUTTONDOWN:
        {
            OnLButtonDown(pMsg);
            return TRUE;
        }
        break;

    case WM_LBUTTONUP:
        {
            OnLButtonUp(pMsg);
            return TRUE;
        }
        break;

    case WM_RBUTTONDOWN:
        {
            OnRButtonDown(pMsg);
            return TRUE;
        }
        break;

    case WM_RBUTTONUP:
        {
            OnRButtonUp(pMsg);
            return TRUE;
        }
        break;

    case WM_KEYDOWN:
        {
            BOOL bHandled = __super::DoProcessMessage(pMsg);
            if (bHandled)
                return TRUE;
        
            bool bInterestMsg = true;
            OnKeyDown(pMsg, &bInterestMsg);
            if (bInterestMsg)
            {
                return TRUE;
            }
        }
        break;

    default:
        return __super::DoProcessMessage(pMsg);
        break;
    }
    return FALSE;
}



void  SingleSelListCtrlMKMgr::handle_selection_change()
{
    if (m_pItemPress)
    {
        if (m_pItemPress->IsSelectable())
        {
            // 只选这一个
            m_pListCtrlBase->SelectItem(m_pItemPress);
        }
        else if (m_pItemPress->IsFocusable())
        {
            m_pListCtrlBase->SetFocusItem(m_pItemPress);
        }
    }
    else //if (!m_pListCtrlBase->TestBaseStyle(LISTCTRLBASE_STYLE_CLICKEMPTY_KEEPSELECTION))
    {
        m_pListCtrlBase->ClearSelectItem(true);
    }
}

// 仅处理选项
void  SingleSelListCtrlMKMgr::OnLButtonDown(UIMSG* pMsg)
{
    long lRet = __super::OnLButtonDown(pMsg);
    if (lRet & IMKER_DONT_CHANGE_SELECTION)
        return;

    LISTCTRLSTYLE s = {0};
    s.changeselection_onlbuttonup = 1;
    if (!m_pListCtrlBase->TestListCtrlStyle(&s))
    {
        handle_selection_change();
    }
}

void  SingleSelListCtrlMKMgr::OnLButtonUp(UIMSG* pMsg)
{
    LISTCTRLSTYLE s = {0};
    s.changeselection_onlbuttonup = 1;
    if (m_pListCtrlBase->TestListCtrlStyle(&s))
    {
        POINT  ptWindow = { 
            GET_X_LPARAM(pMsg->lParam),
            GET_Y_LPARAM(pMsg->lParam) 
        };
        ListItemBase*  pNewHover = GetItemByPos(ptWindow);
        if (m_pItemPress == pNewHover)
        {
            handle_selection_change();
        }
    }

    __super::OnLButtonUp(pMsg);
}

// 仅处理选项
void  SingleSelListCtrlMKMgr::OnRButtonDown(UIMSG* pMsg)
{
    __super::OnRButtonDown(pMsg);

    if (m_pItemHover)
    {
        if (m_pItemHover->IsSelectable())
        {
            // 只选这一个
            m_pListCtrlBase->SelectItem(m_pItemHover);
        }
    }
    else
    {
        m_pListCtrlBase->ClearSelectItem(true);
    }
}
void  SingleSelListCtrlMKMgr::OnRButtonUp(UIMSG* pMsg)
{
    __super::OnRButtonUp(pMsg);
}


void  SingleSelListCtrlMKMgr::OnKeyDown(UIMSG* pMsg, bool* pbInterestMsg)
{
    if (pbInterestMsg)
        *pbInterestMsg = true;

    switch (pMsg->wParam)
    {
    case VK_DOWN:
    case VK_RIGHT:  // icon listview
        OnKeyDown_down(pMsg);
        return;

    case VK_UP:
    case VK_LEFT:  // icon listview
        OnKeyDown_up(pMsg);
        return;

	case VK_SPACE:
		OnKeyDown_space(pMsg);
		break;

    case VK_PRIOR:
        OnKeyDown_prior(pMsg);
        break;

    case VK_NEXT:
        OnKeyDown_next(pMsg);
        break;;

    case VK_HOME:
        OnKeyDown_home(pMsg);
        break;

    case VK_END:
        OnKeyDown_end(pMsg);
        break;

    default:
        if (pbInterestMsg)
            *pbInterestMsg = false;
        break;
    }
}

void  SingleSelListCtrlMKMgr::OnKeyDown_up(UIMSG* pMsg)
{
    if (NULL == m_pItemFocus)
    {
        ListItemBase* pFirstSelectableItem = m_pListCtrlBase->FindFocusableItemFrom(NULL);
        if (pFirstSelectableItem)
        {
            if (pFirstSelectableItem->IsSelectable())
                this->m_pListCtrlBase->SelectItem(pFirstSelectableItem);
            else
                this->m_pListCtrlBase->SetFocusItem(pFirstSelectableItem);
        }
    }
    else
    {
        ListItemBase* pPrevItem = m_pItemFocus->GetPrevFocusableItem();
        if (pPrevItem)
        {
            if (pPrevItem->IsSelectable())
                this->m_pListCtrlBase->SelectItem(pPrevItem);
            else
                this->m_pListCtrlBase->SetFocusItem(pPrevItem);
        }
    }
}

void  SingleSelListCtrlMKMgr::OnKeyDown_down(UIMSG* pMsg)
{
    if (NULL == m_pItemFocus)
    {
        ListItemBase* pFirstSelectableItem = m_pListCtrlBase->FindFocusableItemFrom(NULL);
        if (pFirstSelectableItem)
        {
            if (pFirstSelectableItem->IsSelectable())
                this->m_pListCtrlBase->SelectItem(pFirstSelectableItem);
            else
                this->m_pListCtrlBase->SetFocusItem(pFirstSelectableItem);
        }
    }
    else
    {
        ListItemBase* pNextItem = m_pItemFocus->GetNextFocusableItem();
        if (pNextItem)
        {
            if (pNextItem->IsSelectable())
                this->m_pListCtrlBase->SelectItem(pNextItem);
            else
                this->m_pListCtrlBase->SetFocusItem(pNextItem);
        }
    }
}

void  SingleSelListCtrlMKMgr::OnKeyDown_space(UIMSG* pMsg)
{
	if (NULL == m_pItemFocus)
	{
		ListItemBase* pFirstSelectableItem = m_pListCtrlBase->FindSelectableItemFrom(NULL);
		if (pFirstSelectableItem)
		{
			this->m_pListCtrlBase->SelectItem(pFirstSelectableItem);
		}
	}
	else
	{
		this->m_pListCtrlBase->SelectItem(m_pItemFocus);
	}
}
void  SingleSelListCtrlMKMgr::OnKeyDown_prior(UIMSG* pMsg)
{
    if (NULL == m_pItemFocus)
    {
        ListItemBase* pFirstSelectableItem = m_pListCtrlBase->FindSelectableItemFrom(NULL);
        if (pFirstSelectableItem)
        {
            this->m_pListCtrlBase->SelectItem(pFirstSelectableItem);
        }
    }
    else
    {
        IScrollBarManager*  pScrollMgr = m_pListCtrlBase->GetIScrollBarMgr();
        pScrollMgr->DoPageUp();
        
        this->m_pListCtrlBase->CalcFirstLastDrawItem();
        ListItemBase*  pNewFocusItem = m_pListCtrlBase->GetFirstDrawItem();
        this->m_pListCtrlBase->SelectItem(pNewFocusItem);


        this->m_pListCtrlBase->Invalidate();
    }
}
void  SingleSelListCtrlMKMgr::OnKeyDown_next(UIMSG* pMsg)
{
    if (NULL == m_pItemFocus)
    {
        ListItemBase* pFirstSelectableItem = m_pListCtrlBase->FindSelectableItemFrom(NULL);
        if (pFirstSelectableItem)
        {
            this->m_pListCtrlBase->SelectItem(pFirstSelectableItem);
        }
    }
    else
    {
        IScrollBarManager*  pScrollMgr = m_pListCtrlBase->GetIScrollBarMgr();
        pScrollMgr->DoPageDown();

        this->m_pListCtrlBase->CalcFirstLastDrawItem();
        ListItemBase*  pNewFocusItem = m_pListCtrlBase->GetLastDrawItem();
        this->m_pListCtrlBase->SelectItem(pNewFocusItem);

        this->m_pListCtrlBase->Invalidate();
    }
}
void  SingleSelListCtrlMKMgr::OnKeyDown_home(UIMSG* pMsg)
{
    if (NULL == m_pItemFocus)
    {
        ListItemBase* pFirstSelectableItem = m_pListCtrlBase->FindSelectableItemFrom(NULL);
        if (pFirstSelectableItem)
        {
            this->m_pListCtrlBase->SelectItem(pFirstSelectableItem);
        }
    }
    else
    {
        IScrollBarManager*  pScrollMgr = m_pListCtrlBase->GetIScrollBarMgr();
        pScrollMgr->DoHome();

        this->m_pListCtrlBase->CalcFirstLastDrawItem();
        ListItemBase*  pNewFocusItem = m_pListCtrlBase->GetFirstDrawItem();
        this->m_pListCtrlBase->SelectItem(pNewFocusItem);
       
        this->m_pListCtrlBase->Invalidate();
    }
}
void  SingleSelListCtrlMKMgr::OnKeyDown_end(UIMSG* pMsg)
{
    if (NULL == m_pItemFocus)
    {
        ListItemBase* pFirstSelectableItem = m_pListCtrlBase->FindSelectableItemFrom(NULL);
        if (pFirstSelectableItem)
        {
            this->m_pListCtrlBase->SelectItem(pFirstSelectableItem);
        }
    }
    else
    {
        IScrollBarManager*  pScrollMgr = m_pListCtrlBase->GetIScrollBarMgr();
        pScrollMgr->DoEnd();

        this->m_pListCtrlBase->CalcFirstLastDrawItem();
        ListItemBase*  pNewFocusItem = m_pListCtrlBase->GetLastDrawItem();
        this->m_pListCtrlBase->SelectItem(pNewFocusItem);
        
        this->m_pListCtrlBase->Invalidate();
    }
}
