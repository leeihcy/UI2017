#include "stdafx.h"
#include "iconstringlistitem.h"

namespace UI
{
IconStringListItem::IconStringListItem(IIconStringListItem* p):MessageProxy(p)
{
    m_pIIconStringListItem = p;
    m_pShareData = NULL;
    m_pIconRender = NULL;
}
IconStringListItem::~IconStringListItem()
{
    SAFE_RELEASE(m_pIconRender);
}

void  IconStringListItem::OnInitialize()
{
    DO_PARENT_PROCESS(IIconStringListItem, IListItemBase);
    
    m_pIIconStringListItem->SetItemType(LISTITEM_TYPE_ICONSTRING);
    
    IListCtrlBase* pCtrl = m_pIIconStringListItem->GetIListCtrlBase();
    IIconStringListItemShareData* p = (IIconStringListItemShareData*)pCtrl->GetItemTypeShareData(LISTITEM_TYPE_ICONSTRING);

    if (!p)
    {
        p = IIconStringListItemShareData::CreateInstance(pCtrl->GetSkinRes());
        pCtrl->SetItemTypeShareData(LISTITEM_TYPE_ICONSTRING, p);
    }
    m_pShareData = p->GetImpl();
}


void  IconStringListItem::GetDesiredSize(SIZE* pSize)
{
    pSize->cx = pSize->cy = 0;

    ITextRenderBase* pTextRender = m_pIIconStringListItem->GetIListCtrlBase()->GetTextRender();
    if (pTextRender)
    {
        *pSize = pTextRender->GetDesiredSize(m_pIIconStringListItem->GetText());
    }
    pSize->cx += m_pShareData->m_rcItemPadding.left + m_pShareData->m_rcItemPadding.right;
    pSize->cy += m_pShareData->m_rcItemPadding.top + m_pShareData->m_rcItemPadding.bottom;
}

void  IconStringListItem::OnPaint(IRenderTarget* pRenderTarget)
{
    IListCtrlBase*  pListCtrl = m_pIIconStringListItem->GetIListCtrlBase();

    CRect  rcParent;
    m_pIIconStringListItem->GetParentRect(&rcParent);

    UINT nRenderState = 0;
    IRenderBase* pRender = pListCtrl->GetForeRender();
    if (pRender)
    {
        nRenderState = m_pIIconStringListItem->GetItemRenderState();
        pRender->DrawState(pRenderTarget, &rcParent, nRenderState);
    }

    CRect rcText = rcParent;
    ITextRenderBase* pTextRender = pListCtrl->GetTextRender();
    if (!pTextRender)
        pTextRender = pListCtrl->CreateDefaultTextRender();
    if (pTextRender)
    {
        rcText.DeflateRect(m_pShareData->m_rcItemPadding);
        pTextRender->DrawState(pRenderTarget, &rcText, nRenderState, m_pIIconStringListItem->GetText());
    }
}

}