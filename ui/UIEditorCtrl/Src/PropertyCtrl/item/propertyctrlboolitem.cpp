#include "stdafx.h"
#include "PropertyCtrlBoolItem.h"
using namespace UI;

PropertyCtrlBoolItem::PropertyCtrlBoolItem(IPropertyCtrlBoolItem* p)
    : PropertyCtrlComboBoxItem(p)
{
    m_pIPropertyCtrlBoolItem = p;

    AddOption(XML_BOOL_VALUE_TRUE, 1);
    AddOption(XML_BOOL_VALUE_FALSE, 0);
    m_eItemValueTpye = ComboBoxItemValueLong;
	m_bReadOnly = true;
}

void  PropertyCtrlBoolItem::SetBool(bool b)
{
    SetValueLong(b?1:0);
}

void  PropertyCtrlBoolItem::SetDefault(bool b)
{
    m_lDefualtValue = b?1:0;
}

bool  PropertyCtrlBoolItem::GetBool()
{
    return m_lValue?true:false;
}

void  PropertyCtrlBoolItem::OnLButtonDblClk(UINT nFlags, POINT point)
{
    SetMsgHandled(FALSE);
    SetBool(!GetBool());

    __super::FireValueChanged();
}