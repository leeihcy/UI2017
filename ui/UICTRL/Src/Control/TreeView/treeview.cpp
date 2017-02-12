#include "stdafx.h"
#include "treeview.h"
#include "treeview_desc.h"

using namespace UI;

TreeView::TreeView(ITreeView* p):MessageProxy(p)
{
    m_pITreeView = p;
    m_pExpandIcon = NULL;
	p->SetDescription(TreeViewDescription::Get());
}
TreeView::~TreeView()
{
    SAFE_RELEASE(m_pExpandIcon);
}


HRESULT  TreeView::FinalConstruct(ISkinRes* p)
{
    DO_PARENT_PROCESS(ITreeView, IListCtrlBase);
    if (FAILED(GetCurMsg()->lRet))
        return GetCurMsg()->lRet;

    m_pITreeView->SetLayoutVariableHeight();
    return S_OK;
}

LPCTSTR  TreeView::SaveExpandIconRender()   
{ 
	// TODO:
// 	if (!m_pExpandIcon)
// 		return NULL;
// 
// 	IUIApplication*  pUIApp = m_pITreeView->GetUIApplication();
// 	return pUIApp->GetRenderBaseName(m_pExpandIcon->GetType());
	return NULL;
}
void  TreeView::LoadExpandIconRender(LPCTSTR szName)  
{ 
	// TODO:
// 	SAFE_RELEASE(m_pExpandIcon);
// 	if (szName)
// 	{
// 		IUIApplication*  pUIApp = m_pITreeView->GetUIApplication();
// 		pUIApp->CreateRenderBaseByName(szName, m_pITreeView, &m_pExpandIcon);
// 	}
}

void  TreeView::OnSerialize(SERIALIZEDATA* pData)
{
    IUIApplication*  pUIApp = m_pITreeView->GetUIApplication();

//     if (!pMapAttrib->HasAttrib(XML_TEXTRENDER_TYPE))
//     {
//         if (NULL == m_pITreeView->GetTextRender())
//         {
//             ITextRenderBase*  pTextRender = NULL;
//             pUIApp->CreateTextRenderBase(TEXTRENDER_TYPE_THEME_LISTVIEW, m_pITreeView, &pTextRender);
// 
//             if (pTextRender)
//             {
//                 UI::IMapAttribute*  pMapAttrib = NULL;
//                 UICreateIMapAttribute(&pMapAttrib);
// 
//                 SERIALIZEDATA data = {0};
//                 data.pUIApplication = pUIApp;
//                 data.pMapAttrib = pMapAttrib;
//                 data.szPrefix = NULL;
//                 data.nFlags = SERIALIZEFLAG_LOAD|SERIALIZEFLAG_LOAD_ERASEATTR;
//                 pTextRender->Serialize(&data);
// 
//                 SAFE_RELEASE(pMapAttrib);
// 
//                 pTextRender->SetTextAlignment(DT_SINGLELINE|DT_END_ELLIPSIS|DT_VCENTER|DT_NOPREFIX);
// 
//                 m_pITreeView->SetTextRender(pTextRender);
//                 SAFE_RELEASE(pTextRender);
//             }
//         }
//     }

    DO_PARENT_PROCESS(ITreeView, IListCtrlBase);

//     if (NULL == m_pITreeView->GetBkRender())
//     {
//         IRenderBase*  pRender = NULL;
//         pUIApp->CreateRenderBase(RENDER_TYPE_THEME_LISTBOX_BKGND, m_pITreeView, &pRender);
// 
//         if (pRender)
//         {
//             m_pITreeView->SetBkgndRender(pRender);
//             CRegion4 r(2,2,2,2);
//             m_pITreeView->SetBorderRegion(&r);
//             SAFE_RELEASE(pRender);
//         }
//     }

    // 	if (NULL == m_pForegndRender)
    // 	{
    // 		m_pForegndRender = RenderFactory::GetRender(RENDER_TYPE_COLORLIST, this);
    // 		ColorListRender* p = dynamic_cast<ColorListRender*>(m_pForegndRender);
    // 
    // 		p->SetCount(8);
    // 		p->SetStateColor(LISTCTRLITEM_FOREGND_RENDER_STATE_SELECTED_NORMAL,  RGB(51,153,255),true, 0,false);
    // 		p->SetStateColor(LISTCTRLITEM_FOREGND_RENDER_STATE_SELECTED_HOVER,   RGB(51,153,255),true, 0,false);
    // 		p->SetStateColor(LISTCTRLITEM_FOREGND_RENDER_STATE_SELECTED_PRESS,   RGB(51,153,255),true, 0,false);
    // 		p->SetStateColor(LISTCTRLITEM_FOREGND_RENDER_STATE_SELECTED_DISABLE, RGB(51,153,255),true, 0,false);
    // 	}

//     if (NULL == m_pITreeView->GetForeRender())
//     {
//         IRenderBase*  pRender = NULL;
//         pUIApp->CreateRenderBase(RENDER_TYPE_THEME_LISTVIEW_ITEM, m_pITreeView, &pRender);
// 
//         if (pRender)
//         {
//             m_pITreeView->SetForegndRender(pRender);
//             SAFE_RELEASE(pRender);
//         }
//     }

    {
	    AttributeSerializerWrap  as(pData, TEXT("TreeView"));
	    as.AddStringEnum(XML_EXPANDICON_RENDER_TYPE_PREFIX XML_RENDER_TYPE, this,
		    memfun_cast<pfnStringSetter>(&TreeView::LoadExpandIconRender),
		    memfun_cast<pfnStringGetter>(&TreeView::SaveExpandIconRender))
		    ->FillRenderBaseTypeData()
		    ->ReloadOnChanged();
    }

    if (m_pExpandIcon)
    {
        SERIALIZEDATA data(*pData);
        data.szParentKey = XML_EXPANDICON_RENDER_TYPE_PREFIX XML_RENDER_TYPE;
        data.szPrefix = XML_EXPANDICON_RENDER_TYPE_PREFIX;
        if (data.pUIApplication->IsEditorMode())
            data.SetErase(false);

        m_pExpandIcon->Serialize(&data);
    }
//     if (NULL == m_pExpandIcon)
//     {
//         pUIApp->CreateRenderBase(RENDER_TYPE_THEME_TREEVIEW_EXPANDCOLLAPSE_ICON, m_pITreeView, &m_pExpandIcon);
//     }
//     if (NULL == m_pITreeView->GetFocusRender())
//     {
//         IRenderBase*  pFocusRender = NULL;
//         pUIApp->CreateRenderBase(RENDER_TYPE_THEME_LISTVIEW_FOCUS, m_pITreeView, &pFocusRender);
//         if (pFocusRender)
//         {
//             m_pITreeView->SetFocusRender(pFocusRender);
//             SAFE_RELEASE(pFocusRender);
//         }
//     }

//     IScrollBarManager*  pScrollBarMgr = m_pITreeView->GetIScrollBarMgr();
//     if (pScrollBarMgr)
//     {
//         if (!pScrollBarMgr->GetHScrollBar() && 
// 			pScrollBarMgr->GetScrollBarVisibleType(HSCROLLBAR) != 
// 			SCROLLBAR_VISIBLE_NONE)
//         {
//             ISystemHScrollBar*  p = NULL;
//             ISystemHScrollBar::CreateInstance(pUIApp, &p);
//             p->SetIScrollBarMgr(pScrollBarMgr);
//             m_pITreeView->AddNcChild(p);
//             p->SetId(XML_HSCROLLBAR_PRIFIX);
// 
//             p->SetAttributeByPrefix(XML_HSCROLLBAR_PRIFIX, pMapAttrib, bReload, true);
//             p->SetVisible(false, false, false);
//         }
// 
//         if (!pScrollBarMgr->GetVScrollBar() &&
// 			pScrollBarMgr->GetScrollBarVisibleType(VSCROLLBAR) 
// 				!= SCROLLBAR_VISIBLE_NONE)
//         {
//             ISystemVScrollBar*  p = NULL;
//             ISystemVScrollBar::CreateInstance(pUIApp, &p);
//             p->SetIScrollBarMgr(pScrollBarMgr);
//             m_pITreeView->AddNcChild(p);
//             p->SetId(XML_VSCROLLBAR_PRIFIX);
// 
//             p->SetAttributeByPrefix(XML_VSCROLLBAR_PRIFIX, pMapAttrib, bReload, true);
//             p->SetVisible(false, false, false);
//             pScrollBarMgr->SetVScrollWheel(20);
//         }
//     }
}

void TreeView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
    BOOL bHandled = FALSE;
    DO_PARENT_PROCESS2(ITreeView, IListCtrlBase, bHandled);
    if (bHandled)
        return;

    if (VK_LEFT == nChar)
    {
        IListItemBase* pItem = m_pITreeView->GetFocusItem();
        if (NULL == pItem)
            return;

        // 如果是自己是展开的，则收缩
        if (pItem->HasChild() && pItem->IsExpand())
        {
            m_pITreeView->CollapseItem(pItem, false);
        }
        // 否则选中父结点
        else if (pItem->GetParentItem())
        {
            if (pItem->GetParentItem()->IsSelectable())
                m_pITreeView->SelectItem(pItem->GetParentItem(), false);
            else
                m_pITreeView->SetFocusItem(pItem->GetParentItem());
        }
    }
    else if (VK_RIGHT == nChar)
    {
        IListItemBase* pItem = m_pITreeView->GetFocusItem();
        if (NULL == pItem)
            return;

        // 如果是自己是收缩的，则展开
        if (pItem->HasChild() && pItem->IsCollapsed())
        {
            m_pITreeView->ExpandItem(pItem, false);
        }
        // 否则选中第一个子结点
        else if (NULL != pItem->GetChildItem())
        {
            m_pITreeView->SelectItem(pItem->GetChildItem(), false);
        }
    }
}


LRESULT  TreeView::OnGetExpandIconRender(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    IRenderBase** pp = (IRenderBase**)wParam;
    if (!pp)
        return 0;

    *pp = m_pExpandIcon;
    return 1;
}

ITreeItem*  TreeView::InsertNormalItem(
		UITVITEM* pItem, 
		IListItemBase* pParent, 
		IListItemBase* pInsertAfter)
{
    if (NULL == pItem)
        return NULL;

    INormalTreeItem*  pNewItem = INormalTreeItem::CreateInstance(m_pITreeView->GetSkinRes());

    if (pItem->mask & UITVIF_TEXT && NULL != pItem->pszText)
        pNewItem->SetText(pItem->pszText);
    if (pItem->mask & UITVIF_PARAM)
        pNewItem->SetData(pItem->pData);
    if (pItem->mask & UITVIF_IMAG)
        pNewItem->SetIconRender(pItem->pIconRender);

    bool bRet = m_pITreeView->InsertItem(
		pNewItem, pParent, pInsertAfter);

    if (false == bRet)
    {
        SAFE_RELEASE(pNewItem);
        return NULL;
    }
   
    return pNewItem;
}

ITreeItem*  TreeView::InsertNormalItem(
		LPCTSTR szText, 
		IListItemBase* pParent, 
		IListItemBase* pAfter)
{
    UITVITEM  item = {0};
    item.mask = UITVIF_TEXT;
    item.pszText = (TCHAR*)szText;

    return this->InsertNormalItem(&item, pParent, pAfter);
}

void  TreeView::OnInitialize()
{
	DO_PARENT_PROCESS(ITreeView, IListCtrlBase);
	m_vscrollbarCreator.Initialize(m_pITreeView, m_pITreeView->GetIScrollBarMgr());
	m_hscrollbarCreator.Initialize(m_pITreeView, m_pITreeView->GetIScrollBarMgr());
}
void  TreeView::OnCreateByEditor(CREATEBYEDITORDATA* pData)
{
	DO_PARENT_PROCESS_MAPID(ITreeView, IListCtrlBase, UIALT_CALLLESS);
	m_vscrollbarCreator.CreateByEditor(pData, m_pITreeView);
	m_hscrollbarCreator.CreateByEditor(pData, m_pITreeView);
}
