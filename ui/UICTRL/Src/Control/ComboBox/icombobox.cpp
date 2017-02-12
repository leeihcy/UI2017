#include "stdafx.h"
#include "combobox.h"

using namespace UI;

UI_IMPLEMENT_INTERFACE_ACROSSMODULE(ComboBoxBase, Control)
UI_IMPLEMENT_INTERFACE(ComboBox, ComboBoxBase)

LPCTSTR  IComboBoxBase::GetText()
{
	return __pImpl->GetText(); 
}
void  IComboBoxBase::SetText(LPCTSTR szText)
{
	__pImpl->SetText(szText);
}
IObject*  IComboBoxBase::GetContentObject(LPCTSTR szText)
{
	return __pImpl->GetContentObject(szText);
}

void  IComboBoxBase::CloseUp() 
{ 
	__pImpl->CloseUp(); 
}
IEdit*  IComboBoxBase::GetEdit() 
{ 
	return __pImpl->GetEdit();
}

bool  IComboBox::AddString(LPCTSTR szText)
{
    return __pImpl->AddString(szText);
}
IListBoxItem*  IComboBox::AddStringEx(LPCTSTR szText)
{
    return __pImpl->AddStringEx(szText);
}
bool  IComboBox::SetCurSel(int nIndex)
{
    return __pImpl->SetCurSel(nIndex);
}
IListBox*  IComboBox::GetListBox()
{
    return __pImpl->GetListBox();
}
void  IComboBox::ResetContent()
{
	return __pImpl->ResetContent();
}

int  IComboBox::GetCurSel()
{
    return __pImpl->GetCurSel();
}

void  IComboBox::SelectItem(UI::IListItemBase* pItem, bool bNotify)
{
    __pImpl->SelectItem(pItem, bNotify);
}

void  IComboBox::SetReadOnly(bool b)
{
    __pImpl->SetReadOnly(b);
}
bool  IComboBox::IsReadOnly()
{   
    return __pImpl->IsReadOnly();
}

IEdit*  IComboBox::GetEdit()
{
	return __pImpl->GetEdit();
}

signal<IComboBox*>&  IComboBox::SelectChangedEvent()
{
    return __pImpl->select_changed;
}
#if _MSC_VER >= 1800
signal_r<bool, IComboBoxBase*, UINT>&  IComboBoxBase::KeyDownEvent()
{
    return __pImpl->keydown;
}
#else
signal_r2<bool, IComboBoxBase*, UINT>&  IComboBoxBase::KeyDownEvent()
{
	return __pImpl->keydown;
}
#endif