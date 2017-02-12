#pragma once
#include "..\UISDK\Inc\Interface\icustomwindow.h"
#include "..\UISDK\Inc\Interface\iuiinterface.h"

namespace UI
{

class PopupControlWindow : 
            public ICustomWindow, 
            public IPreTranslateMessage
{
public:
    PopupControlWindow();
    virtual ~PopupControlWindow();

    UI_BEGIN_MSG_MAP_Ixxx(PopupControlWindow)
        UIMSG_KILLFOCUS(OnKillFocus)
        MSG_WM_MOUSEACTIVATE(OnMouseActivate)
        MSG_WM_ACTIVATEAPP(OnActivateApp)
        MSG_WM_DESTROY(OnDestroy)
        //UIMSG_WM_SKINCHANGING(OnSkinChanging)
        UIMSG_INITIALIZE(OnInitialize)
        UIMSG_PRECREATEWINDOW(PreCreateWindow)
   UI_END_MSG_MAP_CHAIN_PARENT(ICustomWindow)

protected:
    virtual BOOL  PreTranslateMessage(MSG* pMsg);

    void  OnInitialize();
    BOOL  PreCreateWindow(CREATESTRUCT* pcs);
    void  OnSkinChanging(BOOL* pbChange);
    BOOL  OnEraseBkgnd(IRenderTarget* pRenderTarget);
    void  OnDestroy();
    int   OnMouseActivate(HWND wndTopLevel, UINT nHitTest, UINT message);
    void  OnActivateApp(BOOL bActive, DWORD dwThreadID);
    void  OnKillFocus(IObject* pNewFocusObj);

public:
    //void  Create(IObject*  pBindObj, IObject* pContentObj, LPCTSTR szId, HWND hParentWnd = NULL);
	virtual void  SetBindObject(IObject* pBindObj);
	virtual void  SetContentObject(IObject* pContentObj);
    void  Show(POINT pt, SIZE size, BOOL bDoModal, long lShowType/*=SW_SHOWNOACTIVATE*/, BOOL bDesignMode=FALSE);
    void  Hide();
    void  Destroy();
	void  SetPopupFromInfo(HWND hWnd, RECT* prcClickInWnd);

protected:
    void  destroy_popup_window();
	void  start_show_animate();

protected:
    IObject*  m_pBindObject;// 如Menu* ComboboxBase*，用于通知、获取属性来源。主要为了实现将popupwindow更多的依赖于combobox，而不是listbox
	IObject*  m_pContentObj;// 弹出窗口中装载的对象指针，如ListBox* Menu* 

    bool      m_bExitLoop;  // 在收到WM_EXITPOPUPLOOP之后，该变量为true
    bool      m_bMouseIn;   // 鼠标在窗口上，用于触发WM_MOUSELEAVE

	// 用于解决在弹出菜单后点击按钮，菜单会隐藏后又显示出来的问题
	HWND  m_hWndClickFrom; // 为NULL时表示不判断
	CRect  m_rcClickFrom;  // 窗口坐标
};

class PopupListBoxWindow : public PopupControlWindow
{
public:
    PopupListBoxWindow();
   
    UI_BEGIN_MSG_MAP_Ixxx(PopupListBoxWindow)
        MSG_WM_KEYDOWN(OnKeyDown)
        UIMSG_INITIALIZE(OnInitialize)
    UIALT_MSG_MAP(1)
    UI_END_MSG_MAP_CHAIN_PARENT(PopupControlWindow)
 
protected:
    virtual BOOL PreTranslateMessage(MSG* pMsg);

    void  OnInitialize();
    void  OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
};

class Menu;
class PopupMenuWindow : public PopupControlWindow
{
public:
    PopupMenuWindow();

    UI_BEGIN_MSG_MAP_Ixxx(PopupMenuWindow)
		UIMSG_PRECREATEWINDOW(PreCreateWindow)
    UI_END_MSG_MAP_CHAIN_PARENT(PopupControlWindow)

public:
	virtual void  SetContentObject(IObject* pContentObj) override;

protected:
    virtual BOOL  PreTranslateMessage(MSG* pMsg);
	BOOL  PreCreateWindow(CREATESTRUCT* pcs);

protected:
    Menu*    m_pMenu;
    POINT    m_ptLastMousePos; // 用于区分是子菜单创建、移动、隐藏导致系统发出来的WM_MOUSEMOVE消息。
    Menu*    m_pLastHoverMenu; // TODO: 突然出现的问题，鼠标移出菜单收不到WM_MOUSELEAVE消息。
                               // 目前推测是因为当收到一个鼠标移出的WM_MOUSEMOVE后直接RETURN TRUE了，导致
                               // TrackMouseEvent没法检测出MOUSELEAVE，因此没有给窗口发出消息。
                               // 为了解决该问题，在这里记录一下上一次鼠标下的菜单，当鼠标移出后，手动发送
                               // WM_MOUSELEAVE
};
}