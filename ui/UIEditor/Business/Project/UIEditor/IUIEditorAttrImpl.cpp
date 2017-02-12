#include "stdafx.h"
#include "IUIEditorAttrImpl.h"
#if 0
void  CreateAttribute(UIEDITOR_ATTR_TYPE eType, IUIEditorAttrBase** ppAttrib)
{
	if (!ppAttrib)
		return;

	switch (eType)
	{
	case UIEDITOR_ATTR_TYPE_GROUP:
		*ppAttrib = static_cast<IUIEditorAttrBase*>(new CGroupAttribute);
		break;

	case UIEDITOR_ATTR_TYPE_TEXT:
		*ppAttrib = static_cast<IUIEditorAttrBase*>(new CTextAttribute);
		break;

	case UIEDITOR_ATTR_TYPE_BOOL:
		*ppAttrib = static_cast<IUIEditorAttrBase*>(new CBoolAttribute);
		break;

    case UIEDITOR_ATTR_TYPE_COMBOBOX:
        *ppAttrib = static_cast<IUIEditorAttrBase*>(new CComboBoxAttribute);
        break;

	case UIEDITOR_ATTR_TYPE_BUTTON:
		*ppAttrib = static_cast<IUIEditorAttrBase*>(new CButtonAttribute);
		break;
	}
}


CGroupAttribute::CGroupAttribute()
{
	m_bRoot = false;
}
CGroupAttribute::~CGroupAttribute()
{
	list<IUIEditorAttrBase*>::iterator iter = m_AttrList.begin();
	for (; iter != m_AttrList.end(); iter++)
	{
		IUIEditorAttrBase* p = *iter;
		SAFE_RELEASE(p);
	}
	m_AttrList.clear();
}
void  CGroupAttribute::AddAttribute(IUIEditorAttrBase* p)
{
	if (!p)
		return;

	m_AttrList.push_back(p);
}

void  CGroupAttribute::Insert2PropertyCtrl(IPropertyCtrl* pCtrl, IListItemBase* pParent)
{
	IListItemBase* pMe = NULL;
	if (!m_bRoot)
	{
		pMe = pCtrl->InsertGroupItem(m_strName.c_str(), m_strDesc.c_str(), pParent, UITVI_LAST, LF_NONE);
	}

	list<IUIEditorAttrBase*>::iterator iter = m_AttrList.begin();
	for (; iter != m_AttrList.end(); iter++)
	{
		IUIEditorAttrBase* p = *iter;
		p->Insert2PropertyCtrl(pCtrl, pMe);
	}
}


void  CTextAttribute::Insert2PropertyCtrl(IPropertyCtrl* pCtrl, IListItemBase* pParent)
{
	IPropertyCtrlEditItem* pItem = pCtrl->InsertEditProperty(
        m_strName.c_str(), NULL, m_strDesc.c_str(), m_strKey.c_str(), pParent, UITVI_LAST, LF_NONE);
    if (m_strDefValue.length() > 0)
        pItem->SetDefaultValueText(m_strDefValue.c_str());
}
void  CBoolAttribute::Insert2PropertyCtrl(IPropertyCtrl* pCtrl, IListItemBase* pParent)
{
	IPropertyCtrlComboBoxItem* pItem = pCtrl->InsertBoolProperty(
        m_strName.c_str(), NULL, m_strDesc.c_str(), m_strKey.c_str(), pParent, UITVI_LAST, LF_NONE);
    if (m_strDefValue.length() > 0)
        pItem->SetDefaultValueText(m_strDefValue.c_str());
}

CComboBoxAttribute::CComboBoxAttribute()
{
    m_bReloadOnChanged = false;
}
CComboBoxAttribute::~CComboBoxAttribute()
{
    vector<ComboBoxOptionItem*>::iterator iter = m_vecItems.begin();
    for (; iter != m_vecItems.end(); iter++)
    {
        delete *iter;
    }
    m_vecItems.clear();
}
void  CComboBoxAttribute::Insert2PropertyCtrl(IPropertyCtrl* pCtrl, IListItemBase* pParent)
{
//     IPropertyCtrlComboBoxItem* pItem = pCtrl->InsertComboBoxProperty(
//         m_strName.c_str(), 
//         NULL, 
//         m_strDesc.c_str(), 
//         m_strKey.c_str(), 
//         pParent, 
//         UITVI_LAST, 
//         LF_NONE);
// 
//     if (m_strDefValue.length() > 0)
//         pItem->SetDefaultValueText(m_strDefValue.c_str());
// 
//     vector<ComboBoxOptionItem*>::iterator iter = m_vecItems.begin();
//     for (; iter != m_vecItems.end(); iter++)
//     {
//         ComboBoxOptionItem* pOptionItem = *iter;
//         pItem->AddOption(pOptionItem->strText.c_str(), pOptionItem->strValue.c_str());
//     }
// 
//     if (m_bReloadOnChanged)
//     {
//         pItem->SetItemFlag(UIEditorComboBoxAttributeFlag_ReloadOnChange);
//     }
}

IUIEditorComboBoxAttribute*  CComboBoxAttribute::AddOption(LPCTSTR szText, LPCTSTR szValue)
{
    if (!szText)
        return static_cast<IUIEditorComboBoxAttribute*>(this);

    ComboBoxOptionItem* pItem = new ComboBoxOptionItem;
    pItem->strText = szText;
    if (szValue)
        pItem->strValue = szValue;
    else
        pItem->strValue = szText;
    m_vecItems.push_back(pItem);
    return static_cast<IUIEditorComboBoxAttribute*>(this);
}

void  CComboBoxAttribute::SetReloadOnChanged(bool b)
{
    m_bReloadOnChanged = b;
}
bool  CComboBoxAttribute::GetReloadOnChanged()
{
    return m_bReloadOnChanged;
}

void  CButtonAttribute::Insert2PropertyCtrl(IPropertyCtrl* pCtrl, IListItemBase* pParent)
{
	IPropertyCtrlButtonItem* pItem = pCtrl->InsertButtonProperty(
        m_strName.c_str(), 
        NULL, 
        m_strDesc.c_str(), 
        m_strKey.c_str(), 
        pParent, 
        UITVI_LAST, LF_NONE);

	if (m_strDefValue.length() > 0)
		pItem->SetDefaultValueText(m_strDefValue.c_str());
}
#endif