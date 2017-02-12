#pragma once
#include "Inc\Interface\icombobox.h"
#include "Inc\Interface\iedit.h"
#include "..\UISDK\Inc\Interface\ilistctrlbase.h"


namespace UI
{

interface IEdit;
interface IButton;
interface IListBox;
interface IListItemBase;
class PopupListBoxWindow;

//
// 需要在皮肤中多配置一个下拉列表
/*
    <CustomWindow id="ComboBox.Default" transparent.type="layered">
      <prop key="back.render" type="color" color="#fff" border="1" border.color="#000"/>
      
      <ListBox border="1">
        <prop key="layout" top="0" left="0" right="0" bottom="0"/>
        <prop key="fore.render" type="colorlist" color=";;;;#03B;#03B;#03B;;"/>
        <prop key="text.render" type="colorlist" color=";;;;#FFF;#FFF;#FFF;;"/>

        <VScrollBar ncchild="1" id="__vscrollbar" width="11">
          <Button id="__scrollbar_thumb" back.render.type="color" back.render.color="#eee"/>
        </VScrollBar>
      </ListBox>
    </CustomWindow>


	  <ComboBox id="combobox" height="30" width="200" border="1">
        <prop key="back.render" type="color" border="1" border.color="#000"/>
        <Edit/>
        <Button width="20" height="20" back.render.type="color" back.render.color="#eee"/>
      </ComboBox>
*/
//


class ComboBoxBase : public MessageProxy
{
public:
	ComboBoxBase(IComboBoxBase* p);
	~ComboBoxBase();

	UI_BEGIN_MSG_MAP()
		UIMSG_STATECHANGED(OnStateChanged)
		UIMSG_ERASEBKGND(OnEraseBkgnd)
		UIMSG_SIZE(OnSize)
		MSG_WM_LBUTTONDOWN(OnLButtonDown)
        MSG_WM_LBUTTONDBLCLK(OnLButtonDown)
		UIMSG_INITPOPUPCONTROLWINDOW(OnInitPopupControlWindow)
		UIMSG_UNINITPOPUPCONTROLWINDOW(OnUnInitPopupControlWindow)
        UIMSG_SERIALIZE(OnSerialize)
        UIMSG_INITIALIZE(OnInitialize)
        UIMSG_FINALRELEASE(FinalRelease)

	UIALT_MSG_MAP(COMBOBOX_BUTTON_MSG_HOOK_MSG_ID)  // Button hook
		MSG_WM_LBUTTONDOWN(OnBtnLButtonDown)
		UIMSG_STATECHANGED(OnChildObjStateChanged)

	UIALT_MSG_MAP(COMBOBOX_EDIT_MSG_HOOK_MSG_ID)
		UIMSG_STATECHANGED(OnChildObjStateChanged)

    UIALT_MSG_MAP(UIALT_CALLLESS)
        UIMSG_CREATEBYEDITOR(OnCreateByEditor)

	UI_END_MSG_MAP_CHAIN_PARENT_Ixxx(ComboBoxBase, IControl)

    IComboBoxBase*  GetIComboBoxBase() 
    { 
        return m_pIComboBoxBase; 
    }

public:
    void  CloseUp();
	void  DropDown();
	bool  IsDropDown();

    IEdit*  GetEdit() { return m_pEdit; }
    void  SetReadOnly(bool b);
    bool  IsReadOnly();

    virtual LPCTSTR  GetText();
    virtual void  SetText(LPCTSTR szText);

	IObject*  GetContentObject(LPCTSTR szText);

protected:
    void  SetDropWndId(LPCTSTR);
    LPCTSTR  GetDropWndId();

protected:
    void  OnInitialize();
    void  FinalRelease();
    void  OnCreateByEditor(CREATEBYEDITORDATA* pData);
	void  OnSerialize(SERIALIZEDATA* pData);
	void  GetAutoSize(SIZE* pSize);

	void  OnStateChanged(UINT nMask);
	void  OnChildObjStateChanged(UINT nMask);
	void  OnRedrawObject();
    bool  OnEditKeyDown(IEdit*, UINT key);

	void  OnEraseBkgnd(IRenderTarget* pRenderTarget);
	void  OnSize(UINT nType, int cx, int cy);
	void  OnLButtonDown(UINT nFlags, POINT point);
	void  OnBtnLButtonDown(UINT nFlags, POINT point);
	void  OnStyleChanged(int nStyleType, LPSTYLESTRUCT lpStyleStruct);
	
	void  OnInitPopupControlWindow();
	void  OnUnInitPopupControlWindow();

    void  OnDrawStaticCurSel(
            IRenderTarget* pRenderTarget, 
            ITextRenderBase* pTextRender);

    CBN_DROPDOWN_RESULT defaultDropDown();
	void  destroyDropWnd();
	PopupListBoxWindow*  GetDropWnd();

    void  layout();

protected:
    IComboBoxBase*  m_pIComboBoxBase;
	IEdit*  m_pEdit;
	IButton*  m_pButton;
	String  m_strDropWndId; // 下拉列表窗口皮肤id

	bool  m_bEditDropList; // 是否是可编辑的combobox
    
private:
	PopupListBoxWindow*  m_pPopupWnd;

public:
#if _MSC_VER >= 1800
    signal_r<bool, IComboBoxBase*, UINT>  keydown;
#else
	signal_r2<bool, IComboBoxBase*, UINT>  keydown;
#endif
};

class ComboBox : public ComboBoxBase
{
public:
    ComboBox(IComboBox* p);

	UI_BEGIN_MSG_MAP()
		UIMSG_PAINT(OnPaint)
		UIMSG_NOTIFY(UI_CBN_DROPDOWN, OnDropDown)
        UIMSG_INITPOPUPCONTROLWINDOW(OnInitPopupControlWindow)
		UIMSG_UNINITPOPUPCONTROLWINDOW(OnUnInitPopupControlWindow)
        UIMSG_QUERYINTERFACE(ComboBox)
        UIMSG_SERIALIZE(OnSerialize)
        UIMSG_INITIALIZE(OnInitialize)
    UI_END_MSG_MAP_CHAIN_PARENT(ComboBoxBase)

public:
    IComboBox*  GetIComboBox() { return m_pIComboBox; }

    bool  AddString(LPCTSTR);
    IListBoxItem*  AddStringEx(LPCTSTR);
    bool  SetCurSel(int nIndex);
    IListBox*  GetListBox();
    void  SelectItem(UI::IListItemBase*, bool bNotify);
	void  ResetContent();
    int   GetCurSel();

    virtual LPCTSTR  GetText() override;
    virtual void  SetText(LPCTSTR szText) override;
private:
    void  OnInitialize();
    void  OnPaint(IRenderTarget*);
	void  OnSerialize(SERIALIZEDATA* pData);
	void  OnLCNSelChanged(IListCtrlBase*);
    void  OnListCtrlClick(IListCtrlBase*, IListItemBase*);
    void  OnListCtrlKeyDown(IListCtrlBase*, UINT, bool&);
	void  OnEditChanged(IEdit*, bool);
    void  OnInitPopupControlWindow();
	void  OnUnInitPopupControlWindow();

    HRESULT  OnDropDown(WPARAM, LPARAM);

    void  do_listbox_select_changed(bool bNotify);

private:
    IComboBox*  m_pIComboBox;

	// 该变量是延迟创建的。要访问该变量，通过GetListBox()获得
    IListBox*   m_pListCtrl; 

	// event
public:
	signal<IComboBox*>  select_changed;
};


}