#include "StdAfx.h"
#include "Dialogs.h"
#include "Dialog\Framework\MainFrame.h"
#include "Business\Project\ProjectData\projectdata.h"
#include "UISDK\Inc\Interface\ixmlwrap.h"
#include "Business\Command\image\ImageItemCommand.h"
#include "Business\uieditorfunction.h"


//////////////////////////////////////////////////////////////////////////
//
//
CNewColorDlg::CNewColorDlg( ISkinRes* hSkin, bool bModifyMode )
{
	m_pSkin = hSkin;
	m_bModifyMode = bModifyMode;
}
CNewColorDlg::~CNewColorDlg()
{
}

BOOL CNewColorDlg::OnInitDialog(CWindow wndFocus, LPARAM lInitParam)
{
	this->CenterWindow();
	if (m_bModifyMode )
	{
		// 修改模式下，暂时不能修改其ID
		CEdit edit;
		edit.Attach(GetDlgItem(IDC_EDIT_ID));
		edit.SetWindowText(m_strId.c_str());
		edit.SetReadOnly(TRUE);
		edit.Detach();

		::SetDlgItemText( m_hWnd, IDC_EDIT_ID, m_strId.c_str() );
		::SetDlgItemText( m_hWnd, IDC_EDIT_COLOR, m_strColor.c_str() );
	}
	else
	{

	}
	return TRUE;
}

void CNewColorDlg::SetColorInfo( TCHAR* szColorName, TCHAR* szColorValue )
{
	m_strId = szColorName;
	m_strColor = szColorValue;
}

void CNewColorDlg::OnChooseColor( UINT,int,HWND	)
{
	CColorDialog  dlg;
	if (IDOK == dlg.DoModal() )
	{
		TCHAR szText[32] = _T("");
		COLORREF c = dlg.GetColor();

		_stprintf(szText, _T("%d,%d,%d"), GetRValue(c),GetGValue(c),GetBValue(c));
		::SetDlgItemText( m_hWnd, IDC_EDIT_COLOR, szText );
	}
}

void CNewColorDlg::OnOk( UINT,int,HWND	)
{
	if (NULL == m_pSkin)
		return;

	IColorManager& rColorMgr = m_pSkin->GetColorManager();

	TCHAR  szID[MAX_STRING_LEN]    = _T("");
	TCHAR  szColor[MAX_STRING_LEN]  = _T("");

	::GetDlgItemText(m_hWnd, IDC_EDIT_ID, szID, MAX_STRING_LEN);
	::GetDlgItemText(m_hWnd, IDC_EDIT_COLOR, szColor, MAX_STRING_LEN);

	m_strId = szID;
	m_strColor = szColor;

	if (m_strId.empty() || m_strColor.empty())
	{
		return;
	}
	if (m_bModifyMode)
	{
		// TODO: if (FAILED(pColorMgr->ModifyColorItem((BSTR)m_strId.c_str(), (BSTR)m_strColor.c_str())))
		{
			::MessageBox( NULL, _T("修改失败"), _T("Error"), MB_OK|MB_ICONERROR );
			return;
		}
	}
	else
	{
		// TODO: if (FAILED(pColorMgr->InsertColorItem((BSTR)m_strId.c_str(), (BSTR)m_strColor.c_str())))
		{
			::MessageBox( NULL, _T("添加失败"), _T("Error"), MB_OK|MB_ICONERROR );
			return;
		}
	}
	EndDialog(IDOK);

	return;
}
void CNewColorDlg::OnCancel(UINT,int,HWND)
{
	EndDialog(IDCANCEL);
}


//////////////////////////////////////////////////////////////////////////
//
//
CNewFontDlg::CNewFontDlg( ISkinRes* hSkin, bool bModifyMode/*=false*/ )
{
	m_pSkin = hSkin;
	m_nSize = 0;
	m_nOrientation = 0;
	m_bBold = m_bItalic = m_bUnderline = m_bStrikeout = false;
	m_bModifyMode = bModifyMode;
}
CNewFontDlg::~CNewFontDlg()
{
	m_btnBold.Detach();
	m_btnItalic.Detach();
	m_btnUnderline.Detach();
	m_btnStrikeout.Detach();
}

BOOL CNewFontDlg::OnInitDialog(CWindow wndFocus, LPARAM lInitParam)
{
	
	m_btnBold.Attach(GetDlgItem(IDC_CHECK_BOLD));
	m_btnItalic.Attach(GetDlgItem(IDC_CHECK_ITALIC));
	m_btnUnderline.Attach(GetDlgItem(IDC_CHECK_UNDERLINE));
	m_btnStrikeout.Attach(GetDlgItem(IDC_CHECK_STRIKEOUT));

	if(m_bModifyMode)
	{
		// 修改模式下，暂时不能修改其ID
		CEdit edit;
		edit.Attach(GetDlgItem(IDC_EDIT_ID));
		edit.SetWindowText(m_strId.c_str());
		edit.SetReadOnly(TRUE);
		edit.Detach();

		::SetDlgItemText( m_hWnd, IDC_EDIT_FACENAME, m_strFaceName.c_str() );
		::SetDlgItemInt( m_hWnd, IDC_EDIT_FONTSIZE, m_nSize, TRUE );
		::SetDlgItemInt( m_hWnd, IDC_EDIT_FONTORIENTATION, m_nOrientation, TRUE );
		if(m_bBold)m_btnBold.SetCheck(BST_CHECKED);
		if(m_bItalic)m_btnItalic.SetCheck(BST_CHECKED);
		if(m_bUnderline)m_btnUnderline.SetCheck(BST_CHECKED);
		if(m_bStrikeout)m_btnStrikeout.SetCheck(BST_CHECKED);
	}
	else
	{
		::SetDlgItemText( m_hWnd, IDC_EDIT_FACENAME, _T("Arial") );
		::SetDlgItemInt( m_hWnd, IDC_EDIT_FONTSIZE, 9, TRUE );
		::SetDlgItemInt( m_hWnd, IDC_EDIT_FONTORIENTATION, 0, TRUE );
	}
	this->CenterWindow();
	return TRUE;
}

void CNewFontDlg::OnOk(UINT,int,HWND)
{
	if (NULL == m_pSkin)
		return;

	IFontManager& rFontMgr = m_pSkin->GetFontManager();

	TCHAR  szID[MAX_STRING_LEN]    = _T("");
	TCHAR  szFaceName[32]  = _T("");

	::GetDlgItemText( m_hWnd, IDC_EDIT_ID, szID, MAX_STRING_LEN );
	::GetDlgItemText( m_hWnd, IDC_EDIT_FACENAME, szFaceName, 32 );
	m_nSize = ::GetDlgItemInt( m_hWnd, IDC_EDIT_FONTSIZE, NULL, FALSE );
	m_nOrientation = ::GetDlgItemInt( m_hWnd, IDC_EDIT_FONTORIENTATION, NULL, FALSE );

	m_strId = szID;
	m_strFaceName = szFaceName;
	m_bBold = m_btnBold.GetCheck() == BST_CHECKED;
	m_bItalic = m_btnItalic.GetCheck() == BST_CHECKED;
	m_bUnderline = m_btnUnderline.GetCheck() == BST_CHECKED;
	m_bStrikeout = m_btnStrikeout.GetCheck() == BST_CHECKED;

	if (m_strId.empty() || m_strFaceName.empty())
	{
		return;
	}

	::ZeroMemory(&m_lf,sizeof(LOGFONT));
	
	// 将size转换成height
	_tcsncpy( m_lf.lfFaceName, m_strFaceName.c_str(), 31 );
	m_lf.lfFaceName[31] = 0;
	m_lf.lfOrientation = m_nOrientation;
	m_lf.lfHeight = FontSize2Height(m_nSize);
	m_lf.lfWeight = m_bBold?FW_BOLD:FW_NORMAL;
	m_lf.lfItalic = m_bItalic?TRUE:FALSE;
	m_lf.lfUnderline = m_bUnderline?TRUE:FALSE;
	m_lf.lfStrikeOut = m_bStrikeout?TRUE:FALSE;
	

	if (m_bModifyMode)
	{
		// TODO: if (FAILED(pFontMgr->ModifyFontItem(szID, &m_lf)))
		{
			::MessageBox( NULL, _T("修改失败"), _T("Error"), MB_OK|MB_ICONERROR );
			return;
		}
	}
	else
	{
		// TODO: if (FAILED(pFontMgr->InsertFontItem(szID, &m_lf)))
		{
			::MessageBox( NULL, _T("添加失败"), _T("Error"), MB_OK|MB_ICONERROR );
			return;
		}
	}
	EndDialog(IDOK);

	return;
}
void CNewFontDlg::OnCancel( UINT,int,HWND	)
{
	EndDialog(IDCANCEL);
}
//
//	在修改模式下，通过设置该结构体，迅速赋值
//	
void CNewFontDlg::SetFontInfo( const String& strID, LOGFONT* plf)
{
	if (NULL == plf )
		return;

	m_strId = strID;
	m_strFaceName = plf->lfFaceName;
	m_nSize = FontHeight2Size(plf->lfHeight);
	m_nOrientation = plf->lfOrientation;
	m_bBold = plf->lfWeight==FW_BOLD?true:false;
	m_bItalic = plf->lfItalic==1?true:false;
	m_bUnderline = plf->lfUnderline==1?true:false;
	m_bStrikeout = plf->lfStrikeOut==1?true:false;

}

//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////

// 
// void Split(const String& str, TCHAR szSep, vector<String>& vRet)
// {
// 	int nIndex = 0;
// 
// 	while (true)
// 	{
// 		int nResult = (int)str.find(szSep, nIndex);
// 		if (-1 == nResult)
// 		{
// 			vRet.push_back(str.substr(nIndex, str.length()-nIndex));
// 			break;
// 		}
// 		else
// 		{
// 			vRet.push_back(str.substr(nIndex, nResult-nIndex));
// 			nIndex = ++nResult;
// 		}
// 	}
// }