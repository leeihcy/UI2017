#include "stdafx.h"
#include "createwindowdlg.h"
#include "UICTRL\Inc\Interface\ilistbox.h"
#include "UICTRL\Inc\Interface\ibutton.h"
#include "UISDK\Inc\Interface\iskindatasource.h"
#include "3rd\markup\markup.h"
#include "UISDK\Inc\Interface\ixmlwrap.h"
#include "Dialog\Framework\MainFrame.h"
#include "Business\Project\ProjectData\projectdata.h"
CCreateWindowDlg::CCreateWindowDlg() : IWindow(CREATE_IMPL_TRUE)
{
    m_pComboSkinList = NULL;
    m_pComboSkinFileList = NULL;
    m_pComboClassList = NULL;
    m_pEditId = NULL;
}
CCreateWindowDlg::~CCreateWindowDlg()
{
    
}

void  CCreateWindowDlg::OnInitialize()
{
    m_pEditId = (IEdit*)FindObject(_T("edit_id"));
    m_pComboClassList = (IComboBox*)FindObject(_T("combo_class"));
    m_pComboSkinList = (IComboBox*)FindObject(_T("combo_skin"));
    m_pComboSkinFileList = (IComboBox*)FindObject(_T("combo_skinfile"));

    if (m_pComboClassList)
    {
        m_pComboClassList->AddStringEx(_T("Window"));
        m_pComboClassList->AddStringEx(_T("CustomWindow"));
        m_pComboClassList->SetCurSel(0);
    }

    if (m_pComboSkinList)
    {
        UINT nSkinCount = g_pGlobalData->m_pProjectData->m_pEditSkinMgr->GetSkinResCount();
        for (UINT i = 0; i < nSkinCount; i++)
        {
            ISkinRes* pSkin = g_pGlobalData->m_pProjectData->m_pEditSkinMgr->GetSkinResByIndex(i);

            if (pSkin->GetDataSource()->GetType() != SKIN_PACKET_TYPE_DIR)  //MsgBox(_T("当前皮肤不支持编辑"));
                continue;

            IListBoxItem* pItem = m_pComboSkinList->AddStringEx(pSkin->GetName());
            pItem->SetData((LPARAM)pSkin);

            if (i==0 && m_pComboSkinFileList)
            {
                UINT nDocCount = pSkin->GetXmlDocCount();
                for (UINT j = 0; j < nDocCount; j++)
                {
					IUIDocument* pDoc = pSkin->GetXmlDoc(j);
                    if (!pDoc)
                        continue;

                    m_pComboSkinFileList->AddStringEx(pDoc->GetSkinPath())
                        ->SetData((LPARAM)pDoc);
                }
                m_pComboSkinFileList->SetCurSel(0);
            }
        }
        m_pComboSkinList->SetCurSel(0);
    }

    IButton* pBtnOk = (IButton*)FindObject(_T("ok"));
    if (pBtnOk)
    {
        pBtnOk->ClickedEvent().connect(this, &CCreateWindowDlg::OnOk);
    }

}

void  CCreateWindowDlg::OnOk(UI::IButton*)
{
    String  strId;
    String  strXmlFile;

    if (m_pEditId)
    {
        strId = m_pEditId->GetText();
        TrimString(strId);
    }
    if (strId.empty())
        return;

    if (m_pComboSkinFileList)
    {
        strXmlFile = m_pComboSkinFileList->GetText();
        TrimString(strXmlFile);
    }
    if (strXmlFile.empty())
        return;


    // 判断皮肤文件是否存在，不存在则创建一个新窗口
    IListBox* pListBox = m_pComboSkinList->GetListBox();
    IListItemBase*  pItem = pListBox->GetFirstSelectItem();
    if (!pItem)
        return;

    ISkinRes* pSkin = (ISkinRes*)pItem->GetData();
    if (!pSkin)
        return;

    String strText = m_pComboSkinFileList->GetText();
    String strFullPath = pSkin->GetPath();
    strFullPath.append(strText);

    if (!PathFileExists(strFullPath.c_str()))
    {
        if (false == CreateNewWindowXml(strFullPath.c_str()))
            return;
		if (false == InsertNewWindowNode2Xml(strFullPath.c_str(), m_pComboClassList->GetText(), m_pEditId->GetText()))
			return;
		if (false == InsertInclude2SkinXml(pSkin, strText.c_str()))
			return;
    }
	else
	{
		// 从文件名查找uidocument指针
		IUIDocument*  pDoc = pSkin->GetXmlDocByName(strText.c_str());
		if (!pDoc)
			return;

		InsertNewWindowNode2Xml2(pDoc,
                m_pComboClassList->GetText(),
                m_pEditId->GetText());
	}

    // 显示到工程树中去
    g_pGlobalData->m_pProjectTreeDialog->InsertWindowItem(
                pSkin, 
                strText.c_str(), 
                m_pComboClassList->GetText(), 
                m_pEditId->GetText());

    EndDialog(IDOK);
}

bool  CCreateWindowDlg::CreateNewWindowXml(LPCTSTR szPath)
{
    CMarkup doc;

    doc.SetDoc(_T("<?xml version=\"1.0\" encoding=\"utf-8\" ?>"));
    doc.AddElem(XML_SKIN);
    doc.IntoElem();
    doc.AddElem(XML_LAYOUT);
    doc.IntoElem();

    return doc.Save(szPath);
}

bool  CCreateWindowDlg::InsertNewWindowNode2Xml(LPCTSTR szXmlPath, LPCTSTR szNodeName, LPCTSTR szId)
{
    CMarkup doc;
    if (false == doc.Load(szXmlPath))
        return false;

    if (false == doc.NextElem(XML_SKIN))
        return false;
    doc.IntoElem();

    if (false == doc.NextElem(XML_LAYOUT) && false == doc.AddElem(XML_LAYOUT))
        return false;
    doc.IntoElem();

    if (false == doc.AddElem(szNodeName))
        return false;
    if (false == doc.AddAttrib(XML_ID, szId))
        return false;
    doc.AddAttrib(XML_WIDTH, 500);
    doc.AddAttrib(XML_HEIGHT, 500);

    doc.Save(szXmlPath);
    return true;
}
bool  CCreateWindowDlg::InsertNewWindowNode2Xml2(IUIDocument* pDoc, LPCTSTR szNodeName, LPCTSTR szId)
{
	bool  bRet = false;
	do 
	{
		IUIElementProxy pElemSkin = pDoc->FindElem(XML_SKIN);
		if (!pElemSkin)
			break;

		IUIElementProxy pElemLayout = pElemSkin->FindChild(XML_LAYOUT);
		if (!pElemLayout)
			break;

		IUIElementProxy pWindowElem = pElemLayout->AddChild(szNodeName);
		if (!pWindowElem)
			break;

		pWindowElem->AddAttrib(XML_ID, szId);
		pWindowElem->AddAttrib(XML_WIDTH, _T("500"));
		pWindowElem->AddAttrib(XML_HEIGHT, _T("500"));

		pDoc->SetDirty(true);
		bRet = true;
	} while (0);
	
	return bRet;
}

bool  CCreateWindowDlg::InsertInclude2SkinXml(ISkinRes*  pSkin, LPCTSTR szXmlPath)
{
    bool bRet = false;
    do 
    {
        IUIDocument*  pDocument = pSkin->GetXmlDoc(0);
        if (!pDocument)
            break;

        IUIElementProxy pSkinElem = pDocument->FindElem(XML_SKIN);
        if (!pSkinElem)
            break;
        
        IUIElementProxy pIncElem = pSkinElem->AddChild(XML_INCLUDE);
        if (!pIncElem)
            break;

        pIncElem->SetData(szXmlPath);
        pDocument->SetDirty(true);
        g_pGlobalData->m_pMainFrame->SetDirty();

        // 解析include标签
        pfnParseSkinTag pfun = NULL;
        g_pGlobalData->m_pProjectData->m_pEditUIApp->GetSkinTagParseFunc(XML_INCLUDE, &pfun);
        if (pfun)
        {
            pfun(pIncElem.get(), pSkin);
        }
        bRet = true;
    }
    while (0);
    
    return bRet;
}