#include "stdafx.h"
#include "toolboxtreeitem.h"
#include "UISDK\Inc\Util\iimage.h"
#include "UISDK\Inc\Util\dragdrop.h"


namespace UI
{
UINT ToolBoxTreeItem::s_nLayoutItemDragCF = 0;

ToolBoxTreeItem::ToolBoxTreeItem(IToolBoxTreeItem* p) : MessageProxy(p)
{
    m_pIToolBoxTreeItem = p;
    m_ptLastLButtonDown.x = m_ptLastLButtonDown.y = 0;

    if (0 == s_nLayoutItemDragCF)
    {
        s_nLayoutItemDragCF = 
            ::RegisterClipboardFormat(_T("UIEDITOR_LAYOUTVIEW_OBJECT_CF"));
    }
	m_pObjectDescription = nullptr;
}

void  ToolBoxTreeItem::OnLButtonUp(UINT nFlags, POINT point)
{
    SetMsgHandled(FALSE);
}

void ToolBoxTreeItem::BindObject(IObjectDescription* p)
{
	m_pObjectDescription = p;
}

void  ToolBoxTreeItem::OnMouseMove(UINT nFlags, POINT point)
{
    if (nFlags & MK_LBUTTON)
    {
        if (point.x != m_ptLastLButtonDown.x || 
            point.y != m_ptLastLButtonDown.y)
        {
            DoDrag(point);

            // DoDrag会将WM_LBUTOTNUP消息吃掉，这里手动发一个
            PostMessage(m_pIToolBoxTreeItem->GetIListCtrlBase()->GetHWND(),
                WM_LBUTTONUP, 0, MAKELPARAM(point.x, point.y));
        }
    }
    SetMsgHandled(FALSE);
}
void  ToolBoxTreeItem::OnLButtonDown(UINT nFlags, POINT point)
{
    m_ptLastLButtonDown.x = point.x;
    m_ptLastLButtonDown.y = point.y;

    SetMsgHandled(FALSE);  // SELECTED/FOCUS
}


void  ToolBoxTreeItem::DoDrag(POINT point)
{
    IDataObject*  pDataObject = NULL;
    IDropSource*  pDropSource = NULL;

    UI::CreateDataObjectInstance(&pDataObject);
    UI::CreateDropSourceInstance(&pDropSource);

#pragma region
    FORMATETC  format = {0};
    format.dwAspect = DVASPECT_CONTENT;
    format.cfFormat = s_nLayoutItemDragCF;
    format.tymed = TYMED_HGLOBAL;

    HGLOBAL hGlobal = GlobalAlloc(0, sizeof(IObjectDescription*));
    LPVOID p = GlobalLock(hGlobal);
	memcpy(p, &m_pObjectDescription, sizeof(IObjectDescription*));;
    ::GlobalUnlock(hGlobal);

    STGMEDIUM medium = {0};
    medium.tymed = TYMED_HGLOBAL;
    medium.hGlobal = hGlobal;
    pDataObject->SetData(&format, &medium, TRUE);
#pragma endregion

    HBITMAP hDragBitmap = NULL;

	CRect rcParent;
	CRect rcWindow;
	m_pIToolBoxTreeItem->GetParentRect(&rcParent);
	m_pIToolBoxTreeItem->GetIListCtrlBase()->ItemRect2WindowRect(&rcParent, &rcWindow);

	POINT ptOffset;
	ptOffset.x = point.x - rcWindow.left + g_rcDragImgPadding.left;
	ptOffset.y = point.y - rcWindow.top + g_rcDragImgPadding.top;

	DWORD dwEffect = 0;
	UIDoDragDrop(pDataObject, pDropSource, DROPEFFECT_MOVE, &dwEffect, NULL, ptOffset);
    
    SAFE_RELEASE(pDataObject);
    SAFE_RELEASE(pDropSource);

    return;
}

// 
// // 注：如果直接将Item画在缓存的(0,0)处，结果拖拽时左侧有若干像素居然显示到右边去了，
// //     具体也不知道windows内部是怎么处理的。因此将缓存图片增大g_rcDragImgPadding，
// //     这样拖拽时显示的图片就能正常了
// HBITMAP  ToolBoxTreeItem::CreateDragBitmap(int* pWidth, int* pHeight)
// {
//     CRect  rcParent;
//     m_pIToolBoxTreeItem->GetParentRect(&rcParent);
// 
//     int nHeight = rcParent.Height() + g_rcDragImgPadding.left + g_rcDragImgPadding.right;
//     int nWidth = rcParent.Width() + g_rcDragImgPadding.top + g_rcDragImgPadding.bottom;
// 
//     // 在Win7无主题的情况下，经N次验证得到这两个数。如果小于它，则拖拽时无法显示图片
//     if (nWidth < 295)
//         nWidth = 295;
//     if (nHeight < 86)
//         nHeight = 86;
// 
//     // 创建位图
//     UI::ImageWrap  image;
//     image.Create(nWidth, nHeight, 32, UI::ImageWrap::createAlphaChannel);
//     HDC hDC = image.GetDC();
// 	SetBkMode(hDC, TRANSPARENT);
// 
//     // 创建rendertarget
//     HWND hWnd = m_pIToolBoxTreeItem->GetIListCtrlBase()->GetHWND();
//     UI::IRenderTarget*  pRenderTarget = UI::UICreateRenderTarget(
// 		NULL, GRAPHICS_RENDER_LIBRARY_TYPE_GDI, true);
// 	
//     pRenderTarget->BindHDC(hDC);
// 
//     // 绘制
//     pRenderTarget->BeginDraw();
//     pRenderTarget->SetOrigin(
//         -rcParent.left+g_rcDragImgPadding.left, 
//         -rcParent.top+g_rcDragImgPadding.top);  // 抵消DrawItemInnerControl中的偏移
// 
// 
//     UISendMessage(m_pIToolBoxTreeItem, WM_PAINT, (WPARAM)pRenderTarget, 0);
//     pRenderTarget->EndDraw();
// 
//     SAFE_RELEASE(pRenderTarget);
//     image.ReleaseDC();
// 
//     if (pWidth)
//         *pWidth = image.GetWidth();
//     if (pHeight)
//         *pHeight = image.GetHeight();
// 
//     return image.Detach();
// }

}