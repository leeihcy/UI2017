#include "StdAfx.h"
#include "GlobalData.h"
#include "Dialog\Framework\MainFrame.h"
#include "Dialog\Framework\project_tree_dialog.h"
#include "Business\Command\Base\commandhistorymgr.h"
#include "Business\Project\ProjectData\projectdata.h"
#include "Dialog\Framework\statusbar.h"
#include "Dialog\Framework\propertydialog.h"
#include "Util\util.h"


IUIApplication*  GetEditUIApplication()
{
    CProjectData* pProjData = GetProjectData();
    if (!pProjData)
        return NULL;

    return pProjData->m_pEditUIApp;
}
ISkinManager*    GetEditSkinManager()
{
    CProjectData* pProjData = GetProjectData();
    if (!pProjData)
        return NULL;

    return pProjData->m_pEditSkinMgr;
}
CProjectData*    GetProjectData()
{
    return g_pGlobalData->m_pProjectData;
}
CUIEditor*  GetUIEditor()
{
    CProjectData* pData = GetProjectData();
    if (!pData)
        return NULL;

    return &pData->m_uieditor;
}

CDependsCtrlDllMgr*  GetProjDependsCtrlDllMgr()
{
    CProjectData* pProjData = GetProjectData();
    if (!pProjData)
        return NULL;

    return &(pProjData->m_mgrDependsCtrlDll);
}

CPropertyDialog*  GetPropertyDialog()
{
	return g_pGlobalData->m_pPropertyDialog;
}

IPropertyCtrl*   GetPropertyCtrl()
{
	if (!g_pGlobalData->m_pPropertyDialog)
		return NULL;

	return g_pGlobalData->m_pPropertyDialog->m_pPropertyCtrl;
}

CGlobalData::CGlobalData(void)
{
    m_pProjectData = NULL;
	m_pRecentProj = NULL;
	m_hWndMainFrame = NULL;
	m_pMainFrame = NULL;
	m_pProjectTreeDialog = NULL;
	m_pPropertyDialog = NULL;
    m_pStatusBar = NULL;
    m_pToolBar = NULL;
	m_pMyUIApp = NULL;
    m_pToolBox = NULL;
	m_pCmdHistroyMgr = NULL;
}

CGlobalData::~CGlobalData(void)
{
    m_pStatusBar = NULL;
    m_pToolBar = NULL;
    
    SAFE_DELETE(m_pCmdHistroyMgr);

	SAFE_DELETE(m_pRecentProj);
	SAFE_RELEASE(m_pMyUIApp);
    SAFE_DELETE(m_pProjectData);
}


bool CGlobalData::Init()
{
    CreateUIApplication(&m_pMyUIApp);     // 加载自己的皮肤
    UICTRL_RegisterUIObject(m_pMyUIApp);
    UIEditor_Ctrl_RegisterUIObject(m_pMyUIApp);

    
    // 加载日志
    // 由UI内部创建LOG实例，外部直接使用该指针
    TCHAR  szLogConfigPath[MAX_PATH] = _T("");
    GetUIEditorConfigFilePath(szLogConfigPath);

    // 加载皮肤
    TCHAR szPath[MAX_PATH] = _T("");
    GetAppPath_(szPath);
    String str = szPath;
    str.append(_T("Editor\\Default"));

    if (!m_pMyUIApp->LoadSkinRes(str.c_str()))
    {
        MessageBox(NULL, _T("加载皮肤失败"), _T("Error"), MB_OK|MB_ICONERROR);
		return false;
    }

	m_pRecentProj = new CRecentProjList;
	m_pRecentProj->Load();

    // 加载命令历史管理器
    m_pCmdHistroyMgr = new CCommandHistroyMgr;
	return true;
}

void CGlobalData::SetStatusText2(LPCTSTR szText)
{
    if (m_pStatusBar)
    {
        m_pStatusBar->SetStatusText2(szText);
    }
}

bool  CGlobalData::OpenUIProj(LPCTSTR  szProjPath)
{
    if (!szProjPath)
        return false;

    UIASSERT(NULL == m_pProjectData);
    m_pProjectData = new CProjectData;
    if (!m_pProjectData->OpenUIProj(szProjPath))
    {
        delete m_pProjectData;
        return false;
    }
    return true;
}
bool  CGlobalData::CloseUIProj()
{
    SAFE_DELETE(m_pProjectData);
    return true;
}