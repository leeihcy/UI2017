#pragma once
#include "Other\resource.h"

//
//	新建工程/皮肤对话框
//
class CNewProjectDlg : public CDialogImpl<CNewProjectDlg>
{
public:
    enum {IDD = IDD_NEW_PROJECT };
    CNewProjectDlg(bool bCreateProject);
    ~CNewProjectDlg();

    BEGIN_MSG_MAP_EX(CNewProjectDlg)
        MSG_WM_INITDIALOG( OnInitDialog )
        MSG_WM_CLOSE( OnClose )
        COMMAND_ID_HANDLER_EX( IDOK, OnOk )
        COMMAND_ID_HANDLER_EX( IDCANCEL, OnCancel )
        COMMAND_ID_HANDLER_EX( IDC_BUTTON_BROWSE, OnBrowse )
        COMMAND_HANDLER_EX( IDC_EDIT_NEW_SKIN_NAME, EN_CHANGE, OnEnChange_NewSkinName )
	END_MSG_MAP()

public:
    void   OnClose();
    void   OnOk( UINT,int,HWND	);
    void   OnCancel( UINT,int,HWND	);
    BOOL   OnInitDialog(CWindow wndFocus, LPARAM lInitParam);

    void   OnBrowse( UINT, int, HWND );
    void   OnEnChange_NewSkinName(UINT, int, HWND );

public:
    String  m_strProjectFolder;    // 工程所在目录 
    String  m_strSkinFolder;       // 皮肤文件夹路径
    String  m_strSkinPath;         // 皮肤路径
    String  m_strSkinName;         // 皮肤名称
    String  m_strUIProjPath;
    bool    m_bCreateProject;      // 当前是创建工程还是仅添加一个皮肤
};