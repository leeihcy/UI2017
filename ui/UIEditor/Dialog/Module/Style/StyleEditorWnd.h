#pragma once
#include "Other\resource.h"
#include "Dialog\Framework\MainFrame.h"
#include "Dialog\Framework\childframe.h"
#include "StylePropertyHandler.h"
#include "Business\Project\UIEditor\IUIEditorImpl.h"
class CStyleEditorWnd;
class CStyleCodeDlg;


class CStyleEditorWnd : 
		public CDialogImpl<CStyleEditorWnd>, 
		public IMDIClientView,
		public StyleChangedCallback
{
public:
	enum { IDD = IDD_DIALOG_STYLE };

	CStyleEditorWnd(void);
	~CStyleEditorWnd(void);


	BEGIN_MSG_MAP_EX(CStyleEditorWnd)
		MSG_WM_INITDIALOG( OnInitDialog )
		MSG_WM_DESTROY(OnDestroy)
		MSG_WM_SIZE( OnSize )
		COMMAND_HANDLER_EX( IDC_BUTTON_ADD_SELECTOR,BN_CLICKED,    OnBtnAddSelector )
		COMMAND_HANDLER_EX( IDC_BUTTON_MODIFY_SELECTOR,BN_CLICKED, OnBtnModifySelector )
		COMMAND_HANDLER_EX( IDC_BUTTON_DELETE_SELECTOR,BN_CLICKED, OnBtnDeleteSelector )
		NOTIFY_HANDLER_EX( IDC_STYLELIST, LVN_ITEMCHANGED, OnStyleItemChanged )
		NOTIFY_HANDLER_EX( IDC_STYLELIST, NM_DBLCLK, OnItemDBClick )
		NOTIFY_HANDLER_EX( IDC_STYLELIST, LVN_KEYDOWN, OnItemKeyDown )
		NOTIFY_HANDLER_EX( IDC_LIST_STYLE_PROP, NM_DBLCLK, OnPropItemDBClick )
	END_MSG_MAP()

public:
	void  AttachSkin( ISkinRes* hSkin );
	ISkinRes* GetSkin() { return m_pSkin; }
	void  OnSize(UINT nType, CSize size);
	BOOL  OnInitDialog( HWND, LPARAM );
	void  OnDestroy();
	void  OnBtnAddSelector(UINT uNotifyCode, int nID, CWindow wndCtl);
	void  OnBtnModifySelector(UINT uNotifyCode, int nID, CWindow wndCtl);
	void  OnBtnDeleteSelector(UINT uNotifyCode, int nID, CWindow wndCtl);

	LRESULT  OnStyleItemChanged(LPNMHDR pnmh);
	LRESULT  OnItemDBClick(LPNMHDR pnmh);
	LRESULT  OnItemKeyDown(LPNMHDR pnmh);
	LRESULT  OnPropItemDBClick(LPNMHDR pnmh);

	bool  Reload();
	void  InsertItem2ListView(IStyleResItem* p, int nPos);

#pragma  region 
	virtual ISkinRes*  GetSkinRes(){ return m_pSkin; }
	virtual void   SetSkinRes(ISkinRes* hSkin) { AttachSkin(hSkin); }

	virtual long*  GetKey();
	virtual UI_RESOURCE_TYPE GetResourceType(){ return UI_RESOURCE_STYLE; }
	virtual HWND   GetHWND() { return m_hWnd; }
#pragma  endregion


#pragma region // stylechangedcallback
	//virtual ISkinRes*  GetSkinRes() override;
	virtual void  OnStyleAdd(IStyleResItem* p) override;
	virtual void  OnStyleRemove(IStyleResItem* p) override;
	virtual void  OnStyleModify(IStyleResItem* p) override;
	virtual void  OnStyleAttributeAdd(IStyleResItem* p, LPCTSTR szKey) override;
	virtual void  OnStyleAttributeRemove(IStyleResItem* p, LPCTSTR szKey) override;
	virtual void  OnStyleAttributeModify(IStyleResItem* p, LPCTSTR szKey) override;
#pragma endregion

private:
	ISkinRes*   m_pSkin;
	StylePropertyHandler  m_PropertyHandler;

	CListViewCtrl m_listctrl;       // 所有样式列表
	CListViewCtrl m_listctrl_prop;  // 属性列表
};

