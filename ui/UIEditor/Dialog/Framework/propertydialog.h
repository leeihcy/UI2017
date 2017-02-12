#pragma once
#include "Other\Resource.h"
#include "ipropertyhandler.h"
#include "UISDK\Inc\Interface\iwindow.h"
#include "UISDK\Inc\Interface\ilistitembase.h"

namespace UI
{
    interface IPropertyCtrl;
    interface IEdit;
	interface IRadioButton;
    interface IRichText;
}

class CPropertyDialog : public IWindow
{
public:
	enum {IDD = IDD_DIALOG_PROPERTY};
	CPropertyDialog(void);
	~CPropertyDialog(void);

 	UI_BEGIN_MSG_MAP_Ixxx(CPropertyDialog)
		UIMSG_INITIALIZE(OnInitDialog)
		MSG_WM_CLOSE(OnClose)
		MSG_WM_DESTROY(OnDestroy)

        if (m_pPropertyHandler)
        {
            if (m_pPropertyHandler->ProcessMessage(pMsg, nMsgMapID, false))
                return TRUE;
        }
 	UI_END_MSG_MAP_CHAIN_PARENT(IWindow)


public:
    IListItemBase*  FindPropertyItemByKey(LPCTSTR szKey);
    bool  AttachHandler(IPropertyHandler* p);
	bool  DetachHandler(IPropertyHandler* p);
	IPropertyHandler*  GetHandler();

	IPropertyCtrl*  GetPropertyCtrl(){
		return m_pPropertyCtrl;
	}
	IRichText*  GetDescriptRichEdit(){
		return m_pRichEditDescript;
	}
	IEdit*  GetCommandEdit(){
		return m_pCommandEdit;
	}

private:
    void   SetDecsript(LPCTSTR szKey, LPCTSTR  szDesc);
    void   ShowStatusText(LPCTSTR  szText);

private:
	BOOL  OnInitDialog();
	void  OnClose();
	void  OnDestroy();
    void  OnPropertyCtrlSelChange(UI::IListCtrlBase*);

public:
    IPropertyHandler*  m_pPropertyHandler;
	IPropertyCtrl*  m_pPropertyCtrl;
    IRichText*  m_pRichEditDescript;
    IEdit*  m_pCommandEdit;
};
