#include "stdafx.h"
#include "treeitem.h"

namespace UI
{

TreeItem::TreeItem(ITreeItem* p):MessageProxy(p)
{
    m_pITreeItem = p;
//    m_pCheckBtnExpandCollapse = NULL;
}
TreeItem::~TreeItem()
{
 //   SAFE_DELETE_Ixxx(m_pCheckBtnExpandCollapse);
}
// 创建内部控件
//    pNewItem->CreateRootPanel();

// 为父ITEM创建展开/收缩按钮
//     ITreeItemBase* pParentItem = pNewItem->GetParentItem();
//     BOOL isParentHasNoChild = (NULL != pParentItem && NULL == pParentItem->GetChildItem()->GetNextItem()); // 现在插入了一ITEM后至少有一个了
//     if (isParentHasNoChild)
//     {
//         pParentItem->CreateExpandCollapseCheckBtn();
//     }




LRESULT TreeItem::OnBtnExpandCollapse(BOOL& bHandled)
{
//    GetTreeCtrlPtr()->ToggleItemExpand(this, false);
    return 1;
}


// BOOL TreeItemBase::ProcessMessage(UIMSG* pMsg)
// {
//     BOOL bHandled = FALSE;
//     switch (pMsg->message)
//     {
//     case UI_WM_NOTIFY:
//         {
//             if (pMsg->code == UI_BN_CLICKED)
//             {
//                 if (pMsg->pObjMsgFrom == static_cast<Message*>(m_pCheckBtnExpandCollapse))
//                 {
//                     bHandled = TRUE;
//                     pMsg->lRet = OnBtnExpandCollapse(bHandled);
//                 }
//             }
//         }
//     }
// 
//     return bHandled;
// }



void TreeItem::CreateExpandCollapseCheckBtn()
{
//     CheckButton* pExpandBtn = NULL;
//     UICreateInstance(&pExpandBtn, m_pListCtrl->GetUIApplication());
// 
//     RenderBase* pRender = RenderFactory::GetRender(RENDER_TYPE_THEME_TREEVIEW_EXPANDCOLLAPSE_ICON, pExpandBtn);
//     if (NULL != pRender)
//     {
//         pExpandBtn->SetForegndRender(pRender);
//         SAFE_RELEASE(pRender); 
//     }
// 
//     ATTRMAP attrmap;
//     pExpandBtn->SetAttribute(attrmap, false);
//     pExpandBtn->SetConfigWidth(((TreeCtrlBase*)m_pListCtrl)->GetExpandIconWidth());
//     pExpandBtn->SetConfigHeight(m_pListCtrl->GetItemHeight());
//     pExpandBtn->SetConfigLeft(CalcDepth() * ((TreeCtrlBase*)m_pListCtrl)->GetChildNodeIndent());
//     pExpandBtn->ModifyStyle(OBJECT_STYLE_TRANSPARENT,0,false);
//     pExpandBtn->SetNotify(m_pListCtrl, LISTCTRLBASE_INNER_CONTROL_MSG_ID);
//     pExpandBtn->SetDrawFocusType(BUTTON_RENDER_DRAW_FOCUS_TYPE_NONE);
// 
//     m_pPanelRoot->AddChild(pExpandBtn);
// 
//     m_pCheckBtnExpandCollapse = pExpandBtn;
//     m_pCheckBtnExpandCollapse->SetOutRef((Object**)&m_pCheckBtnExpandCollapse);
}

}