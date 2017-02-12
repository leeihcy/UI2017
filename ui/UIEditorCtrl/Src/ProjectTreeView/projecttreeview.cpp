#include "stdafx.h"
#include "projecttreeview.h"
#include "..\UISDK\Inc\Interface\iwindow.h"
#include "projecttreeview_desc.h"

namespace UI
{
ProjectTreeView::ProjectTreeView(IProjectTreeView* p) : MessageProxy(p)
{
    m_pIProjectTreeView = p;
    m_pSkinItemText = NULL;
    m_pDragDropHitItem = NULL;
    p->SetDescription(ProjectTreeViewDescription::Get());
}
ProjectTreeView::~ProjectTreeView()
{
    SAFE_RELEASE(m_pSkinItemText);
}

void  ProjectTreeView::OnInitialize()
{
	DO_PARENT_PROCESS(IProjectTreeView, ITreeView);

    // 允许拖拽控件
    m_pIProjectTreeView->GetWindowObject()->SetDroppable(true);

	OBJSTYLE s = {0};
	s.receive_dragdrop_event = 1;
    m_pIProjectTreeView->ModifyObjectStyle(&s, 0);
}

void  ProjectTreeView::SetAttribute(IMapAttribute* pMapAttr, bool bReload)
{
    DO_PARENT_PROCESS(IProjectTreeView, ITreeView);

    // 获取skinitem的文本渲染器
//     pMapAttr->GetAttr_TextRenderBase(
//         XML_PROJECT_TREEVIEW_SKINITEM_TEXT_PREFIX,
//         XML_TEXTRENDER_TYPE,
//         true, 
//         m_pIProjectTreeView->GetUIApplication(), 
//         m_pIProjectTreeView, 
//         &m_pSkinItemText);
}

ISkinTreeViewItem*  ProjectTreeView::InsertSkinItem(
        const TCHAR* szText,
        IListItemBase* pParent,
        IListItemBase* pInsertAfter)
{
    ISkinTreeViewItem*  pNewItem = ISkinTreeViewItem::CreateInstance(
        m_pIProjectTreeView->GetSkinRes());

    pNewItem->SetText(szText);
    pNewItem->SetTextRender(&m_pSkinItemText);

    bool bRet = m_pIProjectTreeView->InsertItem(
            pNewItem, pParent, pInsertAfter);
    if (false == bRet)
    {
        SAFE_RELEASE(pNewItem);
        return NULL;
    }

    return pNewItem;
}

IControlTreeViewItem*  ProjectTreeView::InsertControlItem(
        const TCHAR* szText, 
        IListItemBase* pParent, 
        IListItemBase* pInsertAfter)
{
    IControlTreeViewItem*  pNewItem = IControlTreeViewItem::CreateInstance(
        m_pIProjectTreeView->GetSkinRes());
    pNewItem->SetText(szText);

    bool bRet = m_pIProjectTreeView->InsertItem(pNewItem, pParent, pInsertAfter);
    if (false == bRet)
    {
        SAFE_RELEASE(pNewItem);
        return NULL;
    }

    return pNewItem;
}

void ProjectTreeView::OnDropTargetEvent(DROPTARGETEVENT_TYPE eType, DROPTARGETEVENT_DATA* pData)
{
    UIMSG  msg;
    msg.message = UI_MSG_NOTIFY;
    msg.nCode = UI_PROJECTTREEVIEW_NOTIFY_CONTROLITEM_DROPEVENT;
    msg.pMsgFrom = static_cast<UI::IMessage*>(m_pIProjectTreeView);

    ProjectTreeViewControlItemDropData  data;
    data.eType = eType;
    data.pData = pData;
    msg.wParam = (WPARAM)&data;

    if (eType != _DragLeave)
    {
        m_pIProjectTreeView->ListCtrlDragScroll();
    }

    UINT  nControlItemDragCF = GetProjectTreeControlDragClipboardFormat();  // 拖拽格式
    switch (eType)
    {
    case _Drop:
        {
            if (NULL == pData->pDataObj)
                break;

            m_pIProjectTreeView->ListItemDragDropEvent(
                eType, m_pDragDropHitItem);

            // 先保存
            data.pHitItem = m_pDragDropHitItem;

            if (m_pDragDropHitItem)
            {
                m_pIProjectTreeView->InvalidateItem(m_pDragDropHitItem);
                m_pDragDropHitItem = NULL;
            }
        }
        break;

    case _DragOver:
    case _DragEnter:
        {
            (*pData->pdwEffect) = DROPEFFECT_NONE;
            if (NULL == pData->pDataObj)
                break;

            FORMATETC etc = 
            {
                nControlItemDragCF,
                NULL, DVASPECT_CONTENT, 0, TYMED_HGLOBAL
            };

            if (FAILED(pData->pDataObj->QueryGetData(&etc)))
                break;

            (*pData->pdwEffect) = DROPEFFECT_MOVE;

            POINT  pt = {pData->pt.x, pData->pt.y};
            HWND  hWnd = m_pIProjectTreeView->GetHWND();
            MapWindowPoints(NULL, hWnd, &pt, 1);

            IListItemBase*  pSelectItem = m_pIProjectTreeView->GetFirstSelectItem();

            IListItemBase*  pHitItem = m_pIProjectTreeView->GetItemByWindowPoint(pt);
            if (!pHitItem || pSelectItem == pHitItem)
            {
                (*pData->pdwEffect) = DROPEFFECT_NONE;
                pHitItem = NULL;
            }

            m_pIProjectTreeView->ListItemDragDropEvent(
                eType, m_pDragDropHitItem);

            if (m_pDragDropHitItem != pHitItem)
            {
                m_pDragDropHitItem = pHitItem;
            }

            if (NULL == m_pDragDropHitItem)
                return;

            data.pHitItem = m_pDragDropHitItem;
        }
        break;

    case _DragLeave:
        {
            m_pIProjectTreeView->ListItemDragDropEvent(
                eType, nullptr);

            if (m_pDragDropHitItem)
            {
                m_pIProjectTreeView->InvalidateItem(m_pDragDropHitItem);
                m_pDragDropHitItem = NULL;
            }

            data.pHitItem = m_pDragDropHitItem;
        }
        break;
    }

    m_pIProjectTreeView->DoNotify(&msg);
}

LRESULT  ProjectTreeView::OnDrawItem(WPARAM w, LPARAM l)
{
    IRenderTarget*  pRenderTarget = (IRenderTarget*)w;
     IListItemBase*  pItem = (IListItemBase*)l;

    if (pItem == m_pDragDropHitItem)
    {
        RECT  rc = { 0 };
        pItem->GetParentRect(&rc);

        Color c(184, 202, 220, 255);
        pRenderTarget->Rectangle(&rc, &c, NULL, 1, true);
    }

    return 0;
}
}