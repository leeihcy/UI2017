#pragma once
#include "Business/Project/RecentProjFiles/RecentProjList.h"
class CMainFrame;
class CProjectTreeDialog;
class CPropertyDialog;
class CStatusBar;
class CToolBar;
class CToolBox;
class CCommandHistroyMgr;
class CProjectData;
class CDependsCtrlDllMgr;
class CUIEditor;
interface  ICommand;

class CGlobalData
{
public:
	CGlobalData(void);
	~CGlobalData(void);

	bool  Init();
    void  SetStatusText2(LPCTSTR szText);
    bool  OpenUIProj(LPCTSTR  szProjPath);
    bool  CloseUIProj();

public:
	HWND                 m_hWndMainFrame;

	CMainFrame*          m_pMainFrame;
	CProjectTreeDialog*  m_pProjectTreeDialog;
	CPropertyDialog*     m_pPropertyDialog;
    CToolBox*            m_pToolBox;
    CStatusBar*          m_pStatusBar;
    CToolBar*            m_pToolBar;
	CCommandHistroyMgr*  m_pCmdHistroyMgr;

	CRecentProjList*     m_pRecentProj;
    CProjectData*        m_pProjectData;
	IUIApplication*      m_pMyUIApp;    // 编辑器自己的界面使用的皮肤
};
