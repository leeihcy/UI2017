#pragma once
#include "..\Button\button.h"
namespace UI
{

class HyperLink : public Button
{
public:
	HyperLink(IHyperLink* p);

    UI_BEGIN_MSG_MAP()
        MSG_WM_GETDLGCODE(OnGetDlgCode)
        UIMSG_QUERYINTERFACE(HyperLink)
        UIMSG_SERIALIZE(OnSerialize)
        UIMSG_FINALCONSTRUCT(FinalConstruct)
	UI_END_MSG_MAP_CHAIN_PARENT(Button)


public:
    LPCTSTR  GetHref();
    void  SetHref(LPCTSTR);

protected:
    HRESULT  FinalConstruct(ISkinRes* p);
    void  OnSerialize(SERIALIZEDATA*);
    UINT  OnGetDlgCode(LPMSG lpMsg) { return 0; }

    virtual void virtualOnClicked();

private:
    IHyperLink*  m_pIHyperLink;
    String  m_strHref;
};


}
