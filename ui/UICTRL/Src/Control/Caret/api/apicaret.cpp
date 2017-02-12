#include "stdafx.h"
#include "apicaret.h"
#include "UISDK\Kernel\Inc\Interface\iobject.h"

namespace UI
{
ApiCaret::ApiCaret()
{
    m_hWnd = NULL;
    m_pObject = NULL;
    m_ptLast.x = m_ptLast.y = -1;
    m_sizeCaret.cx = m_sizeCaret.cy = 0;
}
ApiCaret::~ApiCaret()
{

}

BOOL  ApiCaret::Create(IObject* pObj, HWND hWndParent, HBITMAP hbmp, int nWidth, int nHeight)
{
    m_pObject = pObj;
    m_hWnd = hWndParent;

    m_sizeCaret.cx = nWidth;
    m_sizeCaret.cy = nHeight;
    return ::CreateCaret(hWndParent, hbmp, nWidth, nHeight);
}
void  ApiCaret::Destroy(bool bRedraw)
{
    ::DestroyCaret();
    m_hWnd = NULL;
    m_pObject = NULL;
    m_ptLast.x = m_ptLast.y = -1;
}
void  ApiCaret::Show(bool bRedraw)
{
    ::ShowCaret(m_hWnd);
}
void  ApiCaret::Hide(bool bRedraw)
{
    ::HideCaret(m_hWnd);
}
void  ApiCaret::SetPos(int x, int y, bool bRedraw)
{
    m_ptLast.x = x;
    m_ptLast.y = y;

    if (m_pObject)
    {
        // 偏移
        POINT ptCtrl = m_pObject->GetRealPosInWindow(); 

        x += ptCtrl.x;
        y += ptCtrl.y;

        // 裁剪 -- 太复杂了，由ownderdrawcaret实现吧.
//         CRect  rcVisible;
//         m_pObject->GetObjectVisibleRect(&rcVisible, true);
// 
//         CRect  rcCaret = {x, y, x+m_sizeCaret.cx, y+m_sizeCaret.cy};
//         // 完全隐藏的情况
//         if (rcCaret.left > rcVisible.right ||
//             rcCaret.right < rcVisible.left ||
//             rcCaret.bottom < rcVisible.top ||
//             rcCaret.top > rcVisible.bottom)
//         {
//         }
    }

    ::SetCaretPos(x, y);
}


// 无法判断当前光标是显示的还是隐藏的，导致PostCommitWindowBuffer是又强制将光标显示出来了
// 也许是得重写一个自己的caret才好解决该问题
// void  ApiCaret::PreCommitWindowBuffer(HDC hMemDC)
// {
//     Hide();  
// }
// void  ApiCaret::PostCommitWindowBuffer(HDC hMemDC)
// {
//     SetPos(m_ptLast.x, m_ptLast.y);
//     Show();
// }
}