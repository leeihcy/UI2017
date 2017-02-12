#include "StdAfx.h"
#include "Dialog\Framework\MainFrame.h"
#include "Dialog\Module\Dialogs.h"
#include "Dialog\Module\colorbuilddlg.h"
#include "Dialog\Module\FontBuildDlg.h"
#include "Dialog\Module\Style\StyleEditorWnd.h"
#include "Dialog\Module\MenuBuildDlg.h"
#include "Dialog\Module\Image\image_editor_dlg.h"
#include "Dialog\Module\Layout\LayoutEditorWnd.h"
#include "Dialog\Module\CodeWebBrowser.h"
#include "PropertyDialog.h"
#include "UISDK\Inc\Interface\iwindow.h"
#include "Dialog\Module\firstpagedlg.h"
#include "Dialog\Module\createwindowdlg.h"
#include "Dialog\Framework\toolbar.h"
#include "Dialog\Framework\statusbar.h"
#include "Dialog\Framework\toolbox.h"
#include "Business\Project\ProjectData\projectdata.h"
#include "Dialog\Module\newprojectdlg.h"
#include "Dialog\Module\projdependsctrldllconfigdlg.h"
#include "Dialog\Framework\childframe.h"
#include "..\Module\i18n\i18n_editor_dlg.h"

CMainFrame::CMainFrame(void)
{
	m_pPanelProjectTree = NULL;
	m_hWndMDIClient     = NULL;
	m_pDlgFirstPage     = NULL;
    m_bDirty            = false;
	m_pPropertyDialog   = NULL;
    m_pToolBox          = NULL;
    m_pStatusBar        = NULL;
    m_pToolBar          = NULL;

    m_hAccel = NULL;
    m_pCurViewKey = NULL;

	g_pGlobalData->m_pMainFrame = this;
}

CMainFrame::~CMainFrame(void)
{
    if (m_hAccel)
    {
        DestroyAcceleratorTable(m_hAccel);
        m_hAccel = NULL;
    }

	if (g_pGlobalData)
    {
		g_pGlobalData->m_pMainFrame = NULL;
    }
}


LRESULT CMainFrame::OnCreate( LPCREATESTRUCT lpcs )
{
    HICON  hIcon = ::LoadIcon(_Module.m_hInst, MAKEINTRESOURCE(IDI_UIEDITOR));
    SendMessage(m_hWnd, WM_SETICON, (WPARAM)ICON_SMALL, (LPARAM)hIcon);
    DestroyIcon(hIcon);

	// 加载主窗口菜单
	HMENU hMenu = ::LoadMenu(NULL, MAKEINTRESOURCE(IDR_UIBUILDER_UNOPEN));
	this->SetMenu( hMenu );
	DestroyMenu(hMenu);

	// 允许拖拽文件打开工程
	// ::DragAcceptFiles(m_hWnd,TRUE);

	// 快捷键消息拦截
    g_pGlobalData->m_pMyUIApp->GetMessageFilterMgr()->AddMessageFilter(this);

	// 显示首页窗口
    m_pDlgFirstPage = CFirstPageDlg::CreateInstance(
        g_pGlobalData->m_pMyUIApp->GetDefaultSkinRes());

    m_pDlgFirstPage->SetMainFrame(this);
	m_pDlgFirstPage->Create(_T("FirstPageDlg"), m_hWnd, 0, WS_CHILD|WS_VISIBLE);

	// 不刷新子窗口处背景
	this->ModifyStyle( 0, WS_CLIPCHILDREN );

    // 创建工具栏
    m_pToolBar = CToolBar::CreateInstance(
        g_pGlobalData->m_pMyUIApp->GetDefaultSkinRes());

	m_pToolBar->Create(_T("ToolBar"), m_hWnd, 0, WS_CHILD|WS_VISIBLE);
	g_pGlobalData->m_pToolBar = m_pToolBar;

    // 创建状态栏
    m_pStatusBar = CStatusBar::CreateInstance(
        g_pGlobalData->m_pMyUIApp->GetDefaultSkinRes());

    m_pStatusBar->Create(_T("StatusBar"), m_hWnd, 0, WS_CHILD|WS_VISIBLE);
    g_pGlobalData->m_pStatusBar = m_pStatusBar;

    // 加载快捷键
    m_hAccel = LoadAccelerators(
            GetModuleHandle(NULL), 
            MAKEINTRESOURCE(IDC_UIEDITOR));
	return 1;
}

//
// 创建多文档窗口MDIClient，注册相应类型的子窗口
//
void CMainFrame::_InitMDIClient()
{
    if (m_hWndMDIClient)
        return;

	//////////////////////////////////////////////////////////////////////////
	// 创建MDIClient

	CLIENTCREATESTRUCT ccs = { NULL, 1 };
	this->m_hWndMDIClient = CreateWindow( _T("MDICLIENT"), (LPCTSTR) NULL, 
		WS_CHILD | WS_CLIPCHILDREN , 
		0, 0, 0, 0, m_hWnd, (HMENU) NULL, _Module.m_hInst, (LPSTR) &ccs); 

	::ShowWindow(m_hWndMDIClient, SW_SHOW); 
}

void  CMainFrame::OnNewWindow(UINT,int,HWND)
{
    if (!g_pGlobalData->m_pProjectData)
        return;

    CCreateWindowDlg* pDlg = CCreateWindowDlg::CreateInstance(
        g_pGlobalData->m_pMyUIApp->GetDefaultSkinRes());

    if (IDOK == pDlg->DoModal(_T("createwindowdlg"), m_hWnd, false))
    {
		SetDirty();
    }
    SAFE_RELEASE(pDlg);
}

void CMainFrame::OnMenuExit(UINT,int,HWND)
{
	this->OnClose();
}
void CMainFrame::OnClose()
{
    if (m_bDirty)
    {
        if (IDOK == ::MessageBox( NULL, _T("是否保存所有文件?"), _T("关闭"), MB_OKCANCEL|MB_ICONQUESTION))
        {
            DoSave();
        }
        m_bDirty = false;  // 强制清除，避免在后面的_CloseProject函数中又弹出一次提示 
    }

	this->DestroyWindow();
}

void CMainFrame::OnDestroy()
{
    if (m_pDlgFirstPage)
    {
        if (::IsWindow(m_pDlgFirstPage->GetHWND()))
            ::DestroyWindow(m_pDlgFirstPage->GetHWND());
        SAFE_RELEASE(m_pDlgFirstPage);
    }

	if (m_pToolBar)
	{
		if (::IsWindow(m_pToolBar->GetHWND()))
			::DestroyWindow(m_pToolBar->GetHWND());
        SAFE_RELEASE(m_pToolBar);
	}

    if (m_pStatusBar)
    {
        if (::IsWindow(m_pStatusBar->GetHWND()))
            ::DestroyWindow(m_pStatusBar->GetHWND());
        SAFE_RELEASE(m_pStatusBar);
    }
    _CloseProject(true);

    g_pGlobalData->m_pMyUIApp->GetMessageFilterMgr()->RemoveMessageFilter(this);
	PostQuitMessage(0);
}

BOOL CMainFrame::PreTranslateMessage(MSG* pMsg)
{
    if (m_hAccel && m_hWnd)
    {
        TranslateAccelerator(m_hWnd, m_hAccel, pMsg);
    }
	return FALSE;
}

// 接受到一个命令（如快捷键的发出来的命令）时，要先检测该菜单项当前是否可用。
// 这里使用toolbar来检测
bool  CMainFrame::TestCommandIsValid(UINT nCmd)
{
    if (m_pToolBar && !m_pToolBar->TestCommandIsValid(nCmd))
        return false;

    return true;
}

//
//	设置菜单有有效性
//
void CMainFrame::OnInitMenuPopup(HMENU hMenuPopup, UINT nIndex, BOOL bSysMenu)
{
	int nCount = ::GetMenuItemCount(hMenuPopup);
	for (int i = 0; i < nCount; i++)
	{
		int nCommand = ::GetMenuItemID(hMenuPopup,i);
		switch(nCommand)
		{
		case ID_NEW_SKIN:
		case ID_NEW_FILE:
			{
				if (g_pGlobalData->m_pProjectData)
					::EnableMenuItem( hMenuPopup, nCommand, MF_ENABLED|MF_BYCOMMAND );
				else
					::EnableMenuItem( hMenuPopup, nCommand, MF_GRAYED|MF_DISABLED|MF_BYCOMMAND);
			}
			break;

		case ID_NEW_PROJECT:
			{
				if (g_pGlobalData->m_pProjectData)
					::EnableMenuItem( hMenuPopup, nCommand, MF_GRAYED|MF_DISABLED );
				else
					::EnableMenuItem( hMenuPopup, nCommand, MF_ENABLED );
			}
			break;

		case IDM_SAVE:
			{
				UINT nFlag = MF_ENABLED;
				if (!g_pGlobalData->m_pProjectData) 
                {
					nFlag = MF_GRAYED|MF_DISABLED;
                }
				else
				{
					if (m_pPanelProjectTree && false == m_bDirty)
						nFlag = MF_GRAYED|MF_DISABLED;
				}

				::EnableMenuItem(hMenuPopup, nCommand, nFlag);
			}
			break;

        case IDM_UNDO:
        case IDM_REDO:
            {
                UINT nFlag = MF_ENABLED;
                if (m_pToolBar && !m_pToolBar->TestCommandIsValid(nCommand))
                {
                    nFlag = MF_GRAYED|MF_DISABLED;
                }
                ::EnableMenuItem(hMenuPopup, nCommand, nFlag);
            }
            break;

		case ID_MENU_MDI_MINIMIZE:
		case ID_MENU_MDI_RESTORE:
		case ID_MENU_MDI_CLOSE:
			{
				BOOL bMax = FALSE; 
				HWND hWnd = (HWND)::SendMessage(m_hWndMDIClient, WM_MDIGETACTIVE, 0, (LPARAM)&bMax);
				if( NULL != hWnd && bMax )
					::EnableMenuItem( hMenuPopup, nCommand, MF_ENABLED );
				else
					::EnableMenuItem( hMenuPopup, nCommand, MF_GRAYED|MF_DISABLED );
			}
			break;

		case ID_WINDOW_ARRANGE:
		case ID_WINDOW_CASCADE:
		case ID_WINDOW_TILE_HORZ:
			break;
		}
	}
}

void CMainFrame::OnMenuNewProject(UINT,int,HWND)
{
	if (g_pGlobalData->m_pProjectData)
	{
		::MessageBox(NULL, _T("请先关闭当前工程"), _T("Error"), MB_OK|MB_ICONWARNING);
		return;
	}

	CNewProjectDlg dlg(true);
	if (IDCANCEL == dlg.DoModal())
		return;

    if (g_pGlobalData->OpenUIProj(dlg.m_strUIProjPath.c_str()))
    {
		// 加载刚创建的皮肤
		GetEditUIApplication()->LoadSkinRes(dlg.m_strSkinFolder.c_str());

        _InitOnOpenOrNewProject(dlg.m_strUIProjPath.c_str());

		// 创建第一个工程时，强制保存uiproj文件
		GetProjectData()->m_uiprojParse.SetDirty(true);
		GetProjectData()->m_uiprojParse.Save();
    }
}

void CMainFrame::OnMenuOpenProject(UINT,int,HWND lParam)
{
	if (g_pGlobalData->m_pProjectData)
	{
		::MessageBox(NULL, _T("请先关闭当前工程"), _T("Error"), MB_OK|MB_ICONWARNING );
		return;
	}

	String strProjPath;
	RecentProjItem* pItem = (RecentProjItem*)lParam;
	if (NULL == pItem)
	{
        CFileDialog dlg(
            TRUE,
            XML_PROJECT_EXT, 
            NULL, 
            OFN_NONETWORKBUTTON | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, 
            _T("*.uiproj\0*.uiproj\0\0"));
        if (IDCANCEL == dlg.DoModal())
            return;

		strProjPath = dlg.m_szFileName;
	}
	else  // 通过最近打开文件发送的通知
	{
		strProjPath = pItem->szFilePath;
	}

    if (!g_pGlobalData->OpenUIProj(strProjPath.c_str()))
    {
        TCHAR szText[320] = _T("");
        _stprintf(szText, _T("打开工程文件失败: %s"), strProjPath.c_str());
        m_pStatusBar->SetStatusText2(szText);
        return;
    }

    _InitOnOpenOrNewProject(strProjPath);
}

void  CMainFrame::OnMenuCloseProject(UINT,int,HWND)
{
    if (!g_pGlobalData->m_pProjectData)
        return;

    _CloseProject(false);
    g_pGlobalData->CloseUIProj();  // 先别删除pUIEditApp，因为当前可能还有UI窗口没有销毁
}

void CMainFrame::OnMenuProjDependsCtrlDll(UINT, int, HWND)
{   
    CProjectDependsCtrlDllConfigDlg* pDlg = 
        CProjectDependsCtrlDllConfigDlg::CreateInstance(
            g_pGlobalData->m_pMyUIApp->GetDefaultSkinRes());

    pDlg->DoModal(_T("ProjectDependsCtrlDllConfigDlg"), m_hWnd, false);
    SAFE_RELEASE(pDlg);

    // 更新数据
	GetProjectData()->ReRegistUIObject(true);
	SetDirty();
}

void CMainFrame::OnMenuToolBox(UINT, int, HWND)
{
    if (!m_pToolBox)
    {
        m_pToolBox = CToolBox::CreateInstance(
            g_pGlobalData->m_pMyUIApp->GetDefaultSkinRes());

        g_pGlobalData->m_pToolBox = m_pToolBox;

        m_pToolBox->Create(_T("ToolBox"), m_hWnd, 0, 0, WS_EX_TOOLWINDOW);
    }
    CRect rcWindow;
    ::GetWindowRect(m_hWnd, &rcWindow);
    ::SetWindowPos(m_pToolBox->GetHWND(), 0, rcWindow.right-320, 50, 300, rcWindow.Height()-70, SWP_NOZORDER|SWP_NOACTIVATE|SWP_SHOWWINDOW);
    return;
}

void  CMainFrame::OnMenuViewProperty(UINT, int, HWND)
{
    if (!m_pPropertyDialog)
        return;

    m_pPropertyDialog->ShowWindow();
}

void CMainFrame::_InitOnOpenOrNewProject(const String& strProjPath)
{
    m_strSkinProjPath = strProjPath;
    g_pGlobalData->m_pRecentProj->Add(m_strSkinProjPath.c_str());

    // 创建多文档窗口MDIClient
    this->_InitMDIClient();

    // 启用工程菜单
    HMENU hMenu = this->GetMenu();
    if (hMenu)
    {
        DestroyMenu(hMenu);
        hMenu = LoadMenu(_Module.m_hInst, MAKEINTRESOURCE(IDR_UIBUILDER_OPEN));
        this->SetMenu(hMenu);
    }

	CRect  rc;
	this->GetClientRect(&rc);

	if (m_pDlgFirstPage)
	{
        m_pDlgFirstPage->DestroyWindow();
		SAFE_RELEASE(m_pDlgFirstPage);
	}

	if (NULL == m_pPanelProjectTree)
	{
        m_pPanelProjectTree = CProjectTreeDialog::CreateInstance(
            g_pGlobalData->m_pMyUIApp->GetDefaultSkinRes());
        m_pPanelProjectTree->Create(
            _T("ProjectTreeDialog"), m_hWnd, 0, WS_CHILD|WS_VISIBLE);
        m_pPanelProjectTree->SetMainFrame(this);
	}

	if (NULL == m_pPropertyDialog)
	{
		CRect rcWindow;
		this->GetWindowRect(&rcWindow);

        m_pPropertyDialog = CPropertyDialog::CreateInstance(
                g_pGlobalData->m_pMyUIApp->GetDefaultSkinRes());

#if 0 // 弹出式窗口
		m_pPropertyDialog->DoModeless(
                _Module.m_hInst, 
                m_pPropertyDialog->IDD, 
                _T("PropertyDialog"), 
                m_hWnd);

        HMONITOR  hMonitor = MonitorFromWindow(m_hWnd, MONITOR_DEFAULTTOPRIMARY);
        MONITORINFO  mi = {0};
        CRect  rcWorkArea;
        if (GetMonitorInfo(hMonitor, &mi))
        {
            rcWorkArea = mi.rcWork;
        }
        else
        {
            ::SystemParametersInfo(SPI_GETWORKAREA, 0, &rcWorkArea, 0);
        }

		::SetWindowPos(m_pPropertyDialog->GetHWND(), 0, rcWindow.right-420, rcWindow.top+50, 400, rcWindow.Height()-100, SWP_NOZORDER|SWP_NOACTIVATE);

#else // 嵌入式窗口
        m_pPropertyDialog->Create(_T("PropertyDialog"), m_hWnd, NULL, WS_CHILD|WS_VISIBLE);
#endif
	}
    if (m_pToolBar)
    {
        m_pToolBar->OnProjectOpened();
    }
    this->CalcLayout(rc.Width(), rc.Height());

    String strText = _T("UI Editor - ");
    strText.append(m_strSkinProjPath);
    ::SetWindowText(m_hWnd, strText.c_str());

    this->m_bDirty = false;
}

// bExit: 是退出进程，还是关闭工程
void  CMainFrame::_CloseProject(bool bExit)
{
    if (m_bDirty)
    {
        if (IDOK == ::MessageBox( NULL, _T("是否保存所有文件?"), _T("关闭"), MB_OKCANCEL|MB_ICONQUESTION))
        {
            DoSave();
        }
    }

	// 先关闭打开的窗口，这些窗口释放时可能要和工程面板交互
	OnMenuMDIWindow(0, IDM_MDI_CLOSEALLWINDOW, 0);
	if (m_hWndMDIClient)
	{
		::DestroyWindow(m_hWndMDIClient);
		m_hWndMDIClient = NULL;
	}


    m_strSkinProjPath.clear();

    if (m_pPanelProjectTree)
    {
        if (::IsWindow(m_pPanelProjectTree->GetHWND()))
            ::DestroyWindow(m_pPanelProjectTree->GetHWND());
        SAFE_RELEASE(m_pPanelProjectTree);
    }
    
    if (m_pPropertyDialog)
    {
        if (::IsWindow(m_pPropertyDialog->GetHWND()))
            ::DestroyWindow(m_pPropertyDialog->GetHWND());
        SAFE_RELEASE(m_pPropertyDialog);
    }

    if (m_pToolBox)
    {
        if (::IsWindow(m_pToolBox->GetHWND()))
            ::DestroyWindow(m_pToolBox->GetHWND());
        SAFE_RELEASE(m_pToolBox);
    }

    if (!bExit)
    {
        // 显示首页窗口
        if (!m_pDlgFirstPage)
        {
            m_pDlgFirstPage = CFirstPageDlg::CreateInstance(
                g_pGlobalData->m_pMyUIApp->GetDefaultSkinRes());
            m_pDlgFirstPage->SetMainFrame(this);
            m_pDlgFirstPage->Create(
                _T("FirstPageDlg"), m_hWnd, 0, WS_CHILD|WS_VISIBLE);
        }

        // 换回启动菜单
        HMENU hMenu = this->GetMenu();
        if (hMenu)
        {
            DestroyMenu(hMenu);
            hMenu = LoadMenu(_Module.m_hInst, MAKEINTRESOURCE(IDR_UIBUILDER_UNOPEN));
            this->SetMenu(hMenu);
        }

        CRect  rcClient;
        ::GetClientRect(m_hWnd, &rcClient);
        this->CalcLayout(rcClient.Width(), rcClient.Height());
    }

}

void CMainFrame::OnMenuNewSkin(UINT,int,HWND)
{
// 	CNewProjectDlg dlg(false);
// 	if (IDCANCEL == dlg.DoModal())
// 		return;
// 
//     ISkinRes* pSkin = g_pGlobalData->m_pProjectData->m_pEditSkinMgr->AddSkin(dlg.m_strSkinName.c_str());
// 	m_pPanelProjectTree->InsertSkinItem(pSkin);
}

void CMainFrame::OnMenuSave(UINT,int,HWND)
{
    DoSave();
}
void  CMainFrame::OnMenuUndo(UINT,int,HWND)
{
    g_pGlobalData->m_pCmdHistroyMgr->Undo(m_pCurViewKey);
}
void  CMainFrame::OnMenuRedo(UINT,int,HWND)
{
    g_pGlobalData->m_pCmdHistroyMgr->Redo(m_pCurViewKey);
}
void CMainFrame::DoSave()
{
	if (!g_pGlobalData->m_pProjectData || false == m_bDirty)
		return;

	if (false == g_pGlobalData->m_pProjectData->m_pEditSkinMgr->Save(NULL))
        return;

	GetProjectData()->m_uiprojParse.Save();

    TCHAR  szText[320] = _T("");
    ::GetWindowText(m_hWnd, szText, 300);
    int nLength = _tcslen(szText);
    if (szText[nLength-1] == _T('*'))
    {
        szText[nLength-1] = _T('\0');
        ::SetWindowText(m_hWnd, szText);
    }

	// 清除dirty
	m_bDirty = false;

    if (!m_bDirty)
        m_pToolBar->OnDirtyChanged(false);
}
void CMainFrame::OnMenuMDIWindow( UINT,int id,HWND )
{
	switch (id)
	{
	case ID_WINDOW_CASCADE:
		::SendMessage(m_hWndMDIClient, WM_MDICASCADE, MDITILE_ZORDER, 0);
		break;
	case ID_WINDOW_TILE_HORZ:
		::SendMessage(m_hWndMDIClient, WM_MDITILE, MDITILE_HORIZONTAL, 0);
		break;
	case ID_WINDOW_ARRANGE:
		::SendMessage(m_hWndMDIClient, WM_MDIICONARRANGE, 0, 0);
		break;
	case ID_MENU_MDI_CLOSE:
    case SC_CLOSE:
		{
			BOOL bMaximized = FALSE;
			HWND hWndChild = (HWND)::SendMessage(m_hWndMDIClient, WM_MDIGETACTIVE, 0, (LPARAM)&bMaximized);
			if (hWndChild)
			{
				::SendMessage(hWndChild, WM_SYSCOMMAND, SC_CLOSE, 0L);
			}
		}
		break;
	case ID_MENU_MDI_MINIMIZE:
    case SC_MINIMIZE:
		{
			BOOL bMaximized = FALSE;
			HWND hWndChild = (HWND)::SendMessage(m_hWndMDIClient, WM_MDIGETACTIVE, 0, (LPARAM)&bMaximized);
			if (hWndChild)
			{
				::SendMessage(hWndChild, WM_SYSCOMMAND, SC_MINIMIZE, 0L);
			}
		}
		break;
	case ID_MENU_MDI_RESTORE:
    case SC_RESTORE:
		{
			BOOL bMaximized = FALSE;
			HWND hWndChild = (HWND)::SendMessage(m_hWndMDIClient, WM_MDIGETACTIVE, 0, (LPARAM)&bMaximized);
			if (hWndChild)
			{
				::SendMessage(hWndChild, WM_SYSCOMMAND, SC_RESTORE, 0L);
			}
		}
		break;

	case IDM_MDI_CLOSEALLWINDOW:
		{
			while(1)
			{
				list<CChildFrame*>::iterator iter = m_listChildFrame.begin();
				list<CChildFrame*>::iterator iterEnd = m_listChildFrame.end();
				if (iter == iterEnd)
					break;

				CChildFrame* pFrame = *iter;
				if (NULL == pFrame)
					continue;

				::SendMessage(pFrame->m_hWnd, WM_SYSCOMMAND, SC_CLOSE, 0L); // <-- 该操作会引发m_listChildFrame.erase
			}
		}
		break;
		
	case IDM_MDI_CLOSEALLWINDOW_BUTCUR:
		{
			BOOL bMaximized = FALSE;
			HWND hWndActiveChild = (HWND)::SendMessage(m_hWndMDIClient, WM_MDIGETACTIVE, 0, (LPARAM)&bMaximized);

			while (1)
			{
				list<CChildFrame*>::iterator iter = m_listChildFrame.begin();
				list<CChildFrame*>::iterator iterEnd = m_listChildFrame.end();

				if (iter == iterEnd)
					break;

				CChildFrame* pFrame = *iter;
				if (NULL == pFrame)
					continue;

				if (pFrame->m_hWnd == hWndActiveChild)
				{
					iter++;
					if (iter == iterEnd)
						break;
				}

				pFrame = *iter;
				if (NULL == pFrame)
					continue;

				::SendMessage(pFrame->m_hWnd, WM_SYSCOMMAND, SC_CLOSE, 0L);
			}
		}
		break;
	}
}

void CMainFrame::OnSize(UINT nType, CSize size)
{
    if (nType == SIZE_MINIMIZED)
        return;

	this->CalcLayout(size.cx, size.cy);
}

void  CMainFrame::CalcLayout()
{
    CRect  rcClient;
    ::GetClientRect(m_hWnd, &rcClient);
    this->CalcLayout(rcClient.Width(), rcClient.Height());
}

void CMainFrame::CalcLayout(int cx, int cy)
{
    CRect  rcClient(0, 0, cx, cy);

	int nLeftW = 0;
    if (m_pToolBar)
    {
        ::SetWindowPos(m_pToolBar->GetHWND(), NULL, 0, 0, cx, TOOLBAR_HEIGHT, SWP_NOZORDER);
        rcClient.top += TOOLBAR_HEIGHT;
    }

    if (m_pStatusBar)
    {
        ::SetWindowPos(m_pStatusBar->GetHWND(), NULL, 0, cy-STATUSBAR_HEIGHT, cx, STATUSBAR_HEIGHT, SWP_NOZORDER);
        rcClient.bottom -= STATUSBAR_HEIGHT;
    }

	if (m_pPanelProjectTree)
	{
        ::SetWindowPos(m_pPanelProjectTree->GetHWND(), NULL, rcClient.left, rcClient.top, 
            LEFT_TREE_WINDOW_WIDTH, rcClient.Height(), SWP_NOZORDER);
		rcClient.left += LEFT_TREE_WINDOW_WIDTH;
	}

    if (m_pPropertyDialog && //::IsWindowVisible(m_pPropertyDialog->GetHWND()) && 
        ::GetWindowLong(m_pPropertyDialog->GetHWND(),GWL_STYLE)&WS_CHILD)
    {
        ::SetWindowPos(m_pPropertyDialog->GetHWND(), NULL,
            rcClient.right-PROPERTY_DLG_WIDTH, rcClient.top, 
            PROPERTY_DLG_WIDTH, rcClient.Height(), SWP_NOZORDER);
        rcClient.right -= PROPERTY_DLG_WIDTH;
    }

	if (m_hWndMDIClient)
	{
		::SetWindowPos(m_hWndMDIClient, NULL, 
            rcClient.left, rcClient.top, rcClient.Width(), rcClient.Height() ,SWP_NOZORDER);
	}

	if (m_pDlgFirstPage)
	{
		m_pDlgFirstPage->CenterWindow(m_hWnd);
	}
}

void  CMainFrame::SetDirty()
{
    if (false == m_bDirty)
    {
        TCHAR  szText[320] = _T("");
        ::GetWindowText(m_hWnd, szText, 300);
        _tcscat(szText, _T("*"));
        ::SetWindowText(m_hWnd, szText);
    }

    if (!m_bDirty)
        m_pToolBar->OnDirtyChanged(true);

    m_bDirty = true;
}

LRESULT CMainFrame::OnEraseBkgnd( HDC hDC )
{
	if(m_hWndMDIClient)   // view will paint itself instead
		return 1;

    HBRUSH hBrush = (HBRUSH)CreateSolidBrush(RGB(171,171,171));
    CRect  rc;
    this->GetClientRect(&rc);
    ::FillRect(hDC, &rc, hBrush);
    DeleteObject(hBrush);
    return TRUE;

	SetMsgHandled(FALSE);
	return 0;
}


void CMainFrame::OnDropFiles(HDROP hDropInfo)
{
#if 0 
	int nCount = ::DragQueryFile(hDropInfo, UINT(-1), NULL, 0 );
	if (nCount > 1)
		return;

	TCHAR szPath[256] = _T("");
	::DragQueryFile(hDropInfo, 0, szPath, 256 );

	// 文件格式判断
	String strExt  = XML_UI_XMLFILE_EXT;
	String strPath = szPath;
	int    nLength = strExt.length();

	if (strPath.substr(strPath.length()-nLength,nLength) != strExt)
		return;

	this->_OpenProject(szPath);
#endif 
}

HWND  CMainFrame::GetImageWnd(ISkinRes* pSkinRes)
{
	list<CChildFrame*>::iterator iter = m_listChildFrame.begin();
	list<CChildFrame*>::iterator iterEnd = m_listChildFrame.end();
	for (; iter != iterEnd; iter++)
	{
		CChildFrame* p = *iter;
		if (p->m_pClientView->GetSkinRes() == pSkinRes && p->m_pClientView->GetResourceType() == UI_RESOURCE_IMAGE)
		{
			return p->m_pClientView->GetHWND();
		}
	}
	return NULL;
}

//
//	切换到皮肤资源窗口
//
//	Parameter
//		nType
//			[in]	资源类型
//
//		pSkin
//			[in]	当前资源所属皮肤 
//
//
void CMainFrame::SwitchToSkinResPanel(UI_RESOURCE_TYPE nType, ISkinRes* pSkin)
{
// 	TCHAR pszDataSource[MAX_PATH] = _T("");
// 	::UI_GetResourceDataSourcePath(pSkin, nType, pszDataSource, MAX_PATH);

	list<CChildFrame*>::iterator iter = m_listChildFrame.begin();
	list<CChildFrame*>::iterator iterEnd = m_listChildFrame.end();
	for (; iter != iterEnd; iter++)
	{
		CChildFrame* p = *iter;
		if (p->m_pClientView->GetSkinRes() == pSkin && p->m_pClientView->GetResourceType() == nType)
		{
			// 该窗口已经存在，激活它
			::SendMessage(m_hWndMDIClient, WM_MDIACTIVATE, (WPARAM)p->m_hWnd, NULL );
			return;
		}
	}

	switch( nType )
	{
	case UI_RESOURCE_IMAGE:
		{
			CChildFrame* pChildFrame = new CChildFrame(this);    // 自释放,OnFinalMessage
			CImageEditorDlg* pView = CImageEditorDlg::CreateInstance(
                g_pGlobalData->m_pMyUIApp->GetDefaultSkinRes());

            CRect rcClient(0, 0, 500,500);
			pChildFrame->Create(m_hWndMDIClient);
			pView->Create(_T("imageviewdlg"), pChildFrame->m_hWnd, &rcClient,
                WS_CHILD|WS_VISIBLE|WS_CLIPCHILDREN);
            
			pView->SetSkinRes(pSkin);

			pChildFrame->SetClientView(static_cast<IMDIClientView*>(pView));
			this->AddChildFrame(pChildFrame);
		}
		break;
	case UI_RESOURCE_COLOR:
		{
			CChildFrame* pChildFrame = new CChildFrame(this);      // 自释放,OnFinalMessage
			CColorViewDlg* pView = new CColorViewDlg;              // Release时自释放

			pChildFrame->Create(m_hWndMDIClient);
			pView->Create(pChildFrame->m_hWnd);
			pView->SetSkinRes(pSkin);
			pView->ShowWindow(SW_SHOW);

			pChildFrame->SetClientView(static_cast<IMDIClientView*>(pView));
			this->AddChildFrame(pChildFrame);
		}
		break;
	case UI_RESOURCE_FONT:
		{
			CChildFrame* pChildFrame = new CChildFrame(this);    // 自释放,OnFinalMessage
			CFontViewDlg* pView = new CFontViewDlg;              // Release时自释放

			pChildFrame->Create(m_hWndMDIClient);
			pView->Create(pChildFrame->m_hWnd);
			pView->SetSkinRes(pSkin);
			pView->ShowWindow(SW_SHOW);

			pChildFrame->SetClientView(static_cast<IMDIClientView*>(pView));
			this->AddChildFrame(pChildFrame);
		}
		break;

 	case UI_RESOURCE_STYLE:
 		{
			CChildFrame* pChildFrame = new CChildFrame(this);    // 自释放,OnFinalMessage
			CStyleEditorWnd* pView = new CStyleEditorWnd;

			pChildFrame->Create(m_hWndMDIClient);
			pView->Create(pChildFrame->m_hWnd);
			pView->SetSkinRes(pSkin);
			pView->ShowWindow(SW_SHOW);

			pChildFrame->SetClientView(static_cast<IMDIClientView*>(pView));
			this->AddChildFrame(pChildFrame);
 		}
 		break;

	case UI_RESOURCE_I18N:
	{
		CChildFrame* pChildFrame = new CChildFrame(this);    // 自释放,OnFinalMessage
		CI18nEditorDlg* pView = CI18nEditorDlg::CreateInstance(
			g_pGlobalData->m_pMyUIApp->GetDefaultSkinRes());

		CRect rcClient(0, 0, 500, 500);
		pChildFrame->Create(m_hWndMDIClient);
		pView->Create(_T("i18nview"), pChildFrame->m_hWnd, &rcClient,
			WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN);

		pView->SetSkinRes(pSkin);

		pChildFrame->SetClientView(static_cast<IMDIClientView*>(pView));
		this->AddChildFrame(pChildFrame);
	}
	break;
#if 0
	case UI_RESOURCE_LAYOUT:
		{
			CChildFrame* pChildFrame = new CChildFrame(this);    // 自释放,OnFinalMessage
			CLayoutBuildDlg* pView = new CLayoutBuildDlg;        // 自释放,OnFinalMessage

			pChildFrame->Create(m_hWndMDIClient);
			pView->Create(pChildFrame->m_hWnd);
			pView->AttachSkin(hSkin);
			pView->ShowWindow(SW_SHOW);

			pChildFrame->SetView(pView->m_hWnd, hSkin, nType);
			this->AddChildFrame(pChildFrame);
		}
		break;
#endif
	}
}

void  CMainFrame::SwitchToExplorerPanel(ExplorerTreeItemData* pData)
{
	if (NULL == pData)
		return;

	list<CChildFrame*>::iterator iter = m_listChildFrame.begin();
	list<CChildFrame*>::iterator iterEnd = m_listChildFrame.end();
	for (; iter != iterEnd; iter++)
	{
		CChildFrame* p = *iter;
		if (NULL == p || NULL == p->m_pClientView)
			continue;

		if (p->m_pClientView->GetSkinRes() == pData->m_pSkin && p->m_pClientView->GetResourceType() == UI_RESOURCE_SOURCEFILE)
		{
			IExplorerMDIClientView* pView = (IExplorerMDIClientView*)
			    p->m_pClientView->QueryInterface(IID_IExplorerMDIClientView);

			if (NULL == pView)
				continue;

			TCHAR szPath[MAX_PATH] = _T("");
			pView->GetFilePath(szPath);

			if (0 == _tcsicmp(szPath, pData->m_strFilePath.c_str()))
			{
				// 该窗口已经存在，激活它
				::SendMessage(m_hWndMDIClient, WM_MDIACTIVATE, (WPARAM)p->m_hWnd, NULL );
				return;
			}
		}
	}

	CChildFrame* pChildFrame = new CChildFrame(this);    // 自释放,OnFinalMessage
	CCodeWebBrowser* pView = new CCodeWebBrowser;        // Release释放

	RECT rc = {0,0, 700, 450};
	pChildFrame->Create(m_hWndMDIClient, &rc);
	pView->Create(pChildFrame->m_hWnd);
	pView->SetFilePath(pData->m_strFilePath.c_str());
	pView->SetSkinRes(pData->m_pSkin);
	pView->ShowWindow(SW_SHOW);

	pChildFrame->SetClientView(static_cast<IMDIClientView*>(pView));
	this->AddChildFrame(pChildFrame);
}

HWND  CMainFrame::GetLayoutHWND(IObject* pObj)
{
    CChildFrame*  pFrame = GetLayoutChildFrameByObject(pObj);
	if (!pFrame || !pFrame->m_pClientView)
		return NULL;
    
	return pFrame->m_pClientView->GetHWND();
}

void CMainFrame::SwitchToLayoutPanel(LayoutTreeItemData* pData)
{
	if (NULL == pData)
		return;

    if (NULL == pData->m_pObject)
    {
        // 懒加载窗口
        if (pData->m_strNodeName.empty())
        {
            IUIElement* pUIElement = NULL;
            pData->m_pSkin->GetLayoutManager().FindWindowElement(
                    NULL, pData->m_strNodeID.c_str(), &pUIElement);

            if (pUIElement)
            {
                CComBSTR  bstrTagName = pUIElement->GetTagName();
                pData->m_strNodeName = bstrTagName;
                SAFE_RELEASE(pUIElement);
            }
        }

        CChildFrame* pChildFrame = new CChildFrame(this);    // 自释放,OnFinalMessage
        CLayoutEditorDlg* pView = CLayoutEditorDlg::CreateInstance(
            g_pGlobalData->m_pMyUIApp->GetDefaultSkinRes());

        pChildFrame->Create(m_hWndMDIClient, NULL);
        pView->Create(
			_T("layoutview"), 
			pChildFrame->m_hWnd,
			0, 
			WS_CHILD|WS_VISIBLE);
		pView->SetSkinRes(pData->m_pSkin);
		pView->ShowLayoutNode(pData);

        pChildFrame->SetClientView(static_cast<IMDIClientView*>(pView));
        pChildFrame->UpdateLayout();

        this->AddChildFrame(pChildFrame);
        return;
    }

	list<CChildFrame*>::iterator iter = m_listChildFrame.begin();
	list<CChildFrame*>::iterator iterEnd = m_listChildFrame.end();
	for (; iter != iterEnd; iter++)
	{
		CChildFrame* p = *iter;
		if (NULL == p || NULL == p->m_pClientView)
			continue;

		// 需要判断：同一个皮肤的同一个窗口
		if (p->m_pClientView->GetSkinRes() != pData->m_pSkin ||
            p->m_pClientView->GetResourceType() != UI_RESOURCE_LAYOUT)
            continue;

        ILayoutMDIClientView* pView = (ILayoutMDIClientView*)
            p->m_pClientView->QueryInterface(IID_ILayoutMDIClientView);
        if (NULL == pView)
            continue;

        IWindow*  pWindow = pView->GetWindow();
        if (!pWindow)
            continue;

        if (pWindow == pData->m_pObject->GetWindowObject())
        {
            // 该窗口已经存在，激活它
            ::SendMessage(m_hWndMDIClient, WM_MDIACTIVATE, (WPARAM)p->m_hWnd, NULL );
            pView->ShowLayoutNode(pData);
            return;
        }
	}
}

// void  CMainFrame::SwitchToMenuPanel(LayoutTreeItemData* pData)
// {
//     if (NULL == pData->m_pObject)
//     {
//         CChildFrame* pChildFrame = new CChildFrame(this);    // 自释放,OnFinalMessage
//         CMenuViewDlg* pView = NULL;
//         CMenuViewDlg::CreateInstance(g_pGlobalData->m_pMyUIApp, &pView);
// 
//         pChildFrame->Create(m_hWndMDIClient, NULL, 0, WS_CHILD|WS_VISIBLE);
//         pView->Create(_T("menuview"), pChildFrame->m_hWnd);
// 
//         pView->SetSkinRes(pData->m_pSkin);
//         pView->ShowLayoutNode(pData);
//         pView->ShowWindow();
// 
//         pChildFrame->SetClientView(static_cast<IMDIClientView*>(pView));
//         this->AddChildFrame(pChildFrame);
//         return;
//     }
// 
//     list<CChildFrame*>::iterator iter = m_listChildFrame.begin();
//     list<CChildFrame*>::iterator iterEnd = m_listChildFrame.end();
//     for (; iter != iterEnd; iter++)
//     {
//         CChildFrame* p = *iter;
//         if (NULL == p || NULL == p->m_pClientView)
//             continue;
// 
//         // 需要判断：同一个皮肤的同一个窗口
//         if (p->m_pClientView->GetSkinRes() != pData->m_pSkin ||
//             p->m_pClientView->GetResourceType() != UI_RESOURCE_LAYOUT)
//             continue;
// 
//         ILayoutMDIClientView* pView = (ILayoutMDIClientView*)
//             p->m_pClientView->QueryInterface(IID_ILayoutMDIClientView);
//         if (NULL == pView)
//             continue;
// 
//         IWindow*  pWindow = pView->GetWindow();
//         if (!pWindow)
//             continue;
// 
//         if (pWindow == pData->m_pObject->GetWindowObject())
//         {
//             // 该窗口已经存在，激活它
//             ::SendMessage(m_hWndMDIClient, WM_MDIACTIVATE, (WPARAM)p->m_hWnd, NULL );
//             pView->ShowLayoutNode(pData);
//             return;
//         }
//     }
// }

void CMainFrame::RemoveChildFrame(CChildFrame* pChildFrame)
{
	list<CChildFrame*>::iterator iter = m_listChildFrame.begin();
	list<CChildFrame*>::iterator iterEnd = m_listChildFrame.end();
	for (; iter != iterEnd; iter++)
	{
		CChildFrame* pThis = *iter;
		if (pThis == pChildFrame)
		{
			m_listChildFrame.erase(iter);
			break;
		}
	}
}
void CMainFrame::AddChildFrame( CChildFrame* pChildFrame )
{
	m_listChildFrame.push_back(pChildFrame);
}

//
//	切换到日志窗口
//

void  CMainFrame::HandleCmdLine(LPCTSTR szCmdLine)
{
    if (NULL == szCmdLine)
        return;

    int nLength = _tcslen(szCmdLine);
    int nExtLength = _tcslen(XML_PROJECT_EXT);  // uiproj

    if (nLength < nExtLength)  
        return;

    // 去掉命令行中的引号
    String  strProjFilePath;
    if (_T('"') == szCmdLine[0] &&  _T('"') == szCmdLine[nLength-1])
        strProjFilePath.insert(0, szCmdLine+1, nLength-2);
    else
        strProjFilePath = szCmdLine;

    if (g_pGlobalData->OpenUIProj(strProjFilePath.c_str()))
    {
        _InitOnOpenOrNewProject(strProjFilePath);
    }
}

CChildFrame*  CMainFrame::GetChildFrameByHWND(HWND hWnd)
{
    if (NULL == hWnd)
        return NULL;

    list<CChildFrame*>::iterator iter = m_listChildFrame.begin();
    list<CChildFrame*>::iterator iterEnd = m_listChildFrame.end();
    for (; iter != iterEnd; iter++)
    {
        CChildFrame* p = *iter;
        if (p->m_hWnd == hWnd)
        {
            return p;
        }
    }

    return NULL;
}

CChildFrame*  CMainFrame::GetLayoutChildFrameByObject(IObject* pObj)
{
    if (NULL == pObj)
        return NULL;

    list<CChildFrame*>::iterator iter = m_listChildFrame.begin();
    list<CChildFrame*>::iterator iterEnd = m_listChildFrame.end();
    for (; iter != iterEnd; iter++)
    {
        CChildFrame* p = *iter;
        if (UI_RESOURCE_LAYOUT != p->m_pClientView->GetResourceType())
            continue;

        ILayoutMDIClientView* pView = (ILayoutMDIClientView*)
            p->m_pClientView->QueryInterface(IID_ILayoutMDIClientView);

        if (!pView)
            continue;

        IWindow* pWindow = pView->GetWindow();
        if (pWindow)
        {
            if (static_cast<IObject*>(pWindow) != pObj &&
                !pWindow->IsMyChild(pObj, true))
                continue;
        }
        return p;
    }

    return NULL;
}
ILayoutMDIClientView*  CMainFrame::GetLayoutClientViewByObject(IObject* pObj)
{
    CChildFrame*  pFrame = GetLayoutChildFrameByObject(pObj);
    if (!pFrame)
        return NULL;

    return static_cast<ILayoutMDIClientView*>(pFrame->m_pClientView);
}


// 注：nMsg > 0xffff时SendMessage会失败
void  CMainFrame::ForwardCommnd2CurrentView(UINT nMsg, WPARAM wParam, LPARAM lParam)
{
    HWND hWnd = (HWND)SendMessage(m_hWndMDIClient, WM_MDIGETACTIVE, 0, 0);
    if (!hWnd)
        return;

    CChildFrame* pFrame = GetChildFrameByHWND(hWnd);
    if (!pFrame)
        return;

    ::SendMessage(pFrame->m_pClientView->GetHWND(), nMsg, wParam,  lParam);
}


void  CMainFrame::MDIActivateInd(long* pKey, bool bActivate)
{
    if (pKey == m_pCurViewKey)
    {
        if (!bActivate)
            m_pCurViewKey = NULL;
    }
    else
    {
        if (bActivate)
            m_pCurViewKey = pKey;
    }

    if (m_pToolBar)
    {
        m_pToolBar->UpdateBtnState();
    }
}
