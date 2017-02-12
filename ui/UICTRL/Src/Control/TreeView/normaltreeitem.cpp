#include "stdafx.h"
#include "normaltreeitem.h"

namespace UI
{
NormalTreeItemShareData::NormalTreeItemShareData(INormalTreeItemShareData* p):MessageProxy(p)
{
	m_pINormalTreeItemShareData = p;
    pExpandIconRender = NULL;
    nExpandIconWidth = 20;
    nImageWidth = 20;
}
NormalTreeItemShareData::~NormalTreeItemShareData()
{
    SAFE_RELEASE(pExpandIconRender)
}

//////////////////////////////////////////////////////////////////////////
NormalTreeItem::NormalTreeItem(INormalTreeItem* p) : TreeItem(p)
{
    m_pINormalTreeItem = p;
    m_pShareData = NULL;
}

void  NormalTreeItem::OnInitialize()
{
    IListCtrlBase*  pListCtrl = m_pINormalTreeItem->GetIListCtrlBase();

    m_pINormalTreeItem->SetItemType(LISTITEM_TYPE_TREEITEM_NORMAL);
    INormalTreeItemShareData* pData = static_cast<INormalTreeItemShareData*>(
        pListCtrl->GetItemTypeShareData(LISTITEM_TYPE_TREEITEM_NORMAL));
    if (NULL == pData)
    {
        pData = INormalTreeItemShareData::CreateInstance(pListCtrl->GetSkinRes());
        pListCtrl->SetItemTypeShareData(LISTITEM_TYPE_TREEITEM_NORMAL, pData);
        m_pShareData = pData->GetImpl();

        UISendMessage(pListCtrl, UI_TVM_GETEXPANDICON_RENDER, (WPARAM)&m_pShareData->pExpandIconRender);
    }
    else
    {
        m_pShareData = pData->GetImpl();
    }
}

void  NormalTreeItem::OnPaint(IRenderTarget* pRenderTarget)
{
    IListCtrlBase*  pListCtrl = m_pINormalTreeItem->GetIListCtrlBase();
    bool bFocus = pListCtrl->IsFocus();
    UINT nRenderState = m_pINormalTreeItem->GetItemRenderState();

    CRect rcParent;
    m_pINormalTreeItem->GetParentRect(&rcParent);

    if (pListCtrl->GetForeRender())
        pListCtrl->GetForeRender()->DrawState(pRenderTarget, &rcParent, nRenderState);

    CRect  rcItem(rcParent);
#if 0
    if (bFocus && m_pFocusItem == pItem && NULL != m_pFocusRender)
        m_pFocusRender->DrawState(pRenderTarget, &m_rcParent, 0);
#endif
    // 绘制层级线条
    //if (pListCtrl->TestStyleEx(TREE_STYLE_HASLINE))
        this->DrawLine(pRenderTarget);

    rcItem.left += pListCtrl->GetChildNodeIndent()* m_pINormalTreeItem->CalcDepth();

    // 绘制展开收缩图标
    if (m_pShareData->nExpandIconWidth)
    {
        CRect rcExpandIcon = rcItem;
        rcExpandIcon.right = rcItem.left + m_pShareData->nExpandIconWidth;

        if (m_pShareData->pExpandIconRender && m_pINormalTreeItem->GetChildItem())
        {
            if (m_pINormalTreeItem->IsExpand())
            {
                m_pShareData->pExpandIconRender->DrawState(
                    pRenderTarget, 
                    &rcExpandIcon, 
                    TREEVIEWCTRL_EXPANDCOLLAPSEICON_RENDER_STATE_EXPAND_NORMAL
                    );
            }
            else
            {
                m_pShareData->pExpandIconRender->DrawState(
                    pRenderTarget, 
                    &rcExpandIcon, 
                   TREEVIEWCTRL_EXPANDCOLLAPSEICON_RENDER_STATE_COLLAPSE_NORMAL
                    );
            }
        }

        rcItem.left = rcExpandIcon.right;
    }

    // 绘制图片
    IRenderBase*  pIconRender = m_pINormalTreeItem->GetIconRender();
    if (m_pShareData->nImageWidth > 0 && pIconRender)
    {
        CRect rcImage = rcItem;
        rcImage.right = rcImage.left + m_pShareData->nImageWidth;

        pIconRender->DrawState(pRenderTarget, &rcImage, 0);
        rcItem.left += m_pShareData->nImageWidth;
    }

    // 绘制文本
    ITextRenderBase*  pTextRender = pListCtrl->GetTextRender();
    if (!pTextRender)
        pTextRender = pListCtrl->CreateDefaultTextRender();
    if (pTextRender)
    {
#ifdef _DEBUGx
         TCHAR szText[256] = _T("");
         _stprintf(szText, _T("%d.%d.%d.%d : %s"), m_pINormalTreeItem->GetTreeIndex(), 
             m_pINormalTreeItem->GetLineIndex(), m_pINormalTreeItem->GetNeighbourIndex(), 
             m_pINormalTreeItem->CalcDepth(), m_pINormalTreeItem->GetText());
         pTextRender->DrawState(pRenderTarget, &rcItem, nRenderState, szText);
#else
        pTextRender->DrawState(
            pRenderTarget, 
            &rcItem, 
            nRenderState, 
            m_pINormalTreeItem->GetText());
#endif
    }
}

// 绘制树控件的线条
void  NormalTreeItem::DrawLine(IRenderTarget* pRenderTarget)
{
    HDC hDC = pRenderTarget->GetHDC();

    LOGBRUSH logBrush = {BS_SOLID, RGB(128,128,128), 0};

    HPEN hPen = ExtCreatePen(PS_GEOMETRIC|PS_DOT, 1, &logBrush, 0, NULL);
    HPEN hOldPen = (HPEN)::SelectObject(hDC, hPen);

    IListCtrlBase*  pTreeView = m_pINormalTreeItem->GetIListCtrlBase();
    int nChildIndent = pTreeView->GetChildNodeIndent();

    CRect rcLine;
    m_pINormalTreeItem->GetParentRect(&rcLine);

    int nDepth = m_pINormalTreeItem->CalcDepth();
    rcLine.right = rcLine.left + nChildIndent*(nDepth+1);
    rcLine.left = rcLine.right - nChildIndent;
    this->DrawHalfHLine(hDC, &rcLine);

    bool bDrawTop = (m_pINormalTreeItem->GetPrevItem() || m_pINormalTreeItem->GetParentItem());
    bool bDrawBottom = (NULL != m_pINormalTreeItem->GetNextItem());
    if (bDrawTop && bDrawBottom)
        this->DrawFullVLine(hDC, &rcLine);
    else if (bDrawBottom)
        this->DrawHalfVBottomLine(hDC, &rcLine);
    else if (bDrawTop)
        this->DrawHalfVTopLine(hDC, &rcLine);

    rcLine.OffsetRect(-nChildIndent, 0);

    IListItemBase* pParent = m_pINormalTreeItem->GetParentItem();
    for (int i = 0; i <= nDepth; i++)
    {
        if (NULL == pParent)
            break;

        if (pParent->GetNextItem())
        {
            this->DrawFullVLine(hDC, &rcLine);
        }

        rcLine.OffsetRect(-nChildIndent, 0);
        pParent = pParent->GetParentItem();
    }
    SelectObject(hDC, hOldPen);
    DeleteObject(hPen);
    pRenderTarget->ReleaseHDC(hDC);
}

void  NormalTreeItem::DrawFullVLine(HDC hDC, const CRect* prc)
{
    int x = prc->left + prc->Width()/2;
    MoveToEx(hDC, x, prc->top, NULL);
    LineTo(hDC, x, prc->bottom);
}
void  NormalTreeItem::DrawFullHLine(HDC hDC, const CRect* prc)
{
    int y = prc->top + prc->Height()/2;
    MoveToEx(hDC, prc->left, y, NULL);
    LineTo(hDC, prc->right, y);
}
void  NormalTreeItem::DrawHalfVTopLine(HDC hDC, const CRect* prc)
{
    int x = prc->left + prc->Width()/2;
    MoveToEx(hDC, x, prc->top, NULL);
    LineTo(hDC, x, prc->top+prc->Height()/2);
}
void  NormalTreeItem::DrawHalfVBottomLine(HDC hDC, const CRect* prc)
{
    int x = prc->left + prc->Width()/2;
    MoveToEx(hDC, x, prc->top+prc->Height()/2, NULL);
    LineTo(hDC, x, prc->bottom);
}

void  NormalTreeItem::DrawHalfHLine(HDC hDC, const CRect* prc)
{
    int y = prc->top + prc->Height()/2;
    MoveToEx(hDC, prc->left+prc->Width()/2, y, NULL);
    LineTo(hDC, prc->right, y);
}

void  NormalTreeItem::OnLButtonDown(UINT nFlags, POINT point)
{ 
    IListCtrlBase*  pCtrl = m_pINormalTreeItem->GetIListCtrlBase();
    CRect  rcItemExpand;
    GetItemExpandIconRect(&rcItemExpand);

    POINT ptClient;
    pCtrl->WindowPoint2ObjectClientPoint(&point, &ptClient, true);

    if (rcItemExpand.PtInRect(ptClient))   
    {
        pCtrl->ToggleItemExpand(m_pINormalTreeItem, false);
        m_pINormalTreeItem->GetIListCtrlBase()->Invalidate();
    }
    else
    {
        SetMsgHandled(FALSE);  // 单击选中
    }
}

void  NormalTreeItem::OnLButtonDblClk(UINT nFlags, POINT point)
{
    if (!m_pINormalTreeItem->GetChildItem())
        SetMsgHandled(FALSE);

    m_pINormalTreeItem->GetIListCtrlBase()->ToggleItemExpand(
        m_pINormalTreeItem, false);
    m_pINormalTreeItem->GetIListCtrlBase()->Invalidate();
}

void  NormalTreeItem::GetItemExpandIconRect(CRect* prc)
{
    CRect rcParent;
    m_pINormalTreeItem->GetParentRect(&rcParent);
    prc->CopyRect(&rcParent);
    prc->left = 
        m_pINormalTreeItem->GetIListCtrlBase()->GetChildNodeIndent() * 
        m_pINormalTreeItem->CalcDepth();

    prc->right = prc->left + m_pShareData->nExpandIconWidth;
}

void  NormalTreeItem::GetDesiredSize(SIZE* pSize)
{
    IListCtrlBase*  pListCtrl = m_pINormalTreeItem->GetIListCtrlBase();
    ITextRenderBase* pTextRender = pListCtrl->GetTextRender();
    if (!pTextRender)
        pTextRender = pListCtrl->CreateDefaultTextRender();
    if (pTextRender)
    {
        *pSize = pTextRender->GetDesiredSize(m_pINormalTreeItem->GetText());
    }

    pSize->cx += 
        pListCtrl->GetChildNodeIndent() * 
        m_pINormalTreeItem->CalcDepth();
    pSize->cx += m_pShareData->nExpandIconWidth;

    if (m_pINormalTreeItem->GetIconRender())
    {
        pSize->cx += m_pShareData->nImageWidth;
    }
}
}
