#ifndef _UI_IPROPERTYCTRL_H_
#define _UI_IPROPERTYCTRL_H_
#include "..\..\UICTRL\Inc\Interface\itreeview.h"

namespace UI
{
// 属性控件线条及背景颜色
#define PROPERTYCTRL_LINE_COLOR 212,208,200,255  //(233,236,250,255);

// 获取属性控件分隔条的位置指针（相对于控件）
// RETURN：UINT*
#define UI_PROPERTYCTRL_MSG_GETSPLITTERLINEPOS_PTR 136152136

// 获取EDIT控件
// RETURN: IEdit*
#define UI_PROPERTYCTRL_MSG_GETEDITCTRL 136162254
#define UI_PROPERTYCTRL_MSG_GETCOMBOBOXCTRL 136162255
#define UI_PROPERTYCTRL_MSG_GETBUTTONCTRL 136162256


interface IPropertyCtrlEditItem;
struct PROPERTYCTRL_EDIT_ACCEPTCONTENT
{
	IPropertyCtrlEditItem*  pItem;
	LPCTSTR  szKey;
	LPCTSTR  szNewValue;
};
// Edititem接受编辑内容
// WPARAM: IPropertyCtrlEditItem*
// LPARAM: LPCTSTR newText
#define UI_PROPERTYCTRL_NM_EDITITEM_ACCEPTCONTENT 136221752

interface IPropertyCtrlComboBoxItem;
struct PROPERTYCTRL_COMBOBOX_ITEM_ACCEPTCONTENT
{
	IPropertyCtrlComboBoxItem*  pItem;
	LPCTSTR  szKey;
	LPCTSTR  szNewValue;
    long  lNewValue;
};
#define UI_PROPERTYCTRL_NM_COMBOBOXITEM_ACCEPTCONTENT 136221753

// 外部给IListItemBase发送消息，设置属性的值
// WPARAM: LPCTSTR szText
#define UI_PROPERTYCTRLITEM_MSG_SETVALUESTRING  130201219

// 外部给IListItemBase发送消息，设置属性的默认值
// WPARAM: LPCTSTR szText
#define UI_PROPERTYCTRLITEM_MSG_SETDEFAULTVALUESTRING  130202239

// 外部给IListItemBase发送消息，获取key字段
// Return: LPCTSTR szText
#define UI_PROPERTYCTRLITEM_MSG_GETKEYSTRING  132311511
// 外部给IListItemBase发送消息，获取value字段
// Return: LPCTSTR szText
#define UI_PROPERTYCTRLITEM_MSG_GETVALUESTRING   157081316

// AdditionItem接受编辑内容
// MESSAGE: UI_WM_NOITFY
// WPARAM: IPropertyCtrlAdditionItem*
// LPARAM: LPCTSTR newText
// Result: ACCEPT_EDIT_RESULT
#define UI_PROPERTYCTRL_NM_ADDITIONITEM_ACCEPTCONTENT 154190837

class PropertyCtrlItemBaseShareData;
interface IPropertyCtrlItemBaseShareData : public IListItemShareData
{
	enum {FLAG = 154202108};
    UI_DECLARE_INTERFACE_ACROSSMODULE(PropertyCtrlItemBaseShareData);
};

class PropertyCtrlItemBase;
interface UIAPI IPropertyCtrlItemBase : public IListItemBase
{
    LPCTSTR  GetKey();

    UI_DECLARE_INTERFACE_ACROSSMODULE(PropertyCtrlItemBase);
};

class PropertyCtrlEditItem;
interface UIAPI_UUID(87AE2A12-319E-4dde-9C5A-7CDA620F4944) IPropertyCtrlEditItem
 : public IPropertyCtrlItemBase
{
	enum {FLAG = 154202056};

    void  SetValueText(LPCTSTR szText);
	void  SetDefaultValueText(LPCTSTR szText);

    UI_DECLARE_INTERFACE(PropertyCtrlEditItem);
};

class PropertyCtrlAdditionItem;
interface UIAPI_UUID(AB2329E5-E947-4946-91F8-88F091DEDE68) IPropertyCtrlAdditionItem
 : public IPropertyCtrlItemBase
{
	enum {FLAG = 154202057};
	UI_DECLARE_INTERFACE(PropertyCtrlAdditionItem);

};

class PropertyCtrlLongItem;
interface UIAPI_UUID(0A153049-988F-48fe-AAD6-4962687387AF) IPropertyCtrlLongItem
 : public IPropertyCtrlEditItem
{
	enum {FLAG = 154202058};
	void  SetValue(long lValue);

    UI_DECLARE_INTERFACE(PropertyCtrlLongItem);
};

// 列表项，是取字符串，还是取映射值
enum ComboBoxItemValueType
{
	ComboBoxItemValueString,
	ComboBoxItemValueLong,
};

class PropertyCtrlComboBoxItem;
interface UIAPI_UUID(19236792-9745-4d21-B04F-A44FEB740B2B) IPropertyCtrlComboBoxItem
 : public IPropertyCtrlItemBase
{
	enum {FLAG = 154202059};
	UI_DECLARE_INTERFACE(PropertyCtrlComboBoxItem);

	void  SetValueString(LPCTSTR szText);
	void  SetValueLong(long lValue);
	void  AddOption(LPCTSTR szText, long lValue);
};

class PropertyCtrlBoolItem;
interface UIAPI_UUID(B3EB4896-DF57-4B64-B395-1E4037530356) IPropertyCtrlBoolItem
 : public IPropertyCtrlComboBoxItem
{
	enum {FLAG = 154202060};
    UI_DECLARE_INTERFACE(PropertyCtrlBoolItem);
	void  SetBool(bool b);
};

class PropertyCtrlButtonItem;
interface UIAPI_UUID(E9F3CADE-38AE-41ed-8274-A8155DC02BA3) IPropertyCtrlButtonItem
 : public IPropertyCtrlItemBase
{
	enum {FLAG = 154202061};

	void  SetValueText(LPCTSTR szText);
	void  SetDefaultValueText(LPCTSTR szText);

    UI_DECLARE_INTERFACE(PropertyCtrlButtonItem);
};

class PropertyCtrlGroupItem;
interface UIAPI_UUID(8530DBFB-493E-49be-852D-52FA42FA13A4) IPropertyCtrlGroupItem
 : public INormalTreeItem
{
    enum {FLAG = 154011324};
    UI_DECLARE_INTERFACE_ACROSSMODULE(PropertyCtrlGroupItem)
};

struct PropertyCtrlBoolItemInfo
{
    LPCTSTR szText;
    LPCTSTR szKey;
    LPCTSTR szDesc;
    bool  bValue;
    bool  bDefault;
};

struct PropertyCtrlComboBoxItemInfo
{
	LPCTSTR szText;
	LPCTSTR szKey;
	LPCTSTR szDesc;

	bool  bReadOnly; 
    bool  bMultiSel;   // 可多选。例如flags attribute

	ComboBoxItemValueType eType;
	union
	{
		LPCTSTR szValue;
		long lValue;
	};
};

struct PropertyCtrlLongItemInfo
{
	LPCTSTR szText;
	LPCTSTR szKey;
	LPCTSTR szDesc;
	long lValue;
};

struct PropertyCtrlEditItemInfo
{
	LPCTSTR szText;
	LPCTSTR szValue;
	LPCTSTR szDesc;
	LPCTSTR szKey;
};

class PropertyCtrl;
interface UIAPI_UUID(23A2E164-0809-486E-83AA-D8FCC3CE4B51) IPropertyCtrl
 : public ITreeView
{
	IPropertyCtrlGroupItem*  FindGroupItem(IListItemBase* pParentItem, LPCTSTR szName);
    IListItemBase*  FindItemByKey(LPCTSTR szKey);

    IPropertyCtrlGroupItem*  InsertGroupItem(
            LPCTSTR szName,
            LPCTSTR szDesc,
            IListItemBase* pParent = UITVI_ROOT,
            IListItemBase* pInsertAfter = UITVI_LAST);

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

	IPropertyCtrlEditItem*  FindEditItem(LPCTSTR szKey);
    IPropertyCtrlLongItem*  FindLongItem(LPCTSTR szKey);
    IPropertyCtrlComboBoxItem*  FindComboBoxItem(LPCTSTR szKey);
	IPropertyCtrlBoolItem*  FindBoolItem(LPCTSTR szKey);

    UI_DECLARE_INTERFACE_ACROSSMODULE(PropertyCtrl);

};

}

#endif // _UI_IPROPERTYCTRL_H_