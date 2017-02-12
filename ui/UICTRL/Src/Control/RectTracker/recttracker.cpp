#include "stdafx.h"
#include "recttracker.h"
#include <gdiplus.h>
#include "recttracker_desc.h"

using namespace UI;

RectTracker::RectTracker(IRectTracker* p) : MessageProxy(p)
{
    m_pIRectTracker = p;
	p->SetDescription(RectTrackerDescription::Get());

    m_bDrawBroder = true;
    m_realRect.SetRectEmpty();
    m_nHandleSize = 5;
    m_nLineSpace = 0;
    m_nMask = 0;
    m_ptLButtonDown.x = 0;
    m_ptLButtonDown.y = 0;
    m_bMouseDraging = false;
    m_bKeyboardDraging = false;
    m_nKeyboardHitCount = 0;
    m_nKeyPushdown = 0;
    m_prsp = NULL;
	m_colorBorder.m_col = RGBA(255, 0, 0, 255);

	m_rectObjectSave.SetRectEmpty();
	m_bLimitBorderInParent = true;

	m_bStartup = true;
	m_nMask = recttrackermask_sizemove;

    Init();
}

RectTracker::~RectTracker()
{
    Release();
}

void  RectTracker::Init()
{
    // initialize the cursor array
    m_hCursor[0] = ::LoadCursor(NULL, IDC_SIZENWSE);
    m_hCursor[1] = ::LoadCursor(NULL, IDC_SIZENESW);
    m_hCursor[2] = ::LoadCursor(NULL, IDC_SIZENWSE);
    m_hCursor[3] = ::LoadCursor(NULL, IDC_SIZENESW);
    m_hCursor[4] = ::LoadCursor(NULL, IDC_SIZENS);
    m_hCursor[5] = ::LoadCursor(NULL, IDC_SIZEWE);
    m_hCursor[6] = ::LoadCursor(NULL, IDC_SIZENS);
    m_hCursor[7] = ::LoadCursor(NULL, IDC_SIZEWE);
    m_hCursor[8] = ::LoadCursor(NULL, IDC_SIZEALL);
    m_hCursor[9] = ::LoadCursor(NULL, IDC_SIZEALL);
}

void  RectTracker::Release()
{
    for (int i = 0; i < hitCount; i++)
        m_hCursor[i] = NULL;

	SetRsp(nullptr);
}

void  RectTracker::OnInitialize()
{
    // 根据当前控件位置，初始化一个m_realRect
    UpdateRealRect();
}

void   RectTracker::UpdateRealRect()
{
	m_pIRectTracker->GetWindowRect(&m_realRect);
	m_realRect.DeflateRect(
		m_nHandleSize+m_nLineSpace,
		m_nHandleSize+m_nLineSpace);

	rect_changed.emit(m_pIRectTracker, &m_realRect);
}

void  RectTracker::OnSerialize(SERIALIZEDATA* pData)
{
	DO_PARENT_PROCESS(IRectTracker, IControl);

	AttributeSerializerWrap as(pData, L"RectTracker");
	as.AddEnum(XML_RECTTRACKER_MASK, *(long*)&m_nMask)
		->AddOption(recttrackermask_none, XML_RECTTRACKER_MASK_NONE)
		->AddOption(recttrackermask_move, XML_RECTTRACKER_MASK_MOVE)
		->AddOption(recttrackermask_size, XML_RECTTRACKER_MASK_SIZE)
		->AddOption(recttrackermask_sizemove, XML_RECTTRACKER_MASK_SIZEMOVE)
		->SetDefault(recttrackermask_sizemove);

	as.AddColor(XML_BORDER XML_COLOR, m_colorBorder);
	as.AddLong(XML_RECTTRACKER_HANDLE_SIZE, *(long*)&m_nHandleSize)
		->SetDefault(5)
		->SetDpiScaleType(LONGATTR_DPI_SCALE_ENABLE);
	as.AddLong(XML_RECTTRACKER_LINE_SPACE, *(long*)&m_nLineSpace)
		->SetDpiScaleType(LONGATTR_DPI_SCALE_ENABLE);
	as.AddBool(XML_RECTTRACKER_DRAW_BORDER, m_bDrawBroder)
		->SetDefault(true);
	as.AddBool(XML_RECTTRACKER_LIMIT_BORDER_IN_PARENT, m_bLimitBorderInParent)
		->SetDefault(true);
}

void  RectTracker::SetRealRectInParent(RECT* prcRealObj) 
{
    if (prcRealObj)
    {
		CRect rcObj(prcRealObj);
		int nDeflat = m_nHandleSize + m_nLineSpace;
		::InflateRect(&rcObj, nDeflat, nDeflat);
		m_pIRectTracker->SetObjectPos(&rcObj, 0);
    }
    else
    {
        m_bStartup = false;
        m_realRect.SetRectEmpty();
    }
}

void  RectTracker::SetHandleMask(UINT nMask)
{
	if (m_nMask == nMask)
		return;

    m_nMask = nMask;
}

void  RectTracker::OnPaint(IRenderTarget* pRT)
{
	HDC hDC = pRT->GetHDC();

    if (m_prsp)
        m_prsp->Draw(hDC);

    if (m_bDrawBroder)
        DrawGdiplus(hDC);
}

void  RectTracker::DrawGdiplus(HDC  hDC)
{
    // 绘制当前选中的对象区域
    if (!m_bStartup)
		return;

    Gdiplus::Graphics g(hDC);
    g.SetSmoothingMode(Gdiplus::SmoothingModeDefault);
    g.SetPixelOffsetMode(Gdiplus::PixelOffsetModeDefault);  

	CRect  rcDraw;
    GetBolderRectInControl(&rcDraw);

    Gdiplus::Color color;
    color.SetFromCOLORREF(m_colorBorder.m_col);

    Gdiplus::Pen pen(Gdiplus::Color(
		m_colorBorder.a, m_colorBorder.r,m_colorBorder.g, m_colorBorder.b));
    g.DrawRectangle(&pen, rcDraw.left, rcDraw.top, rcDraw.Width()-1, rcDraw.Height()-1);

#if 0 // 边框测试
    Gdiplus::Pen pen2(Gdiplus::Color::Green);
    g.DrawRectangle(&pen2, 0, 0, m_pIRectTracker->GetWidth()-1, 
        m_pIRectTracker->GetHeight()-1);
#endif

    g.SetPixelOffsetMode(Gdiplus::PixelOffsetModeHighQuality);

	// 绘制9个方块
	if (m_nMask & recttrackermask_size)
	{
        RECT  rcHandle = {0};

        Gdiplus::SolidBrush  brush(color);
		for (int i = 0; i < 8; i++)
		{
			GetHandleRect((RectTrackerHit)i, &rcHandle, true);
            g.FillRectangle(&brush, 
                    rcHandle.left,
                    rcHandle.top,
                    rcHandle.right-rcHandle.left, 
                    rcHandle.bottom-rcHandle.top);
		}
	}
}

void  RectTracker::DrawGdi(HDC hDC)
{
    // 绘制当前选中的对象区域
    if (!m_bStartup)
        return;

    CRect  rcDraw;
    GetBolderRectInControl(&rcDraw);

    HBRUSH hBrush = (HBRUSH)GetStockObject(NULL_BRUSH);
    HPEN hPen = CreatePen(1, PS_SOLID, m_colorBorder.m_col);
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
        HBRUSH  hBrush = (HBRUSH)CreateSolidBrush(m_colorBorder.m_col);
        for (int i = 0; i < 8; i++)
        {
            GetHandleRect((RectTrackerHit)i, &rcHandle, true);
            ::FillRect(hDC, &rcHandle, hBrush);
        }
        SAFE_DELETE_GDIOBJECT(hBrush);
    }
}

// bControlRect，获取窗口坐标，还是控件坐标
void   RectTracker::GetHandleRect(RectTrackerHit nIndex, RECT* prc, bool bControlRect)
{
	CRect  rcBorder;
    if (bControlRect)
        GetBolderRectInControl(&rcBorder);
    else
        GetBolderRectInWindow(&rcBorder);

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

void  RectTracker::GetBolderRectInControl(RECT* prc)
{
    int w = m_realRect.Width() + 2*m_nLineSpace;
    int h = m_realRect.Height() + 2*m_nLineSpace;
    int x = m_nHandleSize;
    int y = m_nHandleSize;

    ::SetRect(prc, x, y, x+w, y+h);
}
void  RectTracker::GetBolderRectInWindow(RECT* prc)
{
    ::CopyRect(prc, m_realRect);
    ::InflateRect(prc, m_nLineSpace, m_nLineSpace); 
}

void  RectTracker::CalcWindowRectFromRealRect(RECT* prcReal, RECT* prcCtrl)
{
    ::CopyRect(prcCtrl, prcReal);

    int nInflat = m_nHandleSize + m_nLineSpace;
    InflateRect(prcCtrl, nInflat, nInflat);
}

// void  RectTracker::CalcParentRectFromRealRect(RECT* prcReal, RECT* prcCtrl)
// {
// 	CalcWindowRectFromRealRect(prcReal, prcCtrl);
// 
// 	// 从窗口坐标转换为
// }

void  RectTracker::GetRealRectInControl(RECT* prc)
{
    ::SetRect(prc, 0, 0, 
		m_pIRectTracker->GetWidth(), 
		m_pIRectTracker->GetHeight());
    int nDeflat = m_nHandleSize + m_nLineSpace;

    ::InflateRect(prc, -nDeflat, -nDeflat);
}

void  RectTracker::GetRealRectInParent(RECT* prc)
{
	m_pIRectTracker->GetParentRect(prc);
	int nDeflat = m_nHandleSize + m_nLineSpace;

	::InflateRect(prc, -nDeflat, -nDeflat);
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

RectTrackerHit  RectTracker::HitTest(POINT pt)
{
    CRect  rcBoundOut = m_realRect;
    CRect  rcBoundIn = m_realRect;
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
                    GetHandleRect(hitArray[i], &rc, false);
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
    HWND hWnd = m_pIRectTracker->GetHWND();
    ::MapWindowPoints(NULL, hWnd, &pt, 1);

    if (m_prsp)
        m_prsp->DP2LP(&pt);

    RectTrackerHit hit = HitTest(pt);
    return hit;
}

BOOL  RectTracker::OnSetCursor(HWND wnd, UINT nHitTest, UINT message)
{
    RectTrackerHit hit = HitTestUnderCursor();
    if (hit == hitNothing)
        return FALSE;

    ::SetCursor(m_hCursor[hit]);
    return TRUE;
}
LRESULT  RectTracker::OnLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    POINT pt = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
    if (m_prsp)
        m_prsp->DP2LP(&pt);

    RectTrackerHit hit = HitTest(pt);
    if (hit == hitNothing)
    {
        SetMsgHandled(FALSE);
        return 0;
        //return FALSE;
    }     

    //SetCapture(m_hWnd);
    m_eActionHit = hit;
    m_bMouseDraging = true;
    m_ptLButtonDown = pt;

	OnRectTrackerDragStart();
	
    if (hit == hitMiddle)
    {
        ::SetCursor(m_hCursor[hitMiddle+1]);
    }

    return 0;
    //return TRUE;
}

// WM_CANCELMODE
LRESULT RectTracker::OnLButtonUp(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if (!m_bMouseDraging)
    {
        SetMsgHandled(FALSE);
        return 0;
    }

    // ReleaseCapture();
    m_eActionHit = hitNothing;
    m_bMouseDraging = false;

    OnRectTrackerDragEnd();

	POINT pt = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
	if (m_ptLButtonDown.x == pt.x &&
		m_ptLButtonDown.y == pt.y)
	{
		this->click.emit(m_pIRectTracker);
	}
    return 0;
    //return TRUE;
}   

LRESULT RectTracker::OnCancelMode(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    OnLButtonUp(0, 0, 0);
    return 0;
}


LRESULT  RectTracker::OnMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if (!m_bMouseDraging)
    {
        SetMsgHandled(FALSE);
        return 0;
    }

    POINT pt = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
    if (m_prsp)
        m_prsp->DP2LP(&pt);
	
    OnRectTrackerDrag(m_eActionHit,
		pt.x-m_ptLButtonDown.x, 
		pt.y-m_ptLButtonDown.y);

    return 0;
    //return TRUE;
}

LRESULT  RectTracker::OnKeyDown(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if (wParam == VK_LEFT || wParam == VK_UP || wParam == VK_RIGHT || wParam == VK_DOWN)
    {
        m_nKeyboardHitCount++;
        if (!m_bKeyboardDraging)
        {
            OnRectTrackerDragStart();
            m_bKeyboardDraging = true;
            m_nKeyPushdown = wParam;

            // 如果按下了shitf键，表明是改变大小
            bool bShitdown = IsKeyDown(VK_SHIFT);
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
            {
                return 0;
            }
        }

        if (wParam == VK_LEFT)
        {
            OnRectTrackerDrag(m_eActionHit, -m_nKeyboardHitCount, 0);
        }
        else if (wParam == VK_UP)
        {
            OnRectTrackerDrag(m_eActionHit, 0, -m_nKeyboardHitCount);
        }
        else if (wParam == VK_RIGHT)
        {
            OnRectTrackerDrag(m_eActionHit, m_nKeyboardHitCount, 0);
        }
        else if (wParam == VK_DOWN)
        {
            OnRectTrackerDrag(m_eActionHit, 0, m_nKeyboardHitCount);
        }
    }
    else
    {
        SetMsgHandled(FALSE);
    }

    return 0;
}

LRESULT  RectTracker::OnKeyUp(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if (!m_bKeyboardDraging)
    {
        SetMsgHandled(FALSE);
        return 0;
    }
    if (wParam == VK_LEFT || wParam == VK_UP || wParam == VK_RIGHT || wParam == VK_DOWN)
    {
        if (wParam != m_nKeyPushdown)
            return 0;

		OnRectTrackerDragEnd();
        
        m_bKeyboardDraging = false;
        m_nKeyboardHitCount = 0;
        m_eActionHit = hitNothing;
        m_nKeyPushdown = 0;
    }

    return 0;
}


LRESULT  RectTracker::OnMouseWheel(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    //int  nDelta = ((int)HIWORD(wParam))/WHEEL_DELTA;

    UIMSG  msg;
    msg.message = UI_MSG_NOTIFY;
    msg.nCode = RectTracker_NM_MOUSEWHEEL;
    msg.pMsgFrom = m_pIRectTracker;
    msg.wParam = wParam;
    msg.lParam = lParam;

    return m_pIRectTracker->DoNotify(&msg);
}


void  RectTracker::SetCursor(int nIndex, HCURSOR hCursor)
{
    int nSize = sizeof(m_hCursor)/sizeof(HCURSOR);
    if (nIndex >= nSize)
        return;

    m_hCursor[nIndex] = hCursor;
}

UI::IRectTracker* UI::RectTracker::GetIRectTracker()
{
	return m_pIRectTracker;
}

void UI::RectTracker::SetRsp(IRectTrackerRsp* prsp)
{
	if (m_prsp)
	{
		m_prsp->Release();
		m_prsp = nullptr;
	}
	m_prsp = prsp;
}

UINT UI::RectTracker::GetHandleMask()
{
	return m_nMask;
}

void UI::RectTracker::GetRealRectInWindow(RECT* prc)
{
	::CopyRect(prc, &m_realRect);
}

LRESULT UI::RectTracker::OnPosChanged(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	UpdateRealRect();
	return 0;
}

void UI::RectTracker::OnRectTrackerDragStart()
{
	m_pIRectTracker->GetRealRectInParent(&m_rectObjectSave);

	if (m_prsp)
		m_prsp->OnRectTrackerDragStart();
}

void UI::RectTracker::OnRectTrackerDragEnd()
{
	m_rectObjectSave.SetRectEmpty();

	if (m_prsp)
		m_prsp->OnRectTrackerDragEnd();
}

void UI::RectTracker::OnRectTrackerDrag(
		RectTrackerHit eHit, int xDelta, int yDelta)
{
	IObject*  pParent = m_pIRectTracker->GetParentObject();
	if (!pParent)
		return;

	CRect rcTry = m_rectObjectSave;
	switch (eHit)
	{
	case hitTopLeft:
		rcTry.left += xDelta;
		rcTry.top += yDelta;
		break;

	case hitTop:
		rcTry.top += yDelta;
		break;

	case hitTopRight:
		rcTry.right += xDelta;
		rcTry.top += yDelta;
		break;

	case hitLeft:
		rcTry.left += xDelta;
		break;

	case hitRight:
		rcTry.right += xDelta;
		break;

	case hitBottomLeft:
		rcTry.left += xDelta;
		rcTry.bottom += yDelta;
		break;

	case hitBottom:
		rcTry.bottom += yDelta;
		break;

	case hitBottomRight:
		rcTry.right += xDelta;
		rcTry.bottom += yDelta;
		break;

	case hitMiddle:
		rcTry.OffsetRect(xDelta, yDelta);
		break;

	default:
		return;
	}
	rcTry.NormalizeRect();

	// 将位置限制到父对象范围之内 

	CRect  rcParent;
	pParent->GetObjectClientRect(&rcParent);

	// 边框也不能出父对象。现在有个bug，m_configRight出parent时，可能为负数，和ndef/auto
	// 冲突。
	if (m_bLimitBorderInParent)
	{
		int space = GetHandleSize();
		space += GetLineSpace();
		rcParent.DeflateRect(space, space);
	}

	if (eHit == hitMiddle)
	{
		int xOffset = 0;
		int yOffset = 0;

		if (rcTry.right > rcParent.right)
			xOffset -= rcTry.right-rcParent.right;
		else if (rcTry.left < rcParent.left)
			xOffset += rcParent.left - rcTry.left;

		if (rcTry.bottom > rcParent.bottom)
			yOffset -= rcTry.bottom-rcParent.bottom;
		else if (rcTry.top < rcParent.top)
			yOffset += rcParent.top - rcTry.top;

		rcTry.OffsetRect(xOffset, yOffset);
	}
	else
	{
		if (rcTry.right > rcParent.right)
			rcTry.right = rcParent.right;
		else if (rcTry.left < rcParent.left)
			rcTry.left = rcParent.left;

		if (rcTry.bottom > rcParent.bottom)
			rcTry.bottom = rcParent.bottom;
		else if (rcTry.top< rcParent.top)
			rcTry.top = rcParent.top;
	}

	//m_pIControl->SetObjectPos(&rcTry, 0);
	m_pIRectTracker->SetRealRectInParent(&rcTry);

	// TBD:
	if (m_prsp) 
	{
		m_prsp->OnRectTrackerDrag(m_eActionHit, xDelta, yDelta);
	}
}

