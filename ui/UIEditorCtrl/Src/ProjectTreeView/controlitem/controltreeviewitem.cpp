#include "stdafx.h"
#include "ControlTreeViewItem.h"
#include "UISDK\Inc\Util\iimage.h"
#include "UISDK\Inc\Util\dragdrop.h"

namespace UI
{
UINT ControlTreeViewItem::s_nControlDragCF = 0;

ControlTreeViewItem::ControlTreeViewItem(IControlTreeViewItem* p)
        : MessageProxy(p)
{
    m_pIControlTreeViewItem = p;
    m_ptLastLButtonDown.x = m_ptLastLButtonDown.y = 0;

    if (0 == s_nControlDragCF)
    {
        s_nControlDragCF = ::RegisterClipboardFormat(
            _T("UIEDITOR_PROJECTTREEVIEW_CONTROL_CF"));
    }
}
ControlTreeViewItem::~ControlTreeViewItem()
{

}

void  ControlTreeViewItem::OnMouseMove(UINT nFlags, POINT point)
{
    if (nFlags & MK_LBUTTON)
    {
        if (point.x != m_ptLastLButtonDown.x || 
            point.y != m_ptLastLButtonDown.y)
        {
            DoDrag(point);

            // DoDrag会将WM_LBUTOTNUP消息吃掉，这里手动发一个
            PostMessage(m_pIControlTreeViewItem->GetIListCtrlBase()->GetHWND(),
                WM_LBUTTONUP, 0, MAKELPARAM(point.x, point.y));
        }
    }
    SetMsgHandled(FALSE);
}
void  ControlTreeViewItem::OnLButtonDown(UINT nFlags, POINT point)
{
    m_ptLastLButtonDown.x = point.x;
    m_ptLastLButtonDown.y = point.y;

    SetMsgHandled(FALSE);  // SELECTED/FOCUS
}


void  ControlTreeViewItem::DoDrag(POINT point)
{
    // 先将自己折叠上，不允许拖拽到自己的子结点
    bool  bOldExpand = false;
    if (m_pIControlTreeViewItem->HasChild() && m_pIControlTreeViewItem->IsExpand())
        bOldExpand = true;
    if (bOldExpand)
    {
        m_pIControlTreeViewItem->SetExpand(false);
        m_pIControlTreeViewItem->GetIListCtrlBase()->LayoutItem(m_pIControlTreeViewItem, true);
    }


    IDataObject*  pDataObject = NULL;
    IDropSource*  pDropSource = NULL;
    IDragSourceHelper2*  pDragSourceHelper = NULL;

    UI::CreateDataObjectInstance(&pDataObject);
    UI::CreateDropSourceInstance(&pDropSource);

#pragma region
    FORMATETC  format = {0};
    format.dwAspect = DVASPECT_CONTENT;
    format.cfFormat = s_nControlDragCF;
    format.tymed = TYMED_HGLOBAL;

    const TCHAR* szText = m_pIControlTreeViewItem->GetText();    
    HGLOBAL hGlobal = GlobalAlloc(0, sizeof(TCHAR)*(_tcslen(szText)+1));
    TCHAR* lpDest = (TCHAR*)::GlobalLock(hGlobal);
    _tcscpy(lpDest, szText);
    ::GlobalUnlock(hGlobal);

    STGMEDIUM medium = {0};
    medium.tymed = TYMED_HGLOBAL;
    medium.hGlobal = hGlobal;
    pDataObject->SetData(&format, &medium, TRUE);
#pragma endregion

    CRect rcParent;
    CRect rcWindow;
    m_pIControlTreeViewItem->GetParentRect(&rcParent);
    m_pIControlTreeViewItem->GetIListCtrlBase()->ItemRect2WindowRect(&rcParent, &rcWindow);

    POINT ptOffset;
    ptOffset.x = point.x - rcWindow.left + g_rcDragImgPadding.left;
    ptOffset.y = point.y - rcWindow.top + g_rcDragImgPadding.top;

    DWORD dwEffect = 0;
    UIDoDragDrop(
        pDataObject, 
        pDropSource, 
        DROPEFFECT_COPY|DROPEFFECT_MOVE,
        &dwEffect,
        NULL, 
        ptOffset);

    SAFE_RELEASE(pDataObject);
    SAFE_RELEASE(pDropSource);
    SAFE_RELEASE(pDragSourceHelper);

    if (bOldExpand)
    {
        m_pIControlTreeViewItem->SetExpand(true);
        m_pIControlTreeViewItem->GetIListCtrlBase()->LayoutItem(m_pIControlTreeViewItem, true);
    }
    return;
}


// 注：如果直接将Item画在缓存的(0,0)处，结果拖拽时左侧有若干像素居然显示到右边去了，
//     具体也不知道windows内部是怎么处理的。因此将缓存图片增大g_rcDragImgPadding，
//     这样拖拽时显示的图片就能正常了
HBITMAP  ControlTreeViewItem::CreateDragBitmap(int* pWidth, int* pHeight)
{
    CRect  rcParent;
    m_pIControlTreeViewItem->GetParentRect(&rcParent);
    IListCtrlBase*  pListCtrl = m_pIControlTreeViewItem->GetIListCtrlBase();

    int nHeight = rcParent.Height() + g_rcDragImgPadding.left + g_rcDragImgPadding.right;
    int nWidth = rcParent.Width() + g_rcDragImgPadding.top + g_rcDragImgPadding.bottom;

    // 在Win7无主题的情况下，经N次验证得到这两个数。如果小于它，则拖拽时无法显示图片
    if (nWidth < 295)
        nWidth = 295;
    if (nHeight < 86)
        nHeight = 86;

    // 创建位图
    UI::ImageWrap  image;
    image.Create(nWidth, nHeight, 32, UI::ImageWrap::createAlphaChannel);
    HDC hDC = image.GetDC();
    {
        ITextRenderBase*  pTextRender = m_pIControlTreeViewItem->GetIListCtrlBase()->GetTextRender();
        IRenderFont*  pRenderFont = pTextRender->GetRenderFont();
        LOGFONT  lf = {0};
        pRenderFont->GetLogFont(&lf);

        // 计算文字位置
        Gdiplus::Graphics  g(hDC);
        Gdiplus::RectF  rcLayout(
            (Gdiplus::REAL)g_rcDragImgPadding.left, 
            (Gdiplus::REAL)g_rcDragImgPadding.top, 
            (Gdiplus::REAL)rcParent.Width(), 
            (Gdiplus::REAL)rcParent.Height());

        INormalTreeItemShareData*  pShareData = (INormalTreeItemShareData*)pListCtrl->GetItemTypeShareData(LISTITEM_TYPE_TREEITEM_NORMAL);
        int nOffset = pListCtrl->GetChildNodeIndent()* m_pIControlTreeViewItem->CalcDepth();
        nOffset += pShareData->GetImageWidth() + pShareData->GetExpandIconWidth();
        rcLayout.Offset((Gdiplus::REAL)nOffset, 0);
        
        Gdiplus::Font  font(hDC, &lf);
        Gdiplus::SolidBrush brush(Gdiplus::Color(0xFEFF0000));

        Gdiplus::StringFormat  sf;
        sf.SetAlignment(Gdiplus::StringAlignmentNear);
        sf.SetLineAlignment(Gdiplus::StringAlignmentCenter);

        g.DrawString(m_pIControlTreeViewItem->GetText(), -1, &font, rcLayout, &sf, &brush);
    }
    image.ReleaseDC();

    if (pWidth)
        *pWidth = image.GetWidth();
    if (pHeight)
        *pHeight = image.GetHeight();

    return image.Detach();
}

}