#include "stdafx.h"
#include "layout.h"

#include "Inc\Interface\imapattr.h"
#include "Src\Base\Object\object.h"
#include "Src\UIObject\Window\windowbase.h"
#include "Src\Helper\layout\canvaslayout.h"
#include "Src\Util\DPI\dpihelper.h"

#if 0

SIZE  DockLayout::MeasureChildObject()
{
	SIZE size = {0,0};

	// 1. 先获取center对象的SIZE
	Object*  pCenterObj = NULL;
	Object*  pChild = NULL;
	while (pChild = this->m_pPanel->EnumChildObject(pChild))
	{
#if 1
		int nFlag = pChild->GetConfigLayoutFlags();
		if (nFlag & LAYOUT_ITEM_ALIGN_CENTER)
			pCenterObj = pChild;
		else if (0 == (nFlag&LAYOUT_ITEM_ALIGN_LTRB))
			pCenterObj = pChild;
#else
		String  strDock;
		pChild->GetAttribute(XML_LAYOUT_DOCK_DOCK, strDock);

		if (XML_LAYOUT_DOCK_DOCK_CENTER == strDock ||
			strDock.empty() )
		{
			pCenterObj = pChild;
			break;
		}
#endif
	}

	if (pCenterObj)
		size = pCenterObj->GetDesiredSize();

	// 2. 从后开始遍历停靠在四周的子对象的大小
	pChild = NULL;
	while (pChild = this->m_pPanel->REnumChildObject(pChild))
	{
		if (!pChild->IsVisible())
			continue;

		int nFlag = pChild->GetConfigLayoutFlags();

#if 1
		if (nFlag & LAYOUT_ITEM_ALIGN_LEFT || LAYOUT_ITEM_ALIGN_RIGHT)
#else
		String  strDock;
		pChild->GetAttribute(XML_LAYOUT_DOCK_DOCK, strDock);

		if (XML_LAYOUT_DOCK_DOCK_LEFT == strDock ||
			XML_LAYOUT_DOCK_DOCK_RIGHT == strDock)
#endif
		{
			SIZE s = pChild->GetDesiredSize();
			if (s.cy > size.cy)
				size.cy = s.cy;
			size.cx += s.cx;
		}
		else 
#if 1
		if (nFlag & LAYOUT_ITEM_ALIGN_TOP || LAYOUT_ITEM_ALIGN_BOTTOM)
#else
		if (XML_LAYOUT_DOCK_DOCK_TOP == strDock ||
			 XML_LAYOUT_DOCK_DOCK_BOTTOM == strDock)
#endif
		{
			SIZE s = pChild->GetDesiredSize();
			if (s.cx > size.cx)
				size.cx = s.cx;
			size.cy += s.cy;
		}

		else
		{
			continue;
		}

	}

	return size;
}
void  DockLayout::ArrangeChildObject(Object* pObjToArrage)
{
	// 调用该函数时，自己的大小已经被求出来了
	int  nWidth  = m_pPanel->GetWidth();
	int  nHeight = m_pPanel->GetHeight();

	int  nComsumeTop    = this->m_pPanel->GetPaddingT();
	int  nComsumeLeft   = this->m_pPanel->GetPaddingL();
	int  nComsumeBottom = this->m_pPanel->GetPaddingB();
	int  nComsumeRight  = this->m_pPanel->GetPaddingR();

	Object* pCenterObj  = NULL;  // 记录是否已经设置过 dock布局中的 center 对象了（因为只允许有一个center对象）
	
	// arrange的时候要从前往后开始排布
	Object* pChild = NULL;
	while (pChild = this->m_pPanel->EnumChildObject(pChild))
	{
		if( !pChild->IsVisible() )
			continue;

#if 1
		int nFlag = pChild->GetConfigLayoutFlags();
#else
		String  strDock;
		pChild->GetAttribute(XML_LAYOUT_DOCK_DOCK, strDock);
#endif

		// 计算出 pChild 的 rectP
#if 1
		if (nFlag & LAYOUT_ITEM_ALIGN_LEFT)
#else
		if (XML_LAYOUT_DOCK_DOCK_LEFT == strDock)
#endif
		{
			SIZE s = pChild->GetDesiredSize();
			
			CRect rcObj( 
				nComsumeLeft             +  pChild->GetMarginL(), 
				nComsumeTop              +  pChild->GetMarginT(), 
				nComsumeLeft + s.cx      -  pChild->GetMarginR(), 
				nHeight - nComsumeBottom -  pChild->GetMarginB());
			pChild->SetObjectPos(&rcObj, SWP_NOREDRAW);

			nComsumeLeft += s.cx;
		}
		else
#if 1
		if (nFlag & LAYOUT_ITEM_ALIGN_RIGHT)
#else
		if (XML_LAYOUT_DOCK_DOCK_RIGHT == strDock)
#endif
		{
			SIZE s = pChild->GetDesiredSize();
			
			CRect rcObj( 
				nWidth - nComsumeRight - s.cx  +  pChild->GetMarginL(), 
				nComsumeTop                    +  pChild->GetMarginT(), 
				nWidth - nComsumeRight         -  pChild->GetMarginR(), 
				nHeight - nComsumeBottom       -  pChild->GetMarginB());
			pChild->SetObjectPos(&rcObj, SWP_NOREDRAW);

			nComsumeRight += s.cx;
		}
		else 
#if 1
		if (nFlag & LAYOUT_ITEM_ALIGN_TOP)
#else
		if (XML_LAYOUT_DOCK_DOCK_TOP == strDock)
#endif
		{
			SIZE s = pChild->GetDesiredSize();

			CRect rcObj( 
				nComsumeLeft            +  pChild->GetMarginL() , 
				nComsumeTop             +  pChild->GetMarginT() , 
				nWidth - nComsumeRight  -  pChild->GetMarginR() , 
				nComsumeTop + s.cy      -  pChild->GetMarginB() );
			pChild->SetObjectPos(&rcObj, SWP_NOREDRAW);

			nComsumeTop += s.cy;
		}
		else 
#if 1
		if (nFlag & LAYOUT_ITEM_ALIGN_BOTTOM)
#else
		if (XML_LAYOUT_DOCK_DOCK_BOTTOM == strDock)
#endif
		{
			SIZE s = pChild->GetDesiredSize();

			CRect rcObj(
				nComsumeLeft                    +  pChild->GetMarginL() , 
				nHeight - nComsumeBottom - s.cy +  pChild->GetMarginT() , 
				nWidth - nComsumeRight          -  pChild->GetMarginR() , 
				nHeight - nComsumeBottom        -  pChild->GetMarginB() );
			pChild->SetObjectPos(&rcObj, SWP_NOREDRAW);

			nComsumeBottom += s.cy;	 
		}
		else  // 其他都归为center对象
		{
			if( NULL == pCenterObj )
			{
				pCenterObj = pChild;
				continue;
			}
		}
	}

	if( pCenterObj )
	{
		pCenterObj->SetObjectPos(
			nComsumeLeft                            +  pCenterObj->GetMarginL() , 
			nComsumeTop                             +  pCenterObj->GetMarginT() , 
			nWidth  - nComsumeRight  - nComsumeLeft -  pCenterObj->GetMarginR() , 
			nHeight - nComsumeBottom - nComsumeTop  -  pCenterObj->GetMarginB() ,
			SWP_NOREDRAW);
	}
}

#endif
void  DesktopLayout::Arrange(WindowBase*  pWindow)
{
	if (NULL == pWindow)
		return;

    if (pWindow->IsChildWindow())
    {
        HWND hWndParent = GetParent(pWindow->m_hWnd);
        CRect rc;
        ::GetClientRect(hWndParent, &rc);

        int nCX = rc.Width();
        int nCY = rc.Height();

        CanvasLayout::ArrangeObject(pWindow, nCX, nCY);
        return;
    }

	// 读取其他属性值来设置rectW
	int  x = 0, y = 0;                                 // 最终在屏幕中的坐标
	int  nCXScreen = 800, nCYScreen = 600;             // 屏幕大小
	int  left = NDEF, top = NDEF, right = NDEF, bottom = NDEF;

    CanvasLayoutParam* pParam = CanvasLayout::s_GetObjectLayoutParam(pWindow);
    if (!pParam)
    {
        return;
    }
    left = pParam->GetConfigLeft();
    top = pParam->GetConfigTop();
    right = pParam->GetConfigRight();
    bottom = pParam->GetConfigBottom();

    SIZE s = pWindow->GetDesiredSize();  // 获得的SIZE包括了MARGIN的大小 s.cx=margin.left+width+margin.right
#ifdef _DEBUG
    if (s.cx == 0 && s.cy == 0)
    {
        UI_LOG_WARN(TEXT("GetDesiredSize 返回窗口大小为0，检查是否配置了窗口的widht/height"));
    }
#endif

    // 计算出屏幕工作区域的大小(不使用整个屏幕的大小)
    //nCXScreen = ::GetSystemMetrics( SM_CXSCREEN );
    //nCYScreen = ::GetSystemMetrics( SM_CYSCREEN );

	CRect rcParent;
    if (GetWindowLong(pWindow->m_hWnd, GWL_STYLE)&WS_CHILD)
    {
        HWND hWndParent = GetParent(pWindow->m_hWnd);
        if (hWndParent)
        {
            ::GetClientRect(hWndParent, &rcParent);
        }
    }
    else
    {
		POINT pt = { 0 };
		GetCursorPos(&pt);
		
		MONITORINFO mi = {sizeof(mi), 0};
		if (GetMonitorInfo(MonitorFromPoint(pt, MONITOR_DEFAULTTOPRIMARY), &mi))
		{
			rcParent = mi.rcWork;
		}
		else
		{
			SystemParametersInfo(SPI_GETWORKAREA, 0, &rcParent, 0);
		}
    }
	
    // 如果同时指定了left/right,则忽略width属性
    if (left != NDEF && right != NDEF)
    {
        s.cx = rcParent.Width() - left - right;
    }
    // 如果同时指定了top/bottom，则忽略height属性
    if (top != NDEF && bottom != NDEF)
    {
        s.cy = rcParent.Height() - top - bottom;
    }

	// 计算出坐标
	if (left != NDEF)
	{
		x = rcParent.left + left;
		x += pWindow->GetMarginL();
	}
	else
	{
		if (right != NDEF)
		{
			x = rcParent.right - right - s.cx;  // right是指窗口右侧距离屏幕右侧的距离
			x -= pWindow->GetMarginR();
		}
		else
		{
			// 居中
			x = rcParent.left + (rcParent.Width() - s.cx) / 2;
		}
	}
	if (top != NDEF)
	{
		y = rcParent.top + top;
		y += pWindow->GetMarginT();
	}
	else
	{
		if (bottom != NDEF)
		{
			y = rcParent.bottom - bottom - s.cy; // 同right
			y -= pWindow->GetMarginB();
		}
		else
		{
			// 居中
			y = rcParent.top + (rcParent.Height() - s.cy) / 2;
		}
	}

    CRect  rcClientOld, rcClientNew;
    GetClientRect(pWindow->m_hWnd, &rcClientOld);

	pWindow->SetObjectPos(x, y, s.cx, s.cy, SWP_NOREDRAW);

    // 解决如果窗口大小没有发生改变，改变窗口没有收到WM_SIZE时，手动布局一次
    GetClientRect(pWindow->m_hWnd, &rcClientNew);
    if (rcClientNew.Width() == rcClientOld.Width() &&
        rcClientNew.Height() == rcClientOld.Height())
    {
        pWindow->notify_WM_SIZE(0, rcClientNew.Width(), rcClientNew.Height());
    }

	// 递归
	//pWindow->GetLayout()->Arrange(NULL); // <-- SetObjectPos会触发OnSize，在Window的OnSize消息中会进行layout处理，因此这里可以不调用
}

namespace UI
{


    void  LoadConfigWH(LPCTSTR szText, long& wh, long& whtype)
    {
        if (!szText || !szText[0] || 0 == _tcscmp(szText, XML_AUTO))
        {
            whtype = WH_AUTO;
            wh = 0;
            return;
        }

        int nLength = _tcslen(szText);
        if (szText[nLength - 1] == XML_AVERAGE_CHAR)
        {
            // 只支持平均分配，不支持平均系数
            whtype = WH_AVG;
            wh = 0;
            return;
        }

        if (szText[nLength - 1] == XML_PERCENT_CHAR)
        {
            String str(szText);
            str[nLength - 1] = 0;

            whtype = WH_PERCENT;
            wh = _ttoi(str.c_str());
            return;
        }

        wh = ScaleByDpi(_ttoi(szText));
        whtype = WH_SET;
    }

    LPCTSTR  SaveConfigWH(long wh, long whtype)
    {
        LPTSTR szBuffer = GetTempBuffer();

        switch (whtype)
        {
        case WH_AUTO:
            return NULL;

        case WH_SET:
        {
            _stprintf(szBuffer, TEXT("%d"), RestoreByDpi(wh));
            return szBuffer;
        }

        case WH_AVG:
        {
            szBuffer[0] = XML_AVERAGE_CHAR;
            return szBuffer;
        }

        case WH_PERCENT:
        {
            _stprintf(szBuffer, TEXT("%d%%"), wh);
            return szBuffer;
        }
        }
        return NULL;
    }

}