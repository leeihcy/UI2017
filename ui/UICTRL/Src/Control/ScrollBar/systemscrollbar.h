#pragma once
#include "Inc\Interface\iscrollbar.h"
#include "..\UISDK\Inc\Interface\iscrollbarmanager.h"

namespace UI
{
interface  IButton;

#define MIN_TRACK_BTN_SIZE 20

#define ALT_MSG_ID_BUTTON1       1
#define ALT_MSG_ID_BUTTON2       2
#define ALT_MSG_ID_THUMB_BTN     3

/*
<SystemVScrollBar id="__vscrollbar" width="18" bkgnd.render.type="theme.vscrollbar.bkg">
  <Button id="__scrollbar_up" bkgnd.render.type="theme.vscrollbar.up" height="18"/>
  <Button id="__scrollbar_thumb" bkgnd.render.type="theme.vscrollbar.thumb"/>
  <Button id="__scrollbar_down" bkgnd.render.type="theme.vscrollbar.down" height="18"/>
</SystemVScrollBar>
<SystemHScrollBar id="__hscrollbar" height="18" bkgnd.render.type="theme.hscrollbar.bkg">
  <Button id="__scrollbar_left" bkgnd.render.type="theme.hscrollbar.left" width="18"/>
  <Button id="__scrollbar_thumb" bkgnd.render.type="theme.hscrollbar.thumb"/>
  <Button id="__scrollbar_right" bkgnd.render.type="theme.hscrollbar.right" width="18"/>
</SystemHScrollBar>
*/

// 操作系统类型滚动条：两侧两个按钮 + 中间一个THUBM按钮
class SystemScrollBar : 
            public MessageProxy,
            public UIA::IAnimateEventCallback
{
public:
	SystemScrollBar(ISystemScrollBar* p);
	virtual ~SystemScrollBar();

	UI_BEGIN_MSG_MAP()
		UIMSG_PAINT(OnPaint)
		UIMSG_TIMER(OnTimer)
        MSG_WM_LBUTTONDOWN(OnLButtonDown)
        MSG_WM_LBUTTONDBLCLK(OnLButtonDown)
        MSG_WM_LBUTTONUP(OnLButtonUp)
        UIMSG_FINALRELEASE(FinalRelease)
        UIMSG_HANDLER_EX(UI_WM_SCROLLBAR_INFO_CHANGED, 
                OnScrollBarInfoChanged)
        UIMSG_HANDLER_EX(UI_WM_SCROLLBAR_BINDOBJHOVER_CHANGED, 
                OnBindObjHoverChanged)
        UIMSG_STATECHANGED(OnHoverChanged)
        UIMSG_GETDESIREDSIZE(GetDesiredSize)
		UIMSG_INITIALIZE(OnInitialize)
		UIMSG_FINALCONSTRUCT(FinalConstruct)

	UIALT_MSG_MAP(ALT_MSG_ID_BUTTON1)
		MSG_WM_LBUTTONDOWN(OnBtn1LButtonDown)	
		MSG_WM_LBUTTONUP(OnBtn1LButtonUp)
		UIMSG_KILLFOCUS(OnBtn1KillFocus)
		UIMSG_STATECHANGED(OnHoverChanged)

	UIALT_MSG_MAP(ALT_MSG_ID_BUTTON2)
		MSG_WM_LBUTTONDOWN(OnBtn2LButtonDown)	
		MSG_WM_LBUTTONUP(OnBtn2LButtonUp)
		UIMSG_KILLFOCUS(OnBtn2KillFocus)
		UIMSG_STATECHANGED(OnHoverChanged)

    UIALT_MSG_MAP(ALT_MSG_ID_THUMB_BTN)
		UIMSG_STATECHANGED(OnHoverChanged)

	UIALT_MSG_MAP(UIALT_CALLLESS)
		UIMSG_CREATEBYEDITOR(OnCreateByEditor)

	UI_END_MSG_MAP_CHAIN_PARENT_Ixxx(SystemScrollBar, IControl)

public:
    void  SetIScrollBarMgr(IScrollBarManager* p);

	void  CalcScrollBarVisible();
	void  GetDesiredSize(SIZE* pSize);
    void  SetAutoSetBindObjFocus(bool b);
    void  SetFloatOnClientRegion(bool b);
    void  SetKeepHoldNonClientRegion(bool b);
    
protected:
	HRESULT  FinalConstruct(ISkinRes* p);
    void  OnInitialize();
    void  FinalRelease();
	void  OnCreateByEditor(CREATEBYEDITORDATA* pData);
	void  OnPaint(IRenderTarget*);
	void  OnTimer(UINT_PTR nIDEvent, LPARAM lParam);
	void  OnBtn1LButtonDown(UINT nFlags, POINT point);
	void  OnBtn2LButtonDown(UINT nFlags, POINT point);
	void  OnBtn1LButtonUp(UINT nFlags, POINT point);
	void  OnBtn1KillFocus(IObject* pNewFocusObj);
	void  OnBtn2LButtonUp(UINT nFlags, POINT point);
	void  OnBtn2KillFocus(IObject* pNewFocusObj);
    void  OnLButtonDown(UINT nFlags, POINT point);
    void  OnLButtonUp(UINT nFlags, POINT point);

    LRESULT  OnScrollBarInfoChanged(UINT uMsg, WPARAM wParam, LPARAM lParam);
    //LRESULT  OnHoverChanged(UINT uMsg, WPARAM wParam, LPARAM lParam);
	void  OnHoverChanged(UINT nMask);
    LRESULT  OnBindObjHoverChanged(UINT uMsg, WPARAM wParam, LPARAM lParam);

    virtual UIA::E_ANIMATE_TICK_RESULT  OnAnimateTick(UIA::IStoryboard*) override;

protected:
	IObject*  GetBindObject();
    void  TrySetBindObjFocus();
    void  CheckHoverChanged();
    bool  IsScrollbarHover();

    void  CreateHoverAnimate();
    void  DestroyHoverAnimate();

	bool  CalcThumbButtonPos(bool bNeedUpdateThumbButtonSize); 
	void  CalcChannelRect(CRect* prc);
	int   CalcThumbButtonSize();

    virtual SCROLLBAR_DIRECTION_TYPE  GetDirType() { return VSCROLLBAR; }

private:
	UINT_PTR  m_nTimer1IDToScroll;    // 按住lineup button后，500ms后开始滚动
	UINT_PTR  m_nTimer2IDToScroll;    // 按住linedown button后，500ms后开始滚动
	UINT_PTR  m_nTimer1IDScrolling;   // 按住lineup button后，连续滚动
	UINT_PTR  m_nTimer2IDScrolling;   // 按住linedown button后，连续滚动

protected:
    ISystemScrollBar*   m_pISystemScrollBar;
    IScrollBarManager*  m_pMgrScrollBar;

    IButton*  m_pBtnLineUpLeft;
    IButton*  m_pBtnLineDownRight;
    IButton*  m_pBtnThumb;

    bool  m_bAutoSetBindObjFocus;   // 点击滚动条时，给控件设置焦点

	// 用于检测数据发生改变时，刷新thumbbtn的位置
	int  m_nOldPage;
	int  m_nOldRange;
	int  m_nOldPos;

    bool  m_bHoverVisible;  // 滚动条控件仅在鼠标hover时可见。mac风格

    enum SCROLLBAR_REGION_TYPE
    {
        HOLD_ONLY_VISIBLE,  // 只在可见时占用非客户区
        FLOAT_ON_CLIENT,    // 不占用非客户区位置，飘浮在客户区域
        KEEP_HOLD,          // 一直占用非客户区，不管自己是否可见
    };
    SCROLLBAR_REGION_TYPE  m_eScrollBarRegionType;


    // 动画
    bool  m_bHover;       // 列表控件及滚动条是否hover.press。用于显隐滚动条

	int   m_nClickDiff;   // 鼠标在Thumb btn按下时的位置标识
	bool  m_bTracking;    // 是否正在拖拽Thumb btn滚动
};  


class SystemVScrollBar : public SystemScrollBar
{
public:
    SystemVScrollBar(ISystemVScrollBar* p);

	UI_BEGIN_MSG_MAP()
        UIMSG_SIZE(OnSize)
        UIMSG_CALCPARENTNONCLIENTRECT(OnCalcParentNonClientRect)
        UIMSG_HANDLER_EX(UI_WM_SCROLLBAR_BINDOBJSIZE_CHANGED, OnBindObjSize)
        UIMSG_QUERYINTERFACE(SystemVScrollBar)
    
    UIALT_MSG_MAP(ALT_MSG_ID_THUMB_BTN)
        MSG_WM_LBUTTONDOWN(OnThumbBtnLButtonDown)	
        MSG_WM_LBUTTONUP(OnThumbBtnLButtonUp)	
        MSG_WM_MOUSEMOVE(OnThumbBtnMousemove)

   UI_END_MSG_MAP_CHAIN_PARENT(SystemScrollBar)

public:
    virtual SCROLLBAR_DIRECTION_TYPE  GetDirType() { return VSCROLLBAR; }

protected:
    void  OnSize(UINT nType, int cx, int cy);
    void  OnCalcParentNonClientRect(CRegion4* prcNonClientRegion);
    LRESULT  OnBindObjSize(UINT uMsg, WPARAM wParam, LPARAM lParam);

    int   WindowPoint2TrackPos(int nUIPos);
    void  OnThumbBtnLButtonDown(UINT nFlags, POINT point);
    void  OnThumbBtnLButtonUp(UINT nFlags, POINT point);
    void  OnThumbBtnMousemove(UINT nFlags, POINT point);

private:
    ISystemVScrollBar*  m_pISystemVScrollBar;
};
class SystemHScrollBar : public SystemScrollBar
{
public:
	SystemHScrollBar(ISystemHScrollBar* p);

	UI_BEGIN_MSG_MAP()
		UIMSG_SIZE(OnSize)
		MSG_WM_LBUTTONDOWN(OnLButtonDown)
		MSG_WM_LBUTTONUP(OnLButtonUp)
		UIMSG_CALCPARENTNONCLIENTRECT(OnCalcParentNonClientRect)
        UIMSG_HANDLER_EX(UI_WM_SCROLLBAR_BINDOBJSIZE_CHANGED, OnBindObjSize)
        UIMSG_QUERYINTERFACE(SystemHScrollBar)

	UIALT_MSG_MAP(ALT_MSG_ID_THUMB_BTN)
		MSG_WM_LBUTTONDOWN(OnThumbBtnLButtonDown)	
		MSG_WM_LBUTTONUP(OnThumbBtnLButtonUp)	
		MSG_WM_MOUSEMOVE(OnThumbBtnMousemove)

	UI_END_MSG_MAP_CHAIN_PARENT(SystemScrollBar)

public:
    virtual SCROLLBAR_DIRECTION_TYPE  GetDirType() { return HSCROLLBAR; }

protected:
	void  OnSize(UINT nType, int cx, int cy);
	void  OnCalcParentNonClientRect(CRegion4* prcNonClientRegion);
	LRESULT  OnBindObjSize(UINT uMsg, WPARAM wParam, LPARAM lParam);

	int   WindowPoint2TrackPos(int nUIPos);

	void  OnThumbBtnLButtonDown(UINT nFlags, POINT point);
	void  OnThumbBtnLButtonUp(UINT nFlags, POINT point);
	void  OnThumbBtnMousemove(UINT nFlags, POINT point);

private:
    ISystemHScrollBar*  m_pISystemHScrollBar;
};

}
