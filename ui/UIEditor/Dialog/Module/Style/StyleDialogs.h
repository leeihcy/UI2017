#pragma once
#include "Other\Resource.h"


//
//	添加一个样式窗口
//
class CNewStyleDlg : public CDialogImpl<CNewStyleDlg>
{
public:
	enum {IDD = IDD_NEW_STYLE };
	CNewStyleDlg( ISkinRes* hSkin, bool bModifyMode=false);
	~CNewStyleDlg();

	BEGIN_MSG_MAP_EX(CNewStyleDlg)
		MSG_WM_INITDIALOG( OnInitDialog )
		COMMAND_ID_HANDLER_EX( IDOK, OnOk )
		COMMAND_ID_HANDLER_EX( IDCANCEL, OnCancel )
		COMMAND_ID_HANDLER_EX( IDC_BUTTON_ADD_INHERIT, OnBtnAddInherit )
		COMMAND_ID_HANDLER_EX( IDC_BUTTON_DELETE_INHERIT, OnBtnDeleteInherit )
		COMMAND_ID_HANDLER_EX( IDC_BUTTON_CLEAR_INHERIT, OnBtnClearInherit )
		COMMAND_HANDLER_EX( IDC_LISTBOX_STYLEITEM_NOT, LBN_DBLCLK, OnListBoxNotDBClick )
		END_MSG_MAP()

public:
	void   OnOk( UINT,int,HWND	);
	void   OnCancel( UINT,int,HWND	);
	BOOL   OnInitDialog(CWindow wndFocus, LPARAM lInitParam);
	void   OnBtnAddInherit(UINT uNotifyCode, int nID, CWindow wndCtl);
	void   OnBtnDeleteInherit(UINT uNotifyCode, int nID, CWindow wndCtl);
	void   OnBtnClearInherit(UINT uNotifyCode, int nID, CWindow wndCtl);
	void   OnListBoxNotDBClick(UINT uNotifyCode, int nID, CWindow wndCtl);

	void   SetStyleInfo( STYLE_SELECTOR_TYPE type, const String& strID, const String& strInherit );

protected:
	void   FillAllStyleListBox();

public:
	CComboBox m_comboType;
	CEdit     m_editID;
	CEdit     m_editInherit;
	CListBox  m_listbox_own;
	CListBox  m_listbox_not;

	STYLE_SELECTOR_TYPE  m_type;
	String    m_strID;
	String    m_strInherit;

	map<String, bool> m_mapAllStyle;   // true表示自己继承了的，false表示没有被继承的

private:
	ISkinRes* m_pSkin;
	bool      m_bModifyMode;  //  当前窗口是用于创建，还是被用于修改
};

//
//	添加一个Style属性窗口
//
class CNewStyleAttributeDlg : public CDialogImpl<CNewStyleAttributeDlg>
{
public:
	enum {IDD = IDD_NEW_STYLE_ATTRIBUTE };
	CNewStyleAttributeDlg( ISkinRes* hSkin, bool bModifyMode=false);
	~CNewStyleAttributeDlg();

	BEGIN_MSG_MAP_EX(CNewStyleAttributeDlg)
		MSG_WM_INITDIALOG( OnInitDialog )
		COMMAND_ID_HANDLER_EX( IDOK, OnOk )
		COMMAND_ID_HANDLER_EX( IDCANCEL, OnCancel )
		END_MSG_MAP()

public:
	void   OnOk( UINT,int,HWND	);
	void   OnCancel( UINT,int,HWND	);
	BOOL   OnInitDialog(CWindow wndFocus, LPARAM lInitParam);

	void   SetStyleInfo(STYLE_SELECTOR_TYPE type, const String strStyleID);
	void   SetAttributeInfo( const String& strKey, const String& strValue );

public:
	CEdit  m_editKey;
	CEdit  m_editValue;

	String m_strKey;
	String m_strValue;
	STYLE_SELECTOR_TYPE m_eType;
	String m_strStyleID;

private:
	ISkinRes* m_pSkin;
	bool      m_bModifyMode;  //  当前窗口是用于创建，还是被用于修改
};
