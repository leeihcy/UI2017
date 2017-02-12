#include "StdAfx.h"
#include "caret.h"
#include "controldraw\controldrawcaret.h"

ICaret* CCaret::m_pCaret = NULL;
POINT CCaret::m_ptLast = {0, 0};
namespace UI
{

CCaret::CCaret()
{ 
    
}
CCaret::~CCaret()
{
    SAFE_DELETE(m_pCaret);
}

// pCallback
// 0，默认，表示使用的是窗口坐标
// 1，表示使用的是控件客户区坐标
// 2，表示使用的是控件坐标
// 其它，表示使用的是回调函数
BOOL CCaret::CreateCaret(IObject* pObj, HBITMAP hbmp, int nWidth, int nHeight, 
                         CARET_TYPE eType, ICaretUpdateCallback* pCallback)
{
    if (NULL == pObj)
        return FALSE;

    HWND hWnd = pObj->GetHWND();
    if (CARET_TYPE_UNKNOWN == eType)
    {
    //    if (Util::IsLayeredWindow(hWnd))
            eType = CARET_TYPE_CONTROLDRAW;
    }

    if (m_pCaret && (m_pCaret->GetType() != eType || m_pCaret->GetObject() != pObj))
    {
        SAFE_DELETE(m_pCaret);
    }

    if (!m_pCaret)
    {
        switch (eType)
        {
//          case CARET_TYPE_API:
//              m_pCaret = new ApiCaret;
//              break;
//
//			case CARET_TYPE_OWNERDRAW:
//			    m_pCaret = new OwnerDrawCaret;
//			    break;
// 
//          case CARET_TYPE_WINDOW:
//              m_pCaret = new CaretWindow;
//              break;

        case CARET_TYPE_CONTROLDRAW:
            m_pCaret = new ControlDrawCaret(pCallback);
            break;

        default:
            break;
        }
    }

    if (m_pCaret)
    {
        return m_pCaret->Create(pObj, hWnd, hbmp, nWidth, nHeight);
    }

    return TRUE;
}
void CCaret::DestroyCaret(IObject* pObj, bool bRedraw)
{
    if (m_pCaret && m_pCaret->GetObject() == pObj)
    {   
        m_pCaret->Destroy(bRedraw);
        SAFE_DELETE(m_pCaret);
    }
}

// 2013.6.17 将SetCaretPos的参数修改为相对于控件，而不是窗口
// 这样才好解决Edit滚动的问题
void CCaret::SetCaretPos(IObject* pObj, int x, int y, bool bRedraw)
{
    if (m_pCaret && m_pCaret->GetObject() == pObj)
    {
        m_pCaret->SetPos(x, y, bRedraw);

        m_ptLast.x = x;
        m_ptLast.y = y;
    }
}
void CCaret::ShowCaret(IObject* pObj, bool bRedraw)
{
    if (m_pCaret && m_pCaret->GetObject() == pObj)
    {
        m_pCaret->Show(bRedraw);
    }
}
void CCaret::HideCaret(IObject* pObj, bool bRedraw)
{
    if (m_pCaret && m_pCaret->GetObject() == pObj)
    {
        m_pCaret->Hide(bRedraw);
    }
}
void  CCaret::OnControlPaint(IObject* pObj, IRenderTarget* p)
{
    if (m_pCaret && m_pCaret->GetObject() == pObj)
    {
        m_pCaret->OnControlPaint(p);
    }
}

CARET_TYPE  CCaret::GetCaretType()
{
    if (m_pCaret)
        return m_pCaret->GetType();
    
    return CARET_TYPE_UNKNOWN;
}

}