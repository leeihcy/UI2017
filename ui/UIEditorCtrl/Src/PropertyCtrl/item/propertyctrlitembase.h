#pragma once
#include "Inc\ipropertyctrl.h"


namespace UI
{
interface IButton;
interface IEdit;
interface IComboBox;

class PropertyCtrlItemBaseShareData : public MessageProxy
{
public:
    PropertyCtrlItemBaseShareData(IPropertyCtrlItemBaseShareData* p);

    UI_BEGIN_MSG_MAP()
    UI_END_MSG_MAP_CHAIN_PARENT_Ixxx(
        PropertyCtrlItemBaseShareData, IListItemShareData)

    uint  GetSplitterLinePos();
    void  SetSplitterLinePosPtr(uint* p) { m_pSlitterLinePos = p; }

public:
    IPropertyCtrlItemBaseShareData*  m_pIPropertyCtrlItemBaseShareData;
    IRenderFont*  m_pRenderFont;
    IButton*  pButtonCtrl;
    IEdit*  pEditCtrl;
    IComboBox*  pComboBoxCtrl;
private:
    uint*  m_pSlitterLinePos;
};


class PropertyCtrlItemBase : public MessageProxy
{
public:
    PropertyCtrlItemBase(IPropertyCtrlItemBase*);
    ~PropertyCtrlItemBase();

    enum MAGIC_NUMBER
    {
        TEXT_INDENT = 5,
        SPLITLINE_WIDTH = 1,
    };

    UI_BEGIN_MSG_MAP()
        UIMSG_PAINT(OnPaint)
		UIMSG_HANDLER_EX(UI_PROPERTYCTRLITEM_MSG_GETKEYSTRING, OnGetKey)
        UIMSG_HANDLER_EX(UI_PROPERTYCTRLITEM_MSG_GETVALUESTRING, OnGetValue)
		MSG_WM_LBUTTONDOWN(OnLButtonDown)
        MSG_WM_KEYDOWN(OnKeyDown)
        UIMSG_INITIALIZE(OnInitialize)
    UI_END_MSG_MAP_CHAIN_PARENT_Ixxx(PropertyCtrlItemBase, IListItemBase)

public:

	void  SetKey(LPCTSTR szText);
    LPCTSTR  GetKey();
    void  GetValueColumnRectInCtrl(LPRECT prc);
    void  GetKeyColoumnRectInCtrl(LPRECT prc);
    void  GetValueColumnRectInItem(LPRECT prc);
	void  GetKeyColoumnRectInItem(LPRECT prc);

	bool  IsEditActive();
    bool  IsComboBoxActive();

protected:
	virtual void  OnValueColumnClick(POINT ptWnd, POINT ptItem){};
    virtual LPCTSTR  GetValueText() { return NULL; }

private:
    void  OnInitialize();
    void  OnPaint(IRenderTarget* pRenderTarget);
	void  OnLButtonDown(UINT nFlags, POINT point);
	LRESULT  OnGetKey(UINT uMsg, WPARAM wParam, LPARAM lParam);
    LRESULT  OnGetValue(UINT uMsg, WPARAM wParam, LPARAM lParam);
    void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

protected:
    PropertyCtrlItemBaseShareData*  m_pBaseShareData;
    String  m_strKey;    // 该项在xml中的属性字段名称, m_strText表示该项在控件上面显示的值，不一定是xml中的值

private:
    IPropertyCtrlItemBase*  m_pIPropertyCtrlItemBase;
	
};

}