#pragma once
#include "..\Button\button.h"
#include "Inc\Interface\iradiobutton.h"


namespace UI
{

class RadioButton : public Button
{
public:
    RadioButton(IRadioButton* p);
    
	UI_BEGIN_MSG_MAP()
		MSG_WM_GETDLGCODE(OnGetDlgCode)
        UIMSG_QUERYINTERFACE(RadioButton)
    UI_END_MSG_MAP_CHAIN_PARENT(Button)

public:
    virtual void  virtualOnClicked();
    UINT  OnGetDlgCode(LPMSG lpMsg) { return 0; }

protected:
    IRadioButton*  m_pIRadioButton;
};

}

