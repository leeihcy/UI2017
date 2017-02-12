#include "stdafx.h"
#include "propertyctrl.h"
#include "item\propertyctrlitembase.h"
#include "item\propertyctrlgroupitem.h"
#include "item\propertyctrledititem.h"
#include "item\propertyctrlcomboboxitem.h"
#include "item\propertyctrlbuttonitem.h"
#include "item\propertyctrllongitem.h"
#include "item\propertyctrlboolitem.h"
#include "item\propertyctrladditionitem.h"
#include "UIEditorCtrl\Inc\ipropertyctrl.h"

using namespace UI;


UI_IMPLEMENT_INTERFACE_ACROSSMODULE(PropertyCtrl, TreeView);
UI_IMPLEMENT_INTERFACE_ACROSSMODULE(PropertyCtrlGroupItem, NormalTreeItem);

UI_IMPLEMENT_INTERFACE_ACROSSMODULE(PropertyCtrlItemBase, ListItemBase);
UI_IMPLEMENT_INTERFACE_ACROSSMODULE(PropertyCtrlItemBaseShareData, ListItemShareData);

UI_IMPLEMENT_INTERFACE(PropertyCtrlEditItem, PropertyCtrlItemBase);
UI_IMPLEMENT_INTERFACE(PropertyCtrlAdditionItem, PropertyCtrlItemBase);
UI_IMPLEMENT_INTERFACE(PropertyCtrlComboBoxItem, PropertyCtrlItemBase);
UI_IMPLEMENT_INTERFACE(PropertyCtrlBoolItem, PropertyCtrlComboBoxItem);
UI_IMPLEMENT_INTERFACE(PropertyCtrlLongItem, PropertyCtrlEditItem);
UI_IMPLEMENT_INTERFACE(PropertyCtrlButtonItem, PropertyCtrlItemBase);

IPropertyCtrlGroupItem*  IPropertyCtrl::InsertGroupItem(
        LPCTSTR szName, LPCTSTR szDesc, 
        IListItemBase* pParent, IListItemBase* pInsertAfter)
{
    return __pImpl->InsertGroupItem(
        szName, szDesc, pParent, pInsertAfter);
}

IPropertyCtrlGroupItem*  IPropertyCtrl::FindGroupItem(
    IListItemBase* pParentItem, LPCTSTR szName)
{
    return __pImpl->FindGroupItem(pParentItem, szName);
}
IListItemBase*  IPropertyCtrl::FindItemByKey(LPCTSTR szKey)
{
    return __pImpl->FindItemByKey(szKey);
}

IPropertyCtrlEditItem*   IPropertyCtrl::InsertEditProperty(
        PropertyCtrlEditItemInfo* pInfo,
        IListItemBase* pParentItem, IListItemBase* pInsertAfter)
{
    return __pImpl->InsertEditProperty(
        pInfo, pParentItem, pInsertAfter);
}

IPropertyCtrlLongItem*   IPropertyCtrl::InsertLongProperty(
        PropertyCtrlLongItemInfo* pInfo, 
        IListItemBase* pParentItem, IListItemBase* pInsertAfter)
{
    return __pImpl->InsertLongProperty(
            pInfo, pParentItem, pInsertAfter);
}

IPropertyCtrlBoolItem*   IPropertyCtrl::InsertBoolProperty(
        PropertyCtrlBoolItemInfo* pInfo,
		IListItemBase* pParentItem, IListItemBase* pInsertAfter)
{
    return __pImpl->InsertBoolProperty(pInfo, pParentItem, pInsertAfter);
}

IPropertyCtrlComboBoxItem*   IPropertyCtrl::InsertComboBoxProperty(
        PropertyCtrlComboBoxItemInfo* pInfo,
		IListItemBase* pParentItem, IListItemBase* pInsertAfter)
{
	return __pImpl->InsertComboBoxProperty(pInfo, pParentItem, pInsertAfter);
}

IPropertyCtrlButtonItem*   IPropertyCtrl::InsertButtonProperty(
        LPCTSTR szText, LPCTSTR szValue, LPCTSTR szDesc, LPCTSTR szKey,
		IListItemBase* pParentItem, IListItemBase* pInsertAfter)
{
	return __pImpl->InsertButtonProperty(
        szText, szValue, szDesc, szKey, pParentItem, pInsertAfter);
}

IPropertyCtrlAdditionItem*  IPropertyCtrl::AddAdditionItem()
{
	return __pImpl->AddAdditionItem();
}

IPropertyCtrlEditItem*  IPropertyCtrl::FindEditItem(LPCTSTR szKey)
{
	return __pImpl->FindEditItem(szKey);
}
IPropertyCtrlLongItem*  IPropertyCtrl::FindLongItem(LPCTSTR szKey)
{
    return __pImpl->FindLongItem(szKey);
}
IPropertyCtrlComboBoxItem*  IPropertyCtrl::FindComboBoxItem(LPCTSTR szKey)
{
    return __pImpl->FindComboBoxItem(szKey);
}
IPropertyCtrlBoolItem*  IPropertyCtrl::FindBoolItem(LPCTSTR szKey)
{
	return __pImpl->FindBoolItem(szKey);
}
//////////////////////////////////////////////////////////////////////////

LPCTSTR  IPropertyCtrlItemBase::GetKey()
{
    return __pImpl->GetKey();
}

void  IPropertyCtrlEditItem::SetValueText(LPCTSTR szText)   
{
    __pImpl->SetValueText(szText);
}
void  IPropertyCtrlEditItem::SetDefaultValueText(LPCTSTR szText) 
{
    __pImpl->SetDefaultValueText(szText);
}

void  IPropertyCtrlComboBoxItem::AddOption(LPCTSTR szText, long lValue) 
{
    __pImpl->AddOption(szText, lValue);
}
void  IPropertyCtrlComboBoxItem::SetValueString(LPCTSTR szText)
{
    __pImpl->SetValueString(szText);
}
void  IPropertyCtrlComboBoxItem::SetValueLong(long lValue)
{
    __pImpl->SetValueLong(lValue);
}

void  IPropertyCtrlButtonItem::SetValueText(LPCTSTR szText)   
{
    __pImpl->SetValueText(szText);
}
void  IPropertyCtrlButtonItem::SetDefaultValueText(LPCTSTR szText) 
{
    __pImpl->SetDefaultValueText(szText);
}

void  IPropertyCtrlLongItem::SetValue(long lValue)
{
    __pImpl->SetValue(lValue);
}
void  IPropertyCtrlBoolItem::SetBool(bool b)
{
    __pImpl->SetBool(b);
}