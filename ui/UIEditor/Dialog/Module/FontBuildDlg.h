#pragma once
#include "Other\resource.h"
#include "Dialogs.h"
#include "Dialog\Framework\childframe.h"

//
//	图片操作窗口
//
class CFontViewDlg : public CDialogImpl<CFontViewDlg>, public IMDIClientView
{
public:
	enum { IDD = IDD_DIALOG_FONT_VIEW };

	CFontViewDlg();
	~CFontViewDlg();

	BEGIN_MSG_MAP_EX(CFontViewDlg)
		MSG_WM_INITDIALOG( OnInitDialog )
		MSG_WM_SIZE( OnSize )
		MSG_WM_DESTROY( OnDestroy )
		COMMAND_HANDLER_EX( IDC_BUTTON_ADD,    BN_CLICKED, OnBtnAdd )
		COMMAND_HANDLER_EX( IDC_BUTTON_DELETE, BN_CLICKED, OnBtnRemove )
		NOTIFY_HANDLER_EX( IDC_FONTLIST, LVN_ITEMCHANGED, OnItemChanged )
		NOTIFY_HANDLER_EX( IDC_FONTLIST, NM_DBLCLK, OnItemDBClick )
		MSG_WM_PAINT( OnPaint )
	END_MSG_MAP()

public:
	void  AttachSkin( ISkinRes* hSkin );
	ISkinRes* GetSkin() { return m_pSkin; }
	void  OnSize(UINT nType, CSize size);
	void  OnDestroy();
	void  OnPaint(HDC hDC);
	BOOL  OnInitDialog( HWND, LPARAM );
	LRESULT  OnItemChanged(LPNMHDR pnmh);
	LRESULT  OnItemDBClick(LPNMHDR pnmh);
	void     OnBtnAdd(UINT uNotifyCode, int nID, CWindow wndCtl);
	void     OnBtnRemove(UINT uNotifyCode, int nID, CWindow wndCtl);
	void     InvalidateDisplayArea();

	bool     Reload();
	bool     Clear();

#pragma  region
	virtual ISkinRes*  GetSkinRes(){ return m_pSkin; }
	virtual void   SetSkinRes(ISkinRes* hSkin) { AttachSkin(hSkin); }

	virtual long*  GetKey();
	virtual UI_RESOURCE_TYPE GetResourceType(){ return UI_RESOURCE_FONT; }
	virtual HWND   GetHWND() { return m_hWnd; }
#pragma  endregion

private:
	ISkinRes* m_pSkin;

	CListViewCtrl m_listctrl;
};

#if 0

//
//	图片的代码编辑窗口
//
class CFontCodeDlg : public CDialogImpl<CFontCodeDlg>, public CResourceCodeDlg<CFontCodeDlg>
{
public:
	enum { IDD = IDD_DIALOG_FONT_CODE };

	CFontCodeDlg();
	~CFontCodeDlg();

	BEGIN_MSG_MAP_EX(CFontCodeDlg)
		CHAIN_MSG_MAP(CResourceCodeDlg)
	END_MSG_MAP()

public:
};


class CFontBuildDlg : public CDialogImpl<CFontBuildDlg>
{
public:
	enum{IDD = IDD_DIALOG_FONT_BUILD };
	CFontBuildDlg();
	~CFontBuildDlg();

	virtual void OnFinalMessage(HWND /*hWnd*/)
	{
		// 由于在childframe中不能很好的处理各种builddlg的释放，因此将释放操作放在这里进行
		delete this;
	}

	BEGIN_MSG_MAP_EX( CFontBuildDlg )
		MSG_WM_INITDIALOG(OnInitDialog)
		MSG_WM_SIZE(OnSize)
		NOTIFY_HANDLER_EX(IDC_TAB, TCN_SELCHANGING, OnSelChanging )
	END_MSG_MAP()

public:
	void      AttachSkin( HSKIN hSkin );
	HSKIN     GetSkin() { return m_pSkin; }

protected:
	BOOL      OnInitDialog(CWindow wndFocus, LPARAM lInitParam);
	void      OnSize(UINT nType, CSize size);
	LRESULT   OnSelChanging(LPNMHDR pnmh);

private:
	CTabCtrl        m_tabCtrl;
	CFontViewDlg*   m_pFontViewPanel;
	CFontCodeDlg*   m_pFontCodePanel;
	HSKIN           m_pSkin;
};

#endif