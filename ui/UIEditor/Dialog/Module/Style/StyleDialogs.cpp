#include "stdafx.h"
#include "StyleDialogs.h"
#include "Business\Command\style\StyleCommand.h"
#include "Business\uieditorfunction.h"



CNewStyleDlg::CNewStyleDlg(ISkinRes* hSkin, bool bModifyMode/* =false */)
{
	m_pSkin = hSkin;
	m_bModifyMode = bModifyMode;
}
CNewStyleDlg::~CNewStyleDlg()
{
	m_comboType.Detach();
	m_editID.Detach();
	m_editInherit.Detach();
	m_listbox_own.Detach();
	m_listbox_not.Detach();
}


/*static*/ bool MakeInheritString(const STYLE_SELECTOR_TYPE& eStyletype, const String& strStypeName, TCHAR* szInherit )
{
	if (strStypeName.length() <= 0 || NULL == szInherit )
		return false;

	if (eStyletype == STYLE_SELECTOR_TYPE_ID )
	{
		_tcscpy( szInherit, _T(" ") );
		szInherit[0] = STYLE_ID_PREFIX ;
		_tcscat( szInherit, strStypeName.c_str() );
	}
	else if (eStyletype == STYLE_SELECTOR_TYPE_CLASS )
	{
		_tcscpy( szInherit, _T(" ") );
		szInherit[0] = STYLE_CLASS_PREFIX ;
		_tcscat( szInherit, strStypeName.c_str() );
	}
	else
	{
		_tcscpy( szInherit, strStypeName.c_str() );
	}
	return true;
}

BOOL CNewStyleDlg::OnInitDialog(CWindow wndFocus, LPARAM lInitParam)
{
	m_comboType.Attach(GetDlgItem(IDC_COMBO_TYPE));
	m_editID.Attach(GetDlgItem(IDC_EDIT_ID));
	m_editInherit.Attach(GetDlgItem(IDC_EDIT_INHERIT));
	m_listbox_own.Attach(GetDlgItem(IDC_LISTBOX_STYLEITEM_OWN));
	m_listbox_not.Attach(GetDlgItem(IDC_LISTBOX_STYLEITEM_NOT));

	m_comboType.AddString(_T("CLASS"));
	m_comboType.AddString(_T("ID"));
	m_comboType.AddString(_T("TAG"));
	m_comboType.SetItemData(0, (DWORD_PTR)STYLE_SELECTOR_TYPE_CLASS);
	m_comboType.SetItemData(1, (DWORD_PTR)STYLE_SELECTOR_TYPE_ID);
	m_comboType.SetItemData(2, (DWORD_PTR)STYLE_SELECTOR_TYPE_TAG);
	m_comboType.SetCurSel(0);

	// 获取所有的style id进行填充
	IStyleRes& pStyleRes = m_pSkin->GetStyleRes();
	long nCount = pStyleRes.GetStyleCount();

	TCHAR szText[256] = _T("");
	for (long i = 0; i < nCount; i++)
	{
		IStyleResItem*  pInfo = pStyleRes.GetItem(i);
		if(m_bModifyMode && pInfo->GetSelectorType() == m_type && pInfo->GetId() == m_strID )
		{
			// 不添加自己本身
		}
		else
		{
			TCHAR szInherit[MAX_STRING_LEN] = _T("");
			MakeInheritString(pInfo->GetSelectorType(), pInfo->GetId(), szInherit );
			m_mapAllStyle[szInherit] = false;
		}
	}

	if (m_bModifyMode )
	{
		switch(m_type)
		{
		case STYLE_SELECTOR_TYPE_ID:
			m_comboType.SetCurSel(1);
			break;
		case STYLE_SELECTOR_TYPE_CLASS:
			m_comboType.SetCurSel(0);
			break;
		default:
			m_comboType.SetCurSel(2);
			break;
		}

		m_editID.SetWindowText(m_strID.c_str());

		Util::ISplitStringEnum*  pEnum = NULL;
		int nCount = Util::SplitString(m_strInherit.c_str(), XML_SEPARATOR, &pEnum);
		for( int i = 0; i < nCount; i++ )
		{
			LPCTSTR szText = pEnum->GetText(i);
			String  str(szText);

			m_listbox_own.AddString(szText);
			if (m_mapAllStyle.end() != m_mapAllStyle.find(str))
			{
				m_mapAllStyle[str] = true;
			}
		}
		SAFE_RELEASE(pEnum);
	}

	this->FillAllStyleListBox();

	this->CenterWindow();
	return TRUE;
}

void CNewStyleDlg::SetStyleInfo( STYLE_SELECTOR_TYPE type, const String& strID, const String& strInherit )
{
	m_type = type;
	m_strID = strID;
	m_strInherit = strInherit;
}

void CNewStyleDlg::FillAllStyleListBox()
{
	m_listbox_not.SetRedraw(FALSE);
	m_listbox_not.ResetContent();

	map<String, bool>::iterator iter = m_mapAllStyle.begin();
	map<String, bool>::iterator iterEnd = m_mapAllStyle.end();
	for (; iter != iterEnd; iter ++ )
	{
		if (false == iter->second)
		{
			m_listbox_not.AddString(iter->first.c_str());
		}
	}
	m_listbox_not.SetRedraw(TRUE);
}

void CNewStyleDlg::OnOk( UINT,int,HWND )
{
	if (NULL == m_pSkin)
		return;

	IStyleManager& pStyleMgr = m_pSkin->GetStyleManager();

	if (m_editInherit.m_hWnd == GetFocus())
	{
		this->SendMessage( WM_COMMAND, MAKEWPARAM(IDC_BUTTON_ADD_INHERIT,BN_CLICKED), (LPARAM)GetDlgItem(IDC_BUTTON_ADD_INHERIT).m_hWnd);
		return;
	}

	int nCurSel = m_comboType.GetCurSel();
	if (-1 == nCurSel)
		return;

	CString strID;
	m_editID.GetWindowText(strID);
	if (strID.IsEmpty())
		return;

	if (strID[0] == STYLE_CLASS_PREFIX || strID[0] == STYLE_ID_PREFIX)
	{
		::MessageBox(m_hWnd, _T("id无效"), _T("Error"), MB_OK|MB_ICONWARNING);
		m_editID.SetFocus();
		return;
	}

	STYLE_SELECTOR_TYPE type = (STYLE_SELECTOR_TYPE) m_comboType.GetItemData(nCurSel);
	String strInherit = _T("");

	int nCount = m_listbox_own.GetCount();
	for (int i = 0; i < nCount; i++)
	{
		CString str;
		m_listbox_own.GetText(i, str);

		if (! strInherit.empty())
			strInherit.push_back(XML_SEPARATOR);

		strInherit.append(str);
	}

	if (m_bModifyMode)
	{
		UE_ExecuteCommand(
				ModifyStyleItemCommand::CreateInstance(
				m_pSkin, m_type, m_strID.c_str(), type, strID, strInherit.c_str()));
	}
	else
	{
		UE_ExecuteCommand(
				AddStyleItemCommand::CreateInstance(
				m_pSkin, type, strID, strInherit.c_str()));
	}
	EndDialog(IDOK);
}

void CNewStyleDlg::OnCancel( UINT,int,HWND )
{
	EndDialog(IDCANCEL);
}

void CNewStyleDlg::OnListBoxNotDBClick(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	this->SendMessage( WM_COMMAND, MAKEWPARAM(IDC_BUTTON_ADD_INHERIT,BN_CLICKED), (LPARAM)GetDlgItem(IDC_BUTTON_ADD_INHERIT).m_hWnd);
}

void CNewStyleDlg::OnBtnAddInherit(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	TCHAR szText[MAX_STRING_LEN] = _T("");

	int nIndex = m_listbox_not.GetCurSel();
	if (-1 != nIndex )
	{
		m_listbox_not.GetText(nIndex, szText);
		m_listbox_not.DeleteString(nIndex);
	}
	// 检查直接从EDIT添加的情况
	else
	{
		CString str;
		m_editInherit.GetWindowText(str);
		str.Trim();
		if (str.IsEmpty() )
			return;

		m_editInherit.SetWindowText(_T(""));

		_tcsncpy(szText, str.GetString(), MAX_STRING_LEN-1 );
		szText[MAX_STRING_LEN-1] = _T('\0');
	}

	if (_tcslen(szText) != 0 )
	{
		m_listbox_own.AddString(szText);

		map<String, bool>::iterator iter = m_mapAllStyle.begin();
		map<String, bool>::iterator iterEnd = m_mapAllStyle.end();
		for (; iter != iterEnd; iter ++ )
		{
			if (0 == iter->first.compare(szText) )
			{
				iter->second = true;
				break;
			}
		}
		this->FillAllStyleListBox();
	}
}

void CNewStyleDlg::OnBtnDeleteInherit(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	int nIndex = m_listbox_own.GetCurSel();
	if (-1 == nIndex )
		return;

	TCHAR szText[MAX_STRING_LEN] = _T("");
	m_listbox_own.GetText(nIndex, szText);
	m_listbox_own.DeleteString(nIndex);

	map<String, bool>::iterator iter = m_mapAllStyle.begin();
	map<String, bool>::iterator iterEnd = m_mapAllStyle.end();
	for (; iter != iterEnd; iter ++ )
	{
		if (0 == iter->first.compare(szText) )
		{
			iter->second = false;
			break;
		}
	}
	this->FillAllStyleListBox();
}
void CNewStyleDlg::OnBtnClearInherit(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	if (0 == m_listbox_own.GetCount() )
		return;

	m_listbox_own.ResetContent();

	map<String, bool>::iterator iter = m_mapAllStyle.begin();
	map<String, bool>::iterator iterEnd = m_mapAllStyle.end();
	for (; iter != iterEnd; iter ++ )
	{
		iter->second = false;
	}

	this->FillAllStyleListBox();

}
//////////////////////////////////////////////////////////////////////////

CNewStyleAttributeDlg::CNewStyleAttributeDlg( ISkinRes* hSkin, bool bModifyMode )
{
	m_pSkin = hSkin;
	m_bModifyMode = bModifyMode;
}
CNewStyleAttributeDlg::~CNewStyleAttributeDlg()
{
	m_editKey.Detach();
	m_editValue.Detach();
}

BOOL CNewStyleAttributeDlg::OnInitDialog(CWindow wndFocus, LPARAM lInitParam)
{
	m_editKey.Attach(GetDlgItem(IDC_EDIT_KEY).m_hWnd);
	m_editValue.Attach(GetDlgItem(IDC_EDIT_VALUE).m_hWnd);

	if (m_bModifyMode )
	{
		m_editKey.SetReadOnly(TRUE);

		m_editKey.SetWindowText( m_strKey.c_str() );
		m_editValue.SetWindowText( m_strValue.c_str() );
	}
	else
	{
	}

	this->CenterWindow();
	return TRUE;
}

void CNewStyleAttributeDlg::SetStyleInfo(STYLE_SELECTOR_TYPE type, const String strStyleID)
{
	m_eType = type;
	m_strStyleID = strStyleID;
}
void CNewStyleAttributeDlg::SetAttributeInfo( const String& strKey, const String& strValue )
{
	m_strKey = strKey;
	m_strValue = strValue;
}

void CNewStyleAttributeDlg::OnOk( UINT,int,HWND	)
{
	if (NULL == m_pSkin)
		return;

	IStyleManager& pStyleMgr = m_pSkin->GetStyleManager();

	CString strEditKey, strEditValue;

	m_editKey.GetWindowText(strEditKey);
	m_editValue.GetWindowText(strEditValue);

	strEditKey.Trim();
	strEditValue.Trim();

	if (strEditKey.IsEmpty() || strEditValue.IsEmpty() )
		return;

	m_strKey = strEditKey.GetString();
	m_strValue = strEditValue.GetString();

	if (m_bModifyMode)
	{
		// TODO: if (FAILED(pStyleMgr->ModifyStyleAttribute(m_eType, (BSTR)m_strStyleID.c_str(), (BSTR)m_strKey.c_str(), (BSTR)m_strValue.c_str())))
		{
			::MessageBox(m_hWnd, _T("修改失败"), _T("Error"), MB_OK|MB_ICONWARNING );
			return;
		}
	}
	else
	{
		// TODO: if (FAILED(pStyleMgr->InsertStyleAttribute(m_eType, (BSTR)m_strStyleID.c_str(), (BSTR)m_strKey.c_str(), (BSTR)m_strValue.c_str())))
		{
			::MessageBox(m_hWnd, _T("添加失败"), _T("Error"), MB_OK|MB_ICONWARNING );
			return;
		}
	}

	EndDialog(IDOK);
}
void CNewStyleAttributeDlg::OnCancel(UINT,int,HWND)
{
	EndDialog(IDCANCEL);
}