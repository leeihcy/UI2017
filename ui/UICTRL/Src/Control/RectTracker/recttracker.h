#pragma once
#include "Inc\Interface\irectracker.h"

namespace UI
{
//
// 2017/1/25
// 注： IRectTrackerRsp正在逐步被废弃。采用抛事件的方式现在更好维护一点
//
class RectTracker : public MessageProxy
{
public:
    RectTracker(IRectTracker*);
    ~RectTracker();

    void Init();
    void Release();

    UI_BEGIN_MSG_MAP()
        if (m_bStartup)
        {
            MSG_WM_SETCURSOR(OnSetCursor)
            UIMSG_PAINT(OnPaint)
            MESSAGE_HANDLER_EX(WM_LBUTTONDBLCLK, OnLButtonDown)
            MESSAGE_HANDLER_EX(WM_LBUTTONDOWN, OnLButtonDown)
            MESSAGE_HANDLER_EX(WM_LBUTTONUP, OnLButtonUp)
            MESSAGE_HANDLER_EX(WM_MOUSEMOVE, OnMouseMove)
            MESSAGE_HANDLER_EX(WM_KEYDOWN, OnKeyDown)
            MESSAGE_HANDLER_EX(WM_KEYUP, OnKeyUp)
            MESSAGE_HANDLER_EX(WM_CANCELMODE, OnCancelMode)
            MESSAGE_HANDLER_EX(WM_MOUSEWHEEL, OnMouseWheel) 
        }
		MESSAGE_HANDLER_EX(WM_WINDOWPOSCHANGED, OnPosChanged)
		UIMSG_SERIALIZE(OnSerialize)
        UIMSG_INITIALIZE(OnInitialize)
    UI_END_MSG_MAP_CHAIN_PARENT_Ixxx(RectTracker, IControl)

public:
	IRectTracker*  GetIRectTracker();

    void  SetRsp(IRectTrackerRsp* prsp);
    void  SetRealRectInParent(RECT* prcRealObj);
	void  SetHandleMask(UINT nMask);
	UINT  GetHandleMask();
    void  GetRealRectInWindow(RECT* prc);
    void  GetRealRectInControl(RECT* prc);
	void  GetRealRectInParent(RECT* prc);
    int   GetHandleSize() { return m_nHandleSize; }
    int   GetLineSpace() { return m_nLineSpace; }
    void  SetDrawBorder(bool b) { m_bDrawBroder = b; }

    void  SetCursor(int nIndex, HCURSOR hCursor);

private:
    void  OnPaint(IRenderTarget*);
	void  OnInitialize();
	void  OnSerialize(SERIALIZEDATA* pData);

    BOOL  OnSetCursor(HWND wnd, UINT nHitTest, UINT message);
    LRESULT OnLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam);
    LRESULT OnLButtonUp(UINT uMsg, WPARAM wParam, LPARAM lParam);
    LRESULT OnMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam);
    LRESULT OnKeyDown(UINT uMsg, WPARAM wParam, LPARAM lParam);
    LRESULT OnKeyUp(UINT uMsg, WPARAM wParam, LPARAM lParam);
    LRESULT OnMouseWheel(UINT uMsg, WPARAM wParam, LPARAM lParam);
    LRESULT OnCancelMode(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnPosChanged(UINT uMsg, WPARAM wParam, LPARAM lParam);

private:
    void  DrawGdi(HDC hDC);
    void  DrawGdiplus(HDC hDC);

    RectTrackerHit  HitTest(POINT  pt);
    RectTrackerHit  HitTestUnderCursor();

    bool   IsContainer();
    bool   CanSize();
    bool   CanMove();

	void   GetHandleRect(RectTrackerHit nIndex, RECT* prc, bool bControlRect);
    void   GetBolderRectInControl(RECT* prc);
    void   GetBolderRectInWindow(RECT* prc);
	void   UpdateRealRect();
    void   CalcWindowRectFromRealRect(RECT* prcReal, RECT* prcCtrl);
	// void   CalcParentRectFromRealRect(RECT* prcReal, RECT* prcCtrl);

	void  OnRectTrackerDragStart();
	void  OnRectTrackerDragEnd();
	void  OnRectTrackerDrag(RectTrackerHit eHit, int xDelta, int yDelta);

public:
	bool     m_bStartup;           // 是否有选中对象
    bool     m_bDrawBroder;        // 是否绘制边框线。

    CRect    m_realRect;           // 窗口坐标，实际的对象位置，不是控件位置。由它控制着控件坐标
    int      m_nHandleSize;  
    int      m_nLineSpace;         // 边框线条与对象之间间距

    UINT     m_nMask;   
    IRectTrackerRsp*  m_prsp;      // 响应者
	UI::Color  m_colorBorder;      // 边框颜色

    RectTrackerHit  m_eActionHit;  // 当前鼠标/键盘拖拽时的操作

    bool     m_bMouseDraging;      // 标志，是否开启鼠标拖拽
    POINT    m_ptLButtonDown;      // 记录鼠标按下时的位置，用于移动控件
    
    bool     m_bKeyboardDraging;   // 标志，是否开户键盘拖拽
    UINT     m_nKeyPushdown;       // 按的是哪个键
    long     m_nKeyboardHitCount;  // 键盘事件触发次数(按住不放)

    HCURSOR  m_hCursor[10];        // 对应RectTrackerHit。另外补充一个hitMiddle拖拽时鼠标按下的样式

	CRect  m_rectObjectSave;       // 拖拽控件大小前保存该控件位置
	bool   m_bLimitBorderInParent; // 将边框也限制在父对象内部，解决控件右对齐场景下BUG

private:
    IRectTracker*  m_pIRectTracker;

public:
	signal2<IRectTracker*, LPCRECT>  rect_changed;
	signal<IRectTracker*>  click;
};

}