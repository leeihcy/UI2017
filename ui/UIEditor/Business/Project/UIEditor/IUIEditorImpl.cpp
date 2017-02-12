#include "StdAfx.h"
#include "IUIEditorImpl.h"
#include "Dialog\Framework\PropertyDialog.h"
#include "Dialog\Framework\toolbox.h"
#include "UISDK\Inc\Interface\ixmlwrap.h"
#include "Dialog\Framework\project_tree_dialog.h"
#include "IUIEditorAttrImpl.h"
#include "Dialog\Framework\mainframe.h"
#include "Dialog\Framework\childframe.h"
#include "UIEditorCtrl\Inc\ipropertyctrl.h"

ObjectEditorData::ObjectEditorData()
{
	pTreeItem = NULL;
	pXmlElement = NULL;
    pAttributeProxy = NULL;
}
ObjectEditorData::~ObjectEditorData()
{
    SAFE_RELEASE(pXmlElement);
    SAFE_DELETE(pAttributeProxy);
}

void  ObjectEditorData::SetXmlElem(IUIElement* p)
{
    if (pXmlElement == p)
        return;

    SAFE_RELEASE(pXmlElement);
    pXmlElement = p;
    if (pXmlElement)
        pXmlElement->AddRef();
}

ObjectPropertyEditStatus::ObjectPropertyEditStatus()
{
    m_lVScrollPos = 0;
}
void  ObjectPropertyEditStatus::Clear()
{
    m_listGroupCollapsed.clear();
    m_lVScrollPos = 0;
    m_strSelectedItem.clear();
}
void  ObjectPropertyEditStatus::AddCollapsedGroup(LPCTSTR szParentKey, LPCTSTR szGroupName)
{
    if (!szGroupName)
        return;

    GroupNodeId id;
    id.strName = szGroupName;

    if (szParentKey)
    {
        id.strParentKey = szParentKey;        
    }
    m_listGroupCollapsed.push_back(id);
}

// bool operator==(ObjectPropertyEditStatus::GroupNodeId& left, ObjectPropertyEditStatus::GroupNodeId& right)
// {
//     if (left.strName == right.strName && left.strParentKey == right.strParentKey)
//         return true;
// 
//     return false;
// }

bool  ObjectPropertyEditStatus::IsGroupCollapsed(LPCTSTR szParentKey, LPCTSTR szGroupName)
{
    if (!szGroupName)
        return false;

    GroupNodeId id;
    id.strName = szGroupName;
    if (szParentKey)
        id.strParentKey = szParentKey;

    if (std::find(m_listGroupCollapsed.begin(), m_listGroupCollapsed.end(), id) != m_listGroupCollapsed.end())
        return true;

    return false;
}
void  ObjectPropertyEditStatus::SetVScrollPos(long l)
{
    m_lVScrollPos = l;
}
void  ObjectPropertyEditStatus::SetSelectedItem(LPCTSTR szKey)
{
    if (szKey)
        m_strSelectedItem = szKey;
    else
        m_strSelectedItem.clear();
}
LPCTSTR  ObjectPropertyEditStatus::GetSelectedItem()
{
    return m_strSelectedItem.c_str();
}
long  ObjectPropertyEditStatus::GetVScrollPos()
{
    return m_lVScrollPos;
}

ImageResItemEditorData::ImageResItemEditorData()
{
    pXmlElement = NULL;
}
ImageResItemEditorData::~ImageResItemEditorData()
{
    if (pXmlElement)
    {
        pXmlElement->Release();
    }
}
void  ImageResItemEditorData::SetXmlElem(IUIElement* p)
{
    if (pXmlElement == p)
        return;
     
    SAFE_RELEASE(pXmlElement);
    pXmlElement = p;
    if (pXmlElement)
    {
        pXmlElement->AddRef();
    }
}

CUIEditor::CUIEditor(void)
{
}

CUIEditor::~CUIEditor(void)
{
    Destroy();
}

void  CUIEditor::Destroy()
{
	{
		auto iter = m_mapImageResItemUserData.begin();
		for (; iter != m_mapImageResItemUserData.end(); ++iter)
		{
			SAFE_DELETE(iter->second);
		}
		m_mapImageResItemUserData.clear();
	}

	{
		auto iter = m_mapObjectData.begin();
		for (; iter != m_mapObjectData.end(); ++iter)
		{
			SAFE_DELETE(iter->second);
		}
		m_mapObjectData.clear();
	}
}

// 用于创建组合控件时，同时创建其子控件。
// 如CombobBox调用，用于创建button/edit
bool  CUIEditor::AddObjectRes(EditorAddObjectResData* pData)
{
	if (!pData || !pData->pParentObj || !pData->pParentXml)
		return false;

	IObject* pObj = pData->pUIApp->CreateUIObjectByClsid(
			pData->objiid, 
			pData->pParentObj->GetSkinRes());
	if (!pObj)
		return false;
	
	*pData->ppCreateObj = pObj;

	if (pData->bNcChild)
		pData->pParentObj->AddNcChild(pObj);
	else
		pData->pParentObj->AddChild(pObj);

    IObjectDescription* pDesc = pObj->GetDescription();
    LPCTSTR szObjName = NULL;
    if (pDesc)
        szObjName = pDesc->GetTagName();

    IUIElementProxy xmlChild = 
        pData->pParentXml->AddChild(szObjName);
	if (pData->szId)
	{
		xmlChild->AddAttrib(XML_ID, pData->szId);
		pObj->SetId(pData->szId);
	}
	if (pData->ppCreateXml)
	{
		*pData->ppCreateXml = xmlChild.get();
		(*pData->ppCreateXml)->AddRef();
	}

	CREATEBYEDITORDATA  data = {0};
	data.pUIApp = pData->pUIApp;
	data.pXml = xmlChild.get();
	data.pEditor = static_cast<IUIEditor*>(this);
	UISendMessage(pObj, UI_MSG_CREATEBYEDITOR, (WPARAM)&data, 0,0,0, UIALT_CALLLESS);

	this->OnObjectAttributeLoad(pObj, xmlChild.get());
    return true;
}

void  CUIEditor::OnObjectAttributeLoad(__in IObject* pObject, __in IUIElement* pXmlElem)
{
	if (NULL == pObject)
		return;

	ObjectEditorData* pData = this->GetObjectEditorData(pObject);
	if (NULL == pData)
	{
		pData = new ObjectEditorData;
		m_mapObjectData[pObject] = pData;
	}

    pData->SetXmlElem(pXmlElem);
}

void  CUIEditor::OnObjectDeleteInd(__in IObject* pObj)
{
    _MyObjectMapIter iter = m_mapObjectData.find(pObj);
    if (iter == m_mapObjectData.end())
        return;

    ObjectEditorData*  pUserData = iter->second;
    if (!pUserData)
        return;

	// 从树中删除
    g_pGlobalData->m_pProjectTreeDialog->OnObjectDeleteInd(pUserData);

	// 从布局视图中删除
	IWindowBase*  pWindow = pObj->GetWindowObject();
    if (!pWindow)
        return;

	ISkinRes*  pSkinRes = pWindow->GetSkinRes();
	ILayoutMDIClientView*  pView = g_pGlobalData->m_pMainFrame->GetLayoutClientViewByObject(pWindow);
	if (pView)
	{
		pView->OnObjectDeleteInd(pObj);
	}

    SAFE_DELETE(pUserData);
    m_mapObjectData.erase(iter);
}

void  CUIEditor::OnImageItemLoad(__in IImageResItem*  pItem, __in IUIElement* pXmlElem)
{
    if (NULL == pItem)
        return;

    ImageResItemEditorData* pData = this->GetImageResItemEditorData(pItem);
    if (NULL == pData)
    {
        pData = new ImageResItemEditorData;
        m_mapImageResItemUserData[pItem] = pData;
    }
    pData->SetXmlElem(pXmlElem);
}
void  CUIEditor::OnGifItemLoad(__in IGifResItem*  pItem, __in IUIElement* pXmlElem)
{

}
void  CUIEditor::OnCursorItemLoad(__in ICursorResItem*  pItem, __in IUIElement* pXmlElem)
{

}
void  CUIEditor::OnImageItemDeleteInd(__in IImageResItem*  pItem)
{
    if (NULL == pItem)
        return;

    _MyImageResItemMapIter iter = m_mapImageResItemUserData.find(pItem);
    if (iter == m_mapImageResItemUserData.end())
        return;

    ImageResItemEditorData* pData = iter->second;
    SAFE_DELETE(pData);
    m_mapImageResItemUserData.erase(iter);
}
void  CUIEditor::OnGifItemDeleteInd(__in IGifResItem*  pItem)
{

}
void  CUIEditor::OnCursorItemDeleteInd(__in ICursorResItem*  pItem)
{

}

void  CUIEditor::OnToolBox_AddObject(IObjectDescription* p)
{
    if (g_pGlobalData->m_pToolBox)
        g_pGlobalData->m_pToolBox->InsertObject(p);
}

ObjectEditorData*  CUIEditor::GetObjectEditorData(IObject* pObj)
{
	if (NULL == pObj)
		return NULL;

	_MyObjectMapIter iter = m_mapObjectData.find(pObj);
	if (iter == m_mapObjectData.end())
		return NULL;

	return iter->second;
}
ImageResItemEditorData*  CUIEditor::GetImageResItemEditorData(
        IImageResItem* pImageResItem)
{
    if (NULL == pImageResItem)
        return NULL;

    _MyImageResItemMapIter iter = m_mapImageResItemUserData.find(pImageResItem);
    if (iter == m_mapImageResItemUserData.end())
        return NULL;

    return iter->second;
}   

void CUIEditor::SetObjectHTREEITEM(IObject* pObject, IListItemBase* pTreeItem)
{
	if (NULL == pObject)
		return;

	ObjectEditorData* pData = this->GetObjectEditorData(pObject);
	if (NULL == pData)
	{
		pData = new ObjectEditorData;
		m_mapObjectData[pObject] = pData;
	}

	pData->pTreeItem = pTreeItem;
}

IListItemBase* CUIEditor::GetObjectHTREEITEM(IObject* pObject)
{
	if (NULL == pObject)
		return NULL;

	ObjectEditorData* pData = this->GetObjectEditorData(pObject);
	if (NULL == pData)
		return NULL;

	return pData->pTreeItem;
}

IUIElement* CUIEditor::GetObjectXmlElem(IObject* pObject)
{
	if (NULL == pObject)
		return NULL;

	ObjectEditorData* pData = this->GetObjectEditorData(pObject);
	if (NULL == pData)
		return NULL;

	return pData->pXmlElement;
}

IUIElement*  CUIEditor::GetImageResItemXmlElem(IImageResItem* pImageResItem)
{
    if (!pImageResItem)
        return NULL;

    ImageResItemEditorData* pData = GetImageResItemEditorData(pImageResItem);
    if (!pData)
        return NULL;

    return pData->pXmlElement;
}

IPropertyCtrlGroupItem*  GetGroupItem(LPCTSTR szParentKey, LPCTSTR szGroup)
{
	if (!szGroup || !szGroup[0])
		return NULL;

	IPropertyCtrl*  pPropertyCtrl = GetPropertyCtrl();
	if (!pPropertyCtrl)
		return NULL;

	IPropertyCtrlGroupItem* pItem = NULL;
    IListItemBase* pParentItem = NULL;
    if (!szParentKey || !szParentKey[0])
    {
        pItem = pPropertyCtrl->FindGroupItem(NULL, szGroup);
    }
    else
    {
        pParentItem = pPropertyCtrl->FindItemByKey(szParentKey);
        if (!pParentItem)
            return NULL;
        
        pItem = pPropertyCtrl->FindGroupItem(pParentItem, szGroup);
    }
	if (pItem)
		return pItem;

    return pPropertyCtrl->InsertGroupItem(szGroup, szGroup, pParentItem);
}

// CPropertyDialog::ReloadObjectAttribute中调用 
void  CUIEditor::DestroyObjectAttributeProxy(IObject* pObj)
{
    if (!pObj)
        return;

    ObjectEditorData*  pData = GetObjectEditorData(pObj);
    if (!pData)
        return;

    SAFE_DELETE(pData->pAttributeProxy);
}

void  CUIEditor::LoadAttribute2PropertyCtrl(IObject* pObj)
{
	if (!pObj)
		return;

    ObjectEditorData*  pData = GetObjectEditorData(pObj);
    if (!pData)
        return;

	IPropertyCtrl*  pPropertyCtrl = GetPropertyCtrl();
	if (!pPropertyCtrl)
		return;

    if (!pData->pAttributeProxy)
    {
        pData->pAttributeProxy = new IAttributeEditorProxy(this);
    }

    // 里面会判断是否已加载过了
    pData->pAttributeProxy->LoadAttribute2Editor(pObj);

// 	CGroupAttribute root;
// 	root.m_bRoot = true;
// 
//     EDITORGETOBJECTATTRLISTDATA data;
//     data.pEditor = static_cast<IUIEditor*>(this);
//     data.pGroupAttr = &root;
//     data.szPrefix = NULL;
// 	UISendMessage(pObj, UI_EDITOR_GETOBJECTATTRLIST, (WPARAM)&data);
// 
// 	root.Insert2PropertyCtrl(pCtrl, NULL);
    return;
}


// 将对象的属性，序列化到xml当中
// 修改了对象属性后，直接反映到xml中。采用这种方式最简单了吧。
// 如果等到保存的时候，再根据object属性，同步相应的xml，要考虑的东西太多，搞不定。
void  CUIEditor::SaveAttribute2Xml(IObject* pObj)
{
    if (!pObj)
        return;

    ObjectEditorData*  pData = GetObjectEditorData(pObj);
    if (!pData || !pData->pXmlElement)
        return;

    IListAttribute* pListAttrib = NULL;
    UICreateIListAttribute(&pListAttrib);

    IObjectDescription* pDesc = pObj->GetDescription();

    SERIALIZEDATA  data = {0};
    data.pUIApplication = GetEditUIApplication();
    data.nFlags = SERIALIZEFLAG_SAVE;
    data.pListAttrib = pListAttrib;
    UISendMessage(pObj, UI_MSG_SERIALIZE, (WPARAM)&data);

    LPCTSTR szStyle = pListAttrib->GetAttr(XML_STYLECLASS);

    // 从中减掉styleclass的样式
    IStyleRes&  pStyleRes = pObj->GetSkinRes()->GetStyleRes();
    pStyleRes.UnloadStyle(
        pDesc->GetTagName(),
        szStyle,
        pObj->GetId(),
        pListAttrib);

    // 更新到xml
    pData->pXmlElement->SetAttribList2(pListAttrib);
    SAFE_RELEASE(pListAttrib);
}

void  CUIEditor::EditorStringAttribute(IStringAttribute* pAttribute, EditorAttributeFlag e)
{
	IPropertyCtrl*  pPropertyCtrl = GetPropertyCtrl();
	if (!pPropertyCtrl)
		return;

	if (EDITOR_ATTRIBUTE_ADD == e)
	{
		PropertyCtrlEditItemInfo info = {0};
		info.szText = info.szKey = pAttribute->GetKey();
		info.szDesc = pAttribute->GetDesc();
		info.szValue = pAttribute->Get();

		IPropertyCtrlGroupItem* pGroup = GetGroupItem(pAttribute->GetParentKey(), pAttribute->GetGroupName());
		pPropertyCtrl->InsertEditProperty(&info, pGroup, UITVI_LAST);
	}
	else if (EDITOR_ATTRIBUTE_UPDATE == e)
	{
		IPropertyCtrlEditItem* pEdit = pPropertyCtrl->FindEditItem(pAttribute->GetKey());
		pEdit->SetValueText(pAttribute->Get());

		pPropertyCtrl->InvalidateItem(pEdit);
	}
}
void  CUIEditor::EditorBoolAttribute(IBoolAttribute* pAttribute, EditorAttributeFlag e)
{
	IPropertyCtrl*  pPropertyCtrl = GetPropertyCtrl();
	if (!pPropertyCtrl)
		return;

    if (EDITOR_ATTRIBUTE_ADD == e)
    {
		IPropertyCtrlGroupItem* pGroup = GetGroupItem(
            pAttribute->GetParentKey(),
            pAttribute->GetGroupName());

        PropertyCtrlBoolItemInfo info = {0};
        info.szText = info.szKey = pAttribute->GetKey();
        info.bValue = pAttribute->GetBool();
        info.bDefault = pAttribute->GetDefaultBool();
        
        pPropertyCtrl->InsertBoolProperty(&info, pGroup, UITVI_LAST);
    }
    else if (EDITOR_ATTRIBUTE_UPDATE == e)
    {
		IPropertyCtrlBoolItem* pBoolItem = 
            pPropertyCtrl->FindBoolItem(pAttribute->GetKey());

        if (pBoolItem)
        {
            pBoolItem->SetBool(pAttribute->GetBool());
        }
        else
        {
            // 可能是xml属性编辑状态，所有的属性是edit模式
            IPropertyCtrlEditItem* pItem =
                pPropertyCtrl->FindEditItem(pAttribute->GetKey());
            if (pItem)
            {
                pItem->SetValueText(pAttribute->Get());
            }
        }
    }
}

void  EnumLongAttributeAlias(
        LPCTSTR szText, long lValue, WPARAM wParam, LPARAM lParam)
{
	IPropertyCtrl*  pPropertyCtrl = (IPropertyCtrl*)wParam;
	IPropertyCtrlComboBoxItem*  pItem = (IPropertyCtrlComboBoxItem*)lParam;

	pItem->AddOption(szText, lValue);
}

void  CUIEditor::EditorLongAttribute(
        ILongAttribute* pAttribute, EditorAttributeFlag e)
{
	IPropertyCtrl*  pPropertyCtrl = GetPropertyCtrl();
	if (!pPropertyCtrl)
		return;

	long lCount = pAttribute->GetAliasCount();

    if (EDITOR_ATTRIBUTE_ADD == e)
    {
		IPropertyCtrlGroupItem* pGroup = GetGroupItem(
                pAttribute->GetParentKey(), pAttribute->GetGroupName());
		if (!lCount)
		{
			PropertyCtrlLongItemInfo info = {0};
			info.szText = info.szKey = pAttribute->GetKey();
			info.lValue = pAttribute->GetLong();
			pPropertyCtrl->InsertLongProperty(&info, pGroup, UITVI_LAST);
		}
		else
		{
			PropertyCtrlComboBoxItemInfo info = {0};
			info.szText = info.szKey = pAttribute->GetKey();
			info.eType = ComboBoxItemValueLong;
			
			IPropertyCtrlComboBoxItem*  pItem = pPropertyCtrl->
                    InsertComboBoxProperty(&info, pGroup, UITVI_LAST);

			// 枚举别名
			pAttribute->EnumAlias(
					EnumLongAttributeAlias, (WPARAM)pPropertyCtrl, (LPARAM)pItem);

            pItem->SetValueLong(pAttribute->GetLong());
		}
    }
	else if (EDITOR_ATTRIBUTE_UPDATE == e)
	{
		if (!lCount)
		{
			IPropertyCtrlLongItem* pLongItem = 
                pPropertyCtrl->FindLongItem(pAttribute->GetKey());
            if (pLongItem)
            {
                pLongItem->SetValue(pAttribute->GetLong());
            }
            else
            {
                // 可能是xml属性编辑状态，所有的属性是edit模式
                IPropertyCtrlEditItem* pItem =
                    pPropertyCtrl->FindEditItem(pAttribute->GetKey());
                if (pItem)
                {
                    pItem->SetValueText(pAttribute->Get());
                }
            }
		}
		else
		{
			IPropertyCtrlComboBoxItem* pCombo =
                pPropertyCtrl->FindComboBoxItem(pAttribute->GetKey());
            if (pCombo)
            {
                pCombo->SetValueLong(pAttribute->GetLong());
            }
            else
            {
                // 可能是xml属性编辑状态，所有的属性是edit模式
                IPropertyCtrlEditItem* pItem =
                    pPropertyCtrl->FindEditItem(pAttribute->GetKey());
                if (pItem)
                {
                    pItem->SetValueText(pAttribute->Get());
                }
            }
		}
	}
}


void  CUIEditor::EditorEnumAttribute(
        IEnumAttribute* pAttribute, EditorAttributeFlag e)
{
	IPropertyCtrl*  pPropertyCtrl = GetPropertyCtrl();
	if (!pPropertyCtrl)
		return;

	if (EDITOR_ATTRIBUTE_ADD == e)
	{
		IPropertyCtrlGroupItem* pGroup = GetGroupItem(
            pAttribute->GetParentKey(), 
            pAttribute->GetGroupName());

		PropertyCtrlComboBoxItemInfo info = {0};
		info.szText = info.szKey = pAttribute->GetKey();
		info.eType = ComboBoxItemValueLong;
		info.bReadOnly = true;

		IPropertyCtrlComboBoxItem*  pItem = pPropertyCtrl->
				InsertComboBoxProperty(&info, pGroup, UITVI_LAST);

		pAttribute->EnumAlias(
				EnumLongAttributeAlias, (WPARAM)pPropertyCtrl, (LPARAM)pItem);

        pItem->SetValueLong(pAttribute->GetLong());
	}
	else if (EDITOR_ATTRIBUTE_UPDATE == e)
	{
		IPropertyCtrlComboBoxItem* pCombo = 
            pPropertyCtrl->FindComboBoxItem(pAttribute->GetKey());

        if (pCombo)
        {
            pCombo->SetValueLong(pAttribute->GetLong());
        }
        else
        {
            // 可能是xml属性编辑状态，所有的属性是edit模式
            IPropertyCtrlEditItem* pItem =
                pPropertyCtrl->FindEditItem(pAttribute->GetKey());
            if (pItem)
            {
                pItem->SetValueText(pAttribute->Get());
            }
        }
	}
}

void  CUIEditor::EditorFlagsAttribute(
        IFlagsAttribute* pAttribute, EditorAttributeFlag e)
{
    IPropertyCtrl*  pPropertyCtrl = GetPropertyCtrl();
    if (!pPropertyCtrl)
        return;

    if (EDITOR_ATTRIBUTE_ADD == e)
    {
        IPropertyCtrlGroupItem* pGroup = GetGroupItem(
            pAttribute->GetParentKey(), 
            pAttribute->GetGroupName());

        PropertyCtrlComboBoxItemInfo info = {0};
        info.szText = info.szKey = pAttribute->GetKey();
        info.eType = ComboBoxItemValueLong;
        info.lValue = 0;
        info.bReadOnly = true;
        info.bMultiSel = true;

        IPropertyCtrlComboBoxItem*  pItem = pPropertyCtrl->
            InsertComboBoxProperty(&info, pGroup, UITVI_LAST);

        pAttribute->EnumAlias(
            EnumLongAttributeAlias, (WPARAM)pPropertyCtrl, (LPARAM)pItem);

        pItem->SetValueLong(pAttribute->GetLong());
    }
    else if (EDITOR_ATTRIBUTE_UPDATE == e)
    {
        IPropertyCtrlComboBoxItem* pCombo =
            pPropertyCtrl->FindComboBoxItem(pAttribute->GetKey());
        if (pCombo)
        {
            pCombo->SetValueLong(pAttribute->GetLong());
        }
        else
        {
            // 可能是xml属性编辑状态，所有的属性是edit模式
            IPropertyCtrlEditItem* pItem =
                pPropertyCtrl->FindEditItem(pAttribute->GetKey());
            if (pItem)
            {
                pItem->SetValueText(pAttribute->Get());
            }
        }

    }
}

void  CUIEditor::EditorRectAttribute(
        IRectAttribute* pAttribute, EditorAttributeFlag e)
{
    IPropertyCtrl*  pPropertyCtrl = GetPropertyCtrl();
    if (!pPropertyCtrl)
        return;

    if (EDITOR_ATTRIBUTE_ADD == e)
	{
		IPropertyCtrlGroupItem* pGroup = GetGroupItem(
            pAttribute->GetParentKey(), pAttribute->GetGroupName());

		PropertyCtrlEditItemInfo info = {0};
		info.szText = info.szKey = pAttribute->GetKey();
		info.szDesc = pAttribute->GetDesc();
		info.szValue = pAttribute->Get();

        pPropertyCtrl->InsertEditProperty(&info, pGroup, UITVI_LAST);
    }
    else if (EDITOR_ATTRIBUTE_UPDATE == e)
    {
        IPropertyCtrlEditItem* pEdit = 
            pPropertyCtrl->FindEditItem(pAttribute->GetKey());
        pEdit->SetValueText(pAttribute->Get());

        pPropertyCtrl->InvalidateItem(pEdit);
    }
}

void  CUIEditor::EditorSizeAttribute(
        ISizeAttribute* pAttribute, EditorAttributeFlag e)
{
	IPropertyCtrl*  pPropertyCtrl = GetPropertyCtrl();
	if (!pPropertyCtrl)
		return;

	if (EDITOR_ATTRIBUTE_ADD == e)
	{
		IPropertyCtrlGroupItem* pGroup = GetGroupItem(
            pAttribute->GetParentKey(), pAttribute->GetGroupName());

		PropertyCtrlEditItemInfo info = {0};
		info.szText = info.szKey = pAttribute->GetKey();
		info.szDesc = pAttribute->GetDesc();
		info.szValue = pAttribute->Get();

		pPropertyCtrl->InsertEditProperty(&info, pGroup, UITVI_LAST);
	}
	else if (EDITOR_ATTRIBUTE_UPDATE == e)
	{
		IPropertyCtrlEditItem* pEdit = 
            pPropertyCtrl->FindEditItem(pAttribute->GetKey());
		pEdit->SetValueText(pAttribute->Get());

		pPropertyCtrl->InvalidateItem(pEdit);
	}
}

void  CUIEditor::EditorRegion9Attribute(
        IRegion9Attribute* pAttribute, EditorAttributeFlag e)
{
	IPropertyCtrl*  pPropertyCtrl = GetPropertyCtrl();
	if (!pPropertyCtrl)
		return;

	if (EDITOR_ATTRIBUTE_ADD == e)
	{
		IPropertyCtrlGroupItem* pGroup = GetGroupItem(
            pAttribute->GetParentKey(), pAttribute->GetGroupName());

		PropertyCtrlEditItemInfo info = {0};
		info.szText = info.szKey = pAttribute->GetKey();
		info.szDesc = pAttribute->GetDesc();
		info.szValue = pAttribute->Get();

		pPropertyCtrl->InsertEditProperty(&info, pGroup, UITVI_LAST);
	}
	else if (EDITOR_ATTRIBUTE_UPDATE == e)
	{
		IPropertyCtrlEditItem* pEdit =
            pPropertyCtrl->FindEditItem(pAttribute->GetKey());
		pEdit->SetValueText(pAttribute->Get());

		pPropertyCtrl->InvalidateItem(pEdit);
	}
}

void  EnumStringEnumAttributeCallback(
        LPCTSTR szText, WPARAM wParam, LPARAM lParam)
{
    IPropertyCtrl*  pPropertyCtrl = (IPropertyCtrl*)wParam;
    IPropertyCtrlComboBoxItem*  pItem = (IPropertyCtrlComboBoxItem*)lParam;

    pItem->AddOption(szText, 0);
}

void  CUIEditor::EditorStringEnumAttribute(
        IStringEnumAttribute* pAttribute, EditorAttributeFlag e)
{
    IPropertyCtrl*  pPropertyCtrl = GetPropertyCtrl();
    if (!pPropertyCtrl)
        return;

    if (EDITOR_ATTRIBUTE_ADD == e)
    {
		IPropertyCtrlGroupItem* pGroup = GetGroupItem(
			pAttribute->GetParentKey(), pAttribute->GetGroupName());

        PropertyCtrlComboBoxItemInfo info = {0};
        info.szText = info.szKey = pAttribute->GetKey();
        info.eType = ComboBoxItemValueString;
        info.bReadOnly = true;

        IPropertyCtrlComboBoxItem*  pItem = pPropertyCtrl->
                InsertComboBoxProperty(&info, pGroup, UITVI_LAST);

        pAttribute->EnumString(EnumStringEnumAttributeCallback, 
                (WPARAM)pPropertyCtrl, (LPARAM)pItem);

        pItem->SetValueString(pAttribute->Get());
    }
    else if (EDITOR_ATTRIBUTE_UPDATE == e)
    {
        IPropertyCtrlComboBoxItem* pCombo = 
            pPropertyCtrl->FindComboBoxItem(pAttribute->GetKey());

        if (pCombo)
        {
            pCombo->SetValueString(pAttribute->Get());
        }
        else
        {
            // 可能是xml属性编辑状态，所有的属性是edit模式
            IPropertyCtrlEditItem* pItem =
                pPropertyCtrl->FindEditItem(pAttribute->GetKey());
            if (pItem)
            {
                pItem->SetValueText(pAttribute->Get());
            }
        }
    }
}

void  CUIEditor::EditorColorAttribute(
        IColorAttribute* pAttribute, EditorAttributeFlag e)
{
	IPropertyCtrl*  pPropertyCtrl = GetPropertyCtrl();
	if (!pPropertyCtrl)
		return;

	if (EDITOR_ATTRIBUTE_ADD == e)
	{
		PropertyCtrlEditItemInfo info = { 0 };
		info.szText = info.szKey = pAttribute->GetKey();
		info.szDesc = pAttribute->GetDesc();
		info.szValue = pAttribute->Get();

		IPropertyCtrlGroupItem* pGroup = 
            GetGroupItem(
                pAttribute->GetParentKey(), 
                pAttribute->GetGroupName());

		pPropertyCtrl->InsertEditProperty(&info, pGroup, UITVI_LAST);
	}
	else if (EDITOR_ATTRIBUTE_UPDATE == e)
	{
		IPropertyCtrlEditItem* pEdit = 
            pPropertyCtrl->FindEditItem(pAttribute->GetKey());
		pEdit->SetValueText(pAttribute->Get());

		pPropertyCtrl->InvalidateItem(pEdit);
	}
}

void  CUIEditor::EditorRenderBase(
        IRenderBaseAttribute* pAttribute, EditorAttributeFlag e)
{
    IPropertyCtrl*  pPropertyCtrl = GetPropertyCtrl();
    if (!pPropertyCtrl)
        return;

    if (EDITOR_ATTRIBUTE_ADD == e)
    {
		IPropertyCtrlGroupItem* pGroup = GetGroupItem(
			pAttribute->GetParentKey(), 
			pAttribute->GetGroupName());

        PropertyCtrlEditItemInfo info = { 0 };
        info.szText = info.szKey = pAttribute->GetKey();
        info.szDesc = pAttribute->GetDesc();
        info.szValue = pAttribute->Get();

        pPropertyCtrl->InsertEditProperty(
			&info, pGroup, UITVI_LAST);
    }
    else if (EDITOR_ATTRIBUTE_UPDATE == e)
    {
        IPropertyCtrlEditItem* pEdit = pPropertyCtrl->FindEditItem(pAttribute->GetKey());
        pEdit->SetValueText(pAttribute->Get());

        pPropertyCtrl->InvalidateItem(pEdit);
    }   
}
void  CUIEditor::EditorTextRenderBase(ITextRenderBaseAttribute* pAttribute, EditorAttributeFlag e)
{
    IPropertyCtrl*  pPropertyCtrl = GetPropertyCtrl();
    if (!pPropertyCtrl)
        return;

    if (EDITOR_ATTRIBUTE_ADD == e)
    {
		IPropertyCtrlGroupItem* pGroup = GetGroupItem(
			pAttribute->GetParentKey(),
			pAttribute->GetGroupName());

        PropertyCtrlEditItemInfo info = { 0 };
        info.szText = info.szKey = pAttribute->GetKey();
        info.szDesc = pAttribute->GetDesc();
        info.szValue = pAttribute->Get();

        pPropertyCtrl->InsertEditProperty(
			&info, pGroup, UITVI_LAST);
    }
    else if (EDITOR_ATTRIBUTE_UPDATE == e)
    {
        IPropertyCtrlEditItem* pEdit = pPropertyCtrl->FindEditItem(pAttribute->GetKey());
        pEdit->SetValueText(pAttribute->Get());

        pPropertyCtrl->InvalidateItem(pEdit);
    }
}

void  CUIEditor::RegisterStyleChangedCallback(StyleChangedCallback* p)
{
	m_listStyleItemEditorCallback.push_back(p);
}
void  CUIEditor::UnRegisterStyleChangedCallback(StyleChangedCallback* p)
{
	list<StyleChangedCallback*>::iterator iter = 
		std::find(m_listStyleItemEditorCallback.begin(), m_listStyleItemEditorCallback.end(), p);

	if (iter != m_listStyleItemEditorCallback.end())
		m_listStyleItemEditorCallback.erase(iter);
}

void  CUIEditor::OnStyleChanged(ISkinRes* pSkinRes, IStyleResItem* p, EditorOPType e)
{
	list<StyleChangedCallback*>::iterator iter = m_listStyleItemEditorCallback.begin();
	for (; iter != m_listStyleItemEditorCallback.end(); ++iter)
	{
		if ((*iter)->GetSkinRes() != pSkinRes)
			continue;
			
		switch (e)
		{
		case Editor_Add:
			(*iter)->OnStyleAdd(p);
			break;

		case Editor_Modify:
			(*iter)->OnStyleModify(p);
			break;

		case Editor_Remove:
			(*iter)->OnStyleRemove(p);
			break;
		}
	}
}

void  CUIEditor::OnStyleAttributeChanged(
		ISkinRes* pSkinRes, IStyleResItem* p, LPCTSTR szKey, EditorOPType e)
{
	list<StyleChangedCallback*>::iterator iter = m_listStyleItemEditorCallback.begin();
	for (; iter != m_listStyleItemEditorCallback.end(); ++iter)
	{
		if ((*iter)->GetSkinRes() != pSkinRes)
			continue;

		switch (e)
		{
		case Editor_Add:
			(*iter)->OnStyleAttributeAdd(p, szKey);
			break;

		case Editor_Modify:
			(*iter)->OnStyleAttributeModify(p, szKey);
			break;

		case Editor_Remove:
			(*iter)->OnStyleAttributeRemove(p, szKey);
			break;
		}
	}
}