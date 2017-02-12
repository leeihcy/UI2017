#pragma once
#include "UISDK\Inc\Interface\ilistitembase.h"
#include "UICTRL\Inc\Interface\iedit.h"
#include "propertyctrlitembase.h"

namespace UI
{
interface IEdit;

class PropertyCtrlEditItem : public PropertyCtrlItemBase
{
public:
    PropertyCtrlEditItem(IPropertyCtrlEditItem* p);
    ~PropertyCtrlEditItem();

    UI_BEGIN_MSG_MAP()
        UIMSG_PAINT(OnPaint)
        UIMSG_SIZE(OnSize)
		UIMSG_HANDLER_EX(UI_PROPERTYCTRLITEM_MSG_SETVALUESTRING, OnSetValue)
		UIMSG_HANDLER_EX(UI_PROPERTYCTRLITEM_MSG_SETDEFAULTVALUESTRING, OnSetDefaultValue)
		UIMSG_NOTIFY(UI_EN_ACCEPTEDIT, OnAcceptEdit)

        UIMSG_QUERYINTERFACE(PropertyCtrlEditItem)
        UIMSG_INITIALIZE(OnInitialize)
    UI_END_MSG_MAP_CHAIN_PARENT(PropertyCtrlItemBase)

public:
    void  SetValueText(const TCHAR* szText);
	void  SetDefaultValueText(const TCHAR* szText);

protected:
    void  OnInitialize();
    void  OnPaint(IRenderTarget* pRenderTarget);
    void  OnSize(UINT nType, int cx, int cy);
	LRESULT  OnSetValue(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT  OnSetDefaultValue(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT  OnAcceptEdit(WPARAM wParam, LPARAM lParam);

protected:
    void  BeginEdit();
    void  EndEdit();
    void  AcceptEdit();

	virtual void  OnValueColumnClick(POINT ptWnd, POINT ptItem) override;
    virtual LPCTSTR  GetValueText() override { return m_strValue.c_str(); }

protected:
	String  m_strValue;  // 该项在xml中的属性值
	String  m_strDefautValue;  // 该项的默认值

private:
    IPropertyCtrlEditItem*  m_pIPropertyCtrlEditItem;
};

}