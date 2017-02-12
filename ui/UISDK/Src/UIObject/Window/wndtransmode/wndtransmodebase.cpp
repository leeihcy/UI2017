#include "stdafx.h"
#include "wndtransmodebase.h"
#include "Src/UIObject/Window/customwindow.h"

namespace UI
{

WndTransModeBase::WndTransModeBase():Message(NULL)
{
    m_pWindow = NULL;
}
WndTransModeBase::~WndTransModeBase()
{

}

void  WndTransModeBase::Init(ICustomWindow* pWnd)
{
    UIASSERT(pWnd);
    m_pWindow = pWnd->GetImpl();
}

HWND  WndTransModeBase::GetHWND()
{
	if (!m_pWindow)
		return NULL;

	return m_pWindow->m_hWnd;
}

HRGN   WndTransModeBase::CreateRgnByVectorPoint(vector<POINT>& vec)
{
    int nCount = (int)vec.size();
    RGNDATA*   pRgnData      = (RGNDATA*)new BYTE[ sizeof(RGNDATAHEADER) + nCount*sizeof(RECT) ];
    pRgnData->rdh.dwSize     = sizeof(RGNDATAHEADER);
    pRgnData->rdh.iType      = RDH_RECTANGLES;
    pRgnData->rdh.nCount     = nCount;
    pRgnData->rdh.nRgnSize   = nCount*sizeof(RECT);
    for (int k = 0; k < nCount; k++)
    {
        RECT* prc = (RECT*)pRgnData->Buffer;
        prc[k].left   = vec[k].x;
        prc[k].top    = vec[k].y;
        prc[k].right  = vec[k].x+1;
        prc[k].bottom = vec[k].y+1;
    }

    HRGN hRgn = ::ExtCreateRegion(NULL, sizeof(RGNDATAHEADER) + nCount*sizeof(RECT), pRgnData);
    delete [] pRgnData;
    pRgnData = NULL;

    return hRgn;
}
HRGN   WndTransModeBase::CreateRgnByVectorRect(vector<RECT>& vec)
{
    int nCount = (int)vec.size();
    RGNDATA*   pRgnData      = (RGNDATA*)new BYTE[ sizeof(RGNDATAHEADER) + nCount*sizeof(RECT) ];
    pRgnData->rdh.dwSize     = sizeof(RGNDATAHEADER);
    pRgnData->rdh.iType      = RDH_RECTANGLES;
    pRgnData->rdh.nCount     = nCount;
    pRgnData->rdh.nRgnSize   = nCount*sizeof(RECT);
    for (int k = 0; k < nCount; k++)
    {
        RECT* prc = (RECT*)pRgnData->Buffer;
        prc[k] = vec[k];
    }

    HRGN hRgn = ::ExtCreateRegion(NULL, sizeof(RGNDATAHEADER) + nCount*sizeof(RECT), pRgnData);
    delete [] pRgnData;
    pRgnData = NULL;

    return hRgn;
}

bool WndTransModeBase::GetWindowBitmapInfo(byte** ppScan0, int& nPitch)
{
    if (!ppScan0)
        return false;

	UIASSERT(0 && L"ÐèÒªÖØÐ´");
#if 0
    HDC hMemDC = m_pWindow->GetRenderLayer2()->GetRenderTarget()->GetHDC();
    if (!hMemDC)
		return false;

    BITMAP  bm;
    HBITMAP hBitmap = (HBITMAP)::GetCurrentObject(hMemDC, OBJ_BITMAP);
    if (!hBitmap)
		return false;

    ::GetObject(hBitmap, sizeof(bm), &bm);

    nPitch = bm.bmWidthBytes;
    BYTE*  pScan0 = (BYTE*)bm.bmBits;

    *ppScan0 = pScan0;
#endif
	return true;
}
}