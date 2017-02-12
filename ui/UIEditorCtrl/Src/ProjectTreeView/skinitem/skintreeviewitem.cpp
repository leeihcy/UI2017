#include "stdafx.h"
#include "skintreeviewitem.h"

namespace UI
{
    SkinTreeViewItem::SkinTreeViewItem(ISkinTreeViewItem* p) : MessageProxy(p)
{
    m_pISkinTreeViewItem = p;
}
SkinTreeViewItem::~SkinTreeViewItem()
{

}

void  SkinTreeViewItem::SetTextRender(ITextRenderBase**  pTextRender)
{
    m_ppTextRender = pTextRender;
}

void  SkinTreeViewItem::OnPaint(IRenderTarget* pRenderTarget)
{
    if (!m_ppTextRender || !*m_ppTextRender)
    {
        SetMsgHandled(FALSE);
        return;
    }

    // Ìæ»»×ÖÌå
    ITextRenderBase*  pTextRender = *m_ppTextRender;
    IListCtrlBase*  pListCtrlBase = m_pISkinTreeViewItem->GetIListCtrlBase();
    ITextRenderBase*  pOldTextRender = pListCtrlBase->GetTextRender();

    UIASSERT(0);
//     pListCtrlBase->SetTextRender(pTextRender);
//     DO_PARENT_PROCESS(ISkinTreeViewItem, INormalTreeItem);
//     pListCtrlBase->SetTextRender(pOldTextRender);

    SAFE_RELEASE(pOldTextRender);
    
}

}