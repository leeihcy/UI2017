#pragma once
#include "UISDK\Inc\Interface\ilistitembase.h"
#include "UICTRL\Inc\Interface\icombobox.h"
#include "UICTRL\Inc\Interface\iedit.h"
#include "UICTRL\Inc\Interface\ilistbox.h"
#include "propertyctrlitembase.h"

namespace UI
{
interface IComboBox;


struct OptionItem
{
	String  strText;
	long lValue;
};

class PropertyCtrlComboBoxItem : public PropertyCtrlItemBase
{
public:
    PropertyCtrlComboBoxItem(IPropertyCtrlComboBoxItem* p);
    ~PropertyCtrlComboBoxItem();

    UI_BEGIN_MSG_MAP()
        UIMSG_PAINT(OnPaint)
        UIMSG_STATECHANGED(OnStateChanged)
        UIMSG_SIZE(OnSize)
		// UIMSG_NOTIFY(UI_CBN_SELCHANGED, OnComboBoxSelChanged)
        // UIMSG_NOTIFY(UI_EN_KEYDOWN, OnEditKeyDown)
		UIMSG_HANDLER_EX(UI_PROPERTYCTRLITEM_MSG_SETVALUESTRING, OnSetValue)

        UIMSG_QUERYINTERFACE(PropertyCtrlComboBoxItem)
        UIMSG_INITIALIZE(OnInitialize)
    UI_END_MSG_MAP_CHAIN_PARENT(PropertyCtrlItemBase)

public:
    void  SetValueString(LPCTSTR szText);
    void  SetValueLong(long lValue);
	void  SetValueType(ComboBoxItemValueType e) { m_eItemValueTpye = e; }
	void  SetReadOnly(bool b) { 
        m_bReadOnly = b; 
    }
    void  SetMultiSel(bool b){
        m_bMultiSel = b;
    }

	void  AddOption(LPCTSTR szText, long lValue);
    OptionItem*  FindOption(long lValue);

    void  FireValueChanged();
    void  OnMultiSelValueChanged();

private:
    void  OnInitialize();
    void  OnPaint(IRenderTarget* pRenderTarget);
    void  OnSize(UINT nType, int cx, int cy);
    void  OnStateChanged(UINT nMask);
	LRESULT  OnSetValue(UINT uMsg, WPARAM wParam, LPARAM lParam);
    void  OnComboBoxSelChanged(IComboBox*);
    bool  OnEditKeyDown(IComboBoxBase*, UINT);
    
    virtual  LPCTSTR  GetValueText() override { return m_strValue.c_str(); }

private:
    void  BeginEdit();
    void  CancelEdit();
	void  AcceptEdit();
    void  UpdateValue2Combo();

protected:
    IPropertyCtrlComboBoxItem*  m_pIPropertyCtrlComboBoxItem;

    ComboBoxItemValueType  m_eItemValueTpye;
	bool  m_bReadOnly;
    bool  m_bMultiSel;

    String  m_strDefaultValue;
    long  m_lDefualtValue;

    String  m_strValue;  
    long  m_lValue;
    
    typedef list<OptionItem> _MyList;
    typedef _MyList::iterator _ListIter;
	_MyList  m_mapItems;
};

class MultiSelComboItem;
interface  IMultiSelComboItem : public IListBoxItem
{
    UI_DECLARE_INTERFACE_ACROSSMODULE(MultiSelComboItem)
};

class MultiSelComboItem : public MessageProxy
{
public:
    MultiSelComboItem(IMultiSelComboItem* p);
    ~MultiSelComboItem();

    UI_BEGIN_MSG_MAP()
        UIMSG_PAINT(OnPaint)
        UIMSG_HANDLER_EX(WM_LBUTTONUP, OnLButtonUp)
    UI_END_MSG_MAP_CHAIN_PARENT_Ixxx(MultiSelComboItem, IListItemBase)


    void  SetComboBoxItem(PropertyCtrlComboBoxItem* p)
    {
        m_pBindComboItem = p;
    }

private:
    void  OnPaint(IRenderTarget* pRenderTarget);
    LRESULT  OnLButtonUp(UINT uMsg, WPARAM wParam, LPARAM lParam);

private:
    IMultiSelComboItem*  m_pIMultiSelComboItem;
    PropertyCtrlComboBoxItem*  m_pBindComboItem;
};

}