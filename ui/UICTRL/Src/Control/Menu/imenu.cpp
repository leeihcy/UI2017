#include "stdafx.h"
#include "Inc\Interface\imenu.h"
#include "Src\Control\Menu\menu.h"
#include "Src\Control\Menu\item\menustringitem.h"
#include "Src\Control\Menu\item\menuseparatoritem.h"
#include "Src\Control\Menu\item\menupopupitem.h"

namespace UI
{
UI_IMPLEMENT_INTERFACE_ACROSSMODULE(MenuSeparatorItem, ListItemBase);
UI_IMPLEMENT_INTERFACE_ACROSSMODULE(MenuStringItem,    ListItemBase);
UI_IMPLEMENT_INTERFACE_ACROSSMODULE(MenuPopupItem,     ListItemBase);

UI_IMPLEMENT_INTERFACE_ACROSSMODULE(MenuStringItemShareData, ListItemShareData);
UI_IMPLEMENT_INTERFACE_ACROSSMODULE(MenuSeparatorItemShareData, ListItemShareData);
UI_IMPLEMENT_INTERFACE_ACROSSMODULE(MenuPopupItemShareData, ListItemShareData);

UI_IMPLEMENT_INTERFACE_ACROSSMODULE(Menu, ListCtrlBase)

void  IMenu::OnNewChildElement(IUIElement* pUIElement)
{ 
    __pImpl->OnNewChildElement(pUIElement); 
}
IMenu*  IMenu::GetSubMenuByPos(int nPos) 
{
    return __pImpl->GetSubMenuByPos(nPos);
}
IMenu*  IMenu::GetSubMenuById(long lId)  
{ 
    return __pImpl->GetSubMenuById(lId);
}

IMenuStringItem*  IMenu::AppendString(LPCTSTR szText, UINT nId) 
{ 
    return __pImpl->AppendString(szText, nId);
}
IMenuSeparatorItem*  IMenu::AppendSeparator(UINT nId) 
{
    return __pImpl->AppendSeparator(nId); 
}
IMenuPopupItem*  IMenu::AppendPopup(LPCTSTR szText, UINT nId, IMenu* pSubMenu)
{ 
    return __pImpl->AppendPopup(szText, nId, pSubMenu);
}

int   IMenu::TrackPopupMenu(UINT nFlag, int x, int y, IMessage* pNotifyObj, HWND hWndClickFrom, RECT* prcClickFrom) 
{ 
	return __pImpl->TrackPopupMenu(nFlag, x, y, pNotifyObj, hWndClickFrom, prcClickFrom); 
}
IWindow*  IMenu::CreateMenuWindow()
{
    return __pImpl->CreateMenuWindow();
}
IWindow*  IMenu::GetPopupWindow()
{
	return __pImpl->GetPopupWindow();
}
#if 0
UI_IMPLEMENT_INTERFACE2(IMenuItemBase, MenuItemBase, IListItemBase)
UI_IMPLEMENT_INTERFACE(IMenuItem, MenuItem, MenuItemBase)
UI_IMPLEMENT_INTERFACE2(IMenu, Menu, IListCtrlBase)

bool  IMenuItemBase::IsSeparator() { return m_pMenuItemBaseImpl->IsSeparator(); }
bool  IMenuItemBase::IsPopup() { return m_pMenuItemBaseImpl->IsPopup(); }     
bool  IMenuItemBase::IsChecked() { return m_pMenuItemBaseImpl->IsChecked(); }
bool  IMenuItemBase::IsRadioChecked() { return m_pMenuItemBaseImpl->IsRadioChecked(); }

void  IMenuItemBase::SetMenu(Menu* p) { m_pMenuItemBaseImpl->SetMenu(p); }
void  IMenuItemBase::SetSubMenu(IMenu* p) {  m_pMenuItemBaseImpl->SetSubMenu(p); }
IMenu*  IMenuItemBase::GetSubMenu() { return m_pMenuItemBaseImpl->GetSubMenu(); }

void  IMenuItemBase::SetFlag(UINT nFlag) { m_pMenuItemBaseImpl->SetFlag(nFlag); }
void  IMenuItemBase::SetID(UINT nID) { m_pMenuItemBaseImpl->SetID(nID); }
UINT  IMenuItemBase::GetFlag() { return m_pMenuItemBaseImpl->GetFlag(); }
UINT  IMenuItemBase::GetID() { return m_pMenuItemBaseImpl->GetID(); }

IMenu*  IMenu::GetRootMenu()
{
    Menu* p = __pImpl->GetRootMenu();
    if (p)
        return p->GetIMenu();

    return NULL;
}
IMenu*  IMenu::GetLastMenu()
{
    Menu* p = __pImpl->GetLastMenu();
    if (p)
        return p->GetIMenu();

    return NULL;
}
IMenu*  IMenu::GetPrevMenu()
{
    Menu* p = __pImpl->GetPrevMenu();
    if (p)
        return p->GetIMenu();

    return NULL;
}
IMenu*  IMenu::GetNextMenu()
{
    Menu* p = __pImpl->GetNextMenu();
    if (p)
        return p->GetIMenu();

    return NULL;
}
IMenu*  IMenu::GetMenuByHWND(HWND hWnd)
{
    Menu* p = __pImpl->GetMenuByHWND(hWnd);
    if (p)
        return p->GetIMenu();

    return NULL;
}
IMenu*  IMenu::GetMenuByPos(const POINT& ptScreen)
{
    Menu* p = __pImpl->GetMenuByPos(ptScreen);
    if (p)
        return p->GetIMenu();

    return NULL;
}

IRenderBase*  IMenu::GetSeparatorRender() { return __pImpl->GetSeparatorRender(); }
IRenderBase*  IMenu::GetPopupTriangleRender() { return __pImpl->GetPopupTriangleRender(); }
IRenderBase*  IMenu::GetRadioIconRender() { return __pImpl->GetRadioIconRender(); }
IRenderBase*  IMenu::GetCheckIconRender() { return __pImpl->GetCheckIconRender(); }
IRenderBase*  IMenu::GetRadioCheckIconBkRender() { return __pImpl->GetRadioCheckIconBkRender(); }

bool  IMenu::IsItemHilight(IMenuItem* p) 
{
    if (NULL == p)
        return false;

    return __pImpl->IsItemHilight(p->GetImpl()); 
}
HWND  IMenu::GetPopupWindowHandle() { return __pImpl->GetPopupWindowHandle(); }
int   IMenu::GetPopupTriangleWidth() { return __pImpl->GetPopupTriangleWidth(); }
int   IMenu::GetSeperatorHeight() { return __pImpl->GetSeperatorHeight(); }
#endif

int   IMenu::GetIconGutterWidth() { return __pImpl->GetIconGutterWidth(); }
}