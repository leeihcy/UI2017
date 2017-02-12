#pragma once
#include "UIEditorCtrl\Inc\ipropertyctrl.h"

namespace UI
{

interface IEdit;
interface IButton;
interface IComboBox;

class PropertyCtrl : public MessageProxy
{
public:
    PropertyCtrl(IPropertyCtrl* p);
    ~PropertyCtrl();

    UI_BEGIN_MSG_MAP()
        UIMSG_SIZE(OnSize)
        UIMSG_HANDLER_EX(UI_PROPERTYCTRL_MSG_GETSPLITTERLINEPOS_PTR, OnGetSplitterLinePosPtr)
        UIMSG_HANDLER_EX(UI_MSG_SHOW_TOOLTIP, OnPreShowTooltip)
        UIMSG_HANDLER_EX(UI_PROPERTYCTRL_MSG_GETEDITCTRL, OnGetEditCtrl)
		UIMSG_HANDLER_EX(UI_PROPERTYCTRL_MSG_GETCOMBOBOXCTRL, OnGetComboBoxCtrl)
		UIMSG_HANDLER_EX(UI_PROPERTYCTRL_MSG_GETBUTTONCTRL, OnGetButtonCtrl)
        UIMSG_QUERYINTERFACE(PropertyCtrl)
        UIMSG_INITIALIZE(OnInitialize)
        UIMSG_INITIALIZE2(OnInitialize2)
        UIMSG_FINALCONSTRUCT(FinalConstruct)
        UIMSG_FINALRELEASE(FinalRelease)
// 	UIALT_MSG_MAP(UIALT_CALLLESS)
// 		UIMSG_WM_CREATEBYEDITOR(OnCreateByEditor)
    UI_END_MSG_MAP_CHAIN_PARENT_Ixxx(PropertyCtrl, ITreeView)
  
public:
    IPropertyCtrl*  GetIPropertyCtrl(IPropertyCtrl* p) { return m_pIPropertyCtrl; }
        
    IPropertyCtrlGroupItem*  InsertGroupItem(
        LPCTSTR szName, 
        LPCTSTR szDesc, 
        IListItemBase* pParent = UITVI_ROOT, 
        IListItemBase* pInsertAfter = UITVI_LAST);

    IPropertyCtrlGroupItem*  FindGroupItem(IListItemBase* pParentItem, LPCTSTR szName);
    IListItemBase*  FindItemByKey(LPCTSTR szKey);

	IPropertyCtrlEditItem*   InsertEditProperty(
        PropertyCtrlEditItemInfo* pInfo,
        IListItemBase* pParentItem, 
        IListItemBase* pInsertAfter = UITVI_LAST);

    IPropertyCtrlLongItem*   InsertLongProperty(
        PropertyCtrlLongItemInfo* pInfo, 
        IListItemBase* pParentItem, 
        IListItemBase* pInsertAfter = UITVI_LAST);

	IPropertyCtrlBoolItem*   InsertBoolProperty(
		PropertyCtrlBoolItemInfo* pInfo,
		IListItemBase* pParentItem, 
		IListItemBase* pInsertAfter = UITVI_LAST);

	IPropertyCtrlComboBoxItem*   InsertComboBoxProperty(
		PropertyCtrlComboBoxItemInfo* pInfo,
		IListItemBase* pParentItem, 
		IListItemBase* pInsertAfter = UITVI_LAST);

	IPropertyCtrlButtonItem*   InsertButtonProperty(
		LPCTSTR szText, 
		LPCTSTR szValue, 
		LPCTSTR szDesc, 
		LPCTSTR szKey,
		IListItemBase* pParentItem, 
		IListItemBase* pInsertAfter = UITVI_LAST);

	IPropertyCtrlAdditionItem*  AddAdditionItem();

	IListItemBase*  FindItem(LPCTSTR szKey);
	IPropertyCtrlEditItem*  FindEditItem(LPCTSTR szKey);
    IPropertyCtrlLongItem*  FindLongItem(LPCTSTR szKey);
    IPropertyCtrlComboBoxItem*  FindComboBoxItem(LPCTSTR szKey);
	IPropertyCtrlBoolItem*  FindBoolItem(LPCTSTR szKey);
    // InsertColorProperty
    // Insertxxxx

protected:
    HRESULT  FinalConstruct(ISkinRes* p);
    void  FinalRelease();
	void  OnInitialize();
    void  OnInitialize2();
//	void  OnCreateByEditor(CREATEBYEDITORDATA* pData);
    void  OnSize(UINT nType, int cx, int cy);

    LRESULT  OnGetSplitterLinePosPtr(UINT uMsg, WPARAM wParam, LPARAM lParam);
    LRESULT  OnGetEditCtrl(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT  OnGetComboBoxCtrl(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT  OnGetButtonCtrl(UINT uMsg, WPARAM wParam, LPARAM lParam);
    LRESULT  OnPreShowTooltip(UINT uMsg, WPARAM wParam, LPARAM lParam);

protected:
    IEdit*      m_pEdit;
    IButton*    m_pButton;
    IComboBox*  m_pComboBox;

private:
    IPropertyCtrl*  m_pIPropertyCtrl;
	IPropertyCtrlAdditionItem*  m_pAdditionItem;

    UINT  m_nSplitterLinePercent;   // 分隔线位置，用千分比来表示
    UINT  m_nSplitterLinePos;       // 分隔线位置，px，由percent计算而来
};

}

