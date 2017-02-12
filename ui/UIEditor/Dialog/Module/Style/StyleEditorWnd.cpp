#include "StdAfx.h"
#include "StyleEditorWnd.h"
#include "StyleDialogs.h"
#include "Business\Command\style\StyleCommand.h"
#include "Business\uieditorfunction.h"


CStyleEditorWnd::CStyleEditorWnd(void)
{
}

CStyleEditorWnd::~CStyleEditorWnd(void)
{
}

void CStyleEditorWnd::OnDestroy()
{
	m_listctrl.DeleteAllItems();
	GetUIEditor()->UnRegisterStyleChangedCallback(static_cast<StyleChangedCallback*>(this));
}
BOOL CStyleEditorWnd::OnInitDialog( HWND, LPARAM )
{
	m_listctrl.Attach( GetDlgItem(IDC_STYLELIST) );
	m_listctrl.InsertColumn( 0, _T("type"), LVCFMT_LEFT, 100 );
	m_listctrl.InsertColumn( 1, _T("id"), LVCFMT_LEFT, 100 );
	m_listctrl.InsertColumn( 2, _T("inherit"), LVCFMT_LEFT, 300 );

	m_listctrl.SetExtendedListViewStyle( LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES );
	m_listctrl.ModifyStyle( 0, LVS_SINGLESEL|LVS_SHOWSELALWAYS );

	GetUIEditor()->RegisterStyleChangedCallback(static_cast<StyleChangedCallback*>(this));
	return TRUE;
}

void  CStyleEditorWnd::OnSize(UINT nType, CSize size)
{
	const int BTN_HEIGHT = 22;
	const int BTN_WIDTH  = 100;
	const int PROP_BTN_WIDTH = 70;

	HDWP hdwp = ::BeginDeferWindowPos(7);

	::DeferWindowPos(hdwp,GetDlgItem(IDC_BUTTON_ADD_SELECTOR).m_hWnd, 0, WINDOW_PADDING, WINDOW_PADDING,
		BTN_WIDTH,BTN_HEIGHT,SWP_NOZORDER );

	::DeferWindowPos(hdwp,GetDlgItem(IDC_BUTTON_MODIFY_SELECTOR).m_hWnd, 0, WINDOW_PADDING*2+BTN_WIDTH, WINDOW_PADDING,
		BTN_WIDTH,BTN_HEIGHT,SWP_NOZORDER );

	::DeferWindowPos(hdwp,GetDlgItem(IDC_BUTTON_DELETE_SELECTOR).m_hWnd, 0, WINDOW_PADDING*3+BTN_WIDTH*2, WINDOW_PADDING,
		BTN_WIDTH,BTN_HEIGHT,SWP_NOZORDER );

	::DeferWindowPos(hdwp,GetDlgItem(IDC_BUTTON_ADD_PROP).m_hWnd, 0, size.cx-WINDOW_PADDING*3-PROP_BTN_WIDTH*3, WINDOW_PADDING,
		PROP_BTN_WIDTH,BTN_HEIGHT,SWP_NOZORDER );

	::DeferWindowPos(hdwp,GetDlgItem(IDC_BUTTON_MODIFY_PROP).m_hWnd, 0, size.cx-WINDOW_PADDING*2-PROP_BTN_WIDTH*2, WINDOW_PADDING,
		PROP_BTN_WIDTH,BTN_HEIGHT,SWP_NOZORDER );

	::DeferWindowPos(hdwp,GetDlgItem(IDC_BUTTON_REMOVE_PROP).m_hWnd, 0, size.cx-WINDOW_PADDING-PROP_BTN_WIDTH, WINDOW_PADDING,
		PROP_BTN_WIDTH,BTN_HEIGHT,SWP_NOZORDER );

	::DeferWindowPos(hdwp,m_listctrl.m_hWnd, 0, WINDOW_PADDING, 
		WINDOW_PADDING*2+BTN_HEIGHT, 
		size.cx-2*WINDOW_PADDING,
		size.cy-BTN_HEIGHT-3*WINDOW_PADDING,SWP_NOZORDER );

	::EndDeferWindowPos(hdwp);
}
//
//	设置数据源
//
void CStyleEditorWnd::AttachSkin( ISkinRes* h )
{
	if( m_pSkin == h )
		return;

	// changed notify
	m_pSkin = h;

	this->Reload();
}

void  CStyleEditorWnd::InsertItem2ListView(IStyleResItem* pItem, int nPos)
{
	if (pItem)
	{
		int nItemCount = m_listctrl.GetItemCount();

		TCHAR szText[256] = _T("");
		STYLE_SELECTOR_TYPE type = pItem->GetSelectorType();
		switch(type)
		{
		case STYLE_SELECTOR_TYPE_ID:
			_stprintf(szText, _T("ID"));
			break;
		case STYLE_SELECTOR_TYPE_CLASS:
			_stprintf(szText, _T("CLASS"));
			break;
		default:
			_stprintf(szText, _T("TAG"));
			break;
		}
		m_listctrl.InsertItem(nPos, szText );
		m_listctrl.SetItemText(nPos, 1, pItem->GetId() );
		m_listctrl.SetItemText(nPos, 2, pItem->GetInherits() );

		m_listctrl.SetItemData(nPos, (DWORD_PTR)pItem);
	}
}

bool  CStyleEditorWnd::Reload()
{
	if (NULL == m_pSkin)
		return false;

	IStyleRes& pStyleRes = m_pSkin->GetStyleRes();

	// 获取该皮肤中的所有图片
	m_listctrl.DeleteAllItems();

	long nCount = pStyleRes.GetStyleCount();

	for (long i = 0; i < nCount; i++)
	{
		IStyleResItem*  pInfo = pStyleRes.GetItem(i);
		InsertItem2ListView(pInfo, i);
	}

	m_listctrl.SelectItem(0);
	return true;
}

void  CStyleEditorWnd::OnBtnAddSelector(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	if (NULL == m_pSkin)
		return;

	IStyleRes& pStyleRes = m_pSkin->GetStyleRes();

	CNewStyleDlg dlg(m_pSkin);
	if (IDOK == dlg.DoModal())
	{
		int nCount = m_listctrl.GetItemCount();
		
		TCHAR szText[256] = _T("");
		IStyleResItem*  pInfo = pStyleRes.GetItem(nCount);
		if (pInfo)
		{
			STYLE_SELECTOR_TYPE type = pInfo->GetSelectorType();
			switch(type)
			{
			case STYLE_SELECTOR_TYPE_ID:
				_stprintf(szText, _T("ID"));
				break;
			case STYLE_SELECTOR_TYPE_CLASS:
				_stprintf(szText, _T("CLASS"));
				break;
			default:
				_stprintf(szText, _T("TAG"));
				break;
			}
			m_listctrl.InsertItem( nCount, szText );
			m_listctrl.SetItemText( nCount, 1, pInfo->GetId() );
			m_listctrl.SetItemText( nCount, 2, pInfo->GetInherits() );

			m_listctrl.SetItemData(nCount, (DWORD_PTR)pInfo);

			g_pGlobalData->m_pMainFrame->SetDirty();
		}
	}
}

void  CStyleEditorWnd::OnBtnModifySelector(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	int nIndex = m_listctrl.GetSelectedIndex();
	if( -1 == nIndex )
		return;

	IStyleResItem* pInfo = (IStyleResItem*)m_listctrl.GetItemData(nIndex);
	if (!pInfo)
		return;

	CNewStyleDlg dlg(m_pSkin, true);
	dlg.SetStyleInfo( pInfo->GetSelectorType(), pInfo->GetId(), pInfo->GetInherits() );
	dlg.DoModal();
}

void  CStyleEditorWnd::OnBtnDeleteSelector(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	int nIndex = m_listctrl.GetSelectedIndex();
	if (-1 == nIndex)
		return;

	IStyleResItem* pItem = (IStyleResItem*)m_listctrl.GetItemData(nIndex);
	if (pItem)
		UE_ExecuteCommand(DeleteStyleItemCommand::CreateInstance(m_pSkin, pItem));
}

LRESULT CStyleEditorWnd::OnItemDBClick(LPNMHDR pnmh)
{
	LRESULT lret = this->DefWindowProc();

	LPNMITEMACTIVATE p = (LPNMITEMACTIVATE )pnmh;
	if( -1 == p->iItem )
		return lret;

	this->SendMessage( WM_COMMAND, MAKEWPARAM(IDC_BUTTON_MODIFY_SELECTOR,BN_CLICKED), (LPARAM)GetDlgItem(IDC_BUTTON_MODIFY_SELECTOR).m_hWnd);

	return lret;
}
LRESULT CStyleEditorWnd::OnItemKeyDown(LPNMHDR pnmh)
{
	NMLVKEYDOWN* p = (NMLVKEYDOWN*)pnmh;
	 if (VK_DELETE == p->wVKey)
	 {
		 int nIndex = m_listctrl.GetSelectedIndex();
		 if (-1 == nIndex)
			 return 0;

		 IStyleResItem* pItem = (IStyleResItem*)m_listctrl.GetItemData(nIndex);
		 if (pItem)
			 UE_ExecuteCommand(DeleteStyleItemCommand::CreateInstance(m_pSkin, pItem));
	 }
	 else if (VK_RETURN == p->wVKey)
	 {
		 int nIndex = m_listctrl.GetSelectedIndex();
		 if (-1 == nIndex)
			 return 0;

		 this->SendMessage( WM_COMMAND, MAKEWPARAM(IDC_BUTTON_MODIFY_SELECTOR,BN_CLICKED), 
			 (LPARAM)GetDlgItem(IDC_BUTTON_MODIFY_SELECTOR).m_hWnd);
	 }

	 return 0;
}

LRESULT CStyleEditorWnd::OnPropItemDBClick(LPNMHDR pnmh)
{
	LRESULT lret = this->DefWindowProc();

	LPNMITEMACTIVATE p = (LPNMITEMACTIVATE )pnmh;
	if( -1 == p->iItem )
		return lret;

	this->SendMessage( WM_COMMAND, MAKEWPARAM(IDC_BUTTON_MODIFY_PROP,BN_CLICKED), (LPARAM)GetDlgItem(IDC_BUTTON_MODIFY_PROP).m_hWnd);

	return lret;
}

LRESULT CStyleEditorWnd::OnStyleItemChanged(LPNMHDR pnmh)
{
	LPNMLISTVIEW lpnmh = (LPNMLISTVIEW)pnmh;

	// 被选中，或者被取消选中
	if( (!(lpnmh->uOldState & LVIS_SELECTED ) && ( lpnmh->uNewState & LVIS_SELECTED ) ) ||
		((lpnmh->uOldState & LVIS_SELECTED ) &&	!( lpnmh->uNewState & LVIS_SELECTED ) )
		)
	{
		int nIndex = m_listctrl.GetSelectedIndex();
		if( -1 == nIndex)
			return 0;

		IStyleResItem* pItem = (IStyleResItem*)m_listctrl.GetItemData(nIndex);
		if (pItem)
			m_PropertyHandler.AttachStyle(m_pSkin, pItem);
	}
	return 0;
}	

long*  CStyleEditorWnd::GetKey()
{
	return  (long*)&m_pSkin->GetStyleRes();
}

void  CStyleEditorWnd::OnStyleAdd(IStyleResItem* p)
{
	int nPos = m_pSkin->GetStyleRes().GetItemPos(p);
	this->InsertItem2ListView(p, nPos);
}
void  CStyleEditorWnd::OnStyleRemove(IStyleResItem* p)
{
	int nPos = m_pSkin->GetStyleRes().GetItemPos(p);
	m_listctrl.DeleteItem(nPos);
}
void  CStyleEditorWnd::OnStyleModify(IStyleResItem* p)
{
	int nPos = m_pSkin->GetStyleRes().GetItemPos(p);
	m_listctrl.DeleteItem(nPos);
	InsertItem2ListView(p, nPos);
}
void  CStyleEditorWnd::OnStyleAttributeAdd(IStyleResItem* p, LPCTSTR szKey)
{
	m_PropertyHandler.OnStyleAttributeAdd(p, szKey);
}
void  CStyleEditorWnd::OnStyleAttributeRemove(IStyleResItem* p, LPCTSTR szKey)
{
	m_PropertyHandler.OnStyleAttributeRemove(p, szKey);
}
void  CStyleEditorWnd::OnStyleAttributeModify(IStyleResItem* p, LPCTSTR szKey)
{
	m_PropertyHandler.OnStyleAttributeModify(p, szKey);
}