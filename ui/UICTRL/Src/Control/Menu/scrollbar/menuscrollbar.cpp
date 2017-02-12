#include "stdafx.h"
#include "menuscrollbar.h"
#include "Inc\Interface\imenu.h"
#include "..\..\Button\button.h"

#define MENUSCROLLBAR_TOPBUTTON_ID  TEXT("__scrollbar_top")
#define MENUSCROLLBAR_BOTTOMBUTTON_ID  TEXT("__scrollbar_bottom")

MenuScrollbar::MenuScrollbar() : IMessage(CREATE_IMPL_TRUE)
{
    m_pBtnTop = NULL;
    m_pBtnBottom = NULL;
	m_pMenu = NULL;
}

MenuScrollbar::~MenuScrollbar()
{
	kill_timer();
}

// void  MenuScrollbar::CreateByEditor(CREATEBYEDITORDATA* pData, IObject* pParent)
// {
// 	EditorAddObjectResData data = {0};
// 	data.pUIApp = pData->pUIApp;
// 	data.pParentXml = pData->pXml;
// 	data.pParentObj = pParent;
// 	data.bNcChild = true;
// 	
// 	data.objiid = uiiidof(ISystemVScrollBar);
// 	data.ppCreateObj = (void**)&m_pVScrollBar;
// 	data.szId = VSCROLLBAR_ID;
// 	pData->pEditor->AddObjectRes(&data);
// }

void  MenuScrollbar::Initialize(IMenu* pParent, IScrollBarManager* pMgr)
{
 	IObject* pObjTop = pParent->FindNcObject(MENUSCROLLBAR_TOPBUTTON_ID);
    IObject* pObjBottom = pParent->FindNcObject(MENUSCROLLBAR_BOTTOMBUTTON_ID);

    if (!pObjTop || !pObjBottom)
        return;

    m_pBtnTop = (IButton*)pObjTop->QueryInterface(__uuidof(IButton));
    m_pBtnBottom = (IButton*)pObjBottom->QueryInterface(__uuidof(IButton));

    if (!m_pBtnTop || !m_pBtnBottom)
    {
        m_pBtnBottom = m_pBtnTop = NULL;
        return;
    }

    m_pMenu = pParent;
    m_pScrollBarMgr = pMgr;

    m_pBtnTop->SetVisible(false);
    m_pBtnBottom->SetVisible(false);

	BUTTONSTYLE style = { 0 };
	style.notify_hoverleave = 1;
	m_pBtnTop->GetImpl()->ModifyButtonStyle(&style, nullptr);
	m_pBtnBottom->GetImpl()->ModifyButtonStyle(&style, nullptr);

    m_pBtnTop->AddHook(this, 0, TOP_BUTTON_HOOK_MAPID);
    m_pBtnBottom->AddHook(this, 0, BOTTOM_BUTTON_HOOK_MAPID);
    m_pBtnTop->SetNotify(this, TOP_BUTTON_HOOK_MAPID);
    m_pBtnBottom->SetNotify(this, BOTTOM_BUTTON_HOOK_MAPID);

	m_pBtnTop->ClickedEvent().connect(this, &MenuScrollbar::do_scroll_top);
	m_pBtnBottom->ClickedEvent().connect(this, &MenuScrollbar::do_scroll_bottom);

    pMgr->SetVScrollBar(this);
}

LRESULT  MenuScrollbar::on_scrollinfo_changed(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if (!m_pBtnTop || !m_pBtnBottom)
        return 0;

    bool bScrollBarVisibleChanged = 0;
    UISCROLLINFO* pInfo = (UISCROLLINFO*)wParam;

    bool bNeedUpdateVisibleState = lParam?true:false;
    if (bNeedUpdateVisibleState)
    {
        bool bVisible1 = m_pBtnTop->IsSelfVisible();
        if (pInfo->nPage >= pInfo->nRange)
        {
            m_pBtnTop->SetVisible(false);
            m_pBtnBottom->SetVisible(false);
        }
        else
        {
            m_pBtnTop->SetVisible(true);
            m_pBtnBottom->SetVisible(true);
        }
        bool bVisible2 = m_pBtnTop->IsSelfVisible();

        if (bVisible1 != bVisible2)
        {
            bScrollBarVisibleChanged = 1;
        }
    }

    m_pBtnTop->SetEnable(pInfo->nPos > 0);
    m_pBtnBottom->SetEnable(pInfo->nPos < (pInfo->nRange-pInfo->nPage));

    return bScrollBarVisibleChanged;
}
LRESULT  MenuScrollbar::on_bindobj_size(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if (!m_pMenu || !m_pBtnTop || !m_pBtnBottom)
        return 0;

    REGION4 rBorder;
    REGION4 rNonClientExt;
    m_pMenu->GetBorderRegion(&rBorder);
    m_pMenu->GetExtNonClientRegion(&rNonClientExt);

    {
        int nTop = rBorder.top;
        int nBottom = rBorder.top + rNonClientExt.top;
        
        int nLeft = rBorder.left;
        int nRight = m_pMenu->GetWidth() - rBorder.right - rNonClientExt.right;

        m_pBtnTop->SetObjectPos(nLeft, nTop, nRight-nLeft, nBottom-nTop, SWP_NOREDRAW);
    }

    {
        int nBottom = m_pMenu->GetHeight() - rBorder.bottom;
        int nTop = nBottom - rNonClientExt.bottom;

        int nLeft = rBorder.left;
        int nRight = m_pMenu->GetWidth() - rBorder.right - rNonClientExt.right;

        m_pBtnBottom->SetObjectPos(nLeft, nTop, nRight-nLeft, nBottom-nTop, SWP_NOREDRAW);
    }
    return 0;
}

void  MenuScrollbar::on_top_button_calcparent_nonclient_rect(CRegion4* prcNonClientRegion)
{
    int height = m_pBtnTop->GetHeight();
    if (0 == height)
        height = m_pBtnTop->GetDesiredSize().cy;

    prcNonClientRegion->top += height;
}
void  MenuScrollbar::on_bottom_button_calcparent_nonclient_rect(CRegion4* prcNonClientRegion)
{
    int height = m_pBtnBottom->GetHeight();
    if (0 == height)
        height = m_pBtnBottom->GetDesiredSize().cy;

    prcNonClientRegion->bottom += height;
}

void  MenuScrollbar::on_top_button_hover(WPARAM, LPARAM)
{
    do_scroll_top(0);

    set_timer(TIMER_ID_TOP);
}
void  MenuScrollbar::on_top_button_leave(WPARAM, LPARAM)
{
    kill_timer();
}
void  MenuScrollbar::on_bottom_button_hover(WPARAM, LPARAM)
{
    do_scroll_bottom(0);

    set_timer(TIMER_ID_BOTTOM);
}
void  MenuScrollbar::on_bottom_button_leave(WPARAM, LPARAM)
{
    kill_timer();
}

void  MenuScrollbar::set_timer(UINT_PTR id)
{
    kill_timer();
    
    if (m_pMenu)
    {
        IUIApplication* pUIApp = m_pMenu->GetUIApplication();
        if (pUIApp)
        {
            pUIApp->SetTimerById(200, id, this);
        }
    }
}
void  MenuScrollbar::kill_timer()
{
    if (m_pMenu)
    {
        IUIApplication* pUIApp = m_pMenu->GetUIApplication();
        if (pUIApp)
        {
            pUIApp->KillTimerByNotify(this);
        }
    }
}
void  MenuScrollbar::on_timer(UINT_PTR nIDEvent, LPARAM lParam)
{
    TimerItem* pti = (TimerItem*)lParam;

    if (pti->nId == TIMER_ID_TOP)
    {
        do_scroll_top(0);
    }
    else if (pti->nId == TIMER_ID_BOTTOM)
    {
        do_scroll_bottom(0);
    }
}

void  MenuScrollbar::do_scroll_top(UI::IButton*)
{
    m_pMenu->ScrollY(-m_pMenu->GetItemHeight(), true);
}
void  MenuScrollbar::do_scroll_bottom(UI::IButton*)
{
    m_pMenu->ScrollY(m_pMenu->GetItemHeight(), true);
}