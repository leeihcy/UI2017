#pragma once
#include "UISDK\Control\Src\Control\Caret\icaret.h"
#include "UISDK\Kernel\Inc\Util\layeredwindow.h"

namespace UI
{

//
// 连续困扰了4天的问题：使用窗口作为光标后，在编辑框中输入文字，鼠标会不断的闪烁!
//
// 1. 只要有setwindowpos move or size/show hide window，就会导致当前鼠标下的窗口接受到一个NCHITTEST/SETCURSOR/MOUSEMOVE的消息，
//    无论这是否是一个分层窗口
// 2. 通过采用将CCaretWindow分部镂空实现的隐藏，可以达到不发送这样的消息。但是setcaretpos要怎么用这样的方法来实现？
// 3. Mouse Vanish  -- 没用，代码中修改不了，而且在控制面板--鼠标中设置该选项也不起作用
// 4. 应该不是使用showcursor实现的隐藏光标，否则鼠标移动时就不会显示了
// 5. 难道是setcursor(null)? 怎么破解？
//
//    --> 就是因为内部直接调用的SetCursor(NULL)导致的
//
//
//  注: 20130701为什么要使用分层窗口作为光标：
//      例如Combobox控件，当使用普通窗口作为光标时，光标在Edit中不停的隐藏/显示，导致系统
//      会不停的给当前鼠标下的窗口发送WM_MOUSEMOVE消息，从而干扰listbox当前的hover项。
//
class CaretWindow : public ICaret,   // 注：将ICaret放在CWindowImpl<CCaretWindow>后面，会导致对象释放时崩溃，原因未知
                     public CWindowImpl<CaretWindow>
{
public:
    CaretWindow(void);
    ~CaretWindow(void);

     DECLARE_WND_CLASS(_T("CaretWindow"))	
 
    BEGIN_MSG_MAP_EX(CaretWindow)
         MSG_WM_CREATE(OnCreate)
//         MSG_WM_MOVE(OnMove)
         MSG_WM_DESTROY(OnDestroy)
         MSG_WM_PAINT(OnPaint)
         MSG_WM_ERASEBKGND(OnEraseBkgnd)
         MSG_WM_TIMER(OnTimer)
         MESSAGE_HANDLER(UI_WM_SYNC_WINDOWPOSCHANING, OnSyncWindowPosChanging);
    END_MSG_MAP()

protected:
     int   OnCreate(LPCREATESTRUCT lpCreateStruct);
     void  OnDestroy();
     void  OnPaint(HDC hDC);
     BOOL  OnEraseBkgnd(HDC hDC);
     void  OnTimer(UINT_PTR nIDEvent);
     void  OnMove(CPoint ptPos);
     virtual void  OnControlPaint(IRenderTarget* p){};
     LRESULT OnSyncWindowPosChanging(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

public:
    virtual CARET_TYPE  GetType() { return CARET_TYPE_WINDOW; }
    virtual BOOL  Create(IObject* pObj, HWND hWndParent, HBITMAP hbmp, int nWidth, int nHeight);
    virtual void  Destroy(bool bRedraw);
    virtual void  SetPos(int x, int y, bool bRedraw);
    virtual void  Show(bool bRedraw);
    virtual void  Hide(bool bRedraw);
    virtual IObject*  GetObject() { return m_pObject; }

    void  OnWindowMove();
    void  OnControlMove();
    void  DrawCaretBitmap(HBITMAP hbmp, HDC hDstDC, HBITMAP hDstBmp, int nWidht, int nHeight);

protected:
    void  _SetPos(int x, int y);
public:
    HWND   m_hCtrlWnd;
    IObject*  m_pObject;
    POINT  m_ptLast;   // 用于在主窗口移动的时候维持光标在EDIT中的位置

    bool   m_bShow;    // 是否显示
    bool   m_bVisible; // 闪烁状态
    bool   m_bTestEditVisibleEveryTime;   // 用于解决Edit被滚动的问题
private:
    CLayeredWindow*  m_pLayeredWnd;
};

}