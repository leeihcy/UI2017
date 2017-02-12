#pragma once
#include "propertyctrlcomboboxitem.h"

namespace UI
{

class PropertyCtrlBoolItem : public PropertyCtrlComboBoxItem
{
public:
    PropertyCtrlBoolItem(IPropertyCtrlBoolItem* p);

    UI_BEGIN_MSG_MAP()
        MSG_WM_LBUTTONDBLCLK(OnLButtonDblClk)
        UIMSG_QUERYINTERFACE(PropertyCtrlBoolItem)
    UI_END_MSG_MAP_CHAIN_PARENT(PropertyCtrlComboBoxItem)   

public:

    void  SetBool(bool b);
    void  SetDefault(bool b);
    bool  GetBool();

private:
    void  OnLButtonDblClk(UINT nFlags, POINT point);

private:
    IPropertyCtrlBoolItem*  m_pIPropertyCtrlBoolItem;

};

}