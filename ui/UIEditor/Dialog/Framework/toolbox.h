#pragma once
#include "UISDK\Inc\Interface\iwindow.h"
#include "UICTRL\Inc\Interface\itreeview.h"
#include "UISDK\Inc\Interface\iobjectdescription.h"

class CToolBox : public IWindow
{
public:
    CToolBox();
    ~CToolBox();

    UI_BEGIN_MSG_MAP_Ixxx(CToolBox)
        MSG_WM_CLOSE(OnClose)
        UIMSG_INITIALIZE(OnInitialize)
    UI_END_MSG_MAP_CHAIN_PARENT(IWindow)

public:
    void  InsertObject(IObjectDescription* pDesc);

protected:
    void  OnClose();
    void  OnInitialize();

private:
    UI::ITreeView*  m_pTreeView;
};
