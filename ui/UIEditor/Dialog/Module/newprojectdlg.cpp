#include "stdafx.h"
#include "newprojectdlg.h"
#include "Business\Project\ProjectData\projectdata.h"

//////////////////////////////////////////////////////////////////////////
//
//   CNewProjectDlg

CNewProjectDlg::CNewProjectDlg(bool bCreateProject):m_bCreateProject(bCreateProject)
{
    m_strProjectFolder = _T("C:");
}
CNewProjectDlg::~CNewProjectDlg()
{

}
BOOL CNewProjectDlg::OnInitDialog(CWindow wndFocus, LPARAM lInitParam)
{
    this->CenterWindow();

    if(m_bCreateProject)
    {
        m_strProjectFolder = _T("D:\\Test");
        SetDlgItemText(IDC_EDIT_NEW_PROJECT_NAME, _T("Test"));
        SetDlgItemText(IDC_EDIT_NEW_SKIN_NAME, _T("Default"));

        this->SetWindowText(_T("新建工程"));
        GetDlgItem(IDC_EDIT_NEW_SKIN_NAME).SetFocus();
        GetDlgItem(IDC_BUTTON_BROWSE).EnableWindow(TRUE);
    }
    else  // 为这个工程创建一个皮肤
    {
        //
        // 获取工程名，并disable
        //

        SetDlgItemText(IDC_EDIT_NEW_PROJECT_NAME, _T("Test"));

        TCHAR  szDir[MAX_PATH] = _T("");
        m_strProjectFolder = szDir;

        this->SetWindowText(_T("新建皮肤"));
        GetDlgItem(IDC_EDIT_NEW_SKIN_NAME).SetFocus();
        GetDlgItem(IDC_BUTTON_BROWSE).EnableWindow(FALSE);
    }

    GetDlgItem(IDC_EDIT_NEW_PROJECT_NAME).SendMessage(EM_SETREADONLY, m_bCreateProject? FALSE:TRUE, 0L );

    this->SetDlgItemText(IDC_EDIT_BROWSE, m_strProjectFolder.c_str()); 
    String strLogPath = m_strProjectFolder + _T("\\log.xml");
    this->SetDlgItemText(IDC_EDIT_LOG_XML_FILE, strLogPath.c_str());

    return FALSE;
}

void CNewProjectDlg::OnClose()
{
    this->EndDialog(IDCANCEL);
}
void CNewProjectDlg::OnOk( UINT,int,HWND )
{
    CString cstrProjName;
    this->GetDlgItemText(IDC_EDIT_NEW_PROJECT_NAME, cstrProjName);
    if (cstrProjName.IsEmpty())
    {
        MsgBox(_T("工程名称不能为空"));
        return;
    }

    CString  cstrSkinName;
    this->GetDlgItemText(IDC_EDIT_NEW_SKIN_NAME, cstrSkinName);
    if (cstrSkinName.IsEmpty())
    {
        MsgBox(_T("皮肤名称不能为空"));
        return;
    }

    String  strProjName((LPCTSTR)cstrProjName);
    m_strSkinName = ((LPCTSTR)cstrSkinName);

    m_strSkinFolder = m_strProjectFolder;
    m_strSkinFolder.append(_T("\\"));
    m_strSkinFolder.append(m_strSkinName);

    m_strSkinPath = m_strSkinFolder;
    m_strSkinPath.append(_T("\\skin.xml"));

    m_strUIProjPath = m_strProjectFolder;
    m_strUIProjPath.append(_T("\\"));
    m_strUIProjPath.append((LPCTSTR)cstrProjName);
    m_strUIProjPath.append(XML_PROJECT_EXT2);

    // 在工程目录下面创建皮肤目录 
    if (!PathIsDirectory(m_strProjectFolder.c_str()))
    {
        MsgBox(_T("目录不存在"));
        return;
    }

    if (!m_bCreateProject && PathIsDirectory(m_strSkinFolder.c_str()))
    {
        MsgBox(_T("皮肤命名冲突"));
        return;
    }

    if (!PathIsDirectory(m_strSkinFolder.c_str()))
    {
        CreateDirectory(m_strSkinFolder.c_str(), NULL);
    }

    // 将skin.xml拷贝到皮肤下面
    if (!PathFileExists(m_strSkinPath.c_str()))
    {
        SaveTemplateResFileTo(m_strSkinPath.c_str(), IDR_TEMPLATE_SKIN_XML, _T("TEMPLATE"));
    }

    if (!PathFileExists(m_strUIProjPath.c_str()))
    {
        SaveTemplateResFileTo(m_strUIProjPath.c_str(), IDR_TEMPLATE_UIPROJ, _T("TEMPLATE"));
    }

	if (GetProjectData())
		GetProjectData()->m_uiprojParse.SetDirty(true);

    this->EndDialog(IDOK);
}

void CNewProjectDlg::OnCancel( UINT,int,HWND )
{
    this->EndDialog(IDCANCEL);
}

void CNewProjectDlg::OnBrowse( UINT, int, HWND )
{
    CFolderDialog  dlg(NULL, NULL, BIF_RETURNONLYFSDIRS|BIF_USENEWUI);
    if (IDCANCEL == dlg.DoModal( ))
        return;

    m_strProjectFolder = dlg.GetFolderPath();
    ::SetDlgItemText(m_hWnd, IDC_EDIT_BROWSE, m_strProjectFolder.c_str());

    // 同步路径
    this->OnEnChange_NewSkinName(0,0,0);
    return;

}

// 修改了皮肤名称
void CNewProjectDlg::OnEnChange_NewSkinName(UINT, int, HWND )
{
    CString  strSkinName;
    this->GetDlgItemText( IDC_EDIT_NEW_SKIN_NAME, strSkinName );

    String  strBase, strName;
    strBase = m_strProjectFolder;
    strBase.append(_T("\\"));
    strBase.append(strSkinName);
    strBase.append(_T("\\"));

    strName = strBase + _T("UI.xml");
    this->SetDlgItemText(IDC_EDIT_UI_XML_FILE, strName.c_str());

    strName = strBase + _T("image.xml");
    this->SetDlgItemText(IDC_EDIT_IMAGE_XML_FILE, strName.c_str());

    strName = strBase + _T("font.xml");
    this->SetDlgItemText(IDC_EDIT_FONT_XML_FILE, strName.c_str());

    strName = strBase + _T("color.xml");
    this->SetDlgItemText(IDC_EDIT_COLOR_XML_FILE, strName.c_str());

    strName = strBase + _T("layout.xml");
    this->SetDlgItemText(IDC_EDIT_LAYOUT_XML_FILE, strName.c_str());

    strName = strBase + _T("style.xml");
    this->SetDlgItemText(IDC_EDIT_STYLE_XML_FILE, strName.c_str());
}
