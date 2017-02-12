#ifndef _UI_IMENU_H_
#define _UI_IMENU_H_
#include "..\..\..\UISDK\Inc\Interface\ilistitembase.h"
#include "..\..\..\UISDK\Inc\Interface\ilistctrlbase.h"

namespace UI
{

    //	点击菜单项
//		message: UI_WM_NOTIFY
//		code:    UI_MENU_CLICK 
//      wparam:  
//      lparam: 
//
#define UI_MENU_CLICK 132331849
    
// void OnMenuClick(UINT nMenuID);
#define UIMSG_MENU_CLICK(func)                        \
    if (uMsg == UI_MSG_NOTIFY  &&                     \
        code == UI_MENU_CLICK )                       \
    {                                                 \
        SetMsgHandled(TRUE);                          \
        func((UINT)wParam);                           \
        if (IsMsgHandled())                           \
            return TRUE;                              \
    }

#define MENU_ITEM_SEPARATOR_ID  (-1)

// 获取菜单图标区域的宽度
// Return: long lWidth
#define UI_WM_MENU_GETICONGUTTERWIDTH     136111802

// 获取菜单右侧弹出箭头的宽度
// Return: long lWidth
#define UI_WM_MENU_GETPOPUPTRIANGLEWIDTH  136111803

#define UI_LISTITEM_TYPE_MENU_STRING      136111859
#define UI_LISTITEM_TYPE_MENU_SEPARATOR   136111900
#define UI_LISTITEM_TYPE_MENU_POPUP       136111901

//enum TRACK_POPUP_MENU_FLAG
#define UI_TPM_RETURNCMD    TPM_RETURNCMD
#define UI_TPM_CENTERALIGN  TPM_CENTERALIGN
#define UI_TPM_RIGHTALIGN   TPM_RIGHTALIGN
#define UI_TPM_VCENTERALIGN TPM_VCENTERALIGN
#define UI_TPM_BOTTOMALIGN  TPM_BOTTOMALIGN
#define UI_TPM_SHOWACTIVATE  0x80000000  // 抢焦点，用于支持跨进程的CEF右击弹出菜单 

struct MENUSTYLE
{
	// 父窗口销毁时，是否自动释放子菜单内存。主要用于从xml中load的菜单
	// 因为是内部创建的子菜单，内部负责销毁子菜单。而外部自己create出来
	bool  auto_release_submenu : 1;  
};

class MenuItemBase;
class MenuItem;
class Menu;
interface IMenu;
interface IWindow;

//////////////////////////////////////////////////////////////////////////
class MenuSeparatorItemShareData;
interface IMenuSeparatorItemShareData : public IListItemShareData
{
    UI_DECLARE_INTERFACE_ACROSSMODULE(MenuSeparatorItemShareData);
};
class MenuSeparatorItem;
interface IMenuSeparatorItem : public IListItemBase
{
    UI_DECLARE_INTERFACE_ACROSSMODULE(MenuSeparatorItem);
};

//////////////////////////////////////////////////////////////////////////
class MenuStringItemShareData;
interface IMenuStringItemShareData : public IListItemShareData
{
    UI_DECLARE_INTERFACE_ACROSSMODULE(MenuStringItemShareData);
};

class MenuStringItem;
interface IMenuStringItem : public IListItemBase
{
    UI_DECLARE_INTERFACE_ACROSSMODULE(MenuStringItem);
};
//////////////////////////////////////////////////////////////////////////
class MenuPopupItemShareData;
interface IMenuPopupItemShareData : public IListItemShareData
{
    UI_DECLARE_INTERFACE_ACROSSMODULE(MenuPopupItemShareData);
};
class MenuPopupItem;
interface IMenuPopupItem : public IListItemBase
{
    UI_DECLARE_INTERFACE_ACROSSMODULE(MenuPopupItem);
};
//////////////////////////////////////////////////////////////////////////

interface __declspec(uuid("DE6225F7-9DB1-4D13-A712-6553EF3B7556"))
UICTRL_API IMenu : public IListCtrlBase
{
    void  OnNewChildElement(IUIElement* pUIElement);
    int   TrackPopupMenu(
		UINT nFlag, int x, int y, IMessage* pNotifyObj, 
		HWND hWndClickFrom = NULL, RECT* prcClickFrom = NULL);
    IWindow*  CreateMenuWindow();
 
    IMenuStringItem*  AppendString(LPCTSTR szText, UINT nId);
    IMenuSeparatorItem*  AppendSeparator(UINT nId);
    IMenuPopupItem*  AppendPopup(LPCTSTR szText, UINT nId, IMenu* pSubMenu);

    IMenu*  GetSubMenuByPos(int nPos);
    IMenu*  GetSubMenuById(long lId);

	IWindow*  GetPopupWindow();
	int   GetIconGutterWidth();

	UI_DECLARE_INTERFACE_ACROSSMODULE(Menu);
};


struct LoadMenuData
{
	ISkinRes*  pSkinRes;
	/*__opt*/ LPCTSTR  szWndId;
	/*__opt*/ LPCTSTR  szMenuId;	
    HWND  hWndParent;
};
struct DefaultMenuData
{
	TCHAR  szDefaultWndId[32];
	TCHAR  szDefaultMenuId[32];
};


// 自动调用delete_this,释放菜单
class CMenuPtr
{
public:
	CMenuPtr(IMenu* pData = 0) : m_pData(pData)
	{
	}
	virtual ~CMenuPtr()
	{
		if (m_pData)
			m_pData->Release();
	}
	CMenuPtr& operator =(IMenu* p)
	{
		if (m_pData)
			m_pData->Release();
		m_pData = p;

		return *this;
	}
	IMenu* operator->() {
		return m_pData;
	}
	bool operator!() {
		return (m_pData == NULL);
	}
	operator IMenu*() {
		return m_pData;
	}

private:
	IMenu*  m_pData;
};

}

extern "C" UICTRL_API void  UISetDefaultMenuData(UI::DefaultMenuData* pData);
extern "C" UICTRL_API UI::IMenu*  UILoadMenu(UI::LoadMenuData* pData);
extern "C" UICTRL_API bool  UIDestroyMenu(UI::IMenu* pMenu);

#endif // IMENU_H_31481C5E_BC0B_4872_9C54_FF8D06736CFB