#include "stdafx.h"
#include "systemscrollbar.h"
#include "scrollbar_desc.h"
#include "Inc\Interface\ibutton.h"
#include "..\UISDK\Inc\Interface\irenderlayer.h"

#define SCROLLBAR_UP_ID      _T("__scrollbar_up")      // 滚动条按钮1
#define SCROLLBAR_DOWN_ID    _T("__scrollbar_down")    // 滚动条按钮2
#define SCROLLBAR_LEFT_ID    _T("__scrollbar_left")    // 滚动条按钮1
#define SCROLLBAR_RIGHT_ID   _T("__scrollbar_right")   // 滚动条按钮2
#define SCROLLBAR_THUMB_ID   _T("__scrollbar_thumb")   // 滚动条拖动条

#define TIMER_TIME_TOSCROLL      500
#define TIMER_TIME_SCROLLING     50

#define STORYBOARD_ID_HOVER_ANIMATE  1
using namespace UI;

SystemScrollBar::SystemScrollBar(ISystemScrollBar* p):MessageProxy(p)
{
    m_pISystemScrollBar = p;
    m_pMgrScrollBar =  NULL;

	m_pBtnLineUpLeft = m_pBtnLineDownRight = NULL;
	m_nTimer1IDToScroll = m_nTimer2IDToScroll = 0;
	m_nTimer1IDScrolling = m_nTimer2IDScrolling = 0;

    m_pBtnThumb = NULL;
	m_nOldPage = m_nOldRange = m_nOldPos = 0;
	m_nClickDiff = 0;
	m_bTracking = false;
    m_bHover = false;
    m_bHoverVisible = false;
    m_eScrollBarRegionType = HOLD_ONLY_VISIBLE;
    m_bAutoSetBindObjFocus = false;
}

SystemScrollBar::~SystemScrollBar()
{
	// 这三个对象会在parent的destroyChildObject中被释放，因此这里只需要置空就行
	// 但是在ResetAttribute的时候，又必须我们自己释放这个对象
	m_pBtnLineDownRight = NULL;
	m_pBtnLineUpLeft = NULL;
	m_pBtnThumb = NULL;
}

void  SystemScrollBar::FinalRelease()
{
	SetMsgHandled(FALSE);

     // 虚函数，不能在析构中调用
	if (m_pMgrScrollBar)
	{
		if (m_pMgrScrollBar && HSCROLLBAR == GetDirType())
			m_pMgrScrollBar->SetHScrollBar(NULL);
		else
			m_pMgrScrollBar->SetVScrollBar(NULL);
	}

    DestroyHoverAnimate();
}

HRESULT  SystemScrollBar::FinalConstruct(ISkinRes* p)
{
	HRESULT hr = DO_PARENT_PROCESS3(ISystemScrollBar, IControl);
	if (FAILED(hr))
		return hr;

	OBJSTYLE s = {0};
	s.default_ncobject = 1;
	m_pISystemScrollBar->ModifyObjectStyle(&s, NULL);	

	return S_OK;
}

void  SystemScrollBar::OnInitialize()
{
    DO_PARENT_PROCESS(ISystemScrollBar, IControl);

	bool isVScrollBar = GetDirType()==VSCROLLBAR ? true:false;

	IObject* pObj = m_pISystemScrollBar->FindObject(SCROLLBAR_THUMB_ID);
	if (pObj)
		m_pBtnThumb = (IButton*)pObj->QueryInterface(__uuidof(IButton));

	IUIApplication* pUIApp = m_pISystemScrollBar->GetUIApplication();
	ISkinRes* pSkinRes = m_pISystemScrollBar->GetSkinRes();

    if (!m_pBtnThumb)
    {
        m_pBtnThumb = IButton::CreateInstance(pSkinRes);
		//m_pBtnThumb->SetId(SCROLLBAR_THUMB_ID);
		m_pISystemScrollBar->AddChild(m_pBtnThumb);

		m_pBtnThumb->SetDrawFocusType(BUTTON_RENDER_DRAW_FOCUS_TYPE_NONE);
		m_pBtnThumb->SetAutoSizeType(BUTTON_AUTOSIZE_TYPE_BKIMAGE);
    }

	m_pBtnThumb->SetOutRef((void**)&m_pBtnThumb);
	m_pBtnThumb->AddHook(m_pISystemScrollBar, 0, ALT_MSG_ID_THUMB_BTN);
	m_pBtnThumb->SetButtonType(isVScrollBar ?
		BUTTON_TYPE_VSCROLLTHUMB:BUTTON_TYPE_HSCROLLTHUMB);

	OBJSTYLE style = { 0 };
	style.tabstop = 1;
	//m_pBtnThumb->SetTabstop(false);
	m_pBtnThumb->ModifyObjectStyle(0, &style);

	pObj = m_pISystemScrollBar->TryFindObject(isVScrollBar?
			SCROLLBAR_UP_ID:SCROLLBAR_LEFT_ID);
	if (pObj)
		m_pBtnLineUpLeft = (IButton*)pObj->QueryInterface(__uuidof(IButton));

	if (m_pBtnLineUpLeft)
	{
		m_pBtnLineUpLeft->AddHook(m_pISystemScrollBar, 0, 
			ALT_MSG_ID_BUTTON1);
		m_pBtnLineUpLeft->SetOutRef(
			(void**)&m_pBtnLineUpLeft);

		m_pBtnLineUpLeft->SetButtonType( 
			BUTTON_TYPE_SCROLLLINEUP);  // 先取个默认值
		m_pBtnLineUpLeft->SetDrawFocusType(
			BUTTON_RENDER_DRAW_FOCUS_TYPE_NONE);
		m_pBtnLineUpLeft->SetAutoSizeType(
			BUTTON_AUTOSIZE_TYPE_BKIMAGE);

		//m_pBtnLineUpLeft->SetTabstop(false);
		OBJSTYLE style = { 0 };
		style.tabstop = 1;
		m_pBtnLineUpLeft->ModifyObjectStyle(0, &style);
	}

    pObj = m_pISystemScrollBar->TryFindObject(isVScrollBar ?
			SCROLLBAR_DOWN_ID:SCROLLBAR_RIGHT_ID);
	if (pObj)
		m_pBtnLineDownRight = (IButton*)pObj->QueryInterface(__uuidof(IButton));

	if (m_pBtnLineDownRight)
	{
		m_pBtnLineDownRight->AddHook(
			m_pISystemScrollBar, 0, ALT_MSG_ID_BUTTON2);
		m_pBtnLineDownRight->SetOutRef(
			(void**)&m_pBtnLineDownRight);

		m_pBtnLineDownRight->SetButtonType(
			BUTTON_TYPE_SCROLLLINEDOWN);  // 先取个默认值
		m_pBtnLineDownRight->SetDrawFocusType(
			BUTTON_RENDER_DRAW_FOCUS_TYPE_NONE);
		m_pBtnLineDownRight->SetAutoSizeType(
			BUTTON_AUTOSIZE_TYPE_BKIMAGE);

		//m_pBtnLineDownRight->SetTabstop(false);
		OBJSTYLE style = { 0 };
		style.tabstop = 1;
		m_pBtnLineDownRight->ModifyObjectStyle(0, &style);
	}

	if (m_bHoverVisible)
	{
		OBJSTYLE s = { 0 };
		s.layer = 1;
		m_pISystemScrollBar->ModifyObjectStyle(&s, 0);
		m_pISystemScrollBar->SetOpacity(0);
	}

	OBJSTYLE s = {0};
	if (m_eScrollBarRegionType == FLOAT_ON_CLIENT)
		s.float_on_parent_content = 1;
	m_pISystemScrollBar->ModifyObjectStyle(&s, NULL);	

    // 默认隐藏，这样才能触发第一次滚动条需要显示的场景
    m_pISystemScrollBar->SetVisible(false);
}

void  SystemScrollBar::SetIScrollBarMgr(IScrollBarManager* p)
{
    m_pMgrScrollBar = p; 
    if (p)
    {
        if (HSCROLLBAR == GetDirType())
        {
            p->SetHScrollBar(
                static_cast<IMessage*>(m_pISystemScrollBar));
        }
        else
        {
            p->SetVScrollBar(
                static_cast<IMessage*>(m_pISystemScrollBar));
        }
    }
}

IObject*  SystemScrollBar::GetBindObject() 
{ 
    if (NULL == m_pMgrScrollBar)
        return NULL;

    return m_pMgrScrollBar->GetBindObject(); 
}
void  SystemScrollBar::GetDesiredSize(SIZE* pSize)
{
    pSize->cx = pSize->cy = 0;

    IRenderBase* p = m_pISystemScrollBar->GetBackRender();
    if (p)
    {
        *pSize = p->GetDesiredSize();

        // 2014.3.7 有可能bkgnd是null，导致返回0。此时不处理
        if (pSize->cx || pSize->cy)   
            return;
    }

	if (m_pBtnThumb)
    {
		*pSize = m_pBtnThumb->GetDesiredSize();
        return;
    }

    pSize->cx = GetSystemMetrics(SM_CXVSCROLL);
    pSize->cy = GetSystemMetrics(SM_CYVSCROLL);
}

void  SystemScrollBar::SetFloatOnClientRegion(bool b)
{
    if (b)
        m_eScrollBarRegionType = FLOAT_ON_CLIENT;
    else
        m_eScrollBarRegionType = HOLD_ONLY_VISIBLE;
}
void  SystemScrollBar::SetKeepHoldNonClientRegion(bool b)
{
    if (b)
        m_eScrollBarRegionType = KEEP_HOLD;
    else
        m_eScrollBarRegionType = HOLD_ONLY_VISIBLE;
}


LRESULT SystemScrollBar::OnScrollBarInfoChanged(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    // 用于解决分层后，没有dirty，不响应onpaint，滑块位置变化没机会更新的问题。
//     IRenderLayer* pRenderLayer = m_pISystemScrollBar->GetSelfRenderLayer2();
//     if (pRenderLayer)
//     {
//         pRenderLayer->SetDirty(true);
//     }

    bool bNeedUpdateVisibleState = lParam?true:false;
    if (bNeedUpdateVisibleState)
    {
        UISCROLLINFO* pInfo = (UISCROLLINFO*)wParam;
        bool bVisible1 = m_pISystemScrollBar->IsSelfVisible();
        CalcScrollBarVisible();
        bool bVisible2 = m_pISystemScrollBar->IsSelfVisible();

        if (bVisible1 != bVisible2)
        {
            // 显隐发生改变，重置这些参数。否则下一次显示滚动条时用的还是老数据，会导致不更新
            m_nOldRange = m_nOldPage = m_nOldPos = 0;
            return 1;
        }
    }

    return 0;
}


void  SystemScrollBar::CreateHoverAnimate()
{
	UIASSERT(0);
#if 0
    DestroyHoverAnimate();

    UIA::IAnimateManager* pAnimateMgr = 
            m_pISystemScrollBar->GetUIApplication()->GetAnimateMgr();
    UIA::IStoryboard* pHoverAnimate = pAnimateMgr->CreateStoryboard(
            static_cast<UIA::IAnimateEventCallback*>(this),
            STORYBOARD_ID_HOVER_ANIMATE, 0, 0);

    UIA::IFromToTimeline* pTimeline = pHoverAnimate->CreateTimeline(0);
    if (m_bHover)
    {
        // 这里不使用0->255
        // 有可能当前的动画是取消了255-0的动画，当前的alpha值可能是不确定的。
        // 为了避免闪烁，动画的起始值应该设置为当前opacity
        pTimeline->SetParam(
            m_pISystemScrollBar->GetLayer()->GetOpacity(), 
            255, 200)->SetEaseType(UIA::linear);
    }
    else
    {
        pTimeline->SetParam(
            m_pISystemScrollBar->GetLayer()->GetOpacity(),
            0, 200)->SetEaseType(UIA::linear);
    }

    pHoverAnimate->Begin();
#endif
}

void  SystemScrollBar::DestroyHoverAnimate()
{
    UIA::IAnimateManager* pAnimateMgr = 
            m_pISystemScrollBar->GetUIApplication()->GetAnimateMgr();
    pAnimateMgr->ClearStoryboardByNotify(
            static_cast<UIA::IAnimateEventCallback*>(this));
}


LRESULT  SystemScrollBar::OnBindObjHoverChanged(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    bool bHover = wParam ? true:false;
    CheckHoverChanged();
    return 0;
}

// LRESULT  SystemScrollBar::OnHoverChanged(UINT uMsg, WPARAM wParam, LPARAM lParam)
void  SystemScrollBar::OnHoverChanged(UINT nMask)
{
    SetMsgHandled(FALSE);
    if (!m_bHoverVisible)
        return;

    CheckHoverChanged();

    // 检查另一根滚动条的显示隐
    if (GetDirType() == VSCROLLBAR && m_pMgrScrollBar->GetHScrollBar())
    {
        IMessage* p = m_pMgrScrollBar->GetHScrollBar();
		ISystemHScrollBar* pHScrollBar = (ISystemHScrollBar*)p->QueryInterface(__uuidof(ISystemHScrollBar));
        if (pHScrollBar)
        {
            pHScrollBar->GetImpl()->CheckHoverChanged();
        }
    }
    else if (GetDirType() == HSCROLLBAR && m_pMgrScrollBar->GetVScrollBar())
    {
        IMessage* p = m_pMgrScrollBar->GetVScrollBar();
        ISystemVScrollBar* pVScrollBar = (ISystemVScrollBar*)p->QueryInterface(__uuidof(ISystemVScrollBar));
        if (pVScrollBar)
        {
            pVScrollBar->GetImpl()->CheckHoverChanged();
        }
    }
}


bool  SystemScrollBar::IsScrollbarHover()
{
    bool bHover = false;
    if (m_pISystemScrollBar->IsHover() || m_pISystemScrollBar->IsPress())
    {
        bHover = true;
    }
    else if (m_pBtnThumb && (m_pBtnThumb->IsHover() || m_pBtnThumb->IsPress()))
    {
        bHover = true;
    }
    else if (m_pBtnLineDownRight && (m_pBtnLineDownRight->IsHover() || m_pBtnLineDownRight->IsPress()))
    {
        bHover = true;
    }
    else if (m_pBtnLineUpLeft && (m_pBtnLineUpLeft->IsHover() || m_pBtnLineUpLeft->IsPress()))
    {
        bHover = true;
    }

    return bHover;
}

// 需要同时处理bindobj/scrollbar/thubmbtn/linebtn的hover,press
void  SystemScrollBar::CheckHoverChanged()
{
    if (!m_pISystemScrollBar->IsSelfVisible())
        return;

    SetMsgHandled(FALSE);
    if (!m_bHoverVisible)
        return;

    IObject* pObj = GetBindObject();
    if (!pObj)
        return;

    bool bHover = false;
    if (pObj->IsVisible())
    {
        if (pObj->IsHover() || pObj->IsPress() || pObj->IsForceHover() || pObj->IsForcePress())
        {
            bHover = true;
        }
        else if (IsScrollbarHover())
        {
            bHover = true;
        }
        else if (m_pMgrScrollBar->IsSmoothScrolling())
        {
            bHover= true;
        }
        else if (GetDirType() == VSCROLLBAR && m_pMgrScrollBar->GetHScrollBar())
        {
            IMessage* p = m_pMgrScrollBar->GetHScrollBar();
			ISystemHScrollBar* pHScrollBar = (ISystemHScrollBar*)p->QueryInterface(__uuidof(ISystemHScrollBar));
            if (pHScrollBar)
            {
                if (pHScrollBar->GetImpl()->IsScrollbarHover())
                    bHover = true;
            }
        }
        else if (GetDirType() == HSCROLLBAR && m_pMgrScrollBar->GetVScrollBar())
        {
            IMessage* p = m_pMgrScrollBar->GetVScrollBar();
			ISystemVScrollBar* pVScrollBar = (ISystemVScrollBar*)p->QueryInterface(__uuidof(ISystemVScrollBar));
            if (pVScrollBar)
            {
                if (pVScrollBar->GetImpl()->IsScrollbarHover())
                    bHover = true;
            }
        }
    }

    if (m_bHover == bHover)
        return;

    m_bHover = bHover;
    CreateHoverAnimate();
}

UIA::E_ANIMATE_TICK_RESULT SystemScrollBar::OnAnimateTick(UIA::IStoryboard* pStoryboard)
{
	UIASSERT(0);
#if 0
    if (pStoryboard->GetId() == STORYBOARD_ID_HOVER_ANIMATE)
    {
        int nValue = pStoryboard->GetTimeline(0)->GetCurrentIntValue();

        m_pISystemScrollBar->GetLayer()->SetOpacity(nValue);
        m_pISystemScrollBar->Invalidate();
    }
#endif
    return UIA::ANIMATE_TICK_RESULT_CONTINUE;
}


void  SystemScrollBar::OnPaint(IRenderTarget*)
{
	SetMsgHandled(FALSE);
    if (!m_pMgrScrollBar)
        return;

	int nNewRange = m_pMgrScrollBar->GetScrollRange(GetDirType());
	int nNewPage = m_pMgrScrollBar->GetScrollPage(GetDirType());
	int nNewPos = m_pMgrScrollBar->GetScrollPos(GetDirType());

	if (m_nOldRange != nNewRange ||
		m_nOldPage != nNewPage ||
		m_nOldPos != nNewPos)
	{
		bool bNeedUpdateThumbButtonSize = false;
		if (m_nOldRange != nNewRange ||
			m_nOldPage != nNewPage ||
			nNewPos < 0 ||
			(nNewPos > nNewRange-nNewPage))
		{
			bNeedUpdateThumbButtonSize = true;
		}
		if (this->CalcThumbButtonPos(bNeedUpdateThumbButtonSize))
		{
			m_nOldRange = m_pMgrScrollBar->GetScrollRange(GetDirType());
			m_nOldPage = m_pMgrScrollBar->GetScrollPage(GetDirType());
			m_nOldPos = m_pMgrScrollBar->GetScrollPos(GetDirType());
		}
	}
}

void  SystemScrollBar::OnTimer(UINT_PTR idEvent, LPARAM lParam)
{	
    IUIApplication* pUIApplication = m_pISystemScrollBar->GetUIApplication();
	if(idEvent == m_nTimer1IDToScroll)
	{
		pUIApplication->KillTimer(m_nTimer1IDToScroll);
		m_nTimer1IDScrolling = pUIApplication->SetTimer(TIMER_TIME_SCROLLING, m_pISystemScrollBar);
	}
	else if(idEvent == m_nTimer2IDToScroll)
	{
		pUIApplication->KillTimer(m_nTimer2IDToScroll);
		m_nTimer2IDScrolling = pUIApplication->SetTimer(TIMER_TIME_SCROLLING, m_pISystemScrollBar);
	}
	else if(idEvent == m_nTimer1IDScrolling)
	{
		if (m_pBtnLineUpLeft)
		{
			// 检测鼠标是否在其上面
			POINT pt, ptObj;
			::GetCursorPos(&pt);
			::MapWindowPoints(NULL, m_pISystemScrollBar->GetHWND(), &pt, 1);
			m_pISystemScrollBar->WindowPoint2ObjectPoint(&pt, &ptObj, true);
		
			if(HTNOWHERE != UISendMessage(m_pBtnLineUpLeft, UI_MSG_HITTEST, (WPARAM)&ptObj))
			{
				m_pMgrScrollBar->FireScrollMessage(GetDirType(), SB_LINEUP);
			}
		}
	}
	else if(idEvent == m_nTimer2IDScrolling)
	{
		if (m_pBtnLineDownRight)
		{
			// 检测鼠标是否在其上面
			POINT pt, ptObj;
			::GetCursorPos(&pt);
			::MapWindowPoints(NULL, m_pISystemScrollBar->GetHWND(), &pt, 1);
			m_pISystemScrollBar->WindowPoint2ObjectPoint(&pt, &ptObj, true);

			if(HTNOWHERE != UISendMessage(m_pBtnLineDownRight, UI_MSG_HITTEST, (WPARAM)&ptObj))
			{
				m_pMgrScrollBar->FireScrollMessage(GetDirType(), SB_LINEDOWN);  // ScrollbarMgr::ProcessMessage中会负责刷新
			}
		}
	}
}


void  SystemScrollBar::OnBtn1LButtonDown(UINT nFlags, POINT point)
{
	SetMsgHandled(FALSE);

    IUIApplication* pUIApplication = m_pISystemScrollBar->GetUIApplication();
	if (0 != m_nTimer1IDToScroll)
	{
		pUIApplication->KillTimer(m_nTimer1IDToScroll);
		m_nTimer1IDToScroll = 0;
	}
	if (0 != m_nTimer1IDScrolling)
	{
		pUIApplication->KillTimer(m_nTimer1IDScrolling);
		m_nTimer1IDScrolling = 0;
	}

    TrySetBindObjFocus();

	m_nTimer1IDToScroll = pUIApplication->SetTimer(TIMER_TIME_TOSCROLL, m_pISystemScrollBar);

	m_pMgrScrollBar->FireScrollMessage(GetDirType(), SB_LINEUP);
}

void  SystemScrollBar::OnBtn2LButtonDown(UINT nFlags, POINT point)
{
	SetMsgHandled(FALSE);

    IUIApplication* pUIApplication = m_pISystemScrollBar->GetUIApplication();
	if (0 != m_nTimer2IDToScroll)
	{
		pUIApplication->KillTimer(m_nTimer2IDToScroll);
	}
	if (0 != m_nTimer2IDScrolling)
	{
		pUIApplication->KillTimer(m_nTimer2IDScrolling);
	}

    TrySetBindObjFocus();

	m_nTimer2IDToScroll = pUIApplication->SetTimer(TIMER_TIME_TOSCROLL, m_pISystemScrollBar);

	m_pMgrScrollBar->FireScrollMessage(GetDirType(), SB_LINEDOWN);
}
void  SystemScrollBar::OnBtn1LButtonUp(UINT nFlags, POINT point)
{
	SetMsgHandled(FALSE);

    IUIApplication* pUIApplication = m_pISystemScrollBar->GetUIApplication();
	if (0 != m_nTimer1IDToScroll)
	{
		pUIApplication->KillTimer(m_nTimer1IDToScroll);
	}
	if (0 != m_nTimer1IDScrolling)
	{
		pUIApplication->KillTimer(m_nTimer1IDScrolling);
	}

	m_pMgrScrollBar->FireScrollMessage(GetDirType(), SB_ENDSCROLL);
}

void  SystemScrollBar::OnBtn1KillFocus(IObject* pNewFocusObj)
{
	SetMsgHandled(FALSE);

    IUIApplication* pUIApplication = m_pISystemScrollBar->GetUIApplication();
	if (0 != m_nTimer1IDToScroll)
	{
		pUIApplication->KillTimer(m_nTimer1IDToScroll);
	}
	if (0 != m_nTimer1IDScrolling)
	{
		pUIApplication->KillTimer(m_nTimer1IDScrolling);
		m_pMgrScrollBar->FireScrollMessage(GetDirType(), SB_ENDSCROLL);
	}
}

void  SystemScrollBar::OnBtn2LButtonUp(UINT nFlags, POINT point)
{
	SetMsgHandled(FALSE);

    IUIApplication* pUIApplication = m_pISystemScrollBar->GetUIApplication();
	if (0 != m_nTimer2IDToScroll)
	{
		pUIApplication->KillTimer(m_nTimer2IDToScroll);
	}
	if (0 != m_nTimer2IDScrolling)
	{
		pUIApplication->KillTimer(m_nTimer2IDScrolling);
	}

	m_pMgrScrollBar->FireScrollMessage(GetDirType(), SB_ENDSCROLL);
}
void  SystemScrollBar::OnBtn2KillFocus(IObject* pNewFocusObj)
{
	SetMsgHandled(FALSE);

    IUIApplication* pUIApplication = m_pISystemScrollBar->GetUIApplication();
	if (0 != m_nTimer2IDToScroll)
	{
		pUIApplication->KillTimer(m_nTimer2IDToScroll );
	}
	if (0 != m_nTimer2IDScrolling)
	{
		pUIApplication->KillTimer(m_nTimer2IDScrolling);
		m_pMgrScrollBar->FireScrollMessage(GetDirType(), SB_ENDSCROLL);
	}
}

void  SystemScrollBar::CalcScrollBarVisible()
{
	int nRange = m_pMgrScrollBar->GetScrollRange(GetDirType());
	int nPage = m_pMgrScrollBar->GetScrollPage(GetDirType());

	SCROLLBAR_VISIBLE_TYPE eVisibleType = m_pMgrScrollBar->GetScrollBarVisibleType(GetDirType());
	if (nPage >= nRange)
	{
		switch (eVisibleType)
		{
		case SCROLLBAR_VISIBLE_AUTO:
			if (m_pISystemScrollBar->IsSelfVisible())
				m_pISystemScrollBar->SetVisible(false);
			break;

		case SCROLLBAR_VISIBLE_SHOW_ALWAYS:
            m_pISystemScrollBar->SetVisible(true);
			this->m_pBtnThumb->SetVisible(false);
			break;
		}
	}
	else
	{
		switch (eVisibleType)
		{
		case SCROLLBAR_VISIBLE_AUTO:
			if (false == m_pISystemScrollBar->IsSelfVisible())
				this->m_pISystemScrollBar->SetVisible(true);
			break;

		case SCROLLBAR_VISIBLE_SHOW_ALWAYS:
            m_pISystemScrollBar->SetVisible(true);
			this->m_pBtnThumb->SetVisible(true);
			break;
		}
	}
}

bool  SystemScrollBar::CalcThumbButtonPos(bool bNeedUpdateThumbButtonSize)
{
	if (NULL == m_pBtnThumb)
		return false;

	float nPos = (float)m_pMgrScrollBar->GetScrollPos(GetDirType());
	float nRange = (float)m_pMgrScrollBar->GetScrollRange(GetDirType());
	float nPage = (float)m_pMgrScrollBar->GetScrollPage(GetDirType());

	// bounce edge
	if (nPos < 0)
		nRange += -nPos;
	else if (nPos > nRange-nPage)
		nRange += nPos-(nRange-nPage);

	int nNewSize = 0;
	if (bNeedUpdateThumbButtonSize)
	{
		nNewSize = this->CalcThumbButtonSize();
	}
	else
	{
		if (VSCROLLBAR == GetDirType())
			nNewSize = this->m_pBtnThumb->GetHeight();
		else 
			nNewSize = this->m_pBtnThumb->GetWidth();
	}

	if (nNewSize < MIN_TRACK_BTN_SIZE)
		nNewSize = MIN_TRACK_BTN_SIZE;

	CRect rcChannel;
	this->CalcChannelRect(&rcChannel);
	if (rcChannel.IsRectEmpty())
		return false;

	if (VSCROLLBAR == GetDirType())
	{
        if (rcChannel.Height() < nNewSize)
            nNewSize = rcChannel.Height();

		int nThumbBtnPos = (int)(nPos / (nRange-nPage) * (float)(rcChannel.Height() - nNewSize)) + rcChannel.top;

		// bounce edge
		if (nThumbBtnPos < rcChannel.top)
			nThumbBtnPos = rcChannel.top;

		this->m_pBtnThumb->SetObjectPos(rcChannel.left, nThumbBtnPos, rcChannel.Width(), nNewSize, SWP_NOREDRAW);
	}
	else
	{
        if (rcChannel.Width() < nNewSize)
            nNewSize = rcChannel.Width();

		int nThumbBtnPos = (int)(nPos / (nRange-nPage) * (float)(rcChannel.Width() - nNewSize)) + rcChannel.left;
		
		// bounce edge
		if (nThumbBtnPos < rcChannel.left)
			nThumbBtnPos = rcChannel.left;

		this->m_pBtnThumb->SetObjectPos(nThumbBtnPos, rcChannel.top, nNewSize, rcChannel.Height(), SWP_NOREDRAW);
	}
	return true;
}


void  SystemScrollBar::CalcChannelRect(CRect* prc)
{
	m_pISystemScrollBar->GetClientRectInObject(prc);

	if (GetDirType() == VSCROLLBAR)
	{
		if (m_pBtnLineUpLeft)
		{
			int nHeight = m_pBtnLineUpLeft->GetHeight();
			prc->top += nHeight;
		}
		if (m_pBtnLineDownRight)
		{
			int nHeight = m_pBtnLineDownRight->GetHeight();
			prc->bottom -= nHeight;
		}
	}
	else
	{
		if (m_pBtnLineUpLeft)
			prc->left += m_pBtnLineUpLeft->GetWidth();
		if (m_pBtnLineDownRight)
			prc->right -= m_pBtnLineDownRight->GetWidth();
	}
}


int   SystemScrollBar::CalcThumbButtonSize()
{
	if (NULL == m_pBtnThumb)
		return -1;

	CRect rcChannel;
	this->CalcChannelRect(&rcChannel);

	float nPos = (float)m_pMgrScrollBar->GetScrollPos(GetDirType());
	float nPage = (float)m_pMgrScrollBar->GetScrollPage(GetDirType());
	float nRange = (float)m_pMgrScrollBar->GetScrollRange(GetDirType());
	if (0 == nRange)
		return -1;

	// bounce edge
	if (nPos < 0)
		nRange += -nPos;
	else if (nPos > nRange-nPage)
		nRange += nPos-(nRange-nPage);

	if (GetDirType() == VSCROLLBAR)
	{
		int nBtnH = _round((float)rcChannel.Height()*nPage/nRange);
		return nBtnH;
	}
	else
	{
		int nBtnW = _round((float)rcChannel.Width()*nPage / nRange);
		return nBtnW;
	}
}
void  SystemScrollBar::OnCreateByEditor(CREATEBYEDITORDATA* pData)
{
    DO_PARENT_PROCESS_MAPID(ISystemScrollBar, IControl, UIALT_CALLLESS);

	EditorAddObjectResData data = {0};
	data.pParentObj = m_pISystemScrollBar;
	data.pParentXml = pData->pXml;
	data.pUIApp = pData->pUIApp;
	data.objiid = __uuidof(IButton);

	data.ppCreateObj = (void**)&m_pBtnThumb;
	data.szId = SCROLLBAR_THUMB_ID;
	pData->pEditor->AddObjectRes(&data);

	data.ppCreateObj = (void**)&m_pBtnLineUpLeft;
	data.szId = (GetDirType() == HSCROLLBAR) ? SCROLLBAR_LEFT_ID:SCROLLBAR_UP_ID;
	pData->pEditor->AddObjectRes(&data);

	data.ppCreateObj = (void**)&m_pBtnLineDownRight;
	data.szId = (GetDirType() == HSCROLLBAR) ? SCROLLBAR_RIGHT_ID:SCROLLBAR_DOWN_ID;
	pData->pEditor->AddObjectRes(&data);
}

//////////////////////////////////////////////////////////////////////////
SystemVScrollBar::SystemVScrollBar(ISystemVScrollBar* p):SystemScrollBar(p)
{
    m_pISystemVScrollBar = p;
	if (m_pBtnThumb)
		m_pBtnThumb->SetButtonType(BUTTON_TYPE_VSCROLLTHUMB);

	p->SetDescription(SystemVScrollBarDescription::Get());
}

#if 0
void  SystemVScrollBar::SetAttribute(IMapAttribute* pMapAttrib, bool bReload)
{
	// AttributeSerializerWrap as(pData, TEXT("vscrollbar"));

    IUIApplication* pUIApp = m_pISystemScrollBar->GetUIApplication();
// 	if (m_pBtnLineUpLeft && NULL == m_pBtnLineUpLeft->GetBkRender())
// 	{
// 		m_pBtnLineUpLeft->SetButtonStyle(BUTTON_STYLE_SCROLLLINEUP);
//         
// 		IRenderBase* pRender = NULL;
//         pUIApp->CreateRenderBase(RENDER_TYPE_THEME_SCROLLBAR_LINE_UP_BUTTON, m_pBtnLineUpLeft, &pRender);
//         if (pRender)
//         {
//             SERIALIZEDATA data = {0};
//             data.pUIApplication = pUIApp;
//             data.pMapAttrib = pMapAttrib;
//             data.szPrefix = XML_SCROLLBAR_LINE_BUTTON1_ATTR_PRIFIX XML_BACKGND_RENDER_PREFIX;
//             data.nFlags = SERIALIZEFLAG_LOAD|SERIALIZEFLAG_LOAD_ERASEATTR;
//             pRender->Serialize(&data);
// 
// 		    m_pBtnLineUpLeft->SetBkgndRender(pRender);
// 		    SAFE_RELEASE(pRender);
//         }
// 	}
// 	if (m_pBtnLineDownRight && NULL == m_pBtnLineDownRight->GetBkRender())
// 	{
// 		m_pBtnLineDownRight->SetButtonStyle(BUTTON_STYLE_SCROLLLINEDOWN);
// 
//         IRenderBase* pRender = NULL;
//         pUIApp->CreateRenderBase(RENDER_TYPE_THEME_SCROLLBAR_LINE_DOWN_BUTTON, m_pBtnLineDownRight, &pRender);
//         if (pRender)
//         {
//             SERIALIZEDATA data = {0};
//             data.pUIApplication = pUIApp;
//             data.pMapAttrib = pMapAttrib;
//             data.szPrefix = XML_SCROLLBAR_LINE_BUTTON2_ATTR_PRIFIX XML_BACKGND_RENDER_PREFIX;
//             data.nFlags = SERIALIZEFLAG_LOAD|SERIALIZEFLAG_LOAD_ERASEATTR;
//             pRender->Serialize(&data);
// 
//     		m_pBtnLineDownRight->SetBkgndRender(pRender);
// 		    SAFE_RELEASE(pRender);
//         }
// 	}

// 	if (m_pBtnThumb && NULL == m_pBtnThumb->GetBkRender())
// 	{
//         IRenderBase* pRender = NULL;
//         pUIApp->CreateRenderBase(RENDER_TYPE_THEME_VSCROLLBAR_THUMB_BUTTON, m_pBtnThumb, &pRender);
//         if (pRender)
//         {
//             SERIALIZEDATA data = {0};
//             data.pUIApplication = pUIApp;
//             data.pMapAttrib = pMapAttrib;
//             data.szPrefix = XML_SCROLLBAR_THUMB_BUTTON_ATTR_PRIFIX XML_BACKGND_RENDER_PREFIX;
//             data.nFlags = SERIALIZEFLAG_LOAD|SERIALIZEFLAG_LOAD_ERASEATTR;
//             pRender->Serialize(&data);
// 
//     		m_pBtnThumb->SetBkgndRender(pRender);
// 	    	SAFE_RELEASE(pRender);
//         }
// 	}

	// 默认背景(theme)
// 	if (NULL == m_pISystemScrollBar->GetBkRender())
// 	{
//         IRenderBase* pRender = NULL;
//         pUIApp->CreateRenderBase(RENDER_TYPE_THEME_VSCROLLBARBACKGND, m_pISystemScrollBar, &pRender);
//         if (pRender)
//         {
//             SERIALIZEDATA data = {0};
//             data.pUIApplication = pUIApp;
//             data.pMapAttrib = pMapAttrib;
//             data.szPrefix = XML_BACKGND_RENDER_PREFIX;
//             data.nFlags = SERIALIZEFLAG_LOAD|SERIALIZEFLAG_LOAD_ERASEATTR;
//             pRender->Serialize(&data);
// 
//     		m_pISystemScrollBar->SetBkgndRender(pRender);
// 	    	SAFE_RELEASE(pRender);
//         }
// 	}
}

#endif
void  SystemScrollBar::OnLButtonDown(UINT nFlags, POINT point)
{
    if (NULL == m_pBtnThumb)
        return;

    POINT ptObj;
    m_pBtnThumb->WindowPoint2ObjectPoint(&point, &ptObj, true);

	if (GetDirType() == HSCROLLBAR)
	{
		if (ptObj.x < 0)
			m_pMgrScrollBar->FireScrollMessage(GetDirType(), SB_PAGELEFT);
		else
			m_pMgrScrollBar->FireScrollMessage(GetDirType(), SB_PAGERIGHT);
	}
	else
	{
		if (ptObj.y < 0)
			m_pMgrScrollBar->FireScrollMessage(GetDirType(), SB_PAGEUP);
		else
			m_pMgrScrollBar->FireScrollMessage(GetDirType(), SB_PAGEDOWN);
	}
}
void  SystemScrollBar::OnLButtonUp(UINT nFlags, POINT point)
{
    if (NULL == m_pBtnThumb)
        return;

    m_pMgrScrollBar->FireScrollMessage(GetDirType(), SB_ENDSCROLL);
}


// 将thumb button的最前面的位置转换为当前位置 pt相对于scrollbar
int   SystemVScrollBar::WindowPoint2TrackPos(int nUIPos)
{
	float nRange = (float)m_pMgrScrollBar->GetScrollRange(VSCROLLBAR);
	float nPage = (float)m_pMgrScrollBar->GetScrollPage(VSCROLLBAR);

	CRect rcChannel;
	this->CalcChannelRect(&rcChannel);
	int nUIRange = rcChannel.Height();

	if (m_pBtnThumb)
		nUIRange -= m_pBtnThumb->GetHeight();

	if (0 == nUIRange)
		return 0;

	float nPos =  ((float)(nUIPos - rcChannel.top))/((float)nUIRange)*(nRange-nPage);
	return (int)nPos;
}

void  SystemVScrollBar::OnThumbBtnLButtonDown(UINT nFlags, POINT point)
{
	SetMsgHandled(FALSE);

	// 将窗口坐标转换为相对于控件的坐标
	POINT  ptObj;
	m_pBtnThumb->WindowPoint2ObjectPoint(&point, &ptObj, true);
	m_nClickDiff = ptObj.y;

    TrySetBindObjFocus();
}
void  SystemVScrollBar::OnThumbBtnLButtonUp(UINT nFlags, POINT point)
{
	SetMsgHandled(FALSE);

	if (m_bTracking)
	{
		m_pMgrScrollBar->FireScrollMessage(GetDirType(), SB_ENDSCROLL);
	}
	m_bTracking = false;
	m_nClickDiff = 0;
}
void  SystemVScrollBar::OnThumbBtnMousemove(UINT nFlags, POINT point)
{
	SetMsgHandled(FALSE);

	if (!(nFlags & MK_LBUTTON))
		return;

	POINT ptObj;
	m_pISystemScrollBar->WindowPoint2ObjectPoint(&point, &ptObj, true);
	int nNewPos = this->WindowPoint2TrackPos(ptObj.y - m_nClickDiff);

	m_pMgrScrollBar->FireScrollMessage(VSCROLLBAR, SB_THUMBTRACK, nNewPos);
	m_bTracking = true;
}


//////////////////////////////////////////////////////////////////////////

SystemHScrollBar::SystemHScrollBar(ISystemHScrollBar* p):SystemScrollBar(p)
{
    m_pISystemHScrollBar = p;
	p->SetDescription(SystemHScrollBarDescription::Get());
}
#if 0
void  SystemHScrollBar::SetAttribute(IMapAttribute* pMapAttrib, bool bReload)
{
    IUIApplication* pUIApp = m_pISystemScrollBar->GetUIApplication();

// 	if (m_pBtnLineUpLeft && NULL == m_pBtnLineUpLeft->GetBkRender())
// 	{
// 		m_pBtnLineUpLeft->SetButtonStyle(BUTTON_STYLE_SCROLLLINELEFT);
// 
//         IRenderBase* pRender = NULL;
//         pUIApp->CreateRenderBase(RENDER_TYPE_THEME_SCROLLBAR_LINE_LEFT_BUTTON, m_pBtnLineUpLeft, &pRender);
//         if (pRender)
//         {
//     		m_pBtnLineUpLeft->SetBkgndRender(pRender);
// 	    	SAFE_RELEASE(pRender);
//         }
// 	}
// 	if (m_pBtnLineDownRight && NULL == m_pBtnLineDownRight->GetBkRender())
// 	{
// 		m_pBtnLineDownRight->SetButtonStyle(BUTTON_STYLE_SCROLLLINERIGHT);
// 
//         IRenderBase* pRender = NULL;
//         pUIApp->CreateRenderBase(RENDER_TYPE_THEME_SCROLLBAR_LINE_RIGHT_BUTTON, m_pBtnLineDownRight, &pRender);
//         if (pRender)
//         {
// 		    m_pBtnLineDownRight->SetBkgndRender(pRender);
// 		    SAFE_RELEASE(pRender);
//         }
// 	}
// 	if (m_pBtnThumb && NULL == m_pBtnThumb->GetBkRender())
// 	{
//         IRenderBase* pRender = NULL;
//         pUIApp->CreateRenderBase(RENDER_TYPE_THEME_HSCROLLBAR_THUMB_BUTTON, m_pBtnThumb, &pRender);
//         if (pRender)
//         {
// 	    	m_pBtnThumb->SetBkgndRender(pRender);
// 		    SAFE_RELEASE(pRender);
//         }
// 	}

	// 默认背景(theme)
// 	if (NULL == m_pISystemScrollBar->GetBkRender())
// 	{
//         IRenderBase* pRender = NULL;
//         pUIApp->CreateRenderBase(RENDER_TYPE_THEME_HSCROLLBARBACKGND, m_pISystemScrollBar, &pRender);
//         if (pRender)
//         {
//             m_pISystemScrollBar->SetBkgndRender(pRender);
// 		    SAFE_RELEASE(pRender);
//         }
// 	}
}


#endif

// 将thumb button的最前面的位置转换为当前位置  pt相对于scrollbar
int   SystemHScrollBar::WindowPoint2TrackPos(int nUIPos)
{
	float nRange = (float)m_pMgrScrollBar->GetScrollRange(HSCROLLBAR);
	float nPage = (float)m_pMgrScrollBar->GetScrollPage(HSCROLLBAR);

	CRect rcChannel;
	this->CalcChannelRect(&rcChannel);
	int nUIRange = rcChannel.Width();
	if (m_pBtnThumb)
		nUIRange -= m_pBtnThumb->GetWidth();
	if (0 == nUIRange)
		return 0;

	float nPos =  ((float)(nUIPos - rcChannel.left))/((float)nUIRange)*(nRange-nPage);
	return (int)nPos;
}


void  SystemScrollBar::SetAutoSetBindObjFocus(bool b)
{
    m_bAutoSetBindObjFocus = b;
}
void  SystemScrollBar::TrySetBindObjFocus()
{
    if (m_bAutoSetBindObjFocus)
    {
        IObject*  pObj = GetBindObject();
        if (pObj && !pObj->IsFocus())
        {
            UIASSERT(0);
            // TODO:
            //pObj->SetFocusInWindow();
        }
    }
}

void  SystemHScrollBar::OnThumbBtnLButtonDown(UINT nFlags, POINT point)
{
	SetMsgHandled(FALSE);

	// 将窗口坐标转换为相对于控件的坐标
	POINT  ptObj;
	m_pBtnThumb->WindowPoint2ObjectPoint(&point, &ptObj, true);
	m_nClickDiff = ptObj.x;

    TrySetBindObjFocus();
}
void  SystemHScrollBar::OnThumbBtnLButtonUp(UINT nFlags, POINT point)
{
	SetMsgHandled(FALSE);

	if (m_bTracking)
	{
		m_pMgrScrollBar->FireScrollMessage(GetDirType(), SB_ENDSCROLL);
	}
	m_bTracking = false;
	m_nClickDiff = 0;
}
void  SystemHScrollBar::OnThumbBtnMousemove(UINT nFlags, POINT point)
{
	SetMsgHandled(FALSE);

	if (!(nFlags & MK_LBUTTON))
		return;

	POINT ptObj;
	m_pISystemScrollBar->WindowPoint2ObjectPoint(&point, &ptObj, true);
	int nNewPos = this->WindowPoint2TrackPos(ptObj.x - m_nClickDiff);

	m_pMgrScrollBar->FireScrollMessage(HSCROLLBAR, SB_THUMBTRACK, nNewPos);
	m_bTracking = true;
}


//////////////////////////////////////////////////////////////////////////

// 滚动条使用nonclient ext区域，位于border与padding之间
LRESULT  SystemVScrollBar::OnBindObjSize(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	IObject* pBindObj = this->GetBindObject();
	if (NULL == pBindObj)
		return 0;

	if (!m_pISystemScrollBar->IsSelfVisible())
		return 0;

    int nObjWidth = pBindObj->GetWidth();
    int nObjHeight = pBindObj->GetHeight();

	REGION4 rBorder;
	REGION4 rNonClientExt;
	pBindObj->GetBorderRegion(&rBorder);
	pBindObj->GetExtNonClientRegion(&rNonClientExt);

	int nLeft = 0;
	int nRight = 0;
    if (m_eScrollBarRegionType == FLOAT_ON_CLIENT)
    {
		RECT rcClient;
		pBindObj->GetClientRectInObject(&rcClient);
        SIZE s = m_pISystemScrollBar->GetDesiredSize();
		nRight = rcClient.right;
        nLeft = rcClient.right - s.cx;
    }
    else
    {
		nRight = nObjWidth - rBorder.right;
	    nLeft = nRight - rNonClientExt.right;
    }

    int nTop = rBorder.top;
    int nBottom = nObjHeight - rBorder.bottom - rNonClientExt.bottom;
	m_pISystemScrollBar->SetObjectPos(nLeft, nTop, nRight-nLeft, nBottom-nTop, SWP_NOREDRAW);
    
    return 0;
}

LRESULT  SystemHScrollBar::OnBindObjSize(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	IObject* pBindObj = this->GetBindObject();
	if (NULL == pBindObj)
		return 0;

	if (!m_pISystemScrollBar->IsSelfVisible())
		return 0;

	REGION4 rBorder;
	REGION4 rNonClientExt;
	pBindObj->GetBorderRegion(&rBorder);
    pBindObj->GetExtNonClientRegion(&rNonClientExt);

	int nBottom = pBindObj->GetHeight() - rBorder.bottom;
	int nTop = nBottom - rNonClientExt.bottom;
	int nLeft = rBorder.left;
	int nRight = pBindObj->GetWidth() - rBorder.right - rNonClientExt.right;

	m_pISystemScrollBar->SetObjectPos(nLeft, nTop, nRight-nLeft, nBottom-nTop, SWP_NOREDRAW);
    return 0;
}


void  SystemHScrollBar::OnSize(UINT nType, int cx, int cy)
{
    SetMsgHandled(FALSE);

	CRect rcClient;
	m_pISystemScrollBar->GetClientRectInObject(&rcClient);

	int nX1 = rcClient.left;
	int nX2 = rcClient.right;
	if (m_pBtnLineUpLeft)
	{
		SIZE s = {0,0};
		s.cx = m_pBtnLineUpLeft->GetWidth();
		s.cy = m_pBtnLineUpLeft->GetHeight();

		if (s.cx == 0 || s.cy == 0)
			s = m_pBtnLineUpLeft->GetDesiredSize();

		m_pBtnLineUpLeft->SetObjectPos( 
			rcClient.left /*+ m_pBtnLineUpLeft->GetMarginL()*/,
			rcClient.top /*+ m_pBtnLineUpLeft->GetMarginT()*/, 
			s.cx,
			rcClient.Height() /*- m_pBtnLineUpLeft->GetMarginH()*/,
			SWP_NOREDRAW);

		REGION4 rcMargin;
		m_pBtnLineUpLeft->GetMarginRegion(&rcMargin);
		nX1 += s.cx + rcMargin.left + rcMargin.right;
	}
	if (m_pBtnLineDownRight)
	{
		SIZE s = {0,0};
		s.cx = m_pBtnLineDownRight->GetWidth();
		s.cy = m_pBtnLineDownRight->GetHeight();

		if (s.cx == 0 || s.cy == 0)
			s = m_pBtnLineDownRight->GetDesiredSize();

		m_pBtnLineDownRight->SetObjectPos( 
			rcClient.right - s.cx /*- m_pBtnLineDownRight->GetMarginW()*/,
			rcClient.top /*+ m_pBtnLineDownRight->GetMarginT()*/, 
			s.cx,
			rcClient.Height() /*- m_pBtnLineDownRight->GetMarginH()*/,
			SWP_NOREDRAW);

		nX2 -= s.cx /*+ m_pBtnLineDownRight->GetMarginW()*/;
	}
	this->CalcThumbButtonPos(true);
}

void  SystemVScrollBar::OnSize(UINT nType, int cx, int cy)
{
    SetMsgHandled(FALSE);

	CRect rcClient;
	m_pISystemScrollBar->GetClientRectInObject(&rcClient);

	int nY1 = rcClient.top;
	int nY2 = rcClient.bottom;
	if (m_pBtnLineUpLeft)
	{
		SIZE s = {0,0};
		s.cx = m_pBtnLineUpLeft->GetWidth();
		s.cy = m_pBtnLineUpLeft->GetHeight();

		if (s.cx == 0 || s.cy == 0)
		{
			s = m_pBtnLineUpLeft->GetDesiredSize();
		}

		// 加了margin之后不好显示，暂不处理该margin
		m_pBtnLineUpLeft->SetObjectPos( 
			rcClient.left/* + m_pBtnLineUpLeft->GetMarginL()*/,
			rcClient.top /*+ m_pBtnLineUpLeft->GetMarginT()*/, 
			rcClient.Width() /*- m_pBtnLineUpLeft->GetMarginW()*/, 
			s.cy, 
			SWP_NOREDRAW);
		nY1 += s.cy /*+ m_pBtnLineUpLeft->GetMarginH()*/;
	}
	if(NULL != m_pBtnLineDownRight)
	{
		SIZE s = {0,0};
		s.cx = m_pBtnLineDownRight->GetWidth();
		s.cy = m_pBtnLineDownRight->GetHeight();

		if (s.cx == 0 || s.cy == 0)
		{
			s = m_pBtnLineDownRight->GetDesiredSize();
		}

		m_pBtnLineDownRight->SetObjectPos(
			rcClient.left /*+ m_pBtnLineDownRight->GetMarginL()*/ , 
			rcClient.bottom -s.cy /*- m_pBtnLineDownRight->GetMarginH()*/,
			rcClient.Width() /*- m_pBtnLineDownRight->GetMarginW()*/, 
			s.cy, 
			SWP_NOREDRAW);
		nY2 -= s.cy /*+ m_pBtnLineDownRight->GetMarginH()*/;
	}
	this->CalcThumbButtonPos(true);
}
void SystemHScrollBar::OnCalcParentNonClientRect(CRegion4* prcNonClientRegion)
{
    if (!m_pISystemScrollBar->IsSelfVisible() && 
        m_eScrollBarRegionType != KEEP_HOLD)
        return;

    if (m_eScrollBarRegionType != FLOAT_ON_CLIENT)
    {
	    int nHeight = m_pISystemScrollBar->GetHeight();
	    if (0 == nHeight)
	    {
		    SIZE s = m_pISystemScrollBar->GetDesiredSize();
		    nHeight = s.cy;
	    }
	    prcNonClientRegion->bottom += nHeight;
    }
}
void SystemVScrollBar::OnCalcParentNonClientRect(CRegion4* prcNonClientRegion)
{
	if (!m_pISystemScrollBar->IsSelfVisible() && 
        m_eScrollBarRegionType != KEEP_HOLD)
		return;

    if (m_eScrollBarRegionType != FLOAT_ON_CLIENT)
    {
	    int nWidth = m_pISystemScrollBar->GetWidth();
	    if (0 == nWidth)
	    {
		    SIZE s = m_pISystemScrollBar->GetDesiredSize();
		    nWidth = s.cx;
	    }
	    prcNonClientRegion->right += nWidth;
    }
}
