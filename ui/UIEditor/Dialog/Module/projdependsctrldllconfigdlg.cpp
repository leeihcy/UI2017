#include "stdafx.h"
#include "projdependsctrldllconfigdlg.h"
#include "UICTRL\Inc\Interface\ibutton.h"
#include "Business\Project\DependsCtrlDll\dependsctrldll.h"
#include "Dialog\Framework\statusbar.h"

CProjectDependsCtrlDllConfigDlg::CProjectDependsCtrlDllConfigDlg() :
    IWindow(CREATE_IMPL_TRUE)
{
}
CProjectDependsCtrlDllConfigDlg::~CProjectDependsCtrlDllConfigDlg()
{

}

void CProjectDependsCtrlDllConfigDlg::OnInitialize()
{
    
    IButton*  pBtnAdd = (IButton*)FindObject(_T("btn_add"));
    IButton*  pBtnDel = (IButton*)FindObject(_T("btn_del"));
    IButton*  pBtnOk = (IButton*)FindObject(_T("btn_ok"));

    if (pBtnDel)
    {
        pBtnDel->ClickedEvent().connect(
            this, &CProjectDependsCtrlDllConfigDlg::OnBtnDel);
        pBtnDel->SetEnable(false);
    }
    if (pBtnAdd)
    {
        pBtnAdd->ClickedEvent().connect(
            this, &CProjectDependsCtrlDllConfigDlg::OnBtnAdd);
    }
    if (pBtnOk)
    {
        pBtnOk->ClickedEvent().connect(
            this, &CProjectDependsCtrlDllConfigDlg::OnBtnOk);
    }

    IListBox* pListBox = (IListBox*)FindObject(_T("listbox"));
    if (pListBox)
    {
        pListBox->SelectChangedEvent().connect(
            this, &CProjectDependsCtrlDllConfigDlg::OnListBoxSelChanged);

        CDependsCtrlDllMgr* pMgr = GetProjDependsCtrlDllMgr();
        if (!pMgr)
            return;
        for (UINT i = 0; i < pMgr->m_vStrDlls.size(); i++)
        {
            pListBox->AddString(pMgr->m_vStrDlls[i]->strAbsolutePath.c_str());
        }
    }
}   

void  CProjectDependsCtrlDllConfigDlg::OnBtnAdd(UI::IButton*)
{
    CFileDialog dlg(TRUE, _T(".dll"), NULL, 0, _T("*.dll\0*.dll\0\0"), GetHWND());
    if (IDCANCEL == dlg.DoModal())
        return ;

    HMODULE hModule = ::LoadLibrary(dlg.m_szFileName);
    FARPROC pfun = ::GetProcAddress(hModule, "RegisterUIObject");
    FreeLibrary(hModule);
    if (!pfun)
    {
        TCHAR szText[320] = _T("");
        _stprintf(szText, _T("在%s中未找到RegisterUIObject导出函数"), dlg.m_szFileName);
        g_pGlobalData->m_pStatusBar->SetStatusText2(szText);
        return;
    }

    if (!GetProjDependsCtrlDllMgr()->AddCtrlDll(dlg.m_szFileName))
        return;

    IListBox* pListBox = (IListBox*)FindObject(_T("listbox"));
    if (pListBox)
    {
        pListBox->AddString(dlg.m_szFileName);
    }
}
void  CProjectDependsCtrlDllConfigDlg::OnBtnDel(UI::IButton*)
{
    IListBox* pListBox = (IListBox*)FindObject(_T("listbox"));
    if (!pListBox)
        return;

    IListItemBase* pSel = pListBox->GetFirstSelectItem();
    if (!pSel)
        return;

    if (GetProjDependsCtrlDllMgr()->DelCtrlDll(pSel->GetText()))
    {
        pListBox->RemoveItem(pSel);
    }
}

void  CProjectDependsCtrlDllConfigDlg::OnListBoxSelChanged(
        UI::IListCtrlBase* pListCtrl)
{
    IListItemBase* pNew = pListCtrl->GetFirstSelectItem();

    IButton*  pBtnDel = (IButton*)FindObject(_T("btn_del"));
    if (pBtnDel)
        pBtnDel->SetEnable(pNew ? true : false);
}

void  CProjectDependsCtrlDllConfigDlg::OnBtnOk(UI::IButton*)
{
    EndDialog(IDOK);
}