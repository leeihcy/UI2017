#pragma once
#include "..\Button\button.h"
#include "Inc\Interface\icheckbutton.h"
#include "..\UISDK\Inc\Interface\iuiinterface.h"

namespace UI
{

class CheckButton : public Button
{
public:
    CheckButton(ICheckButton* p);
    ~CheckButton();

	UI_BEGIN_MSG_MAP()
		UIMSG_SERIALIZE(OnSerialize)
        MSG_WM_GETDLGCODE(OnGetDlgCode)
		UIMSG_GET_TOOLTIPINFO(OnGetToolTipInfo)
        UIMSG_QUERYINTERFACE(CheckButton)
    UI_END_MSG_MAP_CHAIN_PARENT(Button)

	virtual void  virtualOnClicked();
    UINT  OnGetDlgCode(LPMSG lpMsg) { return 0; }
	void  OnSerialize(SERIALIZEDATA* pData);
	BOOL  OnGetToolTipInfo(TOOLTIPITEM* pItem, IToolTipUI* pUI);

private:
	LPCTSTR  GetCheckedToolTipText();
	void  SetCheckedToolTipText(LPCTSTR szText);

protected:
    ICheckButton*  m_pICheckButton;
};


}
