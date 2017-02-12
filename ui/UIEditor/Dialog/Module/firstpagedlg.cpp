#include "stdafx.h"
#include "firstpagedlg.h"
#include "Dialog\Framework\MainFrame.h"
#include "UICTRL\Inc\Interface\ibutton.h"
#include "UICTRL\Inc\Interface\ihyperlink.h"


CFirstPageDlg::CFirstPageDlg() : IWindow(CREATE_IMPL_TRUE)
{
	m_pMainFrame = nullptr;
	m_pList = nullptr;
}
void  CFirstPageDlg::SetMainFrame(CMainFrame* p)
{
    m_pMainFrame = p;
}

void  CFirstPageDlg::OnInitialize()
{
    UI::IButton* pBtnNew = (IButton*)FindObject(_T("btnnew"));
    UI::IButton* pBtnOpen = (IButton*)FindObject(_T("btnopen"));
    if (pBtnNew)
    {
        pBtnNew->ClickedEvent().connect0(
			this, &CFirstPageDlg::OnBtnNewProject);
    }
    if (pBtnOpen)
    {
        pBtnOpen->ClickedEvent().connect0(
			this, &CFirstPageDlg::OnBtnOpenProject);
    }

    m_pList = (UI::IListBox*)FindObject(TEXT("list"));
    if (!m_pList)
        return;

    int nCount = g_pGlobalData->m_pRecentProj->GetItemCount();
    nCount = min(nCount, 5);  // 只显示前5个
    for (int i = 0; i < nCount; i++)
    {
        RecentProjItem* pItem = g_pGlobalData->m_pRecentProj->GetItemByIndex(i);
        if (NULL == pItem)
            continue;

        UI::ICustomListItem* pListItem =
            m_pList->InsertCustomItem(TEXT("firstpage.list"), i);

        pListItem->SetToolTip(pItem->szFilePath);
        pListItem->SetData((LPARAM)pItem);

        UI::IHyperLink* pLink = (UI::IHyperLink*)
            pListItem->FindControl(TEXT("link"));
        if (pLink)
        {
            pLink->SetText(pItem->szFilePath);
            pLink->ClickedEvent().connect(
                this, &CFirstPageDlg::OnListLinkClick);
        }

        UI::IButton* pBtn = (UI::IButton*)
            pListItem->FindControl(TEXT("del"));
        if (pBtn)
        {
            pBtn->ClickedEvent().connect(
                this, &CFirstPageDlg::OnListDelClick);
        }
    }
}

void  CFirstPageDlg::OnListLinkClick(UI::IButton* pLink)
{
    UI::ICustomListItem* pItem = m_pList->GetCustomListItem(pLink);
    if (!pItem)
        return;

    OnClickRecentItem((RecentProjItem*)pItem->GetData());
}

void  CFirstPageDlg::OnListDelClick(UI::IButton* pBtn)
{
    UI::ICustomListItem* pItem = m_pList->GetCustomListItem(pBtn);
    if (!pItem)
        return;

    OnEraseRecentItem((RecentProjItem*)pItem->GetData());
}


void CFirstPageDlg::OnBtnNewProject()
{
    if (NULL == m_pMainFrame )
        return;

    m_pMainFrame->PostMessage(WM_COMMAND, ID_NEW_PROJECT);
}

void CFirstPageDlg::OnBtnOpenProject()
{
    m_pMainFrame->PostMessage(WM_COMMAND, ID_OPEN_PROJECT);
}

void  CFirstPageDlg::OnClickRecentItem(RecentProjItem* pItem)
{
    m_pMainFrame->PostMessage( WM_COMMAND, ID_OPEN_PROJECT, (LPARAM)pItem);
}
void  CFirstPageDlg::OnEraseRecentItem(RecentProjItem* pItem)
{

}
