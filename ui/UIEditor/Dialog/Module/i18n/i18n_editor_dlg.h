#pragma once
#include "Other\resource.h"
#include "Dialog\Framework\MainFrame.h"
#include "Dialog\Framework\childframe.h"


namespace UI
{
    interface  IListBox;
	interface  IButton;
	interface  IHwndHost;
}

//
//	Í¼Æ¬²Ù×÷´°¿Ú
//
class CI18nEditorDlg :
        public IMDIClientView,  
		public IWindow
{
public:

	CI18nEditorDlg();
	~CI18nEditorDlg();

	UI_BEGIN_MSG_MAP_Ixxx(CI18nEditorDlg)
		UIMSG_INITIALIZE(OnInitDialog)
	UI_END_MSG_MAP_CHAIN_PARENT(IWindow)

public:
	void  AttachSkin( ISkinRes* hSkin );
	ISkinRes*  GetSkin() { return m_pSkin; }
	void  OnInitDialog();

	bool  Reload();

#pragma  region
	virtual ISkinRes*  GetSkinRes(){ return m_pSkin; }
	virtual void   SetSkinRes(ISkinRes* hSkin) { AttachSkin(hSkin); }

	virtual long*  GetKey();
	virtual UI_RESOURCE_TYPE GetResourceType(){ return UI_RESOURCE_I18N; }
	virtual HWND   GetHWND() { return IWindow::GetHWND(); }
	virtual void   Destroy()
	{
        __super::Release();
	}
#pragma  endregion

private:
	ISkinRes*  m_pSkin;
    IListBox*  m_pListView;
};

