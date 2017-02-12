#include "stdafx.h"
#include "trackrect.h"
#include "UISDK\Inc\Interface\iobject.h"
#include "UISDK\Inc\Util\util.h"

HCURSOR  RectTracker::s_hCursor[9] = {0};
long     RectTracker::s_lRef = 0;

RectTracker::RectTracker()
{
	m_bStartup = false;
    m_rect.SetRectEmpty();
    m_nHandleSize = 4;
    m_nMask = 0;
    m_hWnd = NULL;
    m_eActionHit = hitNothing;
    m_ptLButtonDown.x = m_ptLButtonDown.y = 0;
    m_bMouseDraging = false;
    m_bKeyboardDraging = false;
    m_nKeyboardHitCount = 0;
    m_nKeyPushdown = 0;
    m_prsp = NULL;

    if (0 == s_lRef)
    {
        Init();
    }
    s_lRef++;
}

RectTracker::~RectTracker()
{
    s_lRef--;
    if (0 == s_lRef)
    {
        Release();
    }
}

void  RectTracker::Init()
{
    // initialize the cursor array
    s_hCursor[0] = ::LoadCursor(NULL, IDC_SIZENWSE);
    s_hCursor[1] = ::LoadCursor(NULL, IDC_SIZENESW);
    s_hCursor[2] = ::LoadCursor(NULL, IDC_SIZENWSE);
    s_hCursor[3] = ::LoadCursor(NULL, IDC_SIZENESW);
    s_hCursor[4] = ::LoadCursor(NULL, IDC_SIZENS);
    s_hCursor[5] = ::LoadCursor(NULL, IDC_SIZEWE);
    s_hCursor[6] = ::LoadCursor(NULL, IDC_SIZENS);
    s_hCursor[7] = ::LoadCursor(NULL, IDC_SIZEWE);
    s_hCursor[8] = ::LoadCursor(NULL, IDC_SIZEALL);
}

void  RectTracker::Release()
{
    for (int i = 0; i < hitCount; i++)
        s_hCursor[i] = NULL;
}

void  RectTracker::Update(RECT*  prc, UINT nHandleMask)
{
    if (prc)
    {
		m_bStartup = true;
        m_rect.CopyRect(prc);
        m_nMask = nHandleMask;
    }
    else
    {
		m_bStartup = false;
        m_rect.SetRectEmpty();
        m_nMask = 0;
    }
}

void  RectTracker::Draw(HDC  hDC)
{
    // 绘制当前选中的对象区域
    if (!m_bStartup)
		return;

	CRect  rcDraw = m_rect;
	::InflateRect(&rcDraw, 2, 2);

	HBRUSH hBrush = (HBRUSH)GetStockObject(NULL_BRUSH);
	HPEN hPen = CreatePen(1, PS_SOLID, RGB(255,0,0));
	HBRUSH hOldBrush = (HBRUSH)SelectObject(hDC, hBrush);
	HPEN hOldPen = (HPEN)SelectObject(hDC, hPen);

	::Rectangle(hDC, rcDraw.left, rcDraw.top, rcDraw.right, rcDraw.bottom);

	::SelectObject(hDC, hOldPen);
	::SelectObject(hDC, hOldBrush);
	SAFE_DELETE_GDIOBJECT(hPen);
	SAFE_DELETE_GDIOBJECT(hBrush);

	// 绘制9个方块
	if (m_nMask & recttrackermask_size)
	{
		RECT  rcHandle;
		HBRUSH  hBrush = (HBRUSH)CreateSolidBrush(RGB(255, 0, 0));
		for (int i = 0; i < 8; i++)
		{
			GetHandleRect((RectTrackerHit)i, &rcHandle);
			::FillRect(hDC, &rcHandle, hBrush);
		}
		SAFE_DELETE_GDIOBJECT(hBrush);
	}
}

void   RectTracker::GetHandleRect(RectTrackerHit nIndex, RECT* prc)
{
	CRect  rcBorder = m_rect;

	switch (nIndex)
	{
	case hitTopLeft:
		{
			prc->left = rcBorder.left;
			prc->top = rcBorder.top;
		}
		break;

	case hitTop:
		{
			prc->left = rcBorder.left + (rcBorder.Width())/2;
			prc->top = rcBorder.top;
		}
		break;

	case hitTopRight:
		{
			prc->left = rcBorder.right;
			prc->top = rcBorder.top;
		}
		break;

	case hitLeft:
		{
			prc->left = rcBorder.left;
			prc->top = rcBorder.top + rcBorder.Height()/2;
		}
		break;

	case hitRight:
		{
			prc->left = rcBorder.right;
			prc->top = rcBorder.top + rcBorder.Height()/2;
		}
		break;

	case hitMiddle:
		{
			prc->left = rcBorder.left + (rcBorder.Width())/2;
			prc->top = rcBorder.top + rcBorder.Height()/2;
		}
		break;

	case hitBottomLeft:
		{
			prc->left = rcBorder.left;
			prc->top = rcBorder.bottom;
		}
		break;

	case hitBottom:
		{
			prc->left = rcBorder.left + (rcBorder.Width())/2;
			prc->top = rcBorder.bottom;
		}
		break;

	case hitBottomRight:
		{
			prc->left = rcBorder.right;
			prc->top = rcBorder.bottom;
		}
		break;

	default:
		return;
	}

	prc->left -= m_nHandleSize;
	prc->top -= m_nHandleSize;
	prc->right = prc->left + 2*m_nHandleSize;
	prc->bottom = prc->top + 2*m_nHandleSize;
}

bool  RectTracker::IsContainer()
{
    return m_nMask & recttrackermask_iscontainer ? true:false;
}
bool  RectTracker::CanSize()
{
    return m_nMask & recttrackermask_size ? true:false;
}
bool  RectTracker::CanMove()
{
    return m_nMask & recttrackermask_move ? true:false;
}

RectTrackerHit  RectTracker::HitTest(POINT  pt)
{
    CRect  rcBoundOut = m_rect;
    CRect  rcBoundIn = m_rect;
    rcBoundOut.InflateRect(m_nHandleSize, m_nHandleSize);
    rcBoundIn.DeflateRect(m_nHandleSize, m_nHandleSize);
   
    if (!PtInRect(&rcBoundOut, pt))
        return hitNothing;

    if (IsContainer())
    {
        if (PtInRect(&rcBoundIn, pt))   // 内侧区域用于选择子对象，不用于拖拽
            return hitNothing;
    }

    RectTrackerHit  nHitTest = hitNothing;
    if (pt.x < rcBoundIn.left)
    {
        if (pt.y < rcBoundIn.top)
            nHitTest = hitTopLeft;
        else if (pt.y > rcBoundIn.bottom)
            nHitTest = hitBottomLeft;
        else 
            nHitTest = hitLeft;
    }
    else if (pt.x >= rcBoundIn.right)
    {
        if (pt.y < rcBoundIn.top)
            nHitTest = hitTopRight;
        else if (pt.y > rcBoundIn.bottom)
            nHitTest = hitBottomRight;
        else
            nHitTest = hitRight;
    }
    else if (pt.y < rcBoundIn.top)
    {
        nHitTest = hitTop;
    }
    else if (pt.y >= rcBoundIn.bottom)
    {
        nHitTest = hitBottom;
    }
    else
    {
        nHitTest = hitMiddle;
    }


    if (CanSize() && CanMove())
    {
        // 窗口的move只能改到边框上来
        if (IsContainer())
        {
            RECT rc = {0};
            RectTrackerHit  hitArray[4] = {hitTop, hitLeft, hitRight, hitBottom};
            for (int i = 0; i < 4; i++)
            {
                if (nHitTest == hitArray[i])
                {
                    GetHandleRect(hitArray[i], &rc);
                    if (PtInRect(&rc, pt))
                        break;

                    nHitTest = hitMiddle;
                    break;
                }
            }
        }
    }
    else if (CanMove())  // 不能size
    {
        nHitTest = hitMiddle;
    }
    else if (CanSize())  // 不能move
    {
        if (nHitTest == hitMiddle)
            nHitTest = hitNothing;
    }
    else
    {
        nHitTest = hitNothing;
    }

    return nHitTest;
}

RectTrackerHit  RectTracker::HitTestUnderCursor()
{
    POINT  pt;
    GetCursorPos(&pt);
    ::MapWindowPoints(NULL, m_hWnd, &pt, 1);

    if (m_prsp)
        m_prsp->DP2LP(&pt);

    RectTrackerHit hit = HitTest(pt);
    return hit;
}

BOOL  RectTracker::ProcessMessage(UINT nMsg, WPARAM wParam, LPARAM lParam, long* plRet)
{
    if (!m_bStartup)
        return FALSE;

    switch (nMsg)
    {
    case WM_SETCURSOR:
        {
            RectTrackerHit hit = HitTestUnderCursor();
            if (hit == hitNothing)
                return FALSE;

            SetCursor(s_hCursor[hit]);
            return TRUE;
        }
        break;

    case WM_LBUTTONDOWN:
        {
            POINT pt = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
            if (m_prsp)
                m_prsp->DP2LP(&pt);

            RectTrackerHit hit = HitTest(pt);
            if (hit == hitNothing)
                return FALSE;

            SetCapture(m_hWnd);
            m_eActionHit = hit;
            m_bMouseDraging = true;
            m_ptLButtonDown = pt;

            if (m_prsp)
                m_prsp->OnRectTrackerDragStart();

            return TRUE;
        }
        break;

    case WM_LBUTTONUP:
    case WM_CANCELMODE:
        {
            if (m_bMouseDraging)
            {
                ReleaseCapture();
                m_eActionHit = hitNothing;
                m_bMouseDraging = false;

                if (m_prsp)
                    m_prsp->OnRectTrackerDragEnd();

                return TRUE;
            }
        }
        break;

    case WM_MOUSEMOVE:
        {
            if (m_bMouseDraging)
            {
                POINT pt = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
                if (m_prsp)
                {
                    m_prsp->DP2LP(&pt);
                    m_prsp->OnRectTrackerDrag(m_eActionHit, pt.x-m_ptLButtonDown.x, pt.y-m_ptLButtonDown.y);
                }
                return TRUE;
            }
        }
        break;

    case WM_KEYDOWN:
        {
            if (!m_prsp)
                break;

            if (wParam == VK_LEFT || wParam == VK_UP || wParam == VK_RIGHT || wParam == VK_DOWN)
            {
				if (Util::IsKeyDown(VK_CONTROL))
					return FALSE;

                m_nKeyboardHitCount++;
                if (!m_bKeyboardDraging)
                {
                    m_prsp->OnRectTrackerDragStart();
                    m_bKeyboardDraging = true;
                    m_nKeyPushdown = wParam;

                    // 如果按下了shitf键，表明是改变大小
                    bool bShitdown = Util::IsKeyDown(VK_SHIFT);
                    if (!bShitdown)
                    {
                        m_eActionHit = hitMiddle;
                    }
                    else
                    {
                        if (wParam == VK_LEFT)
                            m_eActionHit = hitRight;
                        else if (wParam == VK_UP)
                            m_eActionHit = hitBottom;
                        else if (wParam == VK_RIGHT)
                            m_eActionHit = hitRight;
                        else if (wParam == VK_DOWN)
                            m_eActionHit = hitBottom;
                    }
                }
                else
                {
                    if (wParam != m_nKeyPushdown)
                        break;
                }

                if (wParam == VK_LEFT)
                {
                    m_prsp->OnRectTrackerDrag(m_eActionHit, -m_nKeyboardHitCount, 0);
                }
                else if (wParam == VK_UP)
                {
                    m_prsp->OnRectTrackerDrag(m_eActionHit, 0, -m_nKeyboardHitCount);
                }
                else if (wParam == VK_RIGHT)
                {
                    m_prsp->OnRectTrackerDrag(m_eActionHit, m_nKeyboardHitCount, 0);
                }
                else if (wParam == VK_DOWN)
                {
                    m_prsp->OnRectTrackerDrag(m_eActionHit, 0, m_nKeyboardHitCount);
                }

				return TRUE;
            }
        }
        break;
    case WM_KEYUP:
        {
            if (m_bKeyboardDraging)
            {
                if (wParam == VK_LEFT || wParam == VK_UP || wParam == VK_RIGHT || wParam == VK_DOWN)
                {
                    if (wParam != m_nKeyPushdown)
                        break;

                    if (m_prsp)
                    {
                        m_prsp->OnRectTrackerDragEnd();
                    }
                    m_bKeyboardDraging = false;
                    m_nKeyboardHitCount = 0;
                    m_eActionHit = hitNothing;
                    m_nKeyPushdown = 0;

					return TRUE;
                }
            }
        }
        break;
    }
    return FALSE;
}

