#include "stdafx.h"
#include "StylePropertyHandler.h"
#include "Dialog\Framework\propertydialog.h"
#include "UISDK\Inc\Interface\ixmlwrap.h"
#include "UICTRL\Inc\Interface\iedit.h"
#include "Business\Command\style\StyleCommand.h"
#include "Business\uieditorfunction.h"

StylePropertyHandler::StylePropertyHandler()
{
	m_pPropertyDialog = NULL;
	m_pPropertyCtrl = NULL;

	m_pSkinRes = NULL;
	m_pStyleResItem = NULL;
	m_eCurAttrType = XmlAttr;
}
StylePropertyHandler::~StylePropertyHandler()
{
	DetachHandler();
}

void  StylePropertyHandler::OnLoad(CPropertyDialog* pWnd)
{
	m_pPropertyDialog = pWnd;
	m_pPropertyCtrl = m_pPropertyDialog->GetPropertyCtrl();

	m_pRadioBtnXmlAttr = (IRadioButton*)pWnd->FindObject(_T("radiobtn_attr1"));
	m_pRadioBtnParsedAttr = (IRadioButton*)pWnd->FindObject(_T("radiobtn_attr2"));
	m_pRadioBtnInheritAttr = (IRadioButton*)pWnd->FindObject(_T("radiobtn_attr3"));

    if (m_pPropertyCtrl)
    {
        m_pPropertyCtrl->KeyDownEvent().connect(this,
            &StylePropertyHandler::OnPropertyKeyDown);
    }
	if (m_pRadioBtnXmlAttr)
	{
		m_pRadioBtnXmlAttr->SetText(TEXT("Xml"));
		m_pRadioBtnXmlAttr->SetToolTipText(TEXT("显示样式在xml中配置的属性"));
		m_pRadioBtnXmlAttr->Click();

        m_pRadioBtnXmlAttr->ClickedEvent().connect(this,
            &StylePropertyHandler::OnRadioButtonAttr);
	}
	if (m_pRadioBtnParsedAttr)
	{
		m_pRadioBtnParsedAttr->SetText(TEXT("Parse"));
		m_pRadioBtnParsedAttr->SetToolTipText(TEXT("解析inherit之后的属性"));

        m_pRadioBtnParsedAttr->ClickedEvent().connect(this,
            &StylePropertyHandler::OnRadioButtonAttr);
	}
	if (m_pRadioBtnInheritAttr)
	{
		m_pRadioBtnInheritAttr->SetText(TEXT("Inherit"));
		m_pRadioBtnInheritAttr->SetToolTipText(
            TEXT("显示自己的属性和inherit的属性"));

        m_pRadioBtnInheritAttr->ClickedEvent().connect(this,
            &StylePropertyHandler::OnRadioButtonAttr);
	}
}
void  StylePropertyHandler::OnUnload()
{
    if (m_pPropertyCtrl)
    {
        m_pPropertyCtrl->KeyDownEvent().disconnect();
		m_pPropertyCtrl->RemoveAllItem();
    }
    if (m_pRadioBtnXmlAttr)
    {
        m_pRadioBtnXmlAttr->ClickedEvent().disconnect();
    }
    if (m_pRadioBtnParsedAttr)
    {
        m_pRadioBtnParsedAttr->ClickedEvent().disconnect();
    }
    if (m_pRadioBtnInheritAttr)
    {
        m_pRadioBtnInheritAttr->ClickedEvent().disconnect();
    }

    m_pPropertyDialog = NULL;
    m_pPropertyCtrl = NULL;
}

void  StylePropertyHandler::AttachHandler()
{
	g_pGlobalData->m_pPropertyDialog->AttachHandler(static_cast<IPropertyHandler*>(this));
}
void  StylePropertyHandler::DetachHandler()
{
	g_pGlobalData->m_pPropertyDialog->DetachHandler(static_cast<IPropertyHandler*>(this));
}

void  StylePropertyHandler::AttachStyle(ISkinRes* pSkinRes, IStyleResItem* pStyleResItem)
{
	if (!m_pPropertyCtrl)
	{
		AttachHandler();
		if (!m_pPropertyCtrl)
			return;
	}

	m_pPropertyCtrl->RemoveAllItem();
	if (!pStyleResItem || !pSkinRes)
		return;

	m_pSkinRes = pSkinRes;
	m_pStyleResItem = pStyleResItem;

	if (m_eCurAttrType == XmlAttr)
	{
		AttachStyle_Xml(pStyleResItem);
	}
	else if (m_eCurAttrType == ParseAttr)
	{
		AttachStyle_Parsed(pStyleResItem);
	}
	else if (m_eCurAttrType == InheritAttr)
	{
		AttachStyle_Inherit(pStyleResItem);
	}
	else
	{
		AttachStyle_Xml(pStyleResItem);
	}
}

// 纯xml样式
void  StylePropertyHandler::AttachStyle_Xml(IStyleResItem* pStyleResItem)
{
	IUIElement* pXml = pStyleResItem->GetXmlElement();
	if (!pXml)
		return;

	IListAttribute* pList = NULL;
	pXml->GetAttribList2(&pList);

	pList->BeginEnum();
	LPCTSTR  szKey = NULL;
	LPCTSTR  szValue = NULL;
	while (pList->EnumNext(&szKey, &szValue))
	{
		PropertyCtrlEditItemInfo info = {0};
		info.szDesc = info.szKey = info.szText = szKey;
		info.szValue = szValue;

		m_pPropertyCtrl->InsertEditProperty(&info, NULL);
	}
	pList->EndEnum();
	SAFE_RELEASE(pList);

	m_pPropertyCtrl->AddAdditionItem();
}

// 解析继承链之后的样式值
void  StylePropertyHandler::AttachStyle_Parsed(IStyleResItem* pStyleResItem)
{
	IMapAttribute* pMap = NULL;
	pStyleResItem->GetAttributeMap(&pMap);
    if (!pMap)
        return;

	LPCTSTR  szKey = NULL;
	LPCTSTR  szValue = NULL;
	pMap->BeginEnum();

	while (pMap->EnumNext(&szKey, &szValue))
	{
		PropertyCtrlEditItemInfo info = {0};
		info.szDesc = info.szKey = info.szText = szKey;
		info.szValue = szValue;

		m_pPropertyCtrl->InsertEditProperty(&info, NULL);
	}
	pMap->EndEnum();
	pMap->Release();
}

// 显示继承链
void  StylePropertyHandler::AttachStyle_Inherit(IStyleResItem* pStyleResItem)
{

}

void  StylePropertyHandler::OnRadioButtonAttr(UI::IButton*)
{
	if (m_pRadioBtnXmlAttr->IsChecked())
		m_eCurAttrType = XmlAttr;
	else if (m_pRadioBtnParsedAttr->IsChecked())
		m_eCurAttrType = ParseAttr;
	else if (m_pRadioBtnInheritAttr->IsChecked())
		m_eCurAttrType = InheritAttr;
	else
		m_eCurAttrType = XmlAttr;

	AttachStyle(m_pSkinRes, m_pStyleResItem);
}

LRESULT  StylePropertyHandler::OnAdditionItem(WPARAM wParam, LPARAM lParam)
{
	if (!m_pStyleResItem)
		return 0;

	LPCTSTR szKey = (LPCTSTR)lParam;
	if (!szKey || !szKey[0])
		return ACCEPT_EDIT_REJECT_AND_CONTINUE_EDIT;

	if (m_eCurAttrType == XmlAttr)
	{
		// 判断是否重复
		IUIElement* pElement = m_pStyleResItem->GetXmlElement();
		if (pElement->HasAttrib(szKey))
			return ACCEPT_EDIT_REJECT_AND_CONTINUE_EDIT;

		UE_ExecuteCommand(AddStyleItemAttributeCommand::CreateInstance(m_pSkinRes, m_pStyleResItem, szKey, NULL));
	}
	return 0;
}

LRESULT  StylePropertyHandler::OnEditItemAcceptContent(WPARAM wParam, LPARAM)
{
	PROPERTYCTRL_EDIT_ACCEPTCONTENT* pParam = (PROPERTYCTRL_EDIT_ACCEPTCONTENT*)wParam;
	IPropertyCtrlEditItem*  pItem = (IPropertyCtrlEditItem*)pParam->pItem;

	OnAcceptContent(pParam->szKey, pParam->szNewValue);
	return 0;
}

void  StylePropertyHandler::OnAcceptContent(LPCTSTR szKey, LPCTSTR szValue)
{
	UE_ExecuteCommand(ModifyStyleItemAttributeCommand::CreateInstance(
        m_pSkinRes, m_pStyleResItem, szKey, szValue));
}

void  StylePropertyHandler::OnPropertyKeyDown(
        IListCtrlBase*, UINT w, bool& bHandled)
{
	if (w == VK_DELETE)
	{
		IListItemBase* pItem = m_pPropertyCtrl->GetFirstSelectItem();
		if (!pItem)
			return;

		if (pItem->GetItemFlag() == IPropertyCtrlEditItem::FLAG)
		{
			UE_ExecuteCommand(
				DeleteStyleItemAttributeCommand::CreateInstance(
				m_pSkinRes, m_pStyleResItem, static_cast<IPropertyCtrlEditItem*>(pItem)->GetKey()));
		}
        bHandled = true;
	}
}

void  StylePropertyHandler::OnStyleAttributeAdd(IStyleResItem* p, LPCTSTR szKey)
{
	if (m_pStyleResItem != p)
		return;

	if (m_eCurAttrType != XmlAttr)
		return;

	PropertyCtrlEditItemInfo info = {0};
	info.szKey = info.szDesc = info.szText = szKey;
	info.szValue = p->GetAttribute(szKey);
	m_pPropertyCtrl->LayoutItem(
		m_pPropertyCtrl->InsertEditProperty(&info, NULL), 
		true);
}

void  StylePropertyHandler::OnStyleAttributeRemove(IStyleResItem* p, LPCTSTR szKey)
{
	if (m_pStyleResItem != p)
		return;

	if (m_eCurAttrType != XmlAttr)
		return;

	IPropertyCtrlEditItem*  pItem = m_pPropertyCtrl->FindEditItem(szKey);
	if (!pItem)
		return;

	m_pPropertyCtrl->RemoveItem(pItem);

}
void  StylePropertyHandler::OnStyleAttributeModify(IStyleResItem* p, LPCTSTR szKey)
{
	if (m_pStyleResItem != p)
		return;

	if (m_eCurAttrType != XmlAttr)
		return;

	IPropertyCtrlEditItem*  pItem = m_pPropertyCtrl->FindEditItem(szKey);
	if (!pItem)
		return;
	pItem->SetValueText(p->GetAttribute(szKey));

	m_pPropertyCtrl->InvalidateItem(pItem);
}