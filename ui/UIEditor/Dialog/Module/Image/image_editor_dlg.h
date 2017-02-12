#pragma once
#include "Other\resource.h"
#include "Dialog\Framework\MainFrame.h"
#include "Dialog\Framework\childframe.h"
#include "UISDK\Inc\Util\iimage.h"
#include "Business\Command\image\ImageItemCommand.h"


namespace UI
{
    interface  IListBox;
	interface  IButton;
	interface  IHwndHost;
}
//
//	专门负责显示一张图片的窗口view
//	
class CImagePreview : public CWindowImpl<CImagePreview>
{
public:
	CImagePreview();
	~CImagePreview();

	BEGIN_MSG_MAP_EX( CImagePreview )
		MSG_WM_CREATE(OnCreate)
		MSG_WM_ERASEBKGND(OnEraseBkgnd)
		MSG_WM_PAINT(OnPaint)
	END_MSG_MAP()

	void OnPaint(HDC hDC);
	BOOL OnEraseBkgnd(HDC hDC);
	void DoPaint(CDCHandle);
	int  OnCreate(LPCREATESTRUCT lpCreateStruct);

	void Attach(ISkinRes* pSkinRes, LPCTSTR pszPath);

	enum IMAGE_DISPLAY_TYPE
	{
		IMAGE_DISPLAY_TYPE_CENTER_BITBLT,
		IMAGE_DISPLAY_TYPE_CENTER_STRETCH,
		IMAGE_DISPLAY_TYPE_LEFTTOP_BITBLT,
	};
private:
	HBRUSH  m_hBkBrush;

	String  m_strImagePath;
    UI::ImageWrap  m_image;

	IMAGE_DISPLAY_TYPE  m_eDisplayType;
};

//
//	图片操作窗口
//
class CImageEditorDlg :
        public IMDIClientView,  
		public IWindow
{
public:

	CImageEditorDlg();
	~CImageEditorDlg();

	UI_BEGIN_MSG_MAP_Ixxx(CImageEditorDlg)
		UIMSG_INITIALIZE(OnInitDialog)
		UIMSG_HANDLER_EX(WM_IMAGE_EDITOR_ADD, OnAddImage)
		UIMSG_HANDLER_EX(WM_IMAGE_EDITOR_DELETE, OnDeleteImage)
	UI_END_MSG_MAP_CHAIN_PARENT(IWindow)

public:
	void  AttachSkin( ISkinRes* hSkin );
	ISkinRes*  GetSkin() { return m_pSkin; }
	void  OnInitDialog();
    void  OnBtnAdd(UI::IButton*);
    void  OnBtnRemove(UI::IButton*);
    void  OnBtnLeft(UI::IButton*);
	void  OnBtnRight(UI::IButton*);
	LRESULT  OnAddImage(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT  OnDeleteImage(UINT uMsg, WPARAM wParam, LPARAM lParam);

	CImagePreview*  GetImageDisplayView() { return m_pImageDisplayView; }

    void  OnItemDBClick(IListCtrlBase*, IListItemBase*);
    void  OnItemChanged(IListCtrlBase*);
    void  OnListKeyDown(IListCtrlBase*, UINT, bool&);

	bool  Reload();

#pragma  region
	virtual ISkinRes*  GetSkinRes(){ return m_pSkin; }
	virtual void   SetSkinRes(ISkinRes* hSkin) { AttachSkin(hSkin); }

	virtual long*  GetKey();
	virtual UI_RESOURCE_TYPE GetResourceType(){ return UI_RESOURCE_IMAGE; }
	virtual HWND   GetHWND() { return IWindow::GetHWND(); }
	virtual void   Destroy()
	{
        __super::Release();
	}
#pragma  endregion

private:
	ISkinRes*  m_pSkin;
//	IListView*  m_pListView;
	IButton*  m_pBtnAdd;
	IButton*  m_pBtnDel;
	IButton*  m_pBtnPrev;
	IButton*  m_pBtnNext;
	IHwndHost*  m_pHwndHost;

    IListBox*  m_pListView;
	CImagePreview*   m_pImageDisplayView;
};

