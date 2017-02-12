#pragma once
#include "UISDK\Inc\Interface\ilistitembase.h"
#include "UICTRL\Inc\Interface\iedit.h"
#include "propertyctrledititem.h"

namespace UI
{

class PropertyCtrlLongItem : public PropertyCtrlEditItem
{
public:
    PropertyCtrlLongItem(IPropertyCtrlLongItem* p);
    ~PropertyCtrlLongItem();

    UI_BEGIN_MSG_MAP()
        UIMSG_QUERYINTERFACE(PropertyCtrlLongItem)
        UIMSG_INITIALIZE(OnInitialize)
    UI_END_MSG_MAP_CHAIN_PARENT(PropertyCtrlEditItem)

public:
    void  SetValue(long lValue);

private:
	void  OnInitialize();

private:
    IPropertyCtrlLongItem*  m_pIPropertyCtrlLongItem;
    long  m_lValue;
	long  m_lDefaultValue;
};

}