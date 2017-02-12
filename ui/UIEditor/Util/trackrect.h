#pragma once
//
// 2014.4.16
// 借鉴MFC CTrackRect，实现控件拖拽
//
// 支持鼠标拖拽、键盘拖拽，支持拖拽模式设置
//
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

enum RectTrackerMask
{
    recttrackermask_none = 0,
    recttrackermask_move = 1,
    recttrackermask_size = 2,
    recttrackermask_sizemove = 3,
    recttrackermask_iscontainer = 4,  // 这是一个容器，内部还有其它元素
};

interface  IRectTrackerRsp
{
    // 通知外部开始移动，先保存一次原始位置
    virtual void  OnRectTrackerDragStart() = 0;
    // 在原始位置的基础上移动的距离
    virtual void  OnRectTrackerDrag(RectTrackerHit eHit, int xDelta, int yDelta) = 0;
    // 通知外部移动结束，保存当前值
    virtual void  OnRectTrackerDragEnd() = 0;

    // 坐标转换
    virtual void  DP2LP(POINT* p) = 0;
};
class RectTracker
{
public:
    static void Init();
    static void Release();
    
    RectTracker();
    ~RectTracker();

    void  SetInfo(HWND hWnd, IRectTrackerRsp* prsp)
    {
        m_hWnd = hWnd;
        m_prsp = prsp;
    }
    void  Update(RECT*  prc, UINT nMask);
    void  Draw(HDC  hDC);
    BOOL  ProcessMessage(UINT nMsg, WPARAM wParam, LPARAM lParam, long* plRet);
    
protected:
    RectTrackerHit  HitTest(POINT  pt);
    RectTrackerHit  HitTestUnderCursor();

    bool   IsContainer();
    bool   CanSize();
    bool   CanMove();

	void   GetHandleRect(RectTrackerHit nIndex, RECT* prc);

public:
	bool     m_bStartup;  // 是否有选中对象

    HWND     m_hWnd;
    CRect    m_rect;
    UINT     m_nMask;   
    int      m_nHandleSize;  
    IRectTrackerRsp*  m_prsp;      // 响应者

    RectTrackerHit  m_eActionHit;  // 当前鼠标/键盘拖拽时的操作

    bool     m_bMouseDraging;      // 标志，是否开启鼠标拖拽
    POINT    m_ptLButtonDown;      // 记录鼠标按下时的位置，用于移动控件
    
    bool     m_bKeyboardDraging;   // 标志，是否开户键盘拖拽
    UINT     m_nKeyPushdown;       // 按的是哪个键
    long     m_nKeyboardHitCount;  // 键盘事件触发次数(按住不放)

    static HCURSOR  s_hCursor[9];
    static long  s_lRef;
};