#pragma once
#include "UIEditorCtrl\Inc\ipropertyctrl.h"

namespace UI
{

class PropertyCtrlGroupItem : public MessageProxy
{
public:
    PropertyCtrlGroupItem(IPropertyCtrlGroupItem* p);
    ~PropertyCtrlGroupItem();

    UI_BEGIN_MSG_MAP()
        UIMSG_PAINT(OnPaint)
        MSG_WM_LBUTTONDOWN(OnLButtonDown)
        UIMSG_QUERYINTERFACE(PropertyCtrlGroupItem)
		UIMSG_INITIALIZE(OnInitialize)
    UI_END_MSG_MAP_CHAIN_PARENT_Ixxx(PropertyCtrlGroupItem, INormalTreeItem)

public:

protected:
    void  OnPaint(IRenderTarget* pRenderTarget);
    void  OnLButtonDown(UINT nFlags, POINT point);
	void  OnInitialize();
private:
	IPropertyCtrlGroupItem*  m_pIPropertyCtrlGroupItem;
	INormalTreeItemShareData*  m_pShareData;
};

}