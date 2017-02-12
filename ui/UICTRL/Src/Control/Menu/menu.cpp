#include "stdafx.h"
#include "menu.h"
#include "menu_desc.h"
#include "..\UISDK\Inc\Interface\ixmlwrap.h"
#include "..\UISDK\Inc\Interface\iuires.h"
#include "layout\menulayout.h"
#include "..\ListBase\PopupControlWindow\popupcontrolwindow.h"

Menu::Menu(IMenu* p):MessageProxy(p)
{
    m_pIMenu = p;
	memset(&m_menuStyle, 0, sizeof(MENUSTYLE));

    m_bPopupAtParentRight = true;
    m_nTrackPopupMenuFlag = 0;
    m_nRetCmd = 0;
    m_pPopupWrapWnd = NULL;
    m_pNextMenu = NULL;
    m_pPrevMenu = NULL;
    m_nTimerIDShowPopupSubMenu = 0;
    m_nTimerIDHidePopupSubMenu = 0;

    m_nIconGutterWidth = 28;
    m_nPopupTriangleWidth = 20;
    m_nSubMenuGap = -2;
    SetRectEmpty(&m_rWindowShadowSpace);

	p->SetDescription(MenuDescription::Get());

//     if (m_pIMenu->GetIScrollBarMgr())
//     {
//         m_pIMenu->GetIScrollBarMgr()->SetScrollBarVisibleType(HSCROLLBAR,SCROLLBAR_VISIBLE_NONE);
//         m_pIMenu->GetIScrollBarMgr()->SetScrollBarVisibleType(VSCROLLBAR,SCROLLBAR_VISIBLE_NONE);
//     }
// 
//     // TODO: 手动切换主题时如果确定是否要修改这些值？
//     if (IsThemeActive())
//     {
//         m_pIMenu->SetItemHeight(22, false);
//         m_nIconGutterWidth = 28;
//     }
//     else
//     {
//         m_pIMenu->SetItemHeight(18, false);
//         m_nIconGutterWidth = 20;
//     }
//     m_nPopupTriangleWidth = 20;
}

Menu::~Menu()
{
    destroyPopupWindow();
}

void  Menu::destroyAllSubMenu()
{
    IListItemBase* pItem = m_pIMenu->GetFirstItem();
    while (pItem)
    {
        if (!(pItem->GetItemFlag() & MF_POPUP))
        {
            pItem = pItem->GetNextItem();
            continue;
        }
        
        IMenu* pSubMenu = (IMenu*)pItem->GetCtrlData();
        if (!pSubMenu)
        {
            pItem = pItem->GetNextItem();
            continue;
        }

		MENUSTYLE style = { 0 };
		style.auto_release_submenu = 1;
        if (pSubMenu->GetImpl()->TestMenuStyle(style))
        {
            UIDestroyMenu(pSubMenu);
			pSubMenu = NULL;
        }
        pItem = pItem->GetNextItem();
    }
}

PARSE_CONTROL_RETURN  Menu::UIParseMenuTag(IUIElement* pElem, ISkinRes* pSkinRes, IObject* pParent, IObject** ppOut)
{
	IUIApplication* pUIApp = pSkinRes->GetUIApplication();

    IMenu*  pIMenu = IMenu::CreateInstance(pSkinRes);
    if (NULL == pIMenu)
        return ParseControl_Failed;

    *ppOut = static_cast<IObject*>(pIMenu);
	// TODO: 这里addchild并没有先获取zorder属性
	// addchild与zorder绑定不太合理
	if (pParent)
		pParent->AddChild(pIMenu);

    // 设置属性
    pIMenu->LoadAttributeFromXml(pElem, false);

    // 遍历其子元素
    IUIElementProxy childElement = pElem->FirstChild();
    while (childElement)
    {
        pIMenu->OnNewChildElement(childElement.get());
        childElement = childElement->NextElement();
    }

    return ParseControl_LoadDescendants;
}

void  Menu::OnNewChildElement(IUIElement* pUIElement)
{
    //	加载所有属性
    IMapAttribute*  pMapAttrib = NULL;
    pUIElement->GetAttribList(&pMapAttrib);

    String strTagName = pUIElement->GetTagName();

    // 特殊处理：扩展style，要将<string>扩展为menu.string..
    String strStyleClass, strID;

    LPCTSTR szText = pMapAttrib->GetAttr(XML_ID, false); // id = Menu.id，因为这里的id都是整数值
    if (szText)
    {
		strID = MenuDescription::Get()->GetTagName();
        strID.push_back(XML_CHILD_SEPARATOR);
        strID.append(szText);
    }

    szText = pMapAttrib->GetAttr(XML_STYLECLASS, true);
    if (szText)
        strStyleClass = szText;

    IStyleRes&  pStyleRes = m_pIMenu->GetSkinRes()->GetStyleRes();
    pStyleRes.LoadStyle(
        strTagName.c_str(), 
        strStyleClass.c_str(), 
        strID.c_str(),
        pMapAttrib);

    // 加载菜单子项
    IListItemBase* pItem = this->LoadMenuItem(pUIElement, strTagName.c_str(), pMapAttrib);
    SAFE_RELEASE(pMapAttrib);
}


// 
//	通过xml属性加载菜单项
//
IListItemBase* Menu::LoadMenuItem(IUIElement* pUIElement, LPCTSTR szTagName, IMapAttribute* pMapAttrib)
{
    int  nID = 0;
    pMapAttrib->GetAttr_int(XML_ID, true, &nID);

    if (0 == _tcscmp(szTagName, XML_MENU_STRINGITEM))
    {
        String strText;
        bool bDisable = false;

        LPCTSTR szText= pMapAttrib->GetAttr(XML_TEXT, true);
        if (szText)
            strText = szText;

        pMapAttrib->GetAttr_bool(XML_MENU_DISABLE, true, &bDisable);

        IListItemBase* pItem = this->AppendString(strText.c_str(), nID);
        if (pItem)
        {
            if (bDisable)
                pItem->SetDisable(true);

			SERIALIZEDATA data = {0};
            data.pUIApplication = m_pIMenu->GetUIApplication();
			data.pMapAttrib = pMapAttrib;
			data.nFlags = SERIALIZEFLAG_LOAD;
			UISendMessage(pItem, UI_MSG_SERIALIZE, (WPARAM)&data);
//            UISendMessage(pItem, UI_WM_SETATTRIBUTE, (WPARAM)pMapAttrib, (LPARAM)false);
        }
        return pItem;
    }
    else if (0 == _tcscmp(szTagName, XML_MENU_SEPARATORITEM))
    {
        IListItemBase* pItem = this->AppendSeparator(nID);
        if (pItem)
        {
			SERIALIZEDATA data = {0};
            data.pUIApplication = m_pIMenu->GetUIApplication();
			data.pMapAttrib = pMapAttrib;
			data.nFlags = SERIALIZEFLAG_LOAD;
			UISendMessage(pItem, UI_MSG_SERIALIZE, (WPARAM)&data);
        }
        return pItem;
    }
    else if (0 == _tcscmp(szTagName, XML_MENU_POPUPITEM))
    {
        String strText;
		String strPopupWndId;
		String strPopupMenuId;
        bool bDisable = false;

		LoadMenuData data = {0};
		data.pSkinRes = m_pIMenu->GetSkinRes();
		
        LPCTSTR szText = NULL;
		if (szText = pMapAttrib->GetAttr(XML_TEXT, true))
            strText = szText;

		if (szText = pMapAttrib->GetAttr(XML_MENU_POPUP_WNDID, true))
		{
			strPopupWndId = szText;
			data.szWndId = strPopupWndId.c_str();
		}
		
		if (szText = pMapAttrib->GetAttr(XML_MENU_POPUP_MENUID, true))
		{
			strPopupMenuId = szText;
			data.szMenuId = strPopupMenuId.c_str();
		}

        pMapAttrib->GetAttr_bool(XML_MENU_DISABLE, true, &bDisable);

		IMenu* pSubMenu = UILoadMenu(&data);  // 有可能失败。即在皮肤里没有配置具体子菜单，由业务逻辑添加
		if (pSubMenu)
		{
			MENUSTYLE style = { 0 };
			style.auto_release_submenu = 1;
			pSubMenu->GetImpl()->ModifyMenuStyle(&style, nullptr);
		}
		
        IListItemBase* pItem = this->AppendPopup(strText.c_str(), nID, pSubMenu);
        if (NULL == pItem)
        {
            // UI_LOG_WARN(_T("AppendMenu failed."));
            UIDestroyMenu(pSubMenu);
			pSubMenu = NULL;
			return NULL;
        }

		if (bDisable)
			pItem->SetDisable(true);

		{
			SERIALIZEDATA data = {0};
            data.pUIApplication = m_pIMenu->GetUIApplication();
			data.pMapAttrib = pMapAttrib;
			data.nFlags = SERIALIZEFLAG_LOAD;
			UISendMessage(pItem, UI_MSG_SERIALIZE, (WPARAM)&data);
		}
        return pItem;
    }
    else
    {
        // 可能是菜单里的Button（滚动条）
        IObject* pObj = m_pIMenu->GetSkinRes()->GetLayoutManager().
            ParseElement(pUIElement, m_pIMenu); 
        
        if (!pObj)
        {
            // UI_LOG_WARN(_T("unknown tagname: %s"), szTagName);
        }
    }
    return NULL;
}


IMenuStringItem*  Menu::AppendString(LPCTSTR szText, UINT nId)
{
    // 检查ID是否存在
    if (m_pIMenu->GetItemById(nId))
    {
        //UI_LOG_WARN(_T("该id已经存在, text=%s, id=%d"), szText, nId);
        return NULL;
    }

    IMenuStringItem* pItem = IMenuStringItem::CreateInstance(m_pIMenu->GetSkinRes());

    if (false == m_pIMenu->AddItem(pItem))
    {
        SAFE_RELEASE(pItem);
        return NULL;
    }

    pItem->SetText(szText);
    pItem->SetId(nId);
    pItem->SetItemFlag(MF_STRING);

    return pItem;
}
IMenuSeparatorItem*  Menu::AppendSeparator(UINT nId)
{
    IMenuSeparatorItem* pItem = IMenuSeparatorItem::
			CreateInstance(m_pIMenu->GetSkinRes());

    if (false == m_pIMenu->AddItem(pItem))
    {
		SAFE_RELEASE(pItem);
        return NULL;
    }

    pItem->SetId(nId);
    pItem->SetDisable(true);
    pItem->SetItemFlag(MF_SEPARATOR);

    return pItem;
}
IMenuPopupItem*  Menu::AppendPopup(LPCTSTR szText, UINT nId, IMenu* pSubMenu)
{
    IMenuPopupItem* pItem = IMenuPopupItem::
		CreateInstance(m_pIMenu->GetSkinRes());

    if (false == m_pIMenu->AddItem(pItem))
    {
		SAFE_RELEASE(pItem);
        return NULL;
    }

    pItem->SetText(szText);
    pItem->SetId(nId);
    pItem->SetCtrlData((LPARAM)pSubMenu);
    pItem->SetItemFlag(MF_POPUP);

    return pItem;
}

HRESULT  Menu::FinalConstruct(ISkinRes* p)
{
    DO_PARENT_PROCESS(IMenu, IListCtrlBase);
    if (FAILED(GetCurMsg()->lRet))
        return GetCurMsg()->lRet;

    MenuItemLayout* pLayout = NULL;
    MenuItemLayout::CreateInstance(&pLayout);
    m_pIMenu->SetLayout(pLayout);

	LISTCTRLSTYLE s = {0};
	s.menu = 1;
	m_pIMenu->ModifyListCtrlStyle(&s, 0);

#ifdef EDITOR_MODE
	if (p->GetUIApplication()->IsEditorMode())
	{
		AppendString(L"StringItem", 1)->SetChecked(true);
		AppendSeparator(2);
		AppendPopup(L"Popup", 3, 0);
	}
#endif

    return S_OK;
}
void  Menu::FinalRelease()
{
    SetMsgHandled(FALSE);
    destroyAllSubMenu();
}

void  Menu::OnIntialize()
{
    DO_PARENT_PROCESS(IMenu, IListCtrlBase);

    m_scrollbarCreator.Initialize(m_pIMenu, m_pIMenu->GetIScrollBarMgr());
}

void  Menu::OnSerialize(SERIALIZEDATA* pData)
{
    DO_PARENT_PROCESS(IMenu, IListCtrlBase);

	AttributeSerializerWrap s(pData, TEXT("Menu"));

	s.AddLong(XML_MENU_ICONGUTTERWIDTH, m_nIconGutterWidth)
		->SetDpiScaleType(LONGATTR_DPI_SCALE_ENABLE);
	s.AddLong(XML_MENU_POPUPTRIANGLEWIDTH, m_nPopupTriangleWidth)
		->SetDpiScaleType(LONGATTR_DPI_SCALE_ENABLE);
	s.AddLong(XML_MENU_SUBMENU_GAP, m_nSubMenuGap)
		->SetDpiScaleType(LONGATTR_DPI_SCALE_ENABLE);
}

IListItemBase* Menu::GetMenuItemBySubMenu(IMenu* pSubMenu)
{
    IListItemBase* pItem = m_pIMenu->GetFirstItem();
    UINT nItemFlag = 0;
    while (pItem)
    {
        if (pItem->GetItemFlag() & MF_POPUP &&
            pItem->GetId() == (long)pSubMenu)
            return pItem;

        pItem = pItem->GetNextItem();
    }
    return NULL;
}

IMenu* Menu::GetSubMenuByPos(int nPos)
{
    IListItemBase* pItem = m_pIMenu->GetItemByPos(nPos);
    if (NULL == pItem)
        return NULL;

    if (pItem->GetItemFlag() & MF_POPUP)
    {
        return (IMenu*)pItem->GetCtrlData();
    }

    return NULL;
}
IMenu*  Menu::GetSubMenuById(long lId)
{
    IListItemBase* pItem = m_pIMenu->GetItemById(lId);
    if (NULL == pItem)
        return NULL;

    if (pItem->GetItemFlag() & MF_POPUP)
    {
        return (IMenu*)pItem->GetCtrlData();
    }

    return NULL;
}


int  Menu::TrackPopupMenu(
		UINT nFlag, int x, int y, 
		IMessage* pNotifyObj, 
		HWND hWndClickFrom,
		RECT* prcClickFrom)
{
	if (!m_pPopupWrapWnd || NULL == m_pPopupWrapWnd->GetHWND())
		return -1;
    if (0 >= m_pIMenu->GetItemCount())
        return -1;

    m_nTrackPopupMenuFlag = nFlag;
    m_nRetCmd = 0;

#if 0
    if (NULL == m_pPopupWrapWnd->GetHWND())
    {
        // TODO: 
        // 注：这里不能传递父窗口。存在这么一种场景，父窗口销毁时，将会把子窗口的菜单窗口也销毁
        //     当trackpopupmenu返回时，再调用SAFE_DELETE_Ixxx将导致野指针崩溃.
        m_pPopupWrapWnd->Create(m_pIMenu, m_pIMenu, _T("PopupMenuWindow"), NULL/*hWndClickFrom*/);
    }
#endif

    m_pIMenu->SetNotify(pNotifyObj, 0);
	m_pPopupWrapWnd->SetPopupFromInfo(hWndClickFrom, prcClickFrom);

    POINT pt = { x, y };
    SIZE size = {0};
    //SIZE size = m_pIMenu->GetDesiredSize();  // 这个方法优化获取的根据父对象的布局。而这里应该直接拿列表的大小
    //UISendMessage(m_pIMenu, UI_WM_GETDESIREDSIZE, (WPARAM)&size);  // 直接获取列表的大小
	size = m_pPopupWrapWnd->GetDesiredSize();


    if (nFlag & UI_TPM_RIGHTALIGN)
    {
        pt.x -= size.cx;
    }
    if (nFlag & UI_TPM_CENTERALIGN)
    {
        pt.x -= size.cx / 2;
    }
    if (nFlag & UI_TPM_BOTTOMALIGN)
    {
        pt.y -= size.cy;
    }
    if (nFlag & UI_TPM_VCENTERALIGN)
    {
        pt.y -= size.cy / 2;
    }

    // 阴影偏移量
    if (nFlag & UI_TPM_RIGHTALIGN)
    {
        pt.x += m_rWindowShadowSpace.right;
    }
    else if (nFlag & UI_TPM_CENTERALIGN){}
    else
    {
        pt.x -= m_rWindowShadowSpace.left;
    }

    if (nFlag & UI_TPM_BOTTOMALIGN)
    {
        pt.y += m_rWindowShadowSpace.bottom;
    }
    else if (nFlag & UI_TPM_VCENTERALIGN) {}
    else
    {
        pt.y -= m_rWindowShadowSpace.top;
    }

    m_pPopupWrapWnd->Show(pt, size, 
        (nFlag&TPM_RETURNCMD)?TRUE:FALSE,
        (nFlag&UI_TPM_SHOWACTIVATE) ? SW_SHOW:SW_SHOWNOACTIVATE);

    return m_nRetCmd;
}

// 本函数主要用于UIEditor当中显示菜单图像
IWindow*  Menu::CreateMenuWindow()
{
    if (!m_pIMenu->GetUIApplication()->IsEditorMode())
        return NULL;

    if (NULL == m_pPopupWrapWnd->GetHWND())
    {
        m_pPopupWrapWnd->Create(_T("PopupMenuWindow"), NULL);
		m_pPopupWrapWnd->SetBindObject(m_pIMenu);
		m_pPopupWrapWnd->SetContentObject(m_pIMenu);
    }
    POINT pt = { 0, 0 };
    SIZE size = m_pIMenu->GetDesiredSize();
    m_pPopupWrapWnd->Show(pt, size, FALSE, TRUE);

    return GetPopupWindow();
}

void  Menu::SetReturnCmd(UINT n) 
{
	m_nRetCmd = n; 
}

HWND Menu::GetPopupWindowHandle()
{
    if (NULL == m_pPopupWrapWnd)
    {
        return NULL;
    }
    return m_pPopupWrapWnd->GetHWND();
}
IWindow*  Menu::GetPopupWindow()
{
	return m_pPopupWrapWnd;
}
void   Menu::SetPopupWindow(PopupMenuWindow* pWindow)
{
	m_pPopupWrapWnd = pWindow;
}

int   Menu::GetIconGutterWidth()
{
    return m_nIconGutterWidth;
}

Menu* Menu::GetRootMenu()
{
    Menu* pRet = this;
    while (NULL != pRet)
    {
        if (NULL == pRet->GetPrevMenu())
            break;

        pRet = pRet->GetPrevMenu();
    }
    return pRet;
}

Menu* Menu::GetLastMenu()
{
    Menu* pRet = this;
    while (NULL != pRet)
    {
        if (NULL == pRet->GetNextMenu())
            break;

        pRet = pRet->GetNextMenu();
    }
    return pRet;
}

Menu* Menu::GetMenuByHWND(HWND hWnd)
{
    Menu* pMenu = this->GetRootMenu();
    while(NULL != pMenu)
    {
        if (hWnd == pMenu->GetPopupWindowHandle())
            return pMenu;

        pMenu = pMenu->GetNextMenu();
    }
    return NULL;
}


Menu* Menu::GetMenuByPos(const POINT& ptScreen)
{
    HWND hWnd = WindowFromPoint(ptScreen);
    return GetMenuByHWND(hWnd);
}

void  Menu::destroyPopupWindow()
{
    m_pNextMenu = NULL;
	m_pIMenu->RemoveMeInTheTree();

	m_pPopupWrapWnd = nullptr;  // 销毁一是由窗口发起的，菜单控件销毁时，不用管
// 	if (m_pPopupWrapWnd)
// 	{
// 		m_pPopupWrapWnd->Destroy();
// 		m_pPopupWrapWnd->Release();
// 	}

    if (0 != m_nTimerIDHidePopupSubMenu)
    {
        m_pIMenu->GetUIApplication()->KillTimer(m_nTimerIDHidePopupSubMenu);
    }
    if (0 != m_nTimerIDShowPopupSubMenu)
    {
        m_pIMenu->GetUIApplication()->KillTimer(m_nTimerIDShowPopupSubMenu);
    }
    if (m_pPrevMenu)
    {
        m_pPrevMenu->m_pNextMenu = NULL;
        m_pPrevMenu = NULL;
    }
}

void  Menu::_HidePopupWindow()
{
    if (m_pNextMenu)
    {
        m_pNextMenu->_HidePopupWindow();
        m_pNextMenu = NULL;
    }

    if (m_pPopupWrapWnd)
    {
        m_pPopupWrapWnd->Hide();
    }
    if (m_pPrevMenu)
    {
        m_pPrevMenu->m_pNextMenu = NULL;
        m_pPrevMenu = NULL;
    }
}


// TODO: VK_LEFT VK_RIGHT键
void Menu::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
    if (VK_DOWN == nChar)
    {
        IListItemBase* pOldHoverItem = m_pIMenu->GetHoverItem();
        m_pIMenu->SetHoverItem(GetNextSelectableItem(pOldHoverItem));

        // 		if (GetHoverItem() != pOldHoverItem)
        // 			this->ReDrawItem(GetHoverItem(), pOldHoverItem);
        m_pIMenu->InvalidateItem(m_pIMenu->GetHoverItem());
        m_pIMenu->InvalidateItem(pOldHoverItem);
    }
    else if (VK_UP == nChar)
    {
        IListItemBase* pOldHoverItem = m_pIMenu->GetHoverItem();
        m_pIMenu->SetHoverItem(GetPrevSelectableItem(pOldHoverItem));

        if (m_pIMenu->GetHoverItem() != pOldHoverItem)
        {
            m_pIMenu->InvalidateItem(m_pIMenu->GetHoverItem());
            m_pIMenu->InvalidateItem(pOldHoverItem);
        }
    }
    else if (VK_RIGHT == nChar)
    {
        IListItemBase*  pHoverItem = m_pIMenu->GetHoverItem();
        if (pHoverItem && (pHoverItem->GetItemFlag() & MF_POPUP))
        {
            if (0 != m_nTimerIDHidePopupSubMenu)
            {
                m_pIMenu->GetUIApplication()->KillTimer(m_nTimerIDHidePopupSubMenu);
            }
            if (0 != m_nTimerIDShowPopupSubMenu)
            {
                m_pIMenu->GetUIApplication()->KillTimer(m_nTimerIDShowPopupSubMenu);
            }
            this->PopupSubMenu(pHoverItem);
        }
        else if (!m_bPopupAtParentRight && NULL != m_pPrevMenu)  // 当自己在父菜单的左侧弹出时，将右键改为销毁子菜单
        {
            this->_HidePopupWindow();
        }
    }
    else if (VK_LEFT == nChar)
    {
        if (m_pPrevMenu)
        {
            this->_HidePopupWindow();
        }
    }
    else if (VK_ESCAPE == nChar)
    {
        this->_HidePopupWindow();
    }
    else if (VK_RETURN == nChar)
    {
        IListItemBase*  pItem = m_pIMenu->GetHoverItem();
        if (pItem)
        {
            OnClick(pItem);
        }
    }
}

// 在pretranslate中屏蔽该消息，以解决在按钮上狂按空格，会一直弹出菜单的问题
void  Menu::OnKeyUp( UINT nChar, UINT nRepCnt, UINT nFlags )
{

}

//
//	被GetNextSelectableItem内部调用
//  当一次没有找到时，会被再调用一次重头找起
//
IListItemBase* Menu::_GetNextSelectableItem(IListItemBase* pItem)
{
    if (NULL == pItem)
    {
        pItem = m_pIMenu->GetFirstItem();
        if (NULL == pItem)
            return NULL;
    }
    else
    {
        pItem = pItem->GetNextItem();
        if (NULL == pItem)
            return NULL;
    }

    while (pItem)	
    {
        if(pItem->IsDisable())
        {
            pItem = pItem->GetNextItem();
            continue;
        }
        else
        {
            break;
        }
    }

    return pItem;
}
IListItemBase* Menu::GetNextSelectableItem(IListItemBase* pItem)
{
    IListItemBase* pItemRet = _GetNextSelectableItem(pItem);
    if (NULL == pItemRet)
    {
        if (NULL == pItem)  // 已经是从头开始查找的，但还是返回NULL
            return NULL;
        else                // 再重头找一次
            pItemRet = _GetNextSelectableItem(NULL);
    }

    return pItemRet;
}

IListItemBase* Menu::_GetPrevSelectableItem(IListItemBase* pItem)
{
    if (NULL == pItem)
    {
        pItem = m_pIMenu->GetLastItem();
        if (NULL == pItem)
            return NULL;
    }
    else
    {
        pItem = pItem->GetPrevItem();
        if (NULL == pItem)
            return NULL;
    }

    while (pItem)	
    {
        if(pItem->IsDisable())
        {
            pItem = pItem->GetPrevItem();
            continue;
        }
        else
        {
            break;
        }
    }

    return pItem;
}
IListItemBase* Menu::GetPrevSelectableItem(IListItemBase* pItem)
{
    IListItemBase* pItemRet = _GetPrevSelectableItem(pItem);
    if (NULL == pItemRet)
    {
        if (NULL == pItem)  // 已经是从头开始查找的，但还是返回NULL
            return NULL;
        else                // 再重头找一次
            pItemRet = _GetPrevSelectableItem(NULL);
    }

    return pItemRet;
}

LRESULT  Menu::OnHoverItemChanged(WPARAM w, LPARAM l)
{
    IListItemBase*  pOldHover = (IListItemBase*)w;
    IListItemBase*  pNewHover = (IListItemBase*)l;

    if (pNewHover)
    {
        if ((pNewHover->GetItemFlag() & MF_POPUP) && pNewHover->GetCtrlData() && !pNewHover->IsDisable())
        {
            ShowPopupSubMenu(pNewHover);
        }
        else
        {
            HidePopupSubMenu();
        }
    }
    return 0;
}
//
//  鼠标在子菜单上移动时，在父窗口上显示高亮路径（递归）
//
void Menu::OnMouseMove(UINT nFlags, CPoint point)
{
    DO_PARENT_PROCESS(IMenu, IListCtrlBase);
    if (m_pPrevMenu)
    {
        m_pPrevMenu->OnSubMenuMouseMove(m_pIMenu);
    }
}
void Menu::OnMouseLeave()
{
    // 为了解决从父菜单的popup menu item直接移动到子菜单上面时,
    // 会收到一个MOUSELEAVE消息，导致hover item闪烁一下的问题。
    // 当发现鼠标是移到了子菜单上面时，不去清空当前的hover item
    POINT  pt;
    GetCursorPos(&pt);
    Menu* pMenu = this->GetMenuByPos(pt);
    if (IsMyChildMenu(pMenu))
    {
        SetMsgHandled(TRUE);
    }
	else
	{
		DO_PARENT_PROCESS(IMenu, IListCtrlBase);
	}

    // 取消将要显示的菜单动作
    if (0 != m_nTimerIDShowPopupSubMenu)
    {
        m_pIMenu->GetUIApplication()->KillTimer(m_nTimerIDShowPopupSubMenu);
    }
}
void Menu::OnSubMenuMouseMove(IMenu* pSubMenu)
{
    if (0 != m_nTimerIDHidePopupSubMenu)  // 防止父菜单的销毁定时器将子菜单又销毁了
        m_pIMenu->GetUIApplication()->KillTimer(m_nTimerIDHidePopupSubMenu);

    IListItemBase* pItem = m_pIMenu->GetFirstItem();
    while (pItem)
    {
        if (pItem->GetItemFlag() & MF_POPUP &&
            pItem->GetCtrlData() == (WPARAM)pSubMenu)
        {
            IListItemBase* pOldHover = m_pIMenu->GetHoverItem();
            m_pIMenu->SetHoverItem(pItem);

            if (m_pPrevMenu)
                m_pPrevMenu->OnSubMenuMouseMove(m_pIMenu);

            m_pIMenu->InvalidateItem(pOldHover);
            m_pIMenu->InvalidateItem(pItem);

            break;
        }
        pItem = pItem->GetNextItem();
    }

    //m_pIMenu->Refresh();  // 由于这是子菜单的mousemove，不会调用父菜单的refresh，因此这里主动刷新
}


void Menu::OnLButtonDown(UINT nFlags, POINT point)
{
    DO_PARENT_PROCESS(IMenu, IListCtrlBase);

    IListItemBase* pPressItem = m_pIMenu->GetPressItem();
    if (pPressItem && (pPressItem->GetItemFlag()&MF_POPUP))
    {
        if (0 != m_nTimerIDHidePopupSubMenu)
        {
            m_pIMenu->GetUIApplication()->KillTimer(m_nTimerIDHidePopupSubMenu);
        }
        if (0 != m_nTimerIDShowPopupSubMenu)
        {
            m_pIMenu->GetUIApplication()->KillTimer(m_nTimerIDShowPopupSubMenu);
        }
        IMenu* pSubMenu = (IMenu*)pPressItem->GetCtrlData();
        if (pSubMenu)
        {
            this->PopupSubMenu(pPressItem);
        }
        m_pIMenu->SetPressItem(NULL);

        return;
    }
}
void Menu::OnLButtonUp(UINT nFlags, POINT point)
{
    SetMsgHandled(FALSE);

//     IListItemBase* pSavePress = m_pIMenu->GetPressItem();
//     IListItemBase* pSaveHover = m_pIMenu->GetHoverItem();
//     if (pSavePress && pSavePress == pSaveHover)
//     {
//         if (pSavePress->IsDisable())
//             return;
// 
//         OnClick(pSavePress);
//     }

	IListItemBase* pSaveHover = m_pIMenu->GetHoverItem();
	if (pSaveHover)
	{
		if (pSaveHover->IsDisable())
			return;

		OnClick(pSaveHover);
	}
}

int  Menu::PopupSubMenu(IListItemBase* pItem)
{
    if (NULL == pItem)
        return -1;

    if (!(pItem->GetItemFlag() & MF_POPUP))
        return -1;

    IMenu* pISubMenu = (IMenu*)pItem->GetCtrlData();
    if (NULL == pISubMenu)
        return -1;

    Menu* pSubMenu = pISubMenu->GetImpl();
    if(pSubMenu == m_pNextMenu)
    {
        // UI_LOG_WARN(_T("m_pNextMenu is same to pSubMenu"));
        return -1;
    }

    if (m_pNextMenu != NULL)
    {
        m_pNextMenu->_HidePopupWindow();
        m_pNextMenu = NULL;
    }

    int nRet = pSubMenu->PopupAsSubMenu(m_nTrackPopupMenuFlag, this, pItem);
    if (-1 != nRet)
    {
        m_pNextMenu = pSubMenu;
    }
    return nRet;
}

// 自己作为子菜单弹出来
int  Menu::PopupAsSubMenu(UINT nFlags, Menu* pParentMenu, IListItemBase* pItem)
{
    if (0 >= m_pIMenu->GetItemCount() || !m_pPopupWrapWnd)
        return -1;

    m_nTrackPopupMenuFlag = nFlags;
    m_pPrevMenu = pParentMenu;

    HWND hParentWnd = pParentMenu->GetPopupWindowHandle();

    SIZE size = m_pPopupWrapWnd->GetDesiredSize();

    // 计算弹出位置
    int x = 0, y = 0;
    CRect rcParent, rcItem;
    ::GetWindowRect(hParentWnd, &rcParent);
    pItem->GetParentRect(&rcItem);

    x = rcParent.right + m_nSubMenuGap;
    y = rcParent.top + rcItem.top;

    CRect rcWorkArea;
    MONITORINFO mi = {sizeof(MONITORINFO), 0};
    if (GetMonitorInfo(MonitorFromWindow(hParentWnd, MONITOR_DEFAULTTONEAREST), &mi))
    {
        rcWorkArea = mi.rcWork;
    }
    else
    {
        ::SystemParametersInfo(SPI_GETWORKAREA, 0, &rcWorkArea, 0);
    }

    m_bPopupAtParentRight = true;

    if (x + size.cx > rcWorkArea.right)
    {
        m_bPopupAtParentRight = false;
        x = rcParent.left - size.cx - m_nSubMenuGap;
    }

    if (x < rcWorkArea.left)
        x = rcWorkArea.left;

    if (y < rcWorkArea.top)
        y = rcWorkArea.top;
    if (y + size.cy > rcWorkArea.bottom)
        y = rcWorkArea.bottom - size.cy;

    ::SetWindowPos(m_pPopupWrapWnd->GetHWND(), HWND_TOP, x,y, 
        size.cx, size.cy, SWP_SHOWWINDOW|SWP_NOACTIVATE);  // 增加HWND_TOP,避免子菜单跑到父菜单的下面去了

    m_pIMenu->ClearNotify();
    pParentMenu->GetIMenu()->CopyNotifyTo(m_pIMenu);

    return 0;
}

void  Menu::OnClick(IListItemBase* pClickItem)
{
    if (NULL == pClickItem)
        return;

    if (pClickItem->GetItemFlag() & MF_POPUP)
    {
        PopupSubMenu(pClickItem);
        return;
    }

    this->GetRootMenu()->_HidePopupWindow();

    if (m_nTrackPopupMenuFlag & TPM_RETURNCMD)
    {
        this->GetRootMenu()->SetReturnCmd(pClickItem->GetId());
    }
    else
    {
		UIASSERT(0); // 未实现
//         UIMSG  msg;
//         msg.message = UI_MSG_NOTIFY;
//         msg.nCode = UI_MENU_CLICK;
//         msg.wParam = (WPARAM)pClickItem->GetId();
//         msg.pMsgFrom = m_pIMenu;
//         m_pIMenu->DoNotify(&msg, true, m_pIMenu->GetUIApplication());
    }
}

void Menu::OnThemeChanged()
{
    SetMsgHandled(FALSE);

//     if (m_pSeparatorRender)
//         m_pSeparatorRender->CheckThemeChanged();
//     if (m_pPopupTriangleRender)
//         m_pPopupTriangleRender->CheckThemeChanged();
//     if (m_pRadioIconRender)
//         m_pRadioIconRender->CheckThemeChanged();
//     if (m_pCheckIconRender)
//         m_pCheckIconRender->CheckThemeChanged();
}



// 检测pMenu是否是我的子孙菜单
bool Menu::IsMyChildMenu(Menu* pMenu)
{
    if (NULL == pMenu)
        return false;

    Menu* pChildMenu = m_pNextMenu;
    while (NULL != pChildMenu)
    {
        if (pChildMenu == pMenu)
            return true;

        pChildMenu = pChildMenu->m_pNextMenu;
    }
    return false;
}


void Menu::OnTimer(UINT_PTR nIDEvent, LPARAM lParam)
{
    TimerItem* pItem = (TimerItem*)lParam;
    if (NULL == pItem)
        return;

    if (nIDEvent == m_nTimerIDHidePopupSubMenu)
    {
        if (m_pNextMenu)
        {
            // 放在DestroyPopupWindow之前，因为DestroyPopupWindow之后，会将m_pNextMenu清空
            IListItemBase* pItem = GetMenuItemBySubMenu(m_pNextMenu->GetIMenu());  

            m_pNextMenu->_HidePopupWindow();
            m_pNextMenu = NULL;

            if (pItem)
            {
                m_pIMenu->InvalidateItem(pItem);
            }
        }
        m_nTimerIDHidePopupSubMenu = 0;
    }
    else if (nIDEvent == m_nTimerIDShowPopupSubMenu)
    {
        IListItemBase*  pMenuItem = (IListItemBase*)(pItem->wParam); 
        IMenu* pSubMenu = (IMenu*)pMenuItem->GetCtrlData();
        if (m_pNextMenu && pSubMenu != m_pNextMenu->GetIMenu())  // 上一个未关闭的popup菜单
        {
            m_pNextMenu->_HidePopupWindow();
            m_pNextMenu = NULL;
        }

        if (pSubMenu)
        {
            this->PopupSubMenu((IListItemBase*)(pItem->wParam));
        }

        m_nTimerIDShowPopupSubMenu = 0;
    }
}


void Menu::ShowPopupSubMenu(IListItemBase* pItem)
{
    if (0 != m_nTimerIDShowPopupSubMenu)
        m_pIMenu->GetUIApplication()->KillTimer(m_nTimerIDShowPopupSubMenu);
    if (0 != m_nTimerIDHidePopupSubMenu)
        m_pIMenu->GetUIApplication()->KillTimer(m_nTimerIDHidePopupSubMenu);

    if (m_pNextMenu && pItem->GetCtrlData() == (WPARAM)m_pNextMenu->GetIMenu()) // 当前显示的就是该子菜单
        return;

    TimerItem  ti;
    ti.nRepeatCount = 1;
    ti.pNotify = m_pIMenu;
    ti.wParam = (WPARAM)pItem;
    m_nTimerIDShowPopupSubMenu = m_pIMenu->GetUIApplication()->SetTimer(500, &ti);
}
void Menu::HidePopupSubMenu()
{
    if (0 != m_nTimerIDShowPopupSubMenu)
    {
        m_pIMenu->GetUIApplication()->KillTimer(m_nTimerIDShowPopupSubMenu);
    }
    if (0 == m_nTimerIDHidePopupSubMenu && NULL != m_pNextMenu)  // 有菜单弹出没有关闭
    {
        TimerItem  ti;
        ti.nRepeatCount = 1;
        ti.pNotify = m_pIMenu;
        m_nTimerIDHidePopupSubMenu = m_pIMenu->GetUIApplication()->SetTimer(500, &ti);
    }
}


void Menu::OnInitPopupControlWindow()
{

}

// 自己的菜单窗口隐藏了
void Menu::OnUnInitPopupControlWindow()
{
    if (m_pNextMenu)
    {
        m_pNextMenu->_HidePopupWindow();
        m_pNextMenu = NULL;
    }
    if (m_pPrevMenu)
    {
        m_pPrevMenu->m_pNextMenu = NULL;
        m_pPrevMenu = NULL;
    }
    m_pIMenu->SetHoverItem(NULL);
}


// 
// bool Menu::IsItemHilight(MenuItem* p)
// {
// 	if (p->IsPress() || (NULL == GetPressItem() && p->IsHover()))
// 	{
// 		return true;
// 	}
// 	else if (m_pNextMenu && p->GetSubMenu() == m_pNextMenu->GetIMenu() && NULL == GetHoverItem())
// 	{
// 		// 注：该判断是用于防止鼠标从p移动到它的子菜单上时，中间会有一个p失去hover的状态，然后又因为移动到子菜单
// 		//     上面又得到hover，导致界面闪烁一次。因此当发现没有hover项时，将p置为hover项。（光在这里返回true 还
// 		//     不够，否则鼠标移到p的上一个item时，不会去刷新p
// 		SetHoverItem(p);
// 		return true;
// 	}
// 	else
// 	{
// 		return false;
// 	}
// }


	extern "C"
	{
		static DefaultMenuData  g_defaultMenuData = { TEXT("Menu.Default"), TEXT("") };
		void  UISetDefaultMenuData(DefaultMenuData* pData)
		{
			if (!pData)
				return;

			_tcsncpy(g_defaultMenuData.szDefaultWndId, pData->szDefaultWndId,
				sizeof(g_defaultMenuData.szDefaultWndId) / sizeof(TCHAR) - 1);
			_tcsncpy(g_defaultMenuData.szDefaultMenuId, pData->szDefaultMenuId,
				sizeof(g_defaultMenuData.szDefaultMenuId) / sizeof(TCHAR) - 1);
		}

		// 从配置中加载一个菜单项
		// 	bool  Menu::LoadMenu(IUIApplication* pUIApplication, LPCTSTR szMenuId, IMenu** ppMenu)
		// 	{
		// 		if (NULL == szMenuId || NULL == ppMenu || NULL == pUIApplication)
		// 			return false;
		// 
		// 		ILayoutManager*  pLayoutMgr = pUIApplication->GetActiveSkinLayoutMgr();
		// 		if (NULL == pLayoutMgr)
		// 			return false;
		// 
		// 		IObject* pObject = NULL;
		// 		if (FAILED(pLayoutMgr->LoadRootLayout(Menu::GetXmlName(), szMenuId, &pObject)) || NULL == pObject)
		// 			return false;
		// 
		// 		*ppMenu = (IMenu*)pObject->QueryInterface(uiiidof(IMenu));
		// 		if (NULL == ppMenu)
		// 			return false;
		// 
		// 		return true;
		// 	}

		IMenu*  UILoadMenu(LoadMenuData* pData)
		{
			if (!pData || !pData->pSkinRes /*|| !pData->szWndId || !pData->szMenuId*/)
				return NULL;

			LPCTSTR szWndId = pData->szWndId;
			LPCTSTR szMenuId = pData->szMenuId;
			if (!szWndId)
				szWndId = g_defaultMenuData.szDefaultWndId;
			if (!szMenuId)
				szMenuId = g_defaultMenuData.szDefaultMenuId;

			IMenu*  pMenu = NULL;
			PopupMenuWindow*  pPopupWindow = NULL;
			do
			{
				pPopupWindow = PopupMenuWindow::CreateInstance(pData->pSkinRes);
				// TODO: 这里默认了所有的弹出菜单都是直接置顶的，没有owner窗口。
				// 原因：在maindlg中保存了一个IMenu，在maindlg销毁时会将所有子窗口也销毁掉，导致menu窗口也被
				// 销毁，这时再在maindlg的析构中销毁imenu就会成为野指针崩溃
				bool bRet = pPopupWindow->Create(szWndId, NULL, NULL);
				if (!bRet)
				{
					break;

					// 如果使用的是默认菜单窗口，而且没有在皮肤中默认默认菜单窗口资料，自动创建一个空白窗口和菜单
					// 				if (0 != _tcscmp(szWndId, g_defaultMenuData.szDefaultWndId))
					// 					break;
					// 
					// 				IMenu::CreateInstance(pData->pUIApplication, &pMenu);
					// 				pPopupWindow->AddChild(pMenu);
				}

				// 如果父窗口是置顶的，这里也置顶
				if (pData->hWndParent && GetWindowLong(pData->hWndParent, GWL_EXSTYLE)&WS_EX_TOPMOST)
				{
					SetWindowPos(pPopupWindow->GetHWND(), HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
				}

				if (!pMenu)
				{
					IObject* pCtrl = pPopupWindow->FindObject(szMenuId);
					if (!pCtrl)
						break;

					pMenu = (IMenu*)pCtrl->QueryInterface(__uuidof(IMenu));
					if (!pMenu)
						break;
				}

				pMenu->GetImpl()->SetPopupWindow(pPopupWindow);
				pPopupWindow->SetBindObject(pMenu);
				pPopupWindow->SetContentObject(pMenu);

				return pMenu;
			} while (0);

			if (pPopupWindow)
			{
				pPopupWindow->DestroyWindow();
				SAFE_RELEASE(pPopupWindow);
			}

			return NULL;
		}
		bool  UIDestroyMenu(IMenu* pMenu)
		{
			if (!pMenu)
				return false;

			IWindow* pWnd = pMenu->GetImpl()->GetPopupWindow();
			if (!pWnd)
				return false;

			// TODO: 如果窗口是在track时再创建出来的，这么处理就不对了
			pWnd->DestroyWindow();
			SAFE_RELEASE(pWnd);
			return true;
		}
	}

void  Menu::ModifyMenuStyle(MENUSTYLE* add, MENUSTYLE* remove)
{
#define __REMOVE(x) \
    if (remove->x) m_menuStyle.x = 0;
#define __ADD(x) \
	if (add->x) m_menuStyle.x = 1;

	if (add)
	{
		__ADD(auto_release_submenu)
	}
	if (remove)
	{
		__REMOVE(auto_release_submenu);
	}
}

bool  Menu::TestMenuStyle(const MENUSTYLE& test)
{
#define __TEST(x) \
    if (test.x && !m_menuStyle.x) return false; 

	__TEST(auto_release_submenu);

	return true; 
}