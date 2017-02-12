#include "stdafx.h"
#include "ColorBuildDlg.h"
#include "Dialogs.h"
#include "Dialog/Framework/mainframe.h"

CColorViewDlg::CColorViewDlg()
{
	
}

CColorViewDlg::~CColorViewDlg()
{

}
void CColorViewDlg::OnDestroy()
{
	this->Clear();
}

void CColorViewDlg::Clear()
{
	int nCount = m_listctrl.GetItemCount();
	for (int i = 0; i < nCount; i++ )
	{
		Color* pcol = (Color*)m_listctrl.GetItemData(i);
		if (pcol)
			pcol->Release();
	}
}
BOOL CColorViewDlg::OnInitDialog( HWND, LPARAM )
{
	m_listctrl.Attach( GetDlgItem(IDC_COLORLIST) );
	m_listctrl.InsertColumn( 0, _T("id"), LVCFMT_LEFT, 120 );
	m_listctrl.InsertColumn( 1, _T("color"), LVCFMT_LEFT, 120 );
	m_listctrl.InsertColumn( 2, _T("display"), LVCFMT_LEFT, 400 );

	m_listctrl.SetExtendedListViewStyle( LVS_EX_FULLROWSELECT );
	m_listctrl.ModifyStyle( 0, LVS_SINGLESEL|LVS_SHOWSELALWAYS );

	return TRUE;
}

void  CColorViewDlg::OnSize(UINT nType, CSize size)
{
	const int BTN_HEIGHT = 22;
	const int BTN_WIDTH  = 50;

	int x = WINDOW_PADDING;/*(size.cx - 2*100 - 1*20 )/2*/
	GetDlgItem(IDC_BUTTON_ADD).SetWindowPos    ( NULL, x,                                  WINDOW_PADDING, BTN_WIDTH, BTN_HEIGHT, SWP_NOZORDER );
	GetDlgItem(IDC_BUTTON_DELETE).SetWindowPos ( NULL, x + BTN_WIDTH   + WINDOW_PADDING,   WINDOW_PADDING, BTN_WIDTH, BTN_HEIGHT, SWP_NOZORDER );

	m_listctrl.SetWindowPos( 0, WINDOW_PADDING, 2*WINDOW_PADDING+BTN_HEIGHT, 
		size.cx-2*WINDOW_PADDING,size.cy-BTN_HEIGHT-3*WINDOW_PADDING,SWP_NOZORDER );
}
//
//	设置数据源
//
void CColorViewDlg::AttachSkin(ISkinRes* h)
{
	if( m_pSkin == h )
		return;

	// changed notify
	m_pSkin = h;

	this->Reload();
}

void CColorViewDlg::Reload()
{
	if (NULL == m_pSkin)
		return;

	IColorRes& rColorRes = m_pSkin->GetColorRes();

	// 获取该皮肤中的所有图片
	m_listctrl.DeleteAllItems();

    long nCount = rColorRes.GetColorCount();
	for (long i = 0; i < nCount; i++)
	{
        IColorResItem*  pInfo = rColorRes.GetColorResItem(i);
		if (pInfo)
		{
			int nItemCount = m_listctrl.GetItemCount();
			m_listctrl.InsertItem(nItemCount, pInfo->GetId());
			m_listctrl.SetItemText(nItemCount, 1, pInfo->GetColorString());

			// 保存数据用于自绘
			Color* pcol = NULL;

            rColorRes.GetColor((BSTR)pInfo->GetId(), &pcol);
            m_listctrl.SetItemData(nItemCount, (DWORD_PTR)pcol);
		}
	}

	// 将新增的显示出来
	m_listctrl.SelectItem(0);
}

void  CColorViewDlg::OnBtnAdd(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	if( NULL == m_pSkin )
		return;

	CNewColorDlg  dlg(m_pSkin);
	if( IDOK == dlg.DoModal() )
	{
		int nCount = m_listctrl.GetItemCount();
		m_listctrl.InsertItem( nCount, dlg.m_strId.c_str() );
		m_listctrl.SetItemText( nCount, 1, dlg.m_strColor.c_str() );

		// 保存数据用于自绘
		IColorRes& rColorRes = m_pSkin->GetColorRes();
		Color* pcol = NULL;
        rColorRes.GetColor((BSTR)dlg.m_strId.c_str(), &pcol);
        m_listctrl.SetItemData( nCount, (DWORD_PTR)pcol );

		// 将新增的显示出来
		m_listctrl.SelectItem(nCount);

		g_pGlobalData->m_pMainFrame->SetDirty();
	}
}
void  CColorViewDlg::OnBtnRemove(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	if (NULL == m_pSkin)
		return ;

	IColorManager& rColorMgr = m_pSkin->GetColorManager();

	// 1. 删除资源 
	int nIndex = this->m_listctrl.GetSelectedIndex();
	if( -1 == nIndex )
		return;

	TCHAR szText[MAX_STRING_LEN] = _T("");
	m_listctrl.GetItemText(nIndex, 0, szText, MAX_STRING_LEN );

	// TODO: if (FAILED(pColorMgr->RemoveColorItem(szText)))
	{
		::MessageBox( NULL, _T("删除失败"), _T("Error"), MB_OK|MB_ICONERROR	);
		return;
	}

	this->m_listctrl.DeleteItem(nIndex);

	// 更新界面
	int nCount = m_listctrl.GetItemCount();
	if( nIndex >= nCount-1 )
	{
		nIndex = nCount-1;
	}
	this->m_listctrl.SelectItem(nIndex);	

	g_pGlobalData->m_pMainFrame->SetDirty();
}

// 列表框自绘
LRESULT CColorViewDlg::OnCustomDraw(LPNMHDR pnmh)
{
	LPNMLVCUSTOMDRAW p = (LPNMLVCUSTOMDRAW)pnmh;
	//if( p->iSubItem == 2 ) // 只绘制第二列

	switch( p->nmcd.dwDrawStage )
	{
	case CDDS_PREPAINT:
		return CDRF_NOTIFYITEMDRAW;
	case CDDS_ITEMPREPAINT:
		return CDRF_NOTIFYSUBITEMDRAW;
	case CDDS_ITEMPREPAINT|CDDS_SUBITEM:
		{
			if (p->iSubItem!=2)
				return CDRF_DODEFAULT;
			
			RECT rc ;
			m_listctrl.GetItemRect(p->nmcd.dwItemSpec, &rc, LVIR_BOUNDS );
			rc.left = p->nmcd.rc.left;
			rc.right = p->nmcd.rc.right;

			Color* pCol  = (Color*)this->m_listctrl.GetItemData(p->nmcd.dwItemSpec);
			if( NULL != pCol )
			{
				HBRUSH hBrush = ::CreateSolidBrush(pCol->GetGDIValue());
				::FillRect(p->nmcd.hdc, &rc, hBrush);
				::DeleteObject(hBrush);
			}
			return CDRF_SKIPDEFAULT;
		}
	}
	return CDRF_DODEFAULT;
}

LRESULT CColorViewDlg::OnItemDBClick(LPNMHDR pnmh)
{
	LRESULT lret = this->DefWindowProc();

	LPNMITEMACTIVATE p = (LPNMITEMACTIVATE )pnmh;
	if( -1 == p->iItem )
		return lret;

	TCHAR szColorName[MAX_STRING_LEN] = _T("");
	TCHAR szColorValue[MAX_PATH] = _T("");
	m_listctrl.GetItemText( p->iItem, 0, szColorName, MAX_STRING_LEN );
	m_listctrl.GetItemText( p->iItem, 1, szColorValue, MAX_PATH );

	CNewColorDlg dlg(m_pSkin,true);
	dlg.SetColorInfo(szColorName,szColorValue);
	if( IDOK == dlg.DoModal() )
	{
		m_listctrl.SetItemText( p->iItem, 1, dlg.m_strColor.c_str() );
		
		RECT rcRect;
		m_listctrl.GetItemRect(m_listctrl.GetSelectedIndex(), &rcRect, LVIR_BOUNDS );
		m_listctrl.InvalidateRect(&rcRect);

		g_pGlobalData->m_pMainFrame->SetDirty();
	}

	return 0;
}

long*  CColorViewDlg::GetKey()
{
	return (long*)&m_pSkin->GetColorRes();
}

#if 0
//////////////////////////////////////////////////////////////////////////

CColorCodeDlg::CColorCodeDlg() : CResourceCodeDlg(UI_RESOURCE_COLOR)
{
}
CColorCodeDlg::~CColorCodeDlg()
{
}



CColorBuildDlg::CColorBuildDlg()
{
	m_pColorViewPanel = NULL;
	m_pColorCodePanel = NULL;
	m_pSkin = NULL;
}

CColorBuildDlg::~CColorBuildDlg()
{
	if( NULL != m_pColorViewPanel )
	{
		delete m_pColorViewPanel;
		m_pColorViewPanel = NULL;
	}
	if( NULL != m_pColorCodePanel )
	{
		delete m_pColorCodePanel;
		m_pColorCodePanel = NULL;
	}
	m_pSkin = NULL;
}

BOOL CColorBuildDlg::OnInitDialog(CWindow wndFocus, LPARAM lInitParam)
{
	m_tabCtrl.Attach(GetDlgItem(IDC_TAB));
	m_tabCtrl.InsertItem(0, _T("view") );
	m_tabCtrl.InsertItem(1, _T("code") );

	m_pColorViewPanel = new CColorViewDlg;
	m_pColorViewPanel->Create(m_tabCtrl.m_hWnd);
	m_pColorCodePanel = new CColorCodeDlg;
	m_pColorCodePanel->Create(m_tabCtrl.m_hWnd);

	m_pColorViewPanel->ShowWindow(SW_SHOW);
	return TRUE;
}
void CColorBuildDlg::OnSize(UINT nType, CSize size)
{
	if (NULL == m_pColorViewPanel || NULL == m_pColorCodePanel )
		return;

	RECT rc = {WINDOW_PADDING,WINDOW_PADDING,size.cx-WINDOW_PADDING,size.cy-WINDOW_PADDING};
	m_tabCtrl.SetWindowPos(NULL,rc.left,rc.top,RectW(&rc),RectH(&rc),SWP_NOZORDER);
	m_tabCtrl.ModifyStyle(0,WS_CLIPCHILDREN);

	RECT rcItem;
	m_tabCtrl.GetItemRect(0,&rcItem);

	m_tabCtrl.GetClientRect(&rc);
//	::InflateRect(&rc, -WINDOW_PADDING, -WINDOW_PADDING );
	rc.bottom -= RectH(&rcItem);

	m_pColorViewPanel->SetWindowPos(0,rc.left,rc.top,RectW(&rc),RectH(&rc),SWP_NOZORDER);
	m_pColorCodePanel->SetWindowPos(0,rc.left,rc.top,RectW(&rc),RectH(&rc),SWP_NOZORDER);
}

LRESULT CColorBuildDlg::OnSelChanging(LPNMHDR pnmh)
{	
	if( NULL == m_pColorCodePanel || NULL == m_pColorViewPanel )
		return 0;

	int nSel = m_tabCtrl.GetCurSel();
	switch(nSel)
	{
	case 1:
		m_pColorCodePanel->Commit_2_TempFile();
		if( false == UI_ReloadResource(m_pSkin, UI_RESOURCE_COLOR) )
		{
			::MessageBox(NULL, _T("更新失败"), _T("Error"), MB_OK|MB_ICONERROR);
			return TRUE;
		}

		m_pColorViewPanel->ShowWindow(SW_SHOW);	
		m_pColorCodePanel->ShowWindow(SW_HIDE);
		m_pColorViewPanel->Reload();

		break;

	case 0:

		if( false == m_pColorCodePanel->Update_From_TempFile() )
		{
			::MessageBox(NULL, _T("更新失败"), _T("Error"), MB_OK|MB_ICONERROR);
			return TRUE;
		}
		m_pColorCodePanel->ShowWindow(SW_SHOW);
		m_pColorViewPanel->ShowWindow(SW_HIDE);	
		m_pColorViewPanel->Clear();
		break;
	}
	return 0;
}

void CColorBuildDlg::AttachSkin( HSKIN hSkin )
{
	if(NULL == m_pColorViewPanel)
		return;

	m_pSkin = hSkin;
	m_pColorViewPanel->AttachSkin(hSkin);
	m_pColorCodePanel->AttachSkin(hSkin);
}
#endif