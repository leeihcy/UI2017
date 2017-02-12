#include "stdafx.h"
#include "controldrawcaret.h"
#include "..\UISDK\Inc\Util\struct.h"
#include "..\UISDK\Inc\Util\iimage.h"

using namespace UI;

void  CreateCaretRenderBitmap(HBITMAP hbmp, UI::IRenderBitmap** pp);


ControlDrawCaret::ControlDrawCaret(ICaretUpdateCallback* pCallback)
{
    m_hWnd = NULL;
    m_pObject = NULL;
    m_ptLast.x = m_ptLast.y = -1;
    m_sizeCaret.cx = m_sizeCaret.cy = 0;
    m_sizeItalicFontCaretOffset.cx = m_sizeItalicFontCaretOffset.cy = 0;
    m_rcDraw.SetRectEmpty();
    m_bVisible = false;
    m_bOn = false;
    m_nTimerId = 0;
    m_hBitmap = NULL;
    m_pRenderBitmap = NULL;

    m_eCoordType = COORD_WINDOW;
    m_pCallback = NULL;
    if (pCallback == (ICaretUpdateCallback*)COORD_CONTROL)
    {
        m_eCoordType = COORD_CONTROL;
    }
    else if (pCallback == (ICaretUpdateCallback*)COORD_CONTROLCLIENT)
    {
        m_eCoordType = COORD_CONTROLCLIENT;
    }
    else
    {
        m_pCallback = pCallback;
    }
}
ControlDrawCaret::~ControlDrawCaret()
{
    DestroyTimer();
    SAFE_RELEASE(m_pRenderBitmap);
}

BOOL  ControlDrawCaret::Create(IObject* pObj, HWND hWndParent, HBITMAP hbmp, int nWidth, int nHeight)
{
    Destroy(false);

    m_hWnd = hWndParent;
    m_pObject = pObj;
    m_sizeCaret.cx = nWidth;
    m_sizeCaret.cy = nHeight;
    m_hBitmap = hbmp;
    m_sizeItalicFontCaretOffset.cx = 0;
    m_sizeItalicFontCaretOffset.cy = 0;

    if (m_hBitmap > (HBITMAP)1)
    {
        BITMAP bm = {0};
        ::GetObject(m_hBitmap, sizeof(bm), &bm);
        m_sizeCaret.cx = bm.bmWidth;
        m_sizeCaret.cy = bm.bmHeight;

        CreateCaretRenderBitmap(hbmp, &m_pRenderBitmap);

        // 纠正richedit斜体字光标偏移问题
        if (bm.bmHeight > 1 &&  // 为1可能是有选区的情况
            m_pCallback && 
            m_pCallback->NeedFixItalicCaretOffset())
        {
            // 字体大小, 光标高度
            // [9, 17] 
            // [10, 19]
            // [11, 20]
            // [12, 21]
            // [13, 23]
            // [14, 25]
            // [15, 27]
            // [16, 28]
            // [17, 30]
            // [18, 31]
            // [19, 32]
            // [20, 35]
            // [21, 36]
            // [22, 38]
            // [23, 40]
            // [24, 41]
            // [25, 43]
            // [26, 46]
            // [27, 46]
            // [28, 48]
            // [32, 57]
            // [36, 62]
            // [37, 64]
            // [40, 68]
            // [48, 83]
            // [60, 104] 
            // [72, 124]

            long& lCaretHeight = bm.bmHeight;
            if (lCaretHeight <= 19) // 10号字以下
            {
                m_sizeItalicFontCaretOffset.cx = -1;
            }
            else if (lCaretHeight <= 21)  // 12号字及以下不变
            {

            }
            else if (lCaretHeight <= 25)  // 14号字及以下+1
            {
                m_sizeItalicFontCaretOffset.cx = 1;
            }
            else if (lCaretHeight <= 31)  // 18号字及以下+2
            {
                m_sizeItalicFontCaretOffset.cx = 2;
            }
            else if (lCaretHeight <= 46)  // 27号字及以下 + 3
            {
                m_sizeItalicFontCaretOffset.cx = 3;
            }
            else  if (lCaretHeight <= 62) // 36号字及以下 + 5
            {
                m_sizeItalicFontCaretOffset.cx = 5;
            }
            else if (lCaretHeight <= 68) // 40号字及以下 + 7
            {
                m_sizeItalicFontCaretOffset.cx = 7;
            }
            else if (lCaretHeight <= 83) // 48号字及以下 + 10
            {
                m_sizeItalicFontCaretOffset.cx = 10; 
            }
            else if (lCaretHeight <= 104) // 60号字及以下 + 12
            {
                m_sizeItalicFontCaretOffset.cx = 12;
            }
            else if (lCaretHeight <= 124) // 72号字及以下 + 15
            {
                m_sizeItalicFontCaretOffset.cx = 15;
            }
        }
    }

    return TRUE;
}
void  ControlDrawCaret::Destroy(bool bRedraw)
{
    Hide(bRedraw);

    m_hWnd = NULL;
    m_pObject = NULL;
    m_ptLast.x = m_ptLast.y = -1;
    m_sizeCaret.cx = m_sizeCaret.cy = 0;
    m_rcDraw.SetRectEmpty();
    m_hBitmap = NULL;
    SAFE_RELEASE(m_pRenderBitmap);
}

// x, y 是object坐标
// -32000表示光标由于控件滚动进入了不可见区域
void  ControlDrawCaret::SetPos(int x, int y, bool bRedraw)
{
    RECT  rcInvalid[2] = {0};

    if (m_ptLast.x != -32000 && m_ptLast.y != -32000)
    {
        SetRect(rcInvalid,
            m_ptLast.x, 
            m_ptLast.y, 
            m_ptLast.x+m_sizeCaret.cx, 
            m_ptLast.y+m_sizeCaret.cy);
        OffsetRect(rcInvalid, m_sizeItalicFontCaretOffset.cx, m_sizeItalicFontCaretOffset.cy);
    }

    m_ptLast.x = x;
    m_ptLast.y = y;
    
    if (!m_bOn)
        return;

    DestroyTimer();
    CreateTimer();
    m_bVisible = true;  // 保持显示

    if (bRedraw && m_pObject)
    {
        if (m_ptLast.x != -32000 && m_ptLast.y != -32000)
        {
            SetRect(&rcInvalid[1],
                m_ptLast.x, 
                m_ptLast.y, 
                m_ptLast.x+m_sizeCaret.cx, 
                m_ptLast.y+m_sizeCaret.cy);

            OffsetRect(&rcInvalid[1], m_sizeItalicFontCaretOffset.cx, m_sizeItalicFontCaretOffset.cy);
        }

        if (m_pCallback)
        {
            m_pCallback->CaretInvalidateRect(rcInvalid, 2);
        }
        else if (m_eCoordType == COORD_CONTROL)
        {
            this->UpdateObjectEx(rcInvalid, 2);
        }
		else if (m_eCoordType == COORD_CONTROLCLIENT)
		{
			m_pObject->ClientRect2ObjectRect(&rcInvalid[0], &rcInvalid[0]);
			m_pObject->ClientRect2ObjectRect(&rcInvalid[1], &rcInvalid[1]);
			this->UpdateObjectEx(rcInvalid, 2);
		}
        else
        {
            m_pObject->WindowRect2ObjectRect(&rcInvalid[0], &rcInvalid[0]);
            m_pObject->WindowRect2ObjectRect(&rcInvalid[1], &rcInvalid[1]);
            this->UpdateObjectEx(rcInvalid, 2);
        }
    }
}

// 只刷新局部区域
void  ControlDrawCaret::UpdateControl()
{
    if (!m_pObject)
        return;

    if (m_ptLast.x == -32000 && m_ptLast.y == -32000)
        return;

    RECT  rcInvalid = {
        m_ptLast.x, 
        m_ptLast.y, 
        m_ptLast.x+m_sizeCaret.cx, 
        m_ptLast.y+m_sizeCaret.cy};

    OffsetRect(&rcInvalid, m_sizeItalicFontCaretOffset.cx, m_sizeItalicFontCaretOffset.cy);

    if (m_pCallback)
    {
        m_pCallback->CaretInvalidateRect(&rcInvalid, 1);
    }
    else if (m_eCoordType == COORD_CONTROL)
    {
        this->UpdateObjectEx(&rcInvalid, 1);
    }
	else if (m_eCoordType == COORD_CONTROLCLIENT)
	{
		m_pObject->ClientRect2ObjectRect(&rcInvalid, &rcInvalid);
		this->UpdateObjectEx(&rcInvalid, 1);
	}
    else
    {
        m_pObject->WindowRect2ObjectRect(&rcInvalid, &rcInvalid);
        //UpdateObjectEx参数为对象区域
        this->UpdateObjectEx(&rcInvalid, 1);
    }
}
void  ControlDrawCaret::OnTimer()
{
    m_bVisible = !m_bVisible;
    UpdateControl();
}

void  ControlDrawCaret::Show(bool bRedraw)
{
    CreateTimer();
    
    m_bOn = true;
    m_bVisible = true;

    if (bRedraw)
        UpdateControl();
}
void  ControlDrawCaret::Hide(bool bRedraw)
{
    DestroyTimer();

    m_bOn = false;
    m_bVisible = false;

    if (bRedraw)
        UpdateControl();
}

void  TimerProc_ControlDrawCaret(UINT_PTR nTimerID, TimerItem* pTimerItem)
{
    ControlDrawCaret* pThis = (ControlDrawCaret*)pTimerItem->lParam;
    pThis->OnTimer();
}
void  ControlDrawCaret::CreateTimer()
{
    if (!m_pObject)
        return;

    if (m_nTimerId)
        m_pObject->GetUIApplication()->KillTimer(m_nTimerId);

    TimerItem item;
    item.pProc = TimerProc_ControlDrawCaret;
    item.lParam = (LPARAM)this;
    m_nTimerId = m_pObject->GetUIApplication()->SetTimer(
        GetCaretBlinkTime(), &item);
}
void  ControlDrawCaret::DestroyTimer()
{
    if (0 != m_nTimerId)
        m_pObject->GetUIApplication()->KillTimer(m_nTimerId);
}

void  ControlDrawCaret::OnControlPaint(IRenderTarget* p)
{
    if (!m_bVisible || !m_pObject)
        return;

    POINT ptObj;
    if (m_pCallback)
    {
        m_pObject->WindowPoint2ObjectClientPoint(&m_ptLast, &ptObj, true);
        ptObj.x += m_sizeItalicFontCaretOffset.cx;
        ptObj.y += m_sizeItalicFontCaretOffset.cy;
    }
    else
    {
        ptObj = m_ptLast;
    }

    if (m_pRenderBitmap)
    {
        DRAWBITMAPPARAM param;
        param.xDest = ptObj.x;
        param.yDest = ptObj.y;
        param.wSrc = param.wDest = m_pRenderBitmap->GetWidth();
        param.hSrc = param.hDest = m_pRenderBitmap->GetHeight();
        param.xSrc = param.ySrc = 0;
        param.nFlag = DRAW_BITMAP_BITBLT;

        p->DrawBitmap(m_pRenderBitmap, &param);
    }
    else
    {
		// 注：这里不能使用Pen来画一条线。
		// 对于宽度为10的pen，画在x=8,y=2坐标处，得到的区域是(3-13)，居中绘制的，并不是(8-18)
        UI::Color c(0,0,0,255);
        if ((HBITMAP)1 == m_hBitmap)
            c.ReplaceRGB(128,128,128);
        
        int x = ptObj.x;
        int y = ptObj.y;

		CRect rc(x, y, x+m_sizeCaret.cx, y+m_sizeCaret.cy);
		p->DrawRect(&rc, &c);
    }
}


// 根据CreateCaret的参数创建一个RenderBitmap用于绘制 
void  CreateCaretRenderBitmap(HBITMAP hbmp, UI::IRenderBitmap** pp)
{
    if ((HBITMAP)0 == hbmp || (HBITMAP)1 == hbmp || NULL == pp)
        return;

    BITMAP bm = {0};
    GetObject(hbmp, sizeof(bm), &bm);

    IRenderBitmap* pRenderBitmap = NULL;
    UI::UICreateRenderBitmap(NULL, UI::GRAPHICS_RENDER_LIBRARY_TYPE_GDI, IMAGE_ITEM_TYPE_IMAGE, &pRenderBitmap);

    if (bm.bmBitsPixel != 32)
    {
        UI::ImageWrap image;
        image.Create(bm.bmWidth, bm.bmHeight, 32, UI::ImageWrap::createAlphaChannel);

        HDC hDC = image.GetDC();
        HDC hMemDC = ::CreateCompatibleDC(NULL);
        HBITMAP hOldBmp = (HBITMAP)::SelectObject(hMemDC, hbmp);
        ::BitBlt(hDC, 0, 0, bm.bmWidth, bm.bmHeight, hMemDC, 0, 0, SRCPAINT);
        ::SelectObject(hMemDC, hOldBmp);
        ::DeleteDC(hMemDC);
        // 
        //         // 不能直接画对角线。还要考虑斜率(大小总是给8)
        //         HPEN hPen = CreatePen(PS_SOLID, 1, RGB(255,255,255));
        //         HPEN hOldPen = (HPEN)SelectObject(hDC, hPen);
        //         MoveToEx(hDC, bm.bmWidth-1, 0, NULL);
        //         LineTo(hDC, 0, bm.bmHeight-1);
        //         SelectObject(hDC, hOldPen);
        //         DeleteObject(hPen);
        image.ReleaseDC();

        byte* pBitsDest = image.GetBits();
        int nPitch = image.GetPitch();
        for (int y = 0; y < bm.bmHeight; y++)
        {
            DWORD* pdwBits = (DWORD*)pBitsDest;
            for (int x = 0; x < bm.bmWidth; x++, pdwBits++)
            {
                if (*pdwBits != 0)
                    *pdwBits = 0xFF000000;
            }
            pBitsDest += nPitch;
        }

#ifdef _DEBUGx
        image.SaveAsPng(L"C:\\aaa.png");
#endif
        pRenderBitmap->Attach(image.Detach(), true);
    }
    else
    {
        pRenderBitmap->Attach(hbmp, false);
    }
    *pp = pRenderBitmap;
}

// 当前光标位置可能超出了控件区域，需要做一次交集。
// 目前updateobjectex不负责求交
void  ControlDrawCaret::UpdateObjectEx(RECT* prcArray, int nCount)
{
    if (!m_pObject)
        return;

    RECT rcObj = {0, 0, m_pObject->GetWidth(), m_pObject->GetHeight()};
    for (int i = 0; i < nCount; i++)
    {
        IntersectRect(&prcArray[i], &prcArray[i], &rcObj);
    }

	for (int i = 0; i < nCount; i++)
	{
		m_pObject->Invalidate(prcArray+i);
	}
}

