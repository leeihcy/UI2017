#include "stdafx.h"
#include "stringlistitem.h"

namespace UI
{
StringListItem::StringListItem(IStringListItem* p):MessageProxy(p)
{
    m_pIStringListItem = p;
    m_pShareData = NULL;
}
StringListItem::~StringListItem()
{

}

void  StringListItem::OnInitialize()
{
    DO_PARENT_PROCESS(IStringListItem, IListItemBase);
    m_pIStringListItem->SetItemType(LISTITEM_TYPE_STRING);

    IStringListItemShareData* p = static_cast<IStringListItemShareData*>(
        m_pIStringListItem->GetIListCtrlBase()->GetItemTypeShareData(LISTITEM_TYPE_STRING));
    if (NULL == p)
    {
         p = IStringListItemShareData::CreateInstance(
				m_pIStringListItem->GetIListCtrlBase()->GetSkinRes());
        m_pIStringListItem->GetIListCtrlBase()->SetItemTypeShareData(LISTITEM_TYPE_STRING, p);
    }
    m_pShareData = p->GetImpl();

}


void  StringListItem::GetDesiredSize(SIZE* pSize)
{
    pSize->cx = pSize->cy = 0;

    ITextRenderBase* pTextRender = m_pIStringListItem->GetIListCtrlBase()->GetTextRender();
    if (pTextRender)
    {
        *pSize = pTextRender->GetDesiredSize(m_pIStringListItem->GetText());
    }
    pSize->cx += m_pShareData->m_rcItemPadding.left + m_pShareData->m_rcItemPadding.right;
    pSize->cy += m_pShareData->m_rcItemPadding.top + m_pShareData->m_rcItemPadding.bottom;
}

void  StringListItem::OnPaint(IRenderTarget* pRenderTarget)
{
    IListCtrlBase*  pListCtrl = m_pIStringListItem->GetIListCtrlBase();

    CRect  rcParent;
    m_pIStringListItem->GetParentRect(&rcParent);

    UINT nRenderState = 0;
    IRenderBase* pRender = pListCtrl->GetForeRender();
    if (pRender)
    {
        nRenderState = m_pIStringListItem->GetItemRenderState();
        pRender->DrawState(pRenderTarget, &rcParent, nRenderState);
    }

    CRect rcText = rcParent;
    ITextRenderBase* pTextRender = pListCtrl->GetTextRender();
    if (!pTextRender)
        pTextRender = pListCtrl->CreateDefaultTextRender();
    if (pTextRender)
    {
        rcText.DeflateRect(m_pShareData->m_rcItemPadding);
        pTextRender->DrawState(pRenderTarget, &rcText, nRenderState, m_pIStringListItem->GetText());
    }
}

}