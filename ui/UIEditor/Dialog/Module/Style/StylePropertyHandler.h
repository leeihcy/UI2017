#pragma once
#include "Dialog\Framework\ipropertyhandler.h"
#include "UICTRL\Inc\Interface\iradiobutton.h"
#include "UISDK\Src\Resource\styleres.h"
#include "..\Layout\LayoutPropertyHandler.h"

class StylePropertyHandler : public IPropertyHandler
{
public:
	enum 
	{
		PROPERTY_HANDLER_TYPE = 154162200,
	};
	StylePropertyHandler();
	~StylePropertyHandler();

	UI_BEGIN_MSG_MAP_Ixxx(StylePropertyHandler)
        UIMSG_NOTIFY(UI_PROPERTYCTRL_NM_ADDITIONITEM_ACCEPTCONTENT, OnAdditionItem)
        UIMSG_NOTIFY(UI_PROPERTYCTRL_NM_EDITITEM_ACCEPTCONTENT, OnEditItemAcceptContent)
    UI_END_MSG_MAP()

private:
	void  OnRadioButtonAttr(UI::IButton*);
	LRESULT  OnAdditionItem(WPARAM, LPARAM);
	LRESULT  OnEditItemAcceptContent(WPARAM, LPARAM);
    void  OnPropertyKeyDown(IListCtrlBase*, UINT, bool&);

private:
	long GetHandlerType(void) { return PROPERTY_HANDLER_TYPE; }
	virtual void  OnLoad(CPropertyDialog* pWnd) ;
	virtual void  OnUnload();

	void  AttachHandler();
	void  DetachHandler();

	void  OnAcceptContent(LPCTSTR szKey, LPCTSTR szValue);

	
public:
	void  AttachStyle(ISkinRes*, IStyleResItem* s);
	void  AttachStyle_Parsed(IStyleResItem*);
	void  AttachStyle_Xml(IStyleResItem*);
	void  AttachStyle_Inherit(IStyleResItem*);

	void  OnStyleAttributeAdd(IStyleResItem* p, LPCTSTR szKey);
	void  OnStyleAttributeRemove(IStyleResItem* p, LPCTSTR szKey);
	void  OnStyleAttributeModify(IStyleResItem* p, LPCTSTR szKey);

private:
	CPropertyDialog*  m_pPropertyDialog;
	IPropertyCtrl*  m_pPropertyCtrl;

	ISkinRes*  m_pSkinRes;
	IStyleResItem*  m_pStyleResItem;

	enum ATTR_TYPE
	{
		XmlAttr,
		ParseAttr,
		InheritAttr
	};
	ATTR_TYPE  m_eCurAttrType;
	IRadioButton*  m_pRadioBtnXmlAttr;
	IRadioButton*  m_pRadioBtnParsedAttr;
	IRadioButton*  m_pRadioBtnInheritAttr;
};