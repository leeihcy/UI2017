#pragma once
#include "Other\resource.h"
#include "Dialog\Framework\project_tree_dialog.h"
#include "UISDK\Inc\Interface\iuiinterface.h"

#define CHILD_WINDOW_CLASS_FONT    _T("WNDCLASS_FontBuilder")
#define CHILD_WINDOW_CLASS_COLOR   _T("WNDCLASS_ColorBuilder")
#define CHILD_WINDOW_CLASS_STYLE   _T("WNDCLASS_StyleBuilder")
#define CHILD_WINDOW_CLASS_LAYOUT  _T("WNDCLASS_LayoutBuilder")

class CLogViewDlg;
class CFirstPageDlg;
class CImageBuildDlg;
class CWorkAreaPanel;
class CPropertyDialog;
class CToolBar;
class CStatusBar;
class CChildFrame;
class CToolBox;
interface  ILayoutMDIClientView;

//
//	主框架窗口
//
class CMainFrame : public CWindowImpl<CMainFrame>, public IPreTranslateMessage
{
public:
	CMainFrame(void);
	~CMainFrame(void);

	BEGIN_MSG_MAP_EX(CMainFrame)
		MSG_WM_CREATE(OnCreate)
		MSG_WM_CLOSE(OnClose)
		MSG_WM_DESTROY(OnDestroy)
		MSG_WM_SIZE(OnSize)
		MSG_WM_ERASEBKGND(OnEraseBkgnd)
		MSG_WM_INITMENUPOPUP(OnInitMenuPopup)
//		MSG_WM_DROPFILES(OnDropFiles)

        if (uMsg == WM_COMMAND && !TestCommandIsValid(LOWORD(wParam)))
            return 0;

		COMMAND_ID_HANDLER_EX(ID_NEW_PROJECT, OnMenuNewProject)
		COMMAND_ID_HANDLER_EX(ID_OPEN_PROJECT, OnMenuOpenProject)
        COMMAND_ID_HANDLER_EX(ID_CLOSE_PROJECT, OnMenuCloseProject)
		COMMAND_ID_HANDLER_EX(ID_NEW_SKIN, OnMenuNewSkin)
		COMMAND_ID_HANDLER_EX(IDM_SAVE, OnMenuSave)
        COMMAND_ID_HANDLER_EX(IDM_UNDO, OnMenuUndo)
        COMMAND_ID_HANDLER_EX(IDM_REDO, OnMenuRedo)
        COMMAND_ID_HANDLER_EX(ID_NEW_WINDOW, OnNewWindow);
        COMMAND_ID_HANDLER_EX(IDM_UIPROJ_DEPENDSCTRLDDL, OnMenuProjDependsCtrlDll)
        COMMAND_ID_HANDLER_EX(IDM_VIEW_TOOLBOX, OnMenuToolBox)
        COMMAND_ID_HANDLER_EX(IDM_VIEW_PROPERTY, OnMenuViewProperty)

        COMMAND_ID_HANDLER_EX(SC_RESTORE,           OnMenuMDIWindow)   // 菜单栏右侧的三个按钮
        COMMAND_ID_HANDLER_EX(SC_MINIMIZE,          OnMenuMDIWindow)
        COMMAND_ID_HANDLER_EX(SC_CLOSE,             OnMenuMDIWindow)
		COMMAND_ID_HANDLER_EX(ID_WINDOW_CASCADE,    OnMenuMDIWindow)
		COMMAND_ID_HANDLER_EX(ID_WINDOW_TILE_HORZ,  OnMenuMDIWindow)
		COMMAND_ID_HANDLER_EX(ID_WINDOW_ARRANGE,    OnMenuMDIWindow)
		COMMAND_ID_HANDLER_EX(ID_MENU_MDI_CLOSE,    OnMenuMDIWindow)
		COMMAND_ID_HANDLER_EX(ID_MENU_MDI_MINIMIZE, OnMenuMDIWindow)
		COMMAND_ID_HANDLER_EX(ID_MENU_MDI_RESTORE,  OnMenuMDIWindow)
		COMMAND_ID_HANDLER_EX(IDM_MDI_CLOSEALLWINDOW,  OnMenuMDIWindow)
		COMMAND_ID_HANDLER_EX(IDM_MDI_CLOSEALLWINDOW_BUTCUR,  OnMenuMDIWindow)		

		COMMAND_ID_HANDLER_EX(IDM_EXIT, OnMenuExit)
	END_MSG_MAP()

    virtual  BOOL PreTranslateMessage(MSG* pMsg) override;

public:
    void  HandleCmdLine(LPCTSTR szCmdLine);
	void  CalcLayout(int cx, int cy);
    void  CalcLayout();
    void  SetDirty();

	void  SwitchToSkinResPanel(UI_RESOURCE_TYPE nType, ISkinRes* hSkin);
	void  SwitchToExplorerPanel(ExplorerTreeItemData* pData);
	void  SwitchToLayoutPanel(LayoutTreeItemData* pData);
//    void  SwitchToMenuPanel(LayoutTreeItemData* pData);

	CChildFrame*  GetChildFrameByHWND(HWND);
	CChildFrame*  GetLayoutChildFrameByObject(IObject* pObj);
    ILayoutMDIClientView*  GetLayoutClientViewByObject(IObject* pObj);
    HWND  GetLayoutHWND(IObject* pObj);
	HWND  GetImageWnd(ISkinRes* pSkinRes);

	void  RemoveChildFrame(CChildFrame* pChildFrame);
	void  AddChildFrame(CChildFrame* pChildFrame);

    void  ForwardCommnd2CurrentView(UINT nMsg, WPARAM wParam, LPARAM lParam);

    long*  GetCurViewKey() { 
			return m_pCurViewKey; 
	}
    void  MDIActivateInd(long* pKey, bool bActivate);

protected:
    LRESULT   OnCreate(LPCREATESTRUCT lpcs);
    void      OnClose();
    void      OnDestroy();
    void      OnSize(UINT nType, CSize size);
    LRESULT   OnEraseBkgnd(HDC hDC);
    void      OnMenuNewProject(UINT,int,HWND);
    void      OnMenuOpenProject(UINT,int,HWND);
    void      OnMenuCloseProject(UINT,int,HWND);
    void      OnMenuNewSkin(UINT,int,HWND);
    void      OnMenuMDIWindow(UINT,int,HWND);
    void      OnMenuExit(UINT,int,HWND);
    void      OnMenuSave(UINT,int,HWND);
    void      OnMenuUndo(UINT,int,HWND);
    void      OnMenuRedo(UINT,int,HWND);
    void      OnNewWindow(UINT,int,HWND);
    void      OnMenuProjDependsCtrlDll(UINT, int, HWND);
    void      OnMenuToolBox(UINT, int, HWND);
    void      OnMenuViewProperty(UINT, int, HWND);
    bool      TestCommandIsValid(UINT nCmd);

    void      OnInitMenuPopup(HMENU menuPopup, UINT nIndex, BOOL bSysMenu);
    void      OnDropFiles(HDROP hDropInfo);

protected:
	void      _InitMDIClient();
	void      _InitOnOpenOrNewProject(const String& strProjPath);
    void      _CloseProject(bool bExit);
    void      DoSave();

private:
	CProjectTreeDialog*  m_pPanelProjectTree;
	CFirstPageDlg*       m_pDlgFirstPage;
    CToolBar*            m_pToolBar;
    CToolBox*            m_pToolBox;
    CPropertyDialog*     m_pPropertyDialog;
    CStatusBar*          m_pStatusBar;

	HWND    m_hWndMDIClient;
    HACCEL  m_hAccel;

    String               m_strSkinProjPath;  // uiproj路径
    bool                 m_bDirty;
	
	list<CChildFrame*>   m_listChildFrame;
    long*  m_pCurViewKey;   // 当前视图的关键字，如IWindow, IImageRes, ...，用于设置按钮的可用性
};
