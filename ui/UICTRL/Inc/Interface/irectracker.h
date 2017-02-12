#ifndef  _INCLUDED_IRECTTRACKER_H_
#define  _INCLUDED_IRECTTRACKER_H_
#include "..\..\..\UISDK\Inc\Interface\icontrol.h"

namespace UI
{


enum RectTrackerHit
{
    hitNothing = -1,
    hitTopLeft = 0, 
    hitTopRight = 1, 
    hitBottomRight = 2, 
    hitBottomLeft = 3,
    hitTop = 4, 
    hitRight = 5, 
    hitBottom = 6, 
    hitLeft = 7, 
    hitMiddle = 8,   // 用于表示拖拽move
    hitCount = 9,    // 
};

#define XML_RECTTRACKER_MASK   TEXT("mask")
#define XML_RECTTRACKER_MASK_NONE  TEXT("none")
#define XML_RECTTRACKER_MASK_MOVE  TEXT("move")
#define XML_RECTTRACKER_MASK_SIZE  TEXT("size")
#define XML_RECTTRACKER_MASK_SIZEMOVE  TEXT("sizemove")
enum RectTrackerMask
{
    recttrackermask_none = 0,
    recttrackermask_move = 1,
    recttrackermask_size = 2,
    recttrackermask_sizemove = 3,
    recttrackermask_iscontainer = 4,  // 这是一个容器，内部还有其它元素
};

#define XML_RECTTRACKER_HANDLE_SIZE   TEXT("handlesize")
#define XML_RECTTRACKER_LINE_SPACE    TEXT("linespace")
#define XML_RECTTRACKER_DRAW_BORDER    TEXT("drawborder")
// 将BORDER也限制在父对象内部（不仅仅是real rect）
#define XML_RECTTRACKER_LIMIT_BORDER_IN_PARENT    TEXT("limitborderinparent")


// 鼠标滚轮通知
// wParam/lParam同WM_MOUSEWHEEL
// return: 0未处理， 1已处理
#define RectTracker_NM_MOUSEWHEEL  153201717

interface  IRectTrackerRsp
{
    // 通知外部开始移动，先保存一次原始位置
    virtual void  OnRectTrackerDragStart() = 0;
    // 在原始位置的基础上移动的距离
    virtual void  OnRectTrackerDrag(RectTrackerHit eHit, int xDelta, int yDelta) = 0;
    // 通知外部移动结束，保存当前值
    virtual void  OnRectTrackerDragEnd() = 0;
    // 绘制
    virtual void  Draw(HDC) = 0;
    // 坐标转换
    virtual void  DP2LP(POINT* p) = 0;
	// 释放
	virtual void  Release() = 0;
};

class RectTracker;
interface __declspec(uuid("5B6657F0-9944-4DFA-9B1C-538AB64B005E")) UICTRL_API IRectTracker : public IControl
{
    void  SetRsp(IRectTrackerRsp* prsp);
    void  SetRealRectInParent(RECT* prc);
    void  SetHandleMask(UINT nMask);
    void  GetRealRectInWindow(RECT* prc);
    void  GetRealRectInControl(RECT* prc);
	void  GetRealRectInParent(RECT* prc);
    int   GetHandleSize();
    int   GetLineSpace(); 
    void  SetDrawBorder(bool b);
    void  SetCursor(int nIndex, HCURSOR hCursor);
	void  SetHandleMask(RectTrackerMask mask);

	signal2<IRectTracker*, LPCRECT>&  RectChangedEvent();
	signal<IRectTracker*>&  ClickEvent();

	UI_DECLARE_INTERFACE_ACROSSMODULE(RectTracker);
};


}


#endif // _INCLUDED_IRECTTRACKER_H_