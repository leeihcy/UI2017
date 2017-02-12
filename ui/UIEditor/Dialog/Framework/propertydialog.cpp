#include "StdAfx.h"
#include "PropertyDialog.h"
#include "UISDK\Inc\Interface\ixmlwrap.h"
#include "statusbar.h"
#include "Business\uieditorfunction.h"
#include "UICTRL\Inc\Interface\irichedit.h"
#include "UICTRL\Inc\Interface\iedit.h"
#include "UIEditorCtrl\Inc\ipropertyctrl.h"
#include "UICTRL\Inc\Interface\irichtext.h"


CPropertyDialog::CPropertyDialog(void) : IWindow(CREATE_IMPL_TRUE)
{
    m_pPropertyHandler = NULL;
	m_pPropertyCtrl = NULL;
    m_pCommandEdit = NULL;
	g_pGlobalData->m_pPropertyDialog = this;
    m_pRichEditDescript = NULL;
}

CPropertyDialog::~CPropertyDialog(void)
{
	if (g_pGlobalData)
	{
		g_pGlobalData->m_pPropertyDialog = NULL;
	}
}

BOOL  CPropertyDialog::OnInitDialog()
{
	m_pPropertyCtrl = (IPropertyCtrl*)this->FindObject(_T("propertyctrl"));
    if (m_pPropertyCtrl)
    {
        m_pPropertyCtrl->SelectChangedEvent().connect(
            this, &CPropertyDialog::OnPropertyCtrlSelChange);
    }
    m_pCommandEdit = (IEdit*)this->FindObject(_T("edit_command"));
    m_pRichEditDescript = (IRichText*)this->FindObject(_T("property_desc"));

    if (m_pCommandEdit)
    {
        SetFocusObject(m_pCommandEdit);
        m_pCommandEdit->SetWantTab(true);
    }

    if (m_pRichEditDescript)
    {
        m_pRichEditDescript->SetEditMode(UI::RT::EDITABLE_Readonly);
    }

	return TRUE;
}

void  CPropertyDialog::OnClose()
{
	::ShowWindow(GetHWND(), SW_HIDE);
}
void  CPropertyDialog::OnDestroy()
{
    if (m_pPropertyHandler)
    {
        m_pPropertyHandler->OnUnload();
        m_pPropertyHandler = NULL;
    }
	m_pPropertyCtrl = NULL;
}

// 获取指定key字段的属性项回调函数
bool ObjXmlPropertyCtrlListItemEnumProc(IListItemBase* pItem, WPARAM w, LPARAM l)
{
    LPCTSTR szKey = (LPCTSTR)w;
    LPCTSTR szItemKey = (LPCTSTR)UISendMessage(pItem, UI_PROPERTYCTRLITEM_MSG_GETKEYSTRING);
    if (!szItemKey)
        return true;

    if (0 == _tcscmp(szKey, szItemKey))
        return false;
    return true;
}
IListItemBase*  CPropertyDialog::FindPropertyItemByKey(LPCTSTR szKey)
{
    IListItemBase* pItem = m_pPropertyCtrl->EnumItemByProc(
        ObjXmlPropertyCtrlListItemEnumProc, NULL, (WPARAM)szKey,NULL);    
    return pItem;
    return NULL;
}

void  CPropertyDialog::ShowStatusText(LPCTSTR  szText)
{
    g_pGlobalData->m_pStatusBar->SetStatusText2(szText);
}

void CPropertyDialog::OnPropertyCtrlSelChange(UI::IListCtrlBase*)
{
	IListItemBase*  pItem = m_pPropertyCtrl->GetFirstSelectItem();
	if (!pItem)
	{
		SetDecsript(NULL,NULL);
		return;
	}

    String strDesc;
    if (pItem->GetToolTip())
        strDesc.append(pItem->GetToolTip());

    // 显示内容，可方便复制
    LPCTSTR szValue = (LPCTSTR)UISendMessage(
            pItem, UI_PROPERTYCTRLITEM_MSG_GETVALUESTRING);
    if (szValue)
        strDesc.append(szValue);
	SetDecsript(pItem->GetText(), strDesc.c_str());

}

void   CPropertyDialog::SetDecsript(LPCTSTR szKey, LPCTSTR  szDesc)
{
	m_pRichEditDescript->SetText(_T(""));
	if (!szKey)
		return ;

    // 粗体字
    UI::CharFormat format;
    m_pRichEditDescript->GetDefaultCharFormat(&format);
    format.bBold = 1;
	m_pRichEditDescript->ReplaceTextEx(szKey, &format);

	// 描述内容
	m_pRichEditDescript->ReplaceText(_T("\r\n"));
	if (szDesc)
	{
		m_pRichEditDescript->ReplaceText(szDesc);	
	}
}

IPropertyHandler*  CPropertyDialog::GetHandler()
{
	return m_pPropertyHandler;
}

bool  CPropertyDialog::AttachHandler(IPropertyHandler* p)
{
	if (m_pPropertyHandler == p)
		return true;

    if (m_pPropertyHandler)
    {
        DetachHandler(m_pPropertyHandler);
    }
    m_pPropertyHandler = p;
    if (m_pPropertyHandler)
    {
        m_pPropertyHandler->OnLoad(this);
    }

    return true;
}

bool  CPropertyDialog::DetachHandler(IPropertyHandler* p)
{
	if (!p || m_pPropertyHandler != p)
		return false;

	m_pPropertyHandler->OnUnload();
	m_pPropertyHandler = NULL;

	return true;
}