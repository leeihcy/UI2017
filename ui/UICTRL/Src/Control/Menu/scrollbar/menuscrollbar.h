#pragma once
#include "..\UISDK\Inc\Interface\iscrollbarmanager.h"
#include "Inc\Interface\ibutton.h"

namespace UI
{
interface IScrollBarManager;
interface IButton;
interface IMenu;
}

class MenuScrollbar : public IMessage
{
public:
	MenuScrollbar();
	~MenuScrollbar();

    enum {
        TOP_BUTTON_HOOK_MAPID = 152811308,
        BOTTOM_BUTTON_HOOK_MAPID = 152811309,

        TIMER_ID_TOP = 1,
        TIMER_ID_BOTTOM = 2,
    };


    UI_BEGIN_MSG_MAP_Ixxx(MenuScrollbar)
        UIMSG_TIMER(on_timer)
		UIMSG_HANDLER_EX(UI_WM_SCROLLBAR_BINDOBJSIZE_CHANGED, on_bindobj_size)
		UIMSG_HANDLER_EX(UI_WM_SCROLLBAR_INFO_CHANGED, on_scrollinfo_changed)

    UIALT_MSG_MAP(TOP_BUTTON_HOOK_MAPID)
        UIMSG_CALCPARENTNONCLIENTRECT(on_top_button_calcparent_nonclient_rect)
        UIMSG_NOTIFY_NORET(UI_BN_HOVER, on_top_button_hover)
        UIMSG_NOTIFY_NORET(UI_BN_LEAVE, on_top_button_leave)
    UIALT_MSG_MAP(BOTTOM_BUTTON_HOOK_MAPID)
        UIMSG_CALCPARENTNONCLIENTRECT(on_bottom_button_calcparent_nonclient_rect)
        UIMSG_NOTIFY_NORET(UI_BN_HOVER, on_bottom_button_hover)
        UIMSG_NOTIFY_NORET(UI_BN_LEAVE, on_bottom_button_leave)
	UI_END_MSG_MAP()

	//void  CreateByEditor(CREATEBYEDITORDATA* pData, IObject* pParent);
    void  Initialize(IMenu* pParent, UI::IScrollBarManager* pMgr);
	
private:
    LRESULT  on_scrollinfo_changed(UINT uMsg, WPARAM wParam, LPARAM lParam);
    LRESULT  on_bindobj_size(UINT uMsg, WPARAM wParam, LPARAM lParam);
   
    void  on_top_button_calcparent_nonclient_rect(CRegion4* prcNonClientRegion);
    void  on_bottom_button_calcparent_nonclient_rect(CRegion4* prcNonClientRegion);
    void  on_top_button_hover(WPARAM, LPARAM);
    void  on_top_button_leave(WPARAM, LPARAM);
    void  on_bottom_button_hover(WPARAM, LPARAM);
    void  on_bottom_button_leave(WPARAM, LPARAM);
    void  do_scroll_top(UI::IButton*);
	void  do_scroll_bottom(UI::IButton*);

    void  set_timer(UINT_PTR id);
    void  kill_timer();
    void  on_timer(UINT_PTR nIDEvent, LPARAM lParam);
  
public:
    IMenu*  m_pMenu;
    IScrollBarManager*  m_pScrollBarMgr;

    IButton*  m_pBtnTop;
    IButton*  m_pBtnBottom;

};

