#include "stdafx.h"
#include "listbox.h"
#include "listboxitem.h"

namespace UI
{
UI_IMPLEMENT_INTERFACE_ACROSSMODULE(ListBox, ListCtrlBase)
UI_IMPLEMENT_INTERFACE_ACROSSMODULE(ListBoxItem, ListItemBase)
UI_IMPLEMENT_INTERFACE_ACROSSMODULE(ListBoxItemShareData, ListItemShareData)

void  IListBox::SetBindObject(IObject* pCombobox)
{
    __pImpl->SetBindObject(pCombobox);
}

IListBoxItem*  IListBox::AddString(LPCTSTR szText)
{
    return __pImpl->AddString(szText);
}

void  IListBox::StartEdit(IListItemBase* pEditItem)
{
	__pImpl->StartEdit(pEditItem);
}
void  IListBox::DiscardEdit()
{
	__pImpl->DiscardEdit();
}

bool  IListBox::SetSel(int nIndex)
{
    return __pImpl->SetSel(nIndex);
}
int  IListBox::GetSel()
{
    return __pImpl->GetSel();
}


void  IListBoxItem::SetStringData(LPCSTR data)
{
	__pImpl->SetStringData(data);
}
LPCSTR  IListBoxItem::GetStringData()
{
	return __pImpl->GetStringData();
}

}