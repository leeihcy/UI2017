#include "StdAfx.h"
#include "CodeWebBrowser.h"
#include "Dialog\Framework\MainFrame.h"

CCodeWebBrowser::CCodeWebBrowser(void)
{

}

CCodeWebBrowser::~CCodeWebBrowser(void)
{
}

BOOL CCodeWebBrowser::OnInitDialog(CWindow wndFocus, LPARAM lInitParam)
{
	m_wndIE = GetDlgItem(IDC_IE); 
	HRESULT hr = m_wndIE.QueryControl (&m_pWebBrowser);
	
	return TRUE;
}

void CCodeWebBrowser::OnSize(UINT nType, CSize size)
{
	if (m_wndIE.m_hWnd != NULL)
	{
		m_wndIE.SetWindowPos(NULL, 0,0, size.cx, size.cy, SWP_NOZORDER);
	}
}

void ** CCodeWebBrowser::QueryInterface(REFIID riid)
{
	if (::IsEqualIID(IID_IExplorerMDIClientView, riid))
	{
		return (void**) static_cast<IExplorerMDIClientView*>(this);
	}
    return __super::QueryInterface(riid);
}

ISkinRes* CCodeWebBrowser::GetSkinRes()
{
	return m_pSkin;
}
void CCodeWebBrowser::SetSkinRes(ISkinRes* hSkin)
{
	m_pSkin = hSkin;
}
HWND CCodeWebBrowser::GetHWND()
{
	return m_hWnd;
}

bool CCodeWebBrowser::GetFilePath(__out TCHAR* szPath)
{
	if (NULL == szPath)
		return false;
	
	_tcscpy(szPath, m_strFilePath.c_str());
	return true;
}

bool CCodeWebBrowser::SetFilePath(__in LPCTSTR szPath)
{
	if (NULL == m_pWebBrowser)
		return false;

	CComVariant v;    // empty variant 
	m_pWebBrowser->Navigate((BSTR)szPath, &v, &v, &v, &v); 

	m_strFilePath = szPath;	
	return true;
}