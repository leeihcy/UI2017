#ifndef _UI_ILISTBOX_H_
#define _UI_ILISTBOX_H_
#include "..\..\..\UISDK\Inc\Interface\ilistctrlbase.h"
#include "..\..\..\UISDK\Inc\Interface\ilistitembase.h"

namespace UI
{
#define LISTCTRL_EDIT_MSGMAP_ID  171131450

struct IListBoxItem;
class ListBox;
struct __declspec(uuid("E1613415-FAA8-4DD5-A646-705BC22CC740"))
UICTRL_API IListBox : public IListCtrlBase
{
    bool  SetSel(int nIndex);
    int  GetSel();
    void  SetBindObject(IObject* pCombobox);
	IListBoxItem*  AddString(LPCTSTR szText);

	void  StartEdit(IListItemBase* pEditItem);
	void  DiscardEdit();

	UI_DECLARE_INTERFACE_ACROSSMODULE(ListBox);
};

class ListBoxItemShareData;
struct IListBoxItemShareData : public IListItemShareData
{
    UI_DECLARE_INTERFACE_ACROSSMODULE(ListBoxItemShareData);
};


class ListBoxItem;
struct UICTRL_API IListBoxItem : public IListItemBase
{
	enum { FLAG = 155091039 };

	void  SetStringData(LPCSTR data);
	LPCSTR  GetStringData();

    UI_DECLARE_INTERFACE_ACROSSMODULE(ListBoxItem);
};


}

#endif // _UI_ILISTBOX_H_