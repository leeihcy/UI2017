#include "stdafx.h"
#include "ownerdrawcaret.h"
#include "UISDK\Kernel\Inc\Interface\iobject.h"

namespace UI
{

OwnerDrawCaret::OwnerDrawCaret()
{
    m_hWnd = NULL;
    m_pObject = NULL;
    m_ptLast.x = m_ptLast.y = -1;
    m_sizeCaret.cx = m_sizeCaret.cy = 0;
    m_rcDraw.SetRectEmpty();
    m_bVisible = false;
    m_nTimerId = 0;
}
OwnerDrawCaret::~OwnerDrawCaret()
{
    DestroyTimer();
}

BOOL  OwnerDrawCaret::Create(IObject* pObj, HWND hWndParent, HBITMAP hbmp, int nWidth, int nHeight)
{
    m_hWnd = hWndParent;
    m_pObject = pObj;
    m_sizeCaret.cx = nWidth;
    m_sizeCaret.cy = nHeight;
    
    return TRUE;
}
void  OwnerDrawCaret::Destroy()
{
    Hide();

    m_hWnd = NULL;
    m_pObject = NULL;
    m_ptLast.x = m_ptLast.y = -1;
    m_sizeCaret.cx = m_sizeCaret.cy = 0;
    m_rcDraw.SetRectEmpty();
}

void  OwnerDrawCaret::Show()
{
    CreateTimer();
}
void  OwnerDrawCaret::Hide()
{
    DestroyTimer();
    if (m_bVisible)
        DoPaint();
}

void  OwnerDrawCaret::SetPosBase(int x, int y)
{
    m_ptLast.x = x;
    m_ptLast.y = y;

    if (m_pObject)
    {
        // 偏移
        POINT ptCtrl = m_pObject->GetRealPosInWindow(); 

        x += ptCtrl.x;
        y += ptCtrl.y;

        //  裁剪 
        RECT  rcVisible;
        m_pObject->GetObjectVisibleRect(&rcVisible, true);

        RECT  rcCaret = {x, y, x+m_sizeCaret.cx, y+m_sizeCaret.cy};
        m_rcDraw.IntersectRect(&rcCaret, &rcVisible);
    }
}

void  OwnerDrawCaret::SetPos(int x, int y)
{
    if (m_bVisible) // 先隐藏
        DoPaint();

    SetPosBase(x, y);

    DoPaint();  // 立即显示，确保能看到光标移动
}

void  OwnerDrawCaret::DoPaint()
{
    if (!m_hWnd)
        return;

    if (m_rcDraw.IsRectEmpty())
        return;

    HDC hDC = GetDC(m_hWnd);
    HPEN hPen = CreatePen(PS_SOLID, 1, 0);
    HPEN hOldPen = (HPEN)::SelectObject(hDC, hPen);

    int nOld = ::SetROP2(hDC, R2_NOTXORPEN);
    ::MoveToEx(hDC, m_rcDraw.left, m_rcDraw.top, NULL);
    ::LineTo(hDC, m_rcDraw.left, m_rcDraw.bottom);
    ::SetROP2(hDC, nOld);

    SelectObject(hDC, hOldPen);
    DeleteObject(hPen);
    ::ReleaseDC(m_hWnd, hDC);

    m_bVisible = !m_bVisible;
}

void  TimerProc_OwnerDrawCaret(UINT_PTR nTimerID, TimerItem* pTimerItem)
{
    OwnerDrawCaret* pThis = (OwnerDrawCaret*)pTimerItem->lParam;
    pThis->DoPaint();
}
void  OwnerDrawCaret::CreateTimer()
{
    if (!m_pObject)
        return;

    if (m_nTimerId)
        m_pObject->GetUIApplication()->KillTimer(m_nTimerId);

    TimerItem item;
    item.pProc = TimerProc_OwnerDrawCaret;
    item.lParam = (LPARAM)this;
    m_nTimerId = m_pObject->GetUIApplication()->SetTimer(
        GetCaretBlinkTime(), &item);
}
void  OwnerDrawCaret::DestroyTimer()
{
    if (0 != m_nTimerId)
        m_pObject->GetUIApplication()->KillTimer(m_nTimerId);
}
// void  OwnerDrawCaret::PostCommitWindowBuffer(HDC hMemDC)
// {
//     SetPosBase(m_ptLast.x, m_ptLast.y);
//     if (m_bVisible)
//     {
//         DoPaint();  // 保持显示
//         m_bVisible = true;
//     }
// }

}