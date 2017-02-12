#include "stdafx.h"
#include "LayoutPropertyHandler.h"
#include "Business\Command\AddObjectAttributeCommand.h"
#include "Business\Command\ModifyObjectAttributeCommand.h"
#include "Business\Command\RemoveObjectAttributeCommand.h"
#include "Business\Command\Base\commandhistorymgr.h"
#include "Business\Project\ProjectData\projectdata.h"
#include "UICTRL\Inc\Interface\iradiobutton.h"
#include "UISDK\Inc\Interface\iscrollbarmanager.h"
#include "Dialog\Framework\propertydialog.h"
#include "UIEditorCtrl\Inc\ipropertyctrl.h"

LayoutPropertyHandler::LayoutPropertyHandler()
{
    m_pPropertyDialog = NULL;
    m_pPropertyCtrl = NULL;
    m_pCommandEdit = NULL;
    m_pRadioBtnXmlAttr = NULL;
    m_pRadioBtnObjAttr = NULL;
    m_pRadioBtnStyleAttr = NULL;
    m_eCurAttrType = ObjAttr;
    m_nSearchStringsIndex = 0;

    m_pSkinRes = NULL;
    m_pObject = NULL;
    m_pXmlElement = NULL;
}

LayoutPropertyHandler::~LayoutPropertyHandler()
{
	DetachHandler();
}
void  LayoutPropertyHandler::OnLoad(CPropertyDialog* pWnd)
{
    if (m_pPropertyDialog)
        return;

    m_pPropertyDialog = pWnd;
    m_pPropertyCtrl = pWnd->GetPropertyCtrl();
    m_pCommandEdit = pWnd->GetCommandEdit();
    m_pRadioBtnObjAttr = (IRadioButton*)pWnd->FindObject(_T("radiobtn_attr1"));
    m_pRadioBtnXmlAttr = (IRadioButton*)pWnd->FindObject(_T("radiobtn_attr2"));
    m_pRadioBtnStyleAttr = (IRadioButton*)pWnd->FindObject(_T("radiobtn_attr3"));

    if (m_pCommandEdit)
    {
        m_pCommandEdit->EnChangeEvent().connect(this,
            &LayoutPropertyHandler::OnCommandEnChange);
        m_pCommandEdit->KeyDownEvent().connect(this,
            &LayoutPropertyHandler::OnCommandEditKeyDown);
    }

	if (m_pRadioBtnObjAttr)
	{
		m_pRadioBtnObjAttr->SetText(TEXT("Object"));
		m_pRadioBtnObjAttr->SetToolTipText(TEXT("显示对象的所有属性"));
		m_pRadioBtnObjAttr->Click();

        m_pRadioBtnObjAttr->ClickedEvent().connect(
            this, &LayoutPropertyHandler::OnRadioButtonAttr);
	}
	if (m_pRadioBtnXmlAttr)
	{
		m_pRadioBtnXmlAttr->SetText(TEXT("Xml"));
		m_pRadioBtnXmlAttr->SetToolTipText(TEXT("显示对象在xml中配置的属性"));

        m_pRadioBtnXmlAttr->ClickedEvent().connect(
            this, &LayoutPropertyHandler::OnRadioButtonAttr);
	}
	if (m_pRadioBtnStyleAttr)
	{
		m_pRadioBtnStyleAttr->SetText(TEXT("Style"));
		m_pRadioBtnStyleAttr->SetToolTipText(TEXT("显示对象由样式继承得到的属性"));

        m_pRadioBtnStyleAttr->ClickedEvent().connect(
            this, &LayoutPropertyHandler::OnRadioButtonAttr);
	}
}

void  LayoutPropertyHandler::OnUnload()
{
    ClearData();
    m_pPropertyDialog = NULL;

    if (m_pCommandEdit)
    {
        m_pCommandEdit->EnChangeEvent().disconnect();
        m_pCommandEdit->KeyDownEvent().disconnect();
    }

    if (m_pRadioBtnObjAttr)
        m_pRadioBtnObjAttr->ClickedEvent().disconnect();
    if (m_pRadioBtnXmlAttr)
        m_pRadioBtnXmlAttr->ClickedEvent().disconnect();
    if (m_pRadioBtnStyleAttr)
        m_pRadioBtnStyleAttr->ClickedEvent().disconnect();

    m_pPropertyCtrl = NULL;
    m_pCommandEdit = NULL;
    m_pRadioBtnXmlAttr = NULL;
    m_pRadioBtnObjAttr = NULL;
    m_pRadioBtnStyleAttr = NULL;
}

void  LayoutPropertyHandler::ClearData()
{
    // 保存当前编辑状态，用于切回来时恢复
    if (m_pObject)
        SaveObjectPropertyEditStatus(m_pObject);

    if (m_pPropertyCtrl)
        m_pPropertyCtrl->RemoveAllItem();

    m_pSkinRes = NULL;
    m_pObject = NULL;
    m_pXmlElement = NULL;
}

void  LayoutPropertyHandler::OnRadioButtonAttr(UI::IButton* p)
{
    if (m_pRadioBtnObjAttr->IsChecked())
        m_eCurAttrType = ObjAttr;
    else if (m_pRadioBtnXmlAttr->IsChecked())
        m_eCurAttrType = XmlAttr;
    else if (m_pRadioBtnStyleAttr->IsChecked())
        m_eCurAttrType = StyleAttr;
    else
        m_eCurAttrType = ObjAttr;

	AttachObject(m_pSkinRes, m_pObject);
}

bool  LayoutPropertyHandler::IsShowXmlAttr()
{
    if (!m_pRadioBtnXmlAttr)
        return false;

    return m_pRadioBtnXmlAttr->IsChecked();
}
bool  LayoutPropertyHandler::IsShowObjAttr()
{
    if (!m_pRadioBtnObjAttr)
        return false;

    return m_pRadioBtnObjAttr->IsChecked();
}

void  LayoutPropertyHandler::AttachHandler()
{
    g_pGlobalData->m_pPropertyDialog->AttachHandler(static_cast<IPropertyHandler*>(this));
}

void  LayoutPropertyHandler::DetachHandler()
{
	g_pGlobalData->m_pPropertyDialog->DetachHandler(static_cast<IPropertyHandler*>(this));
}

// 加载属性列表，并填充属性
void  LayoutPropertyHandler::AttachObject(ISkinRes* hSkin, IObject* pObj)
{
    if (!m_pPropertyCtrl)
	{
        AttachHandler();
		if (!m_pPropertyCtrl)
			return;
	}
	
    this->ClearData();

    if (NULL == hSkin || NULL == pObj)
    {
        return;
    }

    IUIElement* pXmlElem = g_pGlobalData->m_pProjectData->m_uieditor.GetObjectXmlElem(pObj);
    if (NULL == pXmlElem)
        return ;

    m_pSkinRes = hSkin;
    m_pObject = pObj;
    m_pXmlElement = pXmlElem;

    // 填充属性
    FillObjectAttribute();

    if (!::IsWindowVisible(GetPropertyDialog()->GetHWND()))
    {
        ::ShowWindow(GetPropertyDialog()->GetHWND(), SW_SHOWNOACTIVATE);
    }
}

// 重新加载属性
void   LayoutPropertyHandler::ReloadObjectAttribute(IObject* pObj)
{
    if (m_pObject != pObj)
    {
        if (pObj)
        {
            GetUIEditor()->DestroyObjectAttributeProxy(pObj);
        }
        return;
    }

    if (m_eCurAttrType == ObjAttr)
        SaveObjectPropertyEditStatus(m_pObject);

    // 删除老的属性数据
    GetUIEditor()->DestroyObjectAttributeProxy(pObj);

    // 清空
    m_pPropertyCtrl->RemoveAllItem();

    // 重新加载
    FillObjectAttribute();
}

// 由ReloadObjectAttribute或者AttachObject调用
// 只负责往列表中填充属性
void  LayoutPropertyHandler::FillObjectAttribute()
{
#pragma region	// 直接显示xml属性
    if (m_pRadioBtnXmlAttr->IsChecked())
    {
        IPropertyCtrlGroupItem* pParentNode = m_pPropertyCtrl->InsertGroupItem(
            _T("xml"), _T("自有属性"), UITVI_ROOT, UITVI_LAST);

        IListAttribute*  pListAttrib = NULL;
        m_pXmlElement->GetAttribList2(&pListAttrib);

        LPCTSTR  szKey = NULL;
        LPCTSTR  szValue = NULL;
        pListAttrib->BeginEnum();
        while (pListAttrib->EnumNext(&szKey, &szValue))
        {
			PropertyCtrlEditItemInfo info = {0};
			info.szText = info.szKey = szKey;
			info.szValue = szValue;

            m_pPropertyCtrl->InsertEditProperty(&info, pParentNode, UITVI_LAST);
        }
        pListAttrib->EndEnum();
        SAFE_RELEASE(pListAttrib);
    }
#pragma endregion
#pragma region	// 显示对象属性列表
    else if (m_pRadioBtnObjAttr->IsChecked())
    {
        // 2015.3.6 属性的插入，更新都由uiedtiorimpl完成
#if 0
        // 		插入styleclass结点
        // 		IPropertyCtrlEditItem*  pStyleclassItem = m_pPropertyCtrl->InsertEditProperty(XML_STYLECLASS, L"", L"", XML_STYLECLASS, NULL, UITVI_LAST, LISTITEM_OPFLAG_NOALL);
        IPropertyCtrlEditItem*  pStyleclassItem = (IPropertyCtrlEditItem*)m_pPropertyCtrl->GetFirstItem();

        IMapAttribute*  pMapAttr = NULL;  // 该属性列表将被样式管理器解析
        IMapAttribute*  pMapAttrSelf = NULL;  // 用于区分一个属性值是自己的，还是来自样式的
        m_pXmlElement->GetAttribList(&pMapAttr);
        m_pXmlElement->GetAttribList(&pMapAttrSelf);

        LPCTSTR szStyleclass = pMapAttr->GetAttr(XML_STYLECLASS, false);
        if (szStyleclass && pStyleclassItem)
            pStyleclassItem->SetValueText(szStyleclass);

        // 填充属性，如果没有的就直接往后插入
        IStyleManager*  pStyleMgr = m_pSkinRes->GetStyleMgr();
        pStyleMgr->ParseStyle(pMapAttr);

        LPCTSTR  szKey = NULL;
        LPCTSTR  szValue = NULL;
        IPropertyCtrlGroupItem* pParentOtherNode = NULL;

        pMapAttr->BeginEnum();
        while (pMapAttr->EnumNext(&szKey, &szValue))
        {
            IListItemBase* pItem = FindPropertyItemByKey(szKey);
            if (pItem)
            {
                UISendMessage(pItem, UI_PROPERTYCTRLITEM_MSG_SETVALUESTRING, (WPARAM)szValue);
            }
            else
            {
                if (!pParentOtherNode)
                {
                    pParentOtherNode = m_pPropertyCtrl->InsertGroupItem(_T("其它属性"), NULL, UITVI_ROOT, UITVI_LAST, 0);
                }
                pItem = m_pPropertyCtrl->InsertEditProperty(szKey, szValue, NULL, szKey, pParentOtherNode, UITVI_LAST, 0);
            }

            // 是自己的，就不要设置默认值了。 是样式的就设置默认值，当该值被用户修改为与样式的值一样的，就将该属性删除
            if (!pMapAttrSelf->GetAttr(szKey, false))  
            {
                UISendMessage(pItem, UI_PROPERTYCTRLITEM_MSG_SETDEFAULTVALUESTRING, (WPARAM)szValue);
            }
        }

        pMapAttr->EndEnum();
        SAFE_RELEASE(pMapAttr);
        SAFE_RELEASE(pMapAttrSelf);
#endif
        g_pGlobalData->m_pProjectData->m_uieditor.LoadAttribute2PropertyCtrl(m_pObject);

        // 加载完成后，加载属性的上一次编辑状态
        LoadObjectPropertyEditStatus(m_pObject);
    }
#pragma endregion
#pragma region // 对象的样式
    else if (m_pRadioBtnStyleAttr->IsChecked())
    {
        CComBSTR bstrId;
        CComBSTR bstrStyleClass;
        m_pXmlElement->GetAttrib(XML_ID, &bstrId);
        m_pXmlElement->GetAttrib(XML_STYLECLASS, &bstrStyleClass);

        String strId, strStyleClass;
        if (bstrId.Length() > 0)
            strId = (BSTR)bstrId;
        if (bstrStyleClass.Length() > 0)
            strStyleClass = (BSTR)bstrStyleClass;

        //////////////////////////////////////////////////////////////////////////
        // ID 属性

        AttachObject_IDAttrib(m_pSkinRes, m_pObject, strId);

        //////////////////////////////////////////////////////////////////////////
        // Class 属性

        AttachObject_ClassAttrib(m_pSkinRes, m_pObject, strStyleClass);

        //////////////////////////////////////////////////////////////////////////
        // Tag 属性

        IObjectDescription* pDesc = m_pObject->GetDescription();
        AttachObject_TagAttrib(m_pSkinRes, m_pObject, 
            pDesc ? pDesc->GetTagName() : TEXT(""));
    }
#pragma endregion
}

void LayoutPropertyHandler::AttachObject_IDAttrib(ISkinRes* hSkin, IObject* pObj, const String& strID)
{
    if (strID.empty())
        return;

    IStyleRes& rStyleRes = m_pSkinRes->GetStyleRes();
    IStyleResItem* pInfo = rStyleRes.FindItem(STYLE_SELECTOR_TYPE_ID, strID.c_str());
    if (NULL == pInfo)
        return;

    IMapAttribute* pMapAttr = NULL;
    pInfo->GetAttributeMap(&pMapAttr);
    if (NULL == pMapAttr)
        return;

    String strGroupName(_T("Id样式 - "));
    strGroupName.append(strID);
    IPropertyCtrlGroupItem* pGroup = NULL;

    LPCTSTR  szKey = NULL;
    LPCTSTR  szValue = NULL;
    pMapAttr->BeginEnum();
    while (pMapAttr->EnumNext(&szKey, &szValue))
    {
        if (!pGroup)
            pGroup = m_pPropertyCtrl->InsertGroupItem(
            strGroupName.c_str(), _T("Id样式"), UITVI_ROOT, UITVI_LAST);

		PropertyCtrlEditItemInfo info = {0};
		info.szText = info.szKey = szKey;
		info.szValue = szValue;

        m_pPropertyCtrl->InsertEditProperty(&info, pGroup, UITVI_LAST);
    }

    SAFE_RELEASE(pMapAttr);
}

void LayoutPropertyHandler::AttachObject_ClassAttrib(ISkinRes* hSkin, IObject* pObj, const String& strClassStyle)
{
    if (strClassStyle.empty())
        return;

    IStyleRes& rStyleRes = m_pSkinRes->GetStyleRes();

    Util::ISplitStringEnum*  pEnum = NULL;
    int nCount = Util::SplitString(strClassStyle.c_str(), XML_SEPARATOR, &pEnum);
    if (0 == nCount)
        return;

    for (int i = 0; i < nCount; i++)
    {
        LPCTSTR szText = pEnum->GetText(i);
        if (NULL == szText || _tcslen(szText) == 0)
            continue;

        String strGroupName(_T("Class样式 - "));
        strGroupName.append(szText);

        IStyleResItem* pInfo = rStyleRes.FindItem(STYLE_SELECTOR_TYPE_CLASS, szText);
        if (NULL == pInfo)
            continue;

        IMapAttribute* pMapAttr = NULL;
        pInfo->GetAttributeMap(&pMapAttr);
        if (!pMapAttr)
            continue;

        IPropertyCtrlGroupItem* pGroup = m_pPropertyCtrl->InsertGroupItem(
            strGroupName.c_str(), _T("Class样式"), UITVI_ROOT, UITVI_LAST);

        LPCTSTR  szKey = NULL;
        LPCTSTR  szValue = NULL;
        pMapAttr->BeginEnum();
        while (pMapAttr->EnumNext(&szKey, &szValue))
        {
			PropertyCtrlEditItemInfo info = {0};
			info.szText = info.szKey = szKey;
			info.szValue = szValue;

            m_pPropertyCtrl->InsertEditProperty(&info, pGroup, UITVI_LAST);
        }

        SAFE_RELEASE(pMapAttr);
    }

    SAFE_RELEASE(pEnum);
}
void LayoutPropertyHandler::AttachObject_TagAttrib(ISkinRes* hSkin, IObject* pObj, const String& strTagName)
{
    //    m_pPropertyCtrl->InsertGroupItem(_T("Tag样式"), _T("Tag样式"), UITVI_ROOT, UITVI_LAST, LISTITEM_OPFLAG_NOALL);

    if (strTagName.empty())
        return;

    IStyleRes& rStyleRes = m_pSkinRes->GetStyleRes();
    IStyleResItem* pInfo = rStyleRes.FindItem(STYLE_SELECTOR_TYPE_TAG, strTagName.c_str());
    if (!pInfo)
        return;

    IMapAttribute* pMapAttr = NULL;
    pInfo->GetAttributeMap(&pMapAttr);
    if (!pMapAttr)
        return;

    String strName(_T("Tag样式"));
    strName.append(strTagName);
    IPropertyCtrlGroupItem* pGroup = m_pPropertyCtrl->InsertGroupItem(
        strName.c_str(), NULL, UITVI_ROOT, UITVI_LAST);

    LPCTSTR  szKey = NULL;
    LPCTSTR  szValue = NULL;
    pMapAttr->BeginEnum();
    while (pMapAttr->EnumNext(&szKey, &szValue))
    {
		PropertyCtrlEditItemInfo info = {0};
		info.szText = info.szKey = szKey;
		info.szValue = szValue;

        m_pPropertyCtrl->InsertEditProperty(&info, pGroup, UITVI_LAST);
    }

    SAFE_RELEASE(pMapAttr);
}


void  LayoutPropertyHandler::OnAcceptContent(LPCTSTR szKey, LPCTSTR szNewValue/*, bool bDefault, bool& bNeedSetItemValue*/)
{
#if 1
    if (0 == UE_ExecuteCommand(
        ModifyObjectAttributeCommand2::CreateInstance(
        m_pObject, szKey, szNewValue)))
    {
        String str = _T("Modify ");
        str.append(szKey);
        str.append(_T(" =\""));
        str.append(szNewValue);
        str.append(_T("\" 成功"));
        ShowStatusText(str.c_str());
    }
#else

    bNeedSetItemValue = false;
    if (IsShowObjAttr())  // 对象属性列表，在获取属性列表时可以设置默认属性，通过默认属性可以用于判断是否删除该属性
    {
        if (bDefault)
        {
            String strKey = szKey;
            String strValue = szNewValue;

            if (0 == UE_ExecuteCommand(RemoveObjectAttributeCommand::CreateInstance(
                m_pSkinRes, m_pObject, m_pXmlElement, szKey)
                ))
            {
                String str = _T("Remove ");
                str.append(strKey.c_str());
                str.append(_T(" =\""));
                str.append(strValue.c_str());
                str.append(_T("\" 成功"));
                ShowStatusText(str.c_str());
            }
        }
        else
        {
            if (0 == UE_ExecuteCommand(
                ModifyObjectAttributeCommand::CreateInstance(
                m_pSkinRes, m_pObject, m_pXmlElement, szKey, szNewValue)))
            {
                bNeedSetItemValue = true;

                String str = _T("Modify ");
                str.append(szKey);
                str.append(_T(" =\""));
                str.append(szNewValue);
                str.append(_T("\" 成功"));
                ShowStatusText(str.c_str());
            }
            else if (0 == UE_ExecuteCommand(  // 这个属性不存在，改为创建属性
                AddObjectAttributeCommand::CreateInstance(
                m_pSkinRes, m_pObject, m_pXmlElement, szKey, szNewValue)))
            {
                String str = _T("Add ");
                str.append(szKey);
                str.append(_T(" =\""));
                str.append(szNewValue);
                str.append(_T("\" 成功"));
                ShowStatusText(str.c_str());
            }
        }
    }
    else
    {
        if (0 == UE_ExecuteCommand(
            ModifyObjectAttributeCommand::CreateInstance(
            m_pSkinRes, m_pObject, m_pXmlElement, szKey, szNewValue)))
        {
            bNeedSetItemValue = true;

            String str = _T("Modify ");
            str.append(szKey);
            str.append(_T(" =\""));
            str.append(szNewValue);
            str.append(_T("\" 成功"));
            ShowStatusText(str.c_str());
        }
    }
#endif
}

LRESULT  LayoutPropertyHandler::OnEditItemAcceptContent(WPARAM wParam, LPARAM lParam)
{
    PROPERTYCTRL_EDIT_ACCEPTCONTENT* pParam = (PROPERTYCTRL_EDIT_ACCEPTCONTENT*)wParam;
    IPropertyCtrlEditItem*  pItem = (IPropertyCtrlEditItem*)pParam->pItem;

    OnAcceptContent(pParam->szKey, pParam->szNewValue);
    return 0;
}
LRESULT  LayoutPropertyHandler::OnComboBoxItemAcceptContent(WPARAM wParam, LPARAM lParam)
{
    PROPERTYCTRL_COMBOBOX_ITEM_ACCEPTCONTENT* pParam = (PROPERTYCTRL_COMBOBOX_ITEM_ACCEPTCONTENT*)wParam;
    IPropertyCtrlComboBoxItem*  pItem = (IPropertyCtrlComboBoxItem*)pParam->pItem;

    OnAcceptContent(pParam->szKey, pParam->szNewValue);
    return 0;
}
LRESULT  LayoutPropertyHandler::OnDelayReloadObjectAttr(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    ReloadObjectAttribute(m_pObject);
    return 0;
}

bool  LayoutPropertyHandler::OnCommandEditKeyDown(IEdit*, UINT w)
{
    if (w == VK_ESCAPE)
    {
        m_pCommandEdit->SetText(NULL);
        return true;
    }
    else if (w == VK_RETURN)
    {
        LPCTSTR szText = m_pCommandEdit->GetText();

        if (0 == DoCommand(szText))
        {
            // 成功后用全选进行通知
            m_pCommandEdit->SetSel(0, -1);
        }
		return true;
    }
    else if (w == VK_TAB)
    {
        OnCommandEditTab();
		return true;
    }

	return false;
}

void  LayoutPropertyHandler::OnCommandEnChange(IEdit*, bool bSetText)
{
    if (bSetText)
        return;

    if (m_vSearchStrings.size() > 0)
    {
        m_vSearchStrings.clear();
        m_nSearchStringsIndex = -1;
    }
}   

void LayoutPropertyHandler::OnCommandEditTab()
{
    LPCTSTR  szText = m_pCommandEdit->GetText();

    // 判断当前在写什么内容，命令还是第一个参数 
    String  strText(szText);
    TrimStringLeft(strText);

    if (strText.empty())
        return;

    // 命令, 懒得写那么复杂，暂时只支持一个字母进行扩展
    if (String::npos == strText.find_first_of(_T(' '))) 
    {
        if (strText.length() != 1)
            return;

        switch (strText[0])
        {
        case _T('a'): 
            m_pCommandEdit->SetText(_T("add "));
            break;
        case _T('c'):
            m_pCommandEdit->SetText(_T("clear "));
            break;
        case _T('d'):
            m_pCommandEdit->SetText(_T("delete "));
            break;
        case _T('e'):
            m_pCommandEdit->SetText(_T("erase "));
            break;
        case _T('i'):
            m_pCommandEdit->SetText(_T("insert "));
            break;
        case _T('m'):
            m_pCommandEdit->SetText(_T("modify "));
            break;
        case _T('r'):
            m_pCommandEdit->SetText(_T("remove "));
            break;
        }
        return;
    }

    // 对第一个参数进行扩展
    String  strCommand, strArgs;
    ParsePropertyCommand(szText, strCommand, strArgs);
    if (strArgs.empty())
        return;
    if (String::npos != strArgs.find_first_of(_T(' ')))
        return;

    IListItemBase*  pItem = m_pPropertyCtrl->GetFirstItem();
    if (NULL == pItem)
        return;

    if (0 == m_vSearchStrings.size())
    {
        m_strCurSearchPrefix = strArgs;

        pItem = pItem->GetChildItem();
        while (pItem)
        {
            LPCTSTR szText = pItem->GetText();
            if (szText == _tcsstr(szText, strArgs.c_str()))
            {
                m_vSearchStrings.push_back(szText);
            }   
            pItem = pItem->GetNextItem();
        }
    }

    if (m_vSearchStrings.size() > 0)
    {
        m_nSearchStringsIndex++;
        if (m_nSearchStringsIndex >= (int)m_vSearchStrings.size())
            m_nSearchStringsIndex = 0;

        String str = strCommand;
        str.append(_T(" "));
        str.append(m_vSearchStrings[m_nSearchStringsIndex]);
        m_pCommandEdit->SetText(str.c_str());
    }
}

// 保存对象在propertyctrl中的编辑状态
void  LayoutPropertyHandler::SaveObjectPropertyEditStatus(IObject* pObj)
{   
    if (m_eCurAttrType != ObjAttr)
        return;
    if (!pObj || m_pObject != pObj)
        return;
    ObjectEditorData* pData = GetUIEditor()->GetObjectEditorData(m_pObject);
    if (!pData)
        return;

    pData->m_editStatus.Clear();

    IListItemBase* pItem = m_pPropertyCtrl->GetFirstItem();
    while (pItem)
    {
        if (pItem->GetItemFlag() == IPropertyCtrlGroupItem::FLAG)
        {
            if (pItem->IsCollapsed())
            {
                if (pItem->GetParentItem())
                    pData->m_editStatus.AddCollapsedGroup(pItem->GetParentItem()->GetText(), pItem->GetText());
                else
                    pData->m_editStatus.AddCollapsedGroup(NULL, pItem->GetText());
            }
        }
        pItem = pItem->GetNextTreeItem();
    }

    pData->m_editStatus.SetVScrollPos(m_pPropertyCtrl->GetIScrollBarMgr()->GetVScrollPos());
    if (m_pPropertyCtrl->GetFirstSelectItem())
    {
        pData->m_editStatus.SetSelectedItem(m_pPropertyCtrl->GetFirstSelectItem()->GetText());
    }
}

// 还原对象在propertyctrl中的编辑状态
void  LayoutPropertyHandler::LoadObjectPropertyEditStatus(IObject* pObj)
{
    if (!pObj || m_pObject != pObj)
        return;
    ObjectEditorData* pData = GetUIEditor()->GetObjectEditorData(m_pObject);
    if (!pData)
        return;

    IListItemBase* pItem = m_pPropertyCtrl->GetFirstItem();
    while (pItem)
    {
        if (pItem->GetItemFlag() == IPropertyCtrlGroupItem::FLAG)
        {
            LPCTSTR szParentKey = NULL;
            if (pItem->GetParentItem())
                szParentKey = pItem->GetParentItem()->GetText();

            if (pData->m_editStatus.IsGroupCollapsed(szParentKey, pItem->GetText()))
            {
                m_pPropertyCtrl->CollapseItem(pItem, false);
            }
        }
        pItem = pItem->GetNextTreeItem();
    }
    pItem = m_pPropertyCtrl->FindItemByKey(pData->m_editStatus.GetSelectedItem());
    if (pItem)
        m_pPropertyCtrl->SelectItem(pItem, false);

    m_pPropertyCtrl->LayoutItem(NULL, false);
    m_pPropertyCtrl->GetIScrollBarMgr()->SetVScrollPos(pData->m_editStatus.GetVScrollPos());
}


long  LayoutPropertyHandler::AddAttributeInCtrl(LPCTSTR szKey, LPCTSTR szValue)
{
    if (!m_pPropertyCtrl)
        return -1;

    if (IsShowXmlAttr())
    {
        IListItemBase* pParent = m_pPropertyCtrl->GetFirstItem();
        if (pParent)
        {
			PropertyCtrlEditItemInfo info = {0};
			info.szText = info.szKey = szKey;
			info.szValue = szValue;

            m_pPropertyCtrl->InsertEditProperty(&info, pParent);
        }
    }
    else
    {
        IListItemBase* pItem = m_pPropertyDialog->FindPropertyItemByKey(szKey);
        if (pItem)
        {
            UISendMessage(pItem, UI_PROPERTYCTRLITEM_MSG_SETVALUESTRING, (WPARAM)szValue);
            m_pPropertyCtrl->InvalidateItem(pItem);
        }
        else
        {
			PropertyCtrlEditItemInfo info = {0};
			info.szText = info.szKey = szKey;
			info.szValue = szValue;

            m_pPropertyCtrl->InsertEditProperty(&info, NULL);
        }
    }
    return 0;
}

// 用该对象的实际值更新属性控件中对应的列表项值。场景：ctrl+z还原属性
void   LayoutPropertyHandler::UpdateObjectAttributeInCtrl(
            IObject* pObj, LPCTSTR szKey)
{
    if (m_pObject != pObj)
        return;

    ObjectEditorData* pData = GetUIEditor()->GetObjectEditorData(pObj);
    if (!pData)
        return;

	if (!pData->pAttributeProxy)
	{
		// 场景：ModifyObjectAttributeCommand2.on_post_do中修改了styleclass属性，
		//       会先将pAttributeProxy销毁掉了，因此直接reload
		::PostMessage(
            GetPropertyDialog()->GetHWND(),
            MESSAGE_DELAY_RELOAD_OBJECT_ATTR,
            0, 0);

		return;
	}

    // 对于bkg.render.type属性修改，需要重新加载整个列表
	UpdateAttribute2EditorResult eResult =
        pData->pAttributeProxy->UpdateAttribute2Editor(szKey);

    if (UpdateAttribute2EditorNeedReload == eResult)
    {
        // 重新加载.延迟，以防当前在item change notify堆栈上面
		::PostMessage(
            GetPropertyDialog()->GetHWND(),
            MESSAGE_DELAY_RELOAD_OBJECT_ATTR,
            0, 0);
    }
}

long   LayoutPropertyHandler::ModifyAttributeInCtrl(
            LPCTSTR szKey, LPCTSTR szValue)
{
    if (!m_pPropertyCtrl)
        return -1;

    IListItemBase* pItem = m_pPropertyDialog->FindPropertyItemByKey(szKey);
    if (!pItem)
        return -1;

    UISendMessage(
        pItem,
        UI_PROPERTYCTRLITEM_MSG_SETVALUESTRING,
        (WPARAM)szValue);
    m_pPropertyCtrl->InvalidateItem(pItem);
    return 0;
}

long  LayoutPropertyHandler::RemoveAttribInCtrl(LPCTSTR szKey)
{
    if (!m_pPropertyCtrl)
        return -1;

    IListItemBase* pItem = m_pPropertyDialog->FindPropertyItemByKey(szKey);
    if (!pItem)
        return -1;

    if (IsShowXmlAttr())
    {
        m_pPropertyCtrl->RemoveItem(pItem);
    }
    else if (IsShowObjAttr())
    {
        UISendMessage(pItem, UI_PROPERTYCTRLITEM_MSG_SETVALUESTRING, NULL);
        m_pPropertyCtrl->InvalidateItem(pItem);
    }
    return 0;
}
long  LayoutPropertyHandler::ClearAttribInCtrl()
{
    if (!m_pPropertyCtrl)
        return -1;

    if (!IsShowXmlAttr())
        return -1;

    IListItemBase* pFirstItem = m_pPropertyCtrl->GetFirstItem();
    m_pPropertyCtrl->RemoveAllChildItems(pFirstItem);
    return 0;
}

void  LayoutPropertyHandler::ShowStatusText(LPCTSTR  szText)
{
    g_pGlobalData->m_pStatusBar->SetStatusText2(szText);
}

long LayoutPropertyHandler::OnAddGroupProperty(IObject* pObj, LPCTSTR szGroupName)
{
#if 0
    LVGROUP group = {0};
    group.cbSize = sizeof(LVGROUP);
    group.mask = LVGF_HEADER|LVGF_GROUPID|LVGF_ALIGN|LVGF_STATE;
    group.uAlign = LVGA_HEADER_LEFT;
    group.iGroupId = m_listctrlObjectAttrib.GetGroupCount();
    group.pszHeader = (LPWSTR) szGroupName;
    group.state = LVGS_COLLAPSIBLE;
    m_listctrlObjectAttrib.AddGroup(&group);

    return group.iGroupId;
#endif

    IPropertyCtrlGroupItem* pParentNode = m_pPropertyCtrl->InsertGroupItem(
        szGroupName, NULL, UITVI_ROOT, UITVI_LAST);
    return (long)pParentNode;
}
void LayoutPropertyHandler::OnAddProperty(IObject* pObj, long nGroupID, LPCTSTR szName)
{
#if 0
    int iItemCount = m_listctrlObjectAttrib.GetItemCount();

    LVITEM item = {0};
    item.mask = LVIF_GROUPID|LVIF_TEXT;
    item.iItem = iItemCount;
    item.iGroupId = nGroupID;
    item.pszText = (LPWSTR)szName;

    int nIndex = m_listctrlObjectAttrib.InsertItem(&item);
    //m_listctrlObjectAttrib.SetItemText(nIndex, 1, iter2->second.c_str());
#endif

	PropertyCtrlEditItemInfo info = {0};
	info.szText = info.szKey = szName;

    m_pPropertyCtrl->InsertEditProperty(
        &info, (IPropertyCtrlGroupItem*)nGroupID, UITVI_LAST);
}