#include "stdafx.h"
#include "add_image_dlg.h"
#include "Business\Command\image\ImageItemCommand.h"
#include "Business\uieditorfunction.h"
#include "UISDK\Inc\Interface\iskindatasource.h"

CAddImageDlg::CAddImageDlg(ISkinRes* hSkin, bool bModifyMode)
{
    m_pSkin = hSkin;
    m_bModifyMode = bModifyMode;
    m_bFirstTime = true;
    m_pImageResItem = NULL;
}
CAddImageDlg::~CAddImageDlg()
{
    m_editID.Detach();
    m_editPath.Detach();
}

void CAddImageDlg::OnPaint(HDC)
{
    if (m_bFirstTime)
    {
        m_bFirstTime = false;
        if (!m_bModifyMode)
            this->PostMessage(WM_COMMAND, MAKEWPARAM(IDC_BUTTON_BROWSE,BN_CLICKED), (LPARAM)(HWND)GetDlgItem(IDC_BUTTON_BROWSE) );
    }
    SetMsgHandled(FALSE);
}
BOOL CAddImageDlg::OnInitDialog(CWindow wndFocus, LPARAM lInitParam)
{
    m_editID.Attach( GetDlgItem(IDC_EDIT_ID) );
    m_editID.SetLimitText(MAX_STRING_LEN);

    m_editPath.Attach( GetDlgItem(IDC_EDIT_PATH) );
    m_editPath.SetLimitText(MAX_PATH);

    m_comboImageListLayout.Attach(GetDlgItem(IDC_COMBO_IMAGELIST_LAYOUT));
    m_editImageListItemCount.Attach(GetDlgItem(IDC_EDIT_IMAGELIST_COUNT));
    m_editImageListItemCount.SetLimitText(3);

    m_comboImageListLayout.AddString(_T("H"));
    m_comboImageListLayout.AddString(_T("V"));
    m_editImageListItemCount.SetWindowText(_T("1"));

    if (m_bModifyMode)
    {
        m_editID.SetWindowText(m_pImageResItem->GetId());
        m_editPath.SetWindowText(m_pImageResItem->GetPath());
        if (m_pImageResItem->GetUseSkinHLS())
        {
            CheckDlgButton(IDC_CHECK_USEHLS, BST_CHECKED);
        }
        if (m_pImageResItem->GetNeedAntiAliasing())
        {
            CheckDlgButton(IDC_CHECK_ANTI, BST_CHECKED);
        }
        m_editID.SetReadOnly(TRUE);

        GetDlgItem(IDC_CHECK_IMAGELIST).EnableWindow(FALSE);

        IMAGE_ITEM_TYPE eType = m_pImageResItem->GetImageItemType();
        if (eType != IMAGE_ITEM_TYPE_IMAGE_LIST)
        {
            m_comboImageListLayout.EnableWindow(FALSE);
            m_editImageListItemCount.EnableWindow(FALSE);
        }
        else
        {
            ::CheckDlgButton(m_hWnd, IDC_CHECK_IMAGELIST, BST_CHECKED);

            IImageListResItem* pList = (IImageListResItem*)m_pImageResItem;
            CString str;
            str.Format(L"%d", pList->GetItemCount());
            m_editImageListItemCount.SetWindowText(str);

            if (pList->GetLayoutType() == IMAGELIST_LAYOUT_TYPE_V)
                m_comboImageListLayout.SetCurSel(1);
        }
    }

    this->CenterWindow();
    return TRUE;
}
void CAddImageDlg::SetImageInfo(IImageResItem* pItem)
{
    m_pImageResItem = pItem;
}

void CAddImageDlg::OnCancel( UINT,int,HWND	)
{
    EndDialog(IDCANCEL);
}
void CAddImageDlg::OnOk( UINT,int,HWND	)
{
    TCHAR  szID[MAX_STRING_LEN]    = _T("");
    TCHAR  szPath[MAX_PATH]  = _T("");

    ::GetDlgItemText( m_hWnd, IDC_EDIT_ID, szID, MAX_STRING_LEN );
    ::GetDlgItemText( m_hWnd, IDC_EDIT_PATH, szPath, MAX_PATH );

    if ( _tcslen(szID) == 0 || _tcslen(szPath) == 0 )
    {
        return;
    }
    // 	if (! ::PathFileExists( szPath) )
    // 	{
    // 		::MessageBox( NULL, _T("文件不存在"), _T("Error"), MB_OK|MB_ICONERROR );
    // 		return;
    // 	}

    m_strId = szID;
    m_strPath = szPath;

    // 	if (BST_CHECKED == ::IsDlgButtonChecked(m_hWnd, IDC_CHECK_COPYFILE))
    // 	{
    // 		String::size_type  pos = m_strPath.rfind( _T('\\') );
    // 		String    strFileName  = m_strPath.substr( pos, m_strPath.length()-pos );
    // 		String    strNewPath = m_strDataSourceFolder;
    // 		strNewPath.append(strFileName);
    // 
    // 		if (FALSE == ::CopyFile( szPath, strNewPath.c_str(), TRUE))
    // 		{
    // 			::MessageBox( NULL, _T("拷贝文件失败"), _T("Error"), MB_OK|MB_ICONERROR );
    // 			return;
    // 		}
    // 
    // 		m_strPath = strNewPath;
    // 	}

    // 绝对跨径转换为相对路径
    if (IsFullPath(m_strPath.c_str()))
    {
        TCHAR szRelativePath[MAX_PATH] = _T("");
        CalcRelativePathToDir(m_pSkin->GetDataSource()->GetPath(), m_strPath.c_str(), szRelativePath);
        m_strPath = szRelativePath;
    }

    IImageManager& pImageMgr = m_pSkin->GetImageManager();

#if 1
    ImageItemInfo itemInfo;
    itemInfo.strId = m_strId;
    itemInfo.strPath = m_strPath;
    itemInfo.bAntialiasing = IsDlgButtonChecked(IDC_CHECK_ANTI) ? true:false;
    itemInfo.bHls = IsDlgButtonChecked(IDC_CHECK_USEHLS) ? true:false;
    itemInfo.bImageList = IsDlgButtonChecked(IDC_CHECK_IMAGELIST) ? true:false;
    if (itemInfo.bImageList)
    {
        itemInfo.bLayoutH = m_comboImageListLayout.GetCurSel() == 0 ?  true:false;

        TCHAR szText[16] = {0};
        m_editImageListItemCount.GetWindowText(szText, 16);
        itemInfo.nImageListItemCount = _ttoi(szText);
    }

    if (m_bModifyMode)
    {
        UE_ExecuteCommand(ModifyImageItemCommand::CreateInstance(
            m_pSkin, 
            m_pImageResItem, 
            &itemInfo));
    }
    else
    {
        UE_ExecuteCommand(AddImageItemCommand::CreateInstance(m_pSkin, &itemInfo));
    }
#else
    IUIElement* pElem = NULL;
    if (m_bModifyMode)
    {
        pElem = g_pGlobalData->m_pProjectData->m_uieditor.GetImageResItemEditorData(m_pImageResItem)->pXmlElement;
        if (!pElem || FAILED(pImageMgr->ModifyImageItem(m_strId.c_str(), m_strPath.c_str())))
        {
            ::MessageBox( NULL, _T("修改失败"), _T("Error"), MB_OK|MB_ICONERROR );
            return;
        }

        if (pElem->HasAttrib(XML_PATH))
        {
            pElem->ModifyAttrib(XML_PATH, m_strPath.c_str());
        }
        else
        {
            pElem->SetData(m_strPath.c_str());
        }
    }
    else
    {
        m_pImageResItem = pImageMgr->InsertImageItem(m_strId.c_str(), m_strPath.c_str());
        if (!m_pImageResItem)
        {
            ::MessageBox( NULL, _T("添加失败"), _T("Error"), MB_OK|MB_ICONERROR );
            return;
        }
        pElem = g_pGlobalData->m_pProjectData->m_uieditor.GetImageResItemEditorData(m_pImageResItem)->pXmlElement;
    }

    if (IsDlgButtonChecked(IDC_CHECK_ANTI))
    {
        m_pImageResItem->SetNeedAntiAliasing(true);
        if (pElem)
            pElem->AddAttrib(XML_IMAGE_NEED_ANTIALIASING, _T("1"));
    }
    else
    {
        m_pImageResItem->SetNeedAntiAliasing(false);
        if (pElem)
            pElem->RemoveAttrib(XML_IMAGE_NEED_ANTIALIASING);
    }


    if (IsDlgButtonChecked(IDC_CHECK_USEHLS))
    {
        m_pImageResItem->SetUseSkinHLS(true);
        if (pElem)
            pElem->AddAttrib(XML_IMAGE_USESKINHLS, _T("1"));
    }
    else
    {
        m_pImageResItem->SetUseSkinHLS(false);
        if (pElem)
            pElem->RemoveAttrib(XML_IMAGE_USESKINHLS);
    }

    if (pElem)
    {
        IUIDocument* pDoc = NULL;
        pElem->GetDocument(&pDoc);
        pDoc->SetDirty(true);
        SAFE_RELEASE(pDoc);
        g_pGlobalData->m_pMainFrame->SetDirty();
    }
#endif
    EndDialog(IDOK);
}

void CAddImageDlg::OnBrowse( UINT, int, HWND )
{
    CFileDialog  dlg(TRUE, NULL, NULL, 4|2,  
        _T("Support Image Format(*.png,*.bmp,*.jpg,*.ico)\0*.png;*.bmp;*.jpg;*.ico\0")
        _T("Icon (*.ico)\0*.ico\0Bitmap (*.bmp)\0*.bmp\0jpeg (*.jpg)\0*.jpg\0png (*.png)\0*.png\0\0")
        , m_hWnd);

    if (IDCANCEL == dlg.DoModal() )
        return;

    ::SetDlgItemText( m_hWnd, IDC_EDIT_PATH, dlg.m_szFileName );

    // 默认ID值
    // 	TCHAR szText[MAX_STRING_LEN];
    // 	m_editID.GetWindowText(szText, MAX_STRING_LEN);
    // 	if (0 == _tcslen(szText) )
    {
        if (!m_bModifyMode)   // 修改模式下，禁止同步ID 
        {
            TCHAR szId[_MAX_FNAME] = { 0 };
            _tcscpy(szId, dlg.m_szFileTitle);
            TCHAR* p = _tcsrchr(szId, TEXT('.'));
            if (p)
                *p = 0;
            m_editID.SetWindowText(szId);
            m_editID.SetSel(0, -1);
        }
    }
}
