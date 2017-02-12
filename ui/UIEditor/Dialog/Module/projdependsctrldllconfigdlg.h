#pragma once
#include "UISDK\Inc\Interface\iwindow.h"
#include "UICTRL\Inc\Interface\iedit.h"
#include "UICTRL\Inc\Interface\icombobox.h"
#include "UISDK\Inc\Interface\ilistctrlbase.h"
#include "UICTRL\Inc\Interface\ilistbox.h"

namespace UI
{ 
    interface IButton; 
}

class CProjectDependsCtrlDllConfigDlg : public IWindow
{
public:
    CProjectDependsCtrlDllConfigDlg();
    ~CProjectDependsCtrlDllConfigDlg();

    UI_BEGIN_MSG_MAP_Ixxx(CProjectDependsCtrlDllConfigDlg)
        UIMSG_INITIALIZE(OnInitialize)
    UI_END_MSG_MAP_CHAIN_PARENT(IWindow)


protected:
    void  OnInitialize();
    void  OnBtnAdd(UI::IButton*);
    void  OnBtnDel(UI::IButton*);
    void  OnBtnOk(UI::IButton*);
    void  OnListBoxSelChanged(UI::IListCtrlBase*);
};