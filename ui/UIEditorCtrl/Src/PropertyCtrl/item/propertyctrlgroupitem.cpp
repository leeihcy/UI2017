#include "stdafx.h"
#include "propertyctrlgroupitem.h"


namespace UI
{

PropertyCtrlGroupItem::PropertyCtrlGroupItem(IPropertyCtrlGroupItem* p)
    : MessageProxy(p)
{
    m_pIPropertyCtrlGroupItem = p;
	m_pShareData = NULL;
}
PropertyCtrlGroupItem::~PropertyCtrlGroupItem()
{

}

void  PropertyCtrlGroupItem::OnInitialize()
{
	DO_PARENT_PROCESS(IPropertyCtrlGroupItem, INormalTreeItem);

	m_pShareData = (INormalTreeItemShareData*)
		m_pIPropertyCtrlGroupItem->GetIListCtrlBase()->
		GetItemTypeShareData(LISTITEM_TYPE_TREEITEM_NORMAL);

    m_pIPropertyCtrlGroupItem->SetSelectable(false);
    m_pIPropertyCtrlGroupItem->SetItemFlag(IPropertyCtrlGroupItem::FLAG);
}
void  PropertyCtrlGroupItem::OnLButtonDown(UINT nFlags, POINT point)
{
    IListCtrlBase*  pCtrl = m_pIPropertyCtrlGroupItem->GetIListCtrlBase();
    pCtrl->ToggleItemExpand(m_pIPropertyCtrlGroupItem, false);
    m_pIPropertyCtrlGroupItem->GetIListCtrlBase()->Invalidate();
}

void  PropertyCtrlGroupItem::OnPaint(IRenderTarget* pRenderTarget)
{
    IListCtrlBase*  pListCtrl = m_pIPropertyCtrlGroupItem->GetIListCtrlBase();

    // 背景
    Color c(PROPERTYCTRL_LINE_COLOR); 
    CRect rcParent;
    m_pIPropertyCtrlGroupItem->GetParentRect(&rcParent);
    pRenderTarget->DrawRect(&rcParent, &c);

    CRect rcItem = rcParent;
    int nDepth = m_pIPropertyCtrlGroupItem->CalcDepth();
    if (nDepth > 1)
        nDepth--;
    rcItem.left += pListCtrl->GetChildNodeIndent() * nDepth;

    // 绘制展开收缩图标
    if (m_pShareData->GetExpandIconWidth())
    {
        CRect rcExpandIcon = rcItem;
        rcExpandIcon.right = rcItem.left + m_pShareData->GetExpandIconWidth();

        if (m_pShareData->GetExpandIconRender() && m_pIPropertyCtrlGroupItem->GetChildItem())
        {
            if (m_pIPropertyCtrlGroupItem->IsExpand())
                m_pShareData->GetExpandIconRender()->DrawState(pRenderTarget, &rcExpandIcon, TREEVIEWCTRL_EXPANDCOLLAPSEICON_RENDER_STATE_EXPAND_NORMAL);
            else
                m_pShareData->GetExpandIconRender()->DrawState(pRenderTarget, &rcExpandIcon, TREEVIEWCTRL_EXPANDCOLLAPSEICON_RENDER_STATE_COLLAPSE_NORMAL);
        }

        rcItem.left = rcExpandIcon.right;
    }

    // 绘制文本
    ITextRenderBase*  pTextRender = pListCtrl->GetTextRender();
    if (pTextRender)
        pTextRender->DrawState(pRenderTarget, &rcItem, 0, m_pIPropertyCtrlGroupItem->GetText());
}

}