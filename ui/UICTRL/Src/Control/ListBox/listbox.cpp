#include "stdafx.h"
#include "listbox.h"
#include "listboxitem.h"
#include "listbox_desc.h"

namespace UI
{

bool ListBoxCompareProc(IListItemBase* p1, IListItemBase* p2);

ListBox::ListBox(IListBox* p):MessageProxy(p)
{
    m_pIListBox = p;
    m_pBindObject = NULL;

	m_pUIEdit = nullptr;
	m_pEditingItem = nullptr;

	p->SetDescription(ListBoxDescription::Get());
}
ListBox::~ListBox()
{
}

void  ListBox::OnInitialize()
{
	DO_PARENT_PROCESS(IListBox, IListCtrlBase);
	m_pIListBox->SetSortCompareProc(ListBoxCompareProc);
	m_vscrollbarCreator.Initialize(m_pIListBox, m_pIListBox->GetIScrollBarMgr());

	GetEditUIRes();

#ifdef _DEBUG
    if (m_pIListBox->GetUIApplication()->IsEditorMode())
    {
        if (!m_pIListBox->GetFirstItem())
        {
            m_pIListBox->AddString(TEXT("1234567890"));
            m_pIListBox->AddString(TEXT("ABCDEFGHIJ"));
            m_pIListBox->AddString(TEXT("abcdefghij"));
        }
    }
#endif
}

void  ListBox::OnCreateByEditor(CREATEBYEDITORDATA* pData)
{
	DO_PARENT_PROCESS_MAPID(IListBox, IListCtrlBase, UIALT_CALLLESS);
	m_vscrollbarCreator.CreateByEditor(pData, m_pIListBox);

	AddEditUIRes(pData);
}

void ListBox::SetBindObject(IObject* pCombobox)
{
    m_pBindObject = pCombobox;
}

bool  ListBoxCompareProc(IListItemBase* pItem1, IListItemBase* pItem2)
{
    if (pItem1->GetSort() == pItem2->GetSort())
    {
        return (_tcscoll(pItem1->GetText(), pItem2->GetText()) < 0);
    }
    return pItem1->GetSort() < pItem2->GetSort();
}


bool ListBox::SetSel(int nIndex)
{
	IListItemBase* pOldSelItem = m_pIListBox->GetFirstSelectItem();

	// 清除当前选项
	if (-1 == nIndex && NULL != pOldSelItem)
	{
		m_pIListBox->SelectItem(NULL, false);
	}
	else
	{
		IListItemBase* pItem = m_pIListBox->GetItemByPos(nIndex);
		if (false == pItem)
			return false;

		m_pIListBox->SelectItem(pItem, false);
	}

	return true;
}

int   ListBox::GetSel()
{
    IListItemBase* pSelItem = m_pIListBox->GetFirstSelectItem();
    if (nullptr == pSelItem)
        return -1;

    return m_pIListBox->GetItemPos(pSelItem);
}

bool  ListBox::IsPopup()
{
	LISTCTRLSTYLE s = {0};
	s.popuplistbox = 1;
    return m_pIListBox->TestListCtrlStyle(&s);
}

void  ListBox::OnSerialize(SERIALIZEDATA* pData)
{
    DO_PARENT_PROCESS(IListBox, IListCtrlBase);

#if 0
    IUIApplication* pUIApplication = m_pIListBox->GetUIApplication();
    IScrollBarManager*  pScrollBarMgr = m_pIListBox->GetIScrollBarMgr();
    if (pScrollBarMgr)
    {
        if (!pScrollBarMgr->GetHScrollBar() && pScrollBarMgr->GetScrollBarVisibleType(HSCROLLBAR) != SCROLLBAR_VISIBLE_NONE)
        {
            ISystemHScrollBar*  p = NULL;
            ISystemHScrollBar::CreateInstance(pUIApplication, &p);
            p->SetIScrollBarMgr(pScrollBarMgr);
            m_pIListBox->AddNcChild(p);
            p->SetId(XML_HSCROLLBAR_PRIFIX);
            
            p->SetAttributeByPrefix(XML_HSCROLLBAR_PRIFIX, pMapAttrib, bReload, true);
            p->SetVisible(false, false, false);
        }

        if (!pScrollBarMgr->GetVScrollBar() && pScrollBarMgr->GetScrollBarVisibleType(VSCROLLBAR) != SCROLLBAR_VISIBLE_NONE)
        {
            ISystemVScrollBar*  p = NULL;
            ISystemVScrollBar::CreateInstance(pUIApplication, &p);
            p->SetIScrollBarMgr(pScrollBarMgr);
            m_pIListBox->AddNcChild(p);
            p->SetId(XML_VSCROLLBAR_PRIFIX);

            p->SetAttributeByPrefix(XML_VSCROLLBAR_PRIFIX, pMapAttrib, bReload, true);
            p->SetVisible(false, false, false);
            pScrollBarMgr->SetVScrollWheel(20);
        }
    }
#endif
}

IListBoxItem*  ListBox::AddString(LPCTSTR szText)
{
    IListBoxItem* pItem = IListBoxItem::CreateInstance(m_pIListBox->GetSkinRes());

    if (NULL == pItem)
        return NULL;

    if (szText)
        pItem->SetText(szText);

    m_pIListBox->AddItem(pItem);
    return pItem;
}
bool ListBox::RemoveItem(IListBoxItem* pItem)
{
    if (NULL == pItem)
        return false;

    m_pIListBox->RemoveItem(pItem);
    return true;
}

LRESULT ListBox::OnMouseWheel(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	// 编辑过程中，不支持mousewheel。edit位置不好跟随
	if (IsEditing())
	{
		return 0;
	}

	SetMsgHandled(FALSE);
	return 0;
}
}
