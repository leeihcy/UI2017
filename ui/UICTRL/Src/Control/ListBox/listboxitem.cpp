#include "stdafx.h"
#include "listboxitem.h"
#include "listbox.h"

namespace UI
{

ListBoxItemShareData::ListBoxItemShareData(IListBoxItemShareData* p):MessageProxy(p)
{
    m_pIListBoxItemShareData = p;
}

ListBoxItem::ListBoxItem(IListBoxItem* p):MessageProxy(p)
{
    m_pIListBoxItem = p; 
    m_pShareData = NULL; 
	p->SetItemFlag(IListBoxItem::FLAG);
}

void  ListBoxItem::OnInitialize()
{
    DO_PARENT_PROCESS(IListBoxItem, IListItemBase);

	m_pIListBoxItem->SetItemType(IListBoxItem::FLAG);

    IListCtrlBase*  pListCtrl = m_pIListBoxItem->GetIListCtrlBase();
    IListBoxItemShareData*  pData = static_cast<IListBoxItemShareData*>(
        pListCtrl->GetItemTypeShareData(IListBoxItem::FLAG));

    if (!pData)
    {
        pData = IListBoxItemShareData::CreateInstance(pListCtrl->GetSkinRes());
        m_pShareData = pData->GetImpl();
        pListCtrl->SetItemTypeShareData(IListBoxItem::FLAG, pData);
    }
    else
    {
        m_pShareData = pData->GetImpl();
    }
}

void ListBoxItem::OnLButtonDown(UINT nFlags, POINT point)
{
    // 覆盖listitembase的OnLButtonDown，仅在鼠标弹起时选择
	LISTCTRLSTYLE s = {0};
	s.popuplistbox = 1;
    if (m_pIListBoxItem->GetIListCtrlBase()->TestListCtrlStyle(&s)) 
        return;
    
    SetMsgHandled(FALSE);
}

void ListBoxItem::OnLButtonUp(UINT nFlags, POINT point)
{
    // popuplistctrlmousemgr处理了，这里不用再处理
//     IListCtrlBase*  pListCtrl = m_pIListBoxItem->GetIListCtrlBase();
//     if (pListCtrl->TestStyleEx(LISTCTRLBASE_STYLE_POPUPLISTBOX))    // 以最后鼠标弹出来的那项作为被选中的项
//     {
//         IListItemBase* pNewHover = pListCtrl->HitTest(point);  // 重新取hover对象。因为当鼠标移到listbox外面时，会仍然保留最后一个hover item
// 
//         int nUpdateFlag = 0;
//         if (pNewHover && pNewHover != pListCtrl->GetFirstSelectItem())
//             pListCtrl->SelectItem(pListCtrl->GetHoverItem(), false);  // 因为即将要关闭了，所以不刷新
//     }

    SetMsgHandled(FALSE);
}

void  ListBoxItem::OnPaint(IRenderTarget* pRenderTarget)
{
    IListCtrlBase*  pListCtrl = m_pIListBoxItem->GetIListCtrlBase();

    CRect  rcParent;
    m_pIListBoxItem->GetParentRect(&rcParent);

    UINT nRenderState = 0;
    IRenderBase* pRender = pListCtrl->GetForeRender();
    if (pRender)
    {
        nRenderState = m_pIListBoxItem->GetItemRenderState();
        pRender->DrawState(pRenderTarget, &rcParent, nRenderState);
    }

	REGION4 rItemPadding = {0};
	pListCtrl->GetItemContentPadding(&rItemPadding);

    CRect rcText = rcParent;
	rcText.DeflateRect(&rItemPadding);

    ITextRenderBase* pTextRender = pListCtrl->GetTextRender();
    if (!pTextRender)
        pTextRender = pListCtrl->CreateDefaultTextRender();
    if (pTextRender)
    {
        LPCTSTR szText = m_pIListBoxItem->GetText();
        pTextRender->DrawState(pRenderTarget, &rcText, nRenderState, szText);
    }
}


void ListBoxItem::SetStringData(LPCSTR data)
{
	if (data)
		m_strData = data;
	else
		m_strData.clear();
}

LPCSTR ListBoxItem::GetStringData()
{
	return m_strData.c_str();
}

}