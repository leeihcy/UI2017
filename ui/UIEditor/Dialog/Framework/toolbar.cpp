#include "stdafx.h"
#include "toolbar.h"
#include "UICTRL\Inc\Interface\ibutton.h"
#include "Dialog\Framework\mainframe.h"
#include "Business\Command\Base\commandhistorymgr.h"
#include "UICTRL\Inc\Interface\icombobox.h"

CToolBar::CToolBar() : IWindow(CREATE_IMPL_TRUE)
{
    m_pBtnSave = NULL;
    m_pBtnNewWindow = NULL;
    m_pBtnUndo = NULL;
    m_pBtnRedo = NULL;
    m_pBtnPreview = NULL;
}

CToolBar::~CToolBar()
{

}

void  CToolBar::OnInitialize()
{
    IWindow::nvProcessMessage(GetCurMsg(), 0, false);

    m_pBtnSave = (IButton*)FindObject(_T("save"));
    m_pBtnNewWindow = (IButton*)FindObject(_T("newwindow"));
    m_pBtnUndo = (IButton*)FindObject(_T("undo"));
    m_pBtnRedo = (IButton*)FindObject(_T("redo"));
    m_pBtnPreview = (IButton*)FindObject(_T("preview"));

    if (m_pBtnSave)
    {
        m_pBtnSave->ClickedEvent().connect0(this,
            &CToolBar::OnBtnSave);
    }
    if (m_pBtnNewWindow)
    {
        m_pBtnNewWindow->ClickedEvent().connect0(this,
            &CToolBar::OnBtnNewWindow);
    }
    if (m_pBtnUndo)
    {
        m_pBtnUndo->ClickedEvent().connect0(this,
            &CToolBar::OnBtnUndo);
    }
    if (m_pBtnRedo)
    {
        m_pBtnRedo->ClickedEvent().connect0(this,
            &CToolBar::OnBtnRedo);
    }
    if (m_pBtnPreview)
    {
        m_pBtnPreview->ClickedEvent().connect0(this,
            &CToolBar::OnBtnPreview);
    }

    m_pLangCombo = (UI::IComboBox*)
        FindObject(TEXT("lang"));
    if (m_pLangCombo)
    {
        m_pLangCombo->AddString(TEXT("zh_cn"));
        m_pLangCombo->AddString(TEXT("en_us"));
        m_pLangCombo->SetCurSel(0);

        m_pLangCombo->SelectChangedEvent().connect0(
            this, &CToolBar::update_language);
    }
}

void  CToolBar::OnEraseBkgnd(IRenderTarget* pRenderTarget)
{
    CRect  rc(0, 0, GetWidth(), GetHeight());

    UI::Color cBack(241,237,237,255);
    UI::Color cTopLine(145,145,145,255);
    UI::Color cBorder(0xFFFFFFFF);

    rc.bottom--;
    pRenderTarget->DrawRect(&rc, &cBack);

    //rc.bottom++;
    IRenderPen* pPen = pRenderTarget->CreateSolidPen(1, &cTopLine);
    pRenderTarget->DrawLine(rc.left, rc.bottom, rc.right, rc.bottom, pPen);
    SAFE_RELEASE(pPen);
}

long*  CToolBar::GetCurViewKey()
{
    return g_pGlobalData->m_pMainFrame->GetCurViewKey();
}

void  CToolBar::OnDirtyChanged(bool bDirty)
{
    if (!m_pBtnSave)
        return;

    m_pBtnSave->SetEnable(bDirty);
}
void  CToolBar::OnProjectOpened()
{
    if (m_pBtnNewWindow)
        m_pBtnNewWindow->SetEnable(true);

    update_language();
}

void  CToolBar::update_language()
{
    if (!m_pLangCombo)
        return;

    IUIApplication* pUIApp = GetEditUIApplication();
    if (!pUIApp)
        return;

    pUIApp->GetSkinMgr().SetCurrentLanguage(m_pLangCombo->GetText());
}

void  CToolBar::OnUndoRedoCountChanged(long* pKey)
{
	if (pKey == GetCurViewKey())
	{
		UpdateBtnState();
	}
}

void  CToolBar::UpdateBtnState()
{
	int  nUndo = 0;
	int  nRedo = 0;
	if (GetCurViewKey())
	{
        m_pBtnPreview->SetEnable(true);

        g_pGlobalData->m_pCmdHistroyMgr->GetUndoRedoCount(GetCurViewKey(), &nUndo, &nRedo);
	}
    else
    {
        m_pBtnPreview->SetEnable(false);
    }

	if (m_pBtnUndo)
		m_pBtnUndo->SetEnable(nUndo > 0?true:false);
	if (m_pBtnRedo)
		m_pBtnRedo->SetEnable(nRedo > 0?true:false);
}

void  CToolBar::OnProjectClosed()
{
    if (m_pBtnNewWindow)
        m_pBtnNewWindow->SetEnable(false);
}

void  CToolBar::OnBtnSave()
{
    ::SendMessage(g_pGlobalData->m_pMainFrame->m_hWnd, WM_COMMAND,
        IDM_SAVE, (LPARAM)GetHWND());
}
void  CToolBar::OnBtnNewWindow()
{
    ::SendMessage(g_pGlobalData->m_pMainFrame->m_hWnd, WM_COMMAND,
        ID_NEW_WINDOW, (LPARAM)GetHWND());
}
void  CToolBar::OnBtnUndo()
{
    ::SendMessage(g_pGlobalData->m_pMainFrame->m_hWnd, WM_COMMAND,
        IDM_UNDO, (LPARAM)GetHWND());
}

void  CToolBar::OnBtnRedo()
{
    ::SendMessage(g_pGlobalData->m_pMainFrame->m_hWnd, WM_COMMAND,
        IDM_REDO, (LPARAM)GetHWND());
}

void  CToolBar::OnBtnPreview()
{
    g_pGlobalData->m_pMainFrame->ForwardCommnd2CurrentView(MDIVIEW_MESSAGE_PREVIEW, 0, 0);
}

// 使用快捷键触发一个命令时，需要检查当前是否可以执行该命令
bool  CToolBar::TestCommandIsValid(long ld)
{   
    switch (ld)
    {
    case IDM_SAVE:
        {
            if (!m_pBtnSave || !m_pBtnSave->IsEnable())
                return false;
        }
        break;

    case IDM_REDO:
        {
            if (!m_pBtnRedo || !m_pBtnRedo->IsEnable())
                return false;
        }
        break;

    case IDM_UNDO:
        {
            if (!m_pBtnUndo || !m_pBtnUndo->IsEnable())
                return false;
        }
        break;
    }

    return true;
}
