#pragma once
#include "UISDK\Inc\Interface\iwindow.h"
#include "UICTRL\Inc\Interface\iedit.h"
#include "UICTRL\Inc\Interface\icombobox.h"
namespace UI{ interface IButton; }

class CCreateWindowDlg : public IWindow
{
public:
    CCreateWindowDlg();
    ~CCreateWindowDlg();

    UI_BEGIN_MSG_MAP_Ixxx(CCreateWindowDlg)
        UIMSG_INITIALIZE(OnInitialize)
    UI_END_MSG_MAP_CHAIN_PARENT(IWindow)


protected:
    void  OnInitialize();
    void  OnOk(UI::IButton*);

    bool  CreateNewWindowXml(LPCTSTR szPath);
    bool  InsertNewWindowNode2Xml(LPCTSTR szXmlPath, LPCTSTR szNodeName, LPCTSTR szId);
	bool  InsertNewWindowNode2Xml2(IUIDocument* pDoc, LPCTSTR szNodeName, LPCTSTR szId);
    bool  InsertInclude2SkinXml(ISkinRes*  pSkin, LPCTSTR szXmlPath);

private:
    IComboBox*  m_pComboSkinList;
    IComboBox*  m_pComboSkinFileList;
    IComboBox*  m_pComboClassList;
    IEdit*      m_pEditId;
};