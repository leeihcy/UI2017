#ifndef _UI_ICOMBOBOX_H_
#define _UI_ICOMBOBOX_H_
#include "..\..\..\UISDK\Inc\Interface\icontrol.h"
#include "ilistbox.h"

namespace UI
{

// #define UI_CBN_SELCHANGED  132331846

// 
//  点击了下拉按钮或者ComboBox，通知子类去弹出下拉列表
//      message: UI_WM_NOTIFY
//      wParam:
//      lParam:
//      lRet: CBN_DROPDOWN_RESULT 
//
    enum CBN_DROPDOWN_RESULT
    {
        CBN_DROPDOWN_RESULT_NOTHANDLED,  // 未处理
        CBN_DROPDOWN_RESULT_SUCCESS,     // 已处理，下拉成功
        CBN_DROPDOWN_RESULT_FAILED       // 已处理，下拉失败
    };

#define UI_CBN_DROPDOWN  132331847

//  要求关闭下拉列表
//      message: UI_WM_NOTIFY
//      code: UI_CBN_CLOSEUP
//      
#define UI_CBN_CLOSEUP  132331848

    
// void OnCbnSelChanged(UI::IListItemBase* pOldSelItem, UI::IListItemBase* pSelItem)
#define UIMSG_CBN_SELCHANGED(ptr, func)               \
    if (uMsg == UI_WM_NOTIFY  &&                      \
        pMsgFrom == (UI::IMessage*)ptr &&             \
        code == UI_CBN_SELCHANGED )                   \
    {                                                 \
        SetMsgHandled(TRUE);                          \
        func((UI::IListItemBase*)wParam, (UI::IListItemBase*)lParam); \
        if (IsMsgHandled())                           \
            return TRUE;                              \
    }

#define COMBOBOX_BUTTON_MSG_HOOK_MSG_ID  1
//#define COMBOBOX_LISTBOX_MSG_HOOK_MSG_ID 2
#define COMBOBOX_EDIT_MSG_HOOK_MSG_ID    3
#define COMBOBOX_LISTBOX_MSG_NOTIFY_MSG_ID 4

// #define COMBOBOX_BUTTON_ID _T("__combo_button")
// #define COMBOBOX_EDIT_ID   _T("__combo_edit")
// #define COMBOBOX_LIST_ID   _T("__combo_list")

interface IListBoxItem;
interface IListBox;
interface IEdit;

class ComboBoxBase;
interface UICTRL_API IComboBoxBase : public IControl
{
    LPCTSTR  GetText();
	void  SetText(LPCTSTR szText);
	IObject*  GetContentObject(LPCTSTR szText);
    void  CloseUp();
    IEdit*  GetEdit();

#if _MSC_VER >= 1800
	signal_r<bool, IComboBoxBase*, UINT>&  KeyDownEvent();
#else
	signal_r2<bool, IComboBoxBase*, UINT>&  KeyDownEvent();
#endif

	UI_DECLARE_INTERFACE_ACROSSMODULE(ComboBoxBase);
};


class ComboBox;
interface __declspec(uuid("77593B73-0A20-42FF-B66A-94CDA87FC88E"))
UICTRL_API IComboBox : public IComboBoxBase
{
    bool  AddString(LPCTSTR);
    IListBoxItem*  AddStringEx(LPCTSTR);

    bool  SetCurSel(int nIndex);
	void  ResetContent();
    int   GetCurSel();
    void  SelectItem(UI::IListItemBase*, bool bNotify);

    IListBox*  GetListBox();
	IEdit*  GetEdit();

    void  SetReadOnly(bool b);
    bool  IsReadOnly();

    signal<IComboBox*>&  SelectChangedEvent();

	UI_DECLARE_INTERFACE(ComboBox)
};

}

#endif // _UI_ICOMBOBOX_H_