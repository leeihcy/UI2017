#pragma once
#include "scrollbar\menuscrollbar.h"
#include "Inc\Interface\imenu.h"

//
// 2015.2.22 重写菜单与窗口的逻辑。由窗口来创建菜单，而不是菜单中创建窗口
//
// 这样菜单的皮肤与其它窗口的皮肤也一致了
/* 默认皮肤需要在xml中增加配置：
     <CustomWindow id="Menu.Default">
 	    <Menu/>
     </CustomWindow>
*/
namespace UI
{
class PopupMenuWindow;

class Menu : public MessageProxy
{
public:
    Menu(IMenu* p);
    ~Menu();

	UI_BEGIN_MSG_MAP()
        UIMSG_NOTIFY(UI_LCN_HOVERITEMCHANGED, OnHoverItemChanged)
        MSG_WM_KEYDOWN(OnKeyDown)
		MSG_WM_KEYUP(OnKeyUp)
        UIMSG_TIMER(OnTimer)
        MSG_WM_LBUTTONUP(OnLButtonUp)
        MSG_WM_MOUSEMOVE(OnMouseMove)
        MSG_WM_MOUSELEAVE(OnMouseLeave)
        MSG_WM_LBUTTONDOWN(OnLButtonDown)

        UIMSG_HANDLER_EX(UI_WM_MENU_GETICONGUTTERWIDTH, OnGetIconGutterWidth);
		UIMSG_HANDLER_EX(UI_WM_MENU_GETPOPUPTRIANGLEWIDTH, OnGetPopupTriangleWidth)
        UIMSG_INITPOPUPCONTROLWINDOW(OnInitPopupControlWindow)
        UIMSG_UNINITPOPUPCONTROLWINDOW(OnUnInitPopupControlWindow)
        MSG_WM_THEMECHANGED(OnThemeChanged)
        UIMSG_QUERYINTERFACE(Menu)
        UIMSG_SERIALIZE(OnSerialize)
        UIMSG_INITIALIZE(OnIntialize)
        UIMSG_FINALCONSTRUCT(FinalConstruct)
        UIMSG_FINALRELEASE(FinalRelease)

    UI_END_MSG_MAP_CHAIN_PARENT_Ixxx(Menu, IListCtrlBase)

public:
    static PARSE_CONTROL_RETURN UIParseMenuTag(
            IUIElement*, ISkinRes*  pUIApp, 
            IObject* pParent, IObject** ppOut);
    void  OnNewChildElement(IUIElement* pUIElement);

public:
    IMenu*  GetIMenu() { return m_pIMenu; }

	void  ModifyMenuStyle(MENUSTYLE* add, MENUSTYLE* remove);
	bool  TestMenuStyle(const MENUSTYLE& test);

    IWindow*  CreateMenuWindow();
    int  TrackPopupMenu(
            UINT nFlag, int x, int y, 
            IMessage* pNotifyObj,
            HWND hWndClickFrom = NULL,
            RECT* prcClickFrom = NULL);

    IMenuStringItem*  AppendString(LPCTSTR szText, UINT nId);
    IMenuSeparatorItem*  AppendSeparator(UINT nId);
    IMenuPopupItem*  AppendPopup(LPCTSTR szText, UINT nId, IMenu* pSubMenu);

    IMenu*  GetSubMenuByPos(int nPos);
    IMenu*  GetSubMenuById(long lId);
    IListItemBase*  GetMenuItemBySubMenu(IMenu* pSubMenu);

    Menu*  GetPrevMenu() { return m_pPrevMenu; }
    Menu*  GetNextMenu() { return m_pNextMenu; }
    Menu*  GetRootMenu();
    Menu*  GetLastMenu();
    Menu*  GetMenuByHWND(HWND hWnd);
    Menu*  GetMenuByPos(const POINT& ptScreen);
    HWND   GetPopupWindowHandle();
	IWindow*  GetPopupWindow();
	void   SetPopupWindow(PopupMenuWindow* pWindow);
    int    GetIconGutterWidth();

protected:
    void  OnSerialize(SERIALIZEDATA* pData);
    HRESULT  FinalConstruct(ISkinRes* p);
    void  FinalRelease();
    void  OnIntialize();

    void  OnKeyDown( UINT nChar, UINT nRepCnt, UINT nFlags );
	void  OnKeyUp( UINT nChar, UINT nRepCnt, UINT nFlags );
    LRESULT  OnHoverItemChanged(WPARAM w, LPARAM l);
    void  OnLButtonDown(UINT nFlags, POINT point);
    void  OnLButtonUp(UINT nFlags, POINT point);
    void  OnTimer(UINT_PTR nIDEvent, LPARAM lParam);
    void  OnMouseMove(UINT nFlags, CPoint point);
    void  OnMouseLeave();
    void  OnSubMenuMouseMove(IMenu* pSubMenu);
    void  OnThemeChanged();

    void  ShowPopupSubMenu(IListItemBase* pItem);
    void  HidePopupSubMenu();
    int   PopupSubMenu(IListItemBase* pSubMenu);
    int   PopupAsSubMenu(UINT nFlags, Menu* pParentMenu, IListItemBase* pItem);

    void  OnInitPopupControlWindow();
    void  OnUnInitPopupControlWindow();
    LRESULT  OnGetIconGutterWidth(UINT, WPARAM, LPARAM) { return (LRESULT) m_nIconGutterWidth; }
    LRESULT  OnGetPopupTriangleWidth(UINT, WPARAM, LPARAM) { return (LRESULT) m_nPopupTriangleWidth; }

protected:
    IListItemBase*  LoadMenuItem(IUIElement* pUIElement, LPCTSTR szTagName, IMapAttribute* pMapAttrib);

    void  SetReturnCmd(UINT n);
    bool  IsMyChildMenu(Menu* pMenu);
    void  OnClick(IListItemBase* pItem);

    IListItemBase*  GetNextSelectableItem(IListItemBase* pItem);
    IListItemBase*  GetPrevSelectableItem(IListItemBase* pItem);
    IListItemBase*  _GetNextSelectableItem(IListItemBase* pItem);
    IListItemBase*  _GetPrevSelectableItem(IListItemBase* pItem);

    void  destroyPopupWindow();
    void  _HidePopupWindow();
    void  destroyAllSubMenu();

private:
    IMenu*  m_pIMenu;
	MENUSTYLE  m_menuStyle;

    PopupMenuWindow*  m_pPopupWrapWnd;    // 用于包装菜单的窗口
    MenuScrollbar  m_scrollbarCreator;  // 用于包装菜单滚动条的创建

    Menu*  m_pNextMenu;                 // 当前弹出的菜单列表中，自己的子菜单
    Menu*  m_pPrevMenu;                 // 当前弹出的菜单列表中，自己的父菜单
    UINT   m_nTimerIDShowPopupSubMenu;  // 计时器ID，用于弹出子窗口
    UINT   m_nTimerIDHidePopupSubMenu;  // 计时器ID，用于关闭子窗口

    REGION4  m_rWindowShadowSpace;      // 窗口的四周阴影偏移量。用于指定一个POINT时，保存菜单实际位置就在该处，不用外部再计算阴影偏移量

    UINT  m_nTrackPopupMenuFlag;        // 菜单弹出样式，例如指定TPM_RETURNCMD
    UINT  m_nRetCmd;                    // 当指定TPM_RETURNCMD类型时，返回的ID
    bool  m_bPopupAtParentRight;        // 记录自己作为子菜单时在父菜单的哪一个方向弹出来了，用于VK_LEFT/VK_RIGHT关闭

    long   m_nIconGutterWidth;           // 菜单左侧图标列 的宽度
    long   m_nPopupTriangleWidth;        // 菜单右侧弹出菜单类型的三角形
    long   m_nSubMenuGap;                // 子菜单与父菜单之间的间隙
};
}