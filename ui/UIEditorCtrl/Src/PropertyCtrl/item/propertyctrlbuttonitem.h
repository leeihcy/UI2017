#pragma once
#include "UISDK\Inc\Interface\ilistitembase.h"
#include "UICTRL\Inc\Interface\ibutton.h"
#include "UICTRL\Inc\Interface\iedit.h"
#include "propertyctrlitembase.h"

namespace UI
{
class PropertyCtrlButtonItem : public PropertyCtrlItemBase
{
public:
    PropertyCtrlButtonItem(IPropertyCtrlButtonItem* p);
    ~PropertyCtrlButtonItem();

    UI_BEGIN_MSG_MAP()
        UIMSG_PAINT(OnPaint)
        UIMSG_STATECHANGED(OnStateChanged)
        UIMSG_SIZE(OnSize)
//         UIMSG_WM_NOTIFY(UI_EN_ESC, OnEditEsc)
//         UIMSG_WM_NOTIFY(UI_EN_RETURN, OnEditReturn)
		UIMSG_HANDLER_EX(UI_PROPERTYCTRLITEM_MSG_SETVALUESTRING, OnSetValue)
		UIMSG_HANDLER_EX(UI_PROPERTYCTRLITEM_MSG_SETDEFAULTVALUESTRING, OnSetDefaultValue)
        UIMSG_QUERYINTERFACE(PropertyCtrlButtonItem)
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

protected:
    void  BeginEdit();
    void  CancelEdit();

private:
    IPropertyCtrlButtonItem*  m_pIPropertyCtrlButtonItem;

    String  m_strValue;  // 该项在xml中的属性值
	String  m_strDefautValue;  // 该项的默认值
};

}