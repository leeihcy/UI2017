#pragma once
#include "Other\resource.h"
#include "UISDK\Inc\Interface\iwindow.h"
#include "UICTRL\Inc\Interface\ilistbox.h"

namespace UI
{
    interface IHyperLink;
    interface IButton;
    interface IListBox;
}

#define MAX_RECENT_COUNT  5  // 只显示5个最近的工程

//
//	首页，创建/打开工程
//
class CFirstPageDlg : public  IWindow
{
public:
    CFirstPageDlg();

    UI_BEGIN_MSG_MAP_Ixxx(CFirstPageDlg)
        UIMSG_INITIALIZE(OnInitialize)
    UI_END_MSG_MAP_CHAIN_PARENT(IWindow)

public:
    void  SetMainFrame(CMainFrame* p);
    void  OnInitialize();
    LRESULT  OnBtnClick(WPARAM w, LPARAM l);
    void  OnBtnNewProject();
    void  OnBtnOpenProject();

    void  OnClickRecentItem(RecentProjItem* pItem);
    void  OnEraseRecentItem(RecentProjItem* pItem);
    void  OnListLinkClick(UI::IButton*);
    void  OnListDelClick(UI::IButton*);

private:
    CMainFrame*   m_pMainFrame;
    UI::IListBox*  m_pList;
};