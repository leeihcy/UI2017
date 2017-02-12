#pragma  once
#include "Inc\Interface\ilistbox.h"
#include "..\ScrollBar\vscrollbar_creator.h"
#include "Inc\Interface\iedit.h"

/*
	listbox皮肤配置示例：

	  <ListBox id="listbox" border="1">
        <prop>
          <x key="layout" width="200" height="100" top="50" left="10"/>
          <x key="back.render" type="color" border="1" border.color="#000"/>
          <x key="fore.render" type="colorlist" color=";;;;#03B;#03B;#03B;;"/>
          <x key="text.render" type="colorlist" color=";;;;#FFF;#FFF;#FFF;;"/>
        </prop>

        <VScrollBar ncchild="1" id="__vscrollbar" width="11">
          <Button id="__scrollbar_thumb" back.render.type="color" back.render.color="#eee"/>
        </VScrollBar>
      </ListBox>
*/

namespace UI
{
class ListBoxItem;
interface IInstantEdit;

class ListBox : public MessageProxy
{
public:
	ListBox(IListBox* p);
	~ListBox();

	UI_BEGIN_MSG_MAP()
		MESSAGE_HANDLER_EX(WM_MOUSEWHEEL, OnMouseWheel)
		MESSAGE_HANDLER_EX(UI_MSG_LISTCTRL_DISCARDEDIT, OnDiscardEditReq)
        UIMSG_SERIALIZE(OnSerialize)
		UIMSG_QUERYINTERFACE(ListBox)
		UIMSG_INITIALIZE(OnInitialize)
	UIALT_MSG_MAP(UIALT_CALLLESS)
		UIMSG_CREATEBYEDITOR(OnCreateByEditor)
	UIALT_MSG_MAP(LISTCTRL_EDIT_MSGMAP_ID)
		UIMSG_NOTIFY_CODE_FROM(UI_EN_STARTEDIT, m_pUIEdit, OnPreStartEdit)
		UIMSG_NOTIFY_CODE_FROM(UI_EN_ACCEPTEDIT, m_pUIEdit, OnAcceptEdit)
		UIMSG_NOTIFY_CODE_FROM(UI_EN_CANCELEDIT, m_pUIEdit, OnCancelEdit)
		UIMSG_NOTIFY_CODE_FROM(UI_EN_ENDEDIT, m_pUIEdit, OnEditFinish)
	UI_END_MSG_MAP_CHAIN_PARENT_Ixxx(ListBox, IListCtrlBase)

    IListBox*  GetIListBox()
	{ 
		return m_pIListBox; 
	}

public:
    bool  SetSel(int nIndex);
    int   GetSel();
    void  SetBindObject(IObject* pCombobox);

	IListBoxItem*  AddString(LPCTSTR szText);
	bool  RemoveItem(IListBoxItem* pItem);

	void  StartEdit(IListItemBase* pEditItem);
	void  DiscardEdit();
	IInstantEdit*  GetEditCtrl();
	bool  IsItemEditing(IListItemBase* pEditItem);
	bool  IsEditing();

protected:
	void  OnSerialize(SERIALIZEDATA* pData);
	void  OnInitialize();
	void  OnCreateByEditor(CREATEBYEDITORDATA* pData);
	LRESULT  OnMouseWheel(UINT uMsg, WPARAM wParam, LPARAM lParam);
	
	void  AddEditUIRes(CREATEBYEDITORDATA* pData);
	void  GetEditUIRes();
	void  DoEditFinishWork();
	LRESULT  OnEditFinish(WPARAM w, LPARAM l);
	LRESULT  OnPreStartEdit(WPARAM w, LPARAM l);
	LRESULT  OnAcceptEdit(WPARAM w, LPARAM l);
	LRESULT  OnCancelEdit(WPARAM w, LPARAM l);
	LRESULT  OnDiscardEditReq(UINT uMsg, WPARAM wParam, LPARAM lParam);

private:
    bool  IsPopup();

protected:
    IListBox*  m_pIListBox;
    IObject*  m_pBindObject;   // 例如从combobox弹出来的列表框，m_pBindObject将指向Combobox*

	UI::IInstantEdit*  m_pUIEdit;
	UI::IListItemBase*  m_pEditingItem;           // 正在编辑的对象

	VScrollbarCreator  m_vscrollbarCreator;
};
}