#pragma once
#include "propertyctrlitembase.h"
#include "UICTRL\Inc\Interface\iedit.h"

namespace UI
{
interface IEdit;

class PropertyCtrlAdditionItem : public PropertyCtrlItemBase
{
public:
    PropertyCtrlAdditionItem(IPropertyCtrlAdditionItem* p);
    ~PropertyCtrlAdditionItem();

    UI_BEGIN_MSG_MAP()
        UIMSG_PAINT(OnPaint)
        UIMSG_STATECHANGED(OnStateChanged)
        UIMSG_SIZE(OnSize)
        // UIMSG_NOTIFY(UI_EN_KEYDOWN, OnEditKeyDown)
		UIMSG_HANDLER_EX(UI_PROPERTYCTRLITEM_MSG_SETVALUESTRING, OnSetValue)
		UIMSG_HANDLER_EX(UI_PROPERTYCTRLITEM_MSG_SETDEFAULTVALUESTRING, OnSetDefaultValue)

        UIMSG_QUERYINTERFACE(PropertyCtrlAdditionItem)
        UIMSG_INITIALIZE(OnInitialize)
    UI_END_MSG_MAP_CHAIN_PARENT(PropertyCtrlItemBase)

public:
    void  SetValueText(const TCHAR* szText);
	void  SetDefaultValueText(const TCHAR* szText);

protected:
    void  OnInitialize();
    void  OnPaint(IRenderTarget* pRenderTarget);
    void  OnSize(UINT nType, int cx, int cy);
    void  OnStateChanged(UINT nMask);
	LRESULT  OnSetValue(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT  OnSetDefaultValue(UINT uMsg, WPARAM wParam, LPARAM lParam);
    bool  OnEditKeyDown(IEdit*, UINT);

protected:
    void  BeginEdit();
    void  EndEdit();
    void  AcceptEdit();

protected:
	String  m_strValue;  // 该项在xml中的属性值
	String  m_strDefautValue;  // 该项的默认值

private:
    IPropertyCtrlAdditionItem*  m_pIPropertyCtrlAdditionItem;
};

}