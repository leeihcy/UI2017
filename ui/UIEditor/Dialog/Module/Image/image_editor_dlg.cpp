#include "StdAfx.h"
#include "image_editor_dlg.h"
#include "UISDK\Inc\Interface\iskindatasource.h"
#include "UICTRL\Inc\Interface\ibutton.h"
#include "UISDK\Inc\Interface\ihwndhost.h"
#include "Business\uieditorfunction.h"
#include "add_image_dlg.h"
#include "UICTRL\Inc\Interface\ilistbox.h"
#include "UICTRL\Inc\Interface\ilabel.h"

CImagePreview::CImagePreview()
{
	m_hBkBrush = NULL;

	m_eDisplayType = IMAGE_DISPLAY_TYPE_CENTER_STRETCH;
}
CImagePreview::~CImagePreview()
{
	SAFE_DELETE_GDIOBJECT(m_hBkBrush);
}

int CImagePreview::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	HBITMAP hBitmap = ::LoadBitmap( _Module.m_hInst, MAKEINTRESOURCE(IDB_TRANSPARENT_BK) );
	if (hBitmap)
	{
		m_hBkBrush = ::CreatePatternBrush(hBitmap);
		::DeleteObject(hBitmap);
	}
	return 0;
}

void CImagePreview::Attach(ISkinRes* pSkinRes, LPCTSTR pszPath)
{
	if (!m_image.IsNull())
		m_image.Destroy();

    if (pszPath && pSkinRes)
        pSkinRes->GetDataSource()->Load_Image(pszPath, &m_image);
	
	this->Invalidate();
	this->UpdateWindow();

#if 0 // TODO
	if( m_strImagePath != strImagePath )
	{
		m_image.Release();
		m_strImagePath = strImagePath;
		m_image.SetPath( strImagePath );
		m_image.AddRef();

		this->Invalidate();
		this->UpdateWindow();
	}
#endif
	return;
}

void CImagePreview::OnPaint( HDC hDC )
{
	PAINTSTRUCT ps;
	hDC = this->BeginPaint(&ps);

	CRect rc;
	this->GetClientRect(&rc);

	HDC     hMemDC   = ::CreateCompatibleDC(hDC);
	HBITMAP hBitmap  = ::CreateCompatibleBitmap(hDC, rc.Width(), rc.Height());
	HBITMAP hOldBmp  = (HBITMAP)::SelectObject(hMemDC,hBitmap);

	CDCHandle dc(hMemDC);
	DoPaint(dc);

	::BitBlt( hDC, 0,0, rc.Width(), rc.Height(), hMemDC, 0,0, SRCCOPY );
	::SelectObject(hMemDC,hOldBmp);
	::DeleteObject(hBitmap);
	::DeleteDC(hMemDC);

	this->EndPaint(&ps);
}

BOOL CImagePreview::OnEraseBkgnd( HDC hDC )
{

	return 1;
}

void CImagePreview::DoPaint(CDCHandle dc )
{
	CRect  rc;
	this->GetClientRect(&rc);

	// 透明背景
	HBRUSH hOldBrush = dc.SelectBrush(m_hBkBrush);
	dc.PatBlt( 0,0, rc.right, rc.bottom, PATCOPY );
	dc.SelectBrush(hOldBrush);


	// 灰色边框
	HPEN hPen = ::CreatePen(PS_SOLID, 1, RGB(0xCC,0xCC,0xCC) );
	POINT pts[] = 
	{
		{ 0, 0 },
		{ rc.right-1, 0 },
		{ rc.right-1, rc.bottom-1 },
		{ 0, rc.bottom-1 },
		{ 0, 0 },
	};

	HPEN hOldPen = dc.SelectPen( hPen );
	dc.Polyline(pts, 5);
	dc.SelectPen(hOldPen);
	::DeleteObject(hPen);

	if (m_image.IsNull())
		return;

	switch(m_eDisplayType)
	{
	case IMAGE_DISPLAY_TYPE_CENTER_BITBLT:
		m_image.Draw(dc.m_hDC, (rc.Width()-m_image.GetWidth())/2, (rc.Height()-m_image.GetHeight())/2 );
		break;

	case IMAGE_DISPLAY_TYPE_CENTER_STRETCH:
		{
			::SetStretchBltMode(dc.m_hDC, HALFTONE);
			double tan_x_y_image  = 0;
			double tan_x_y_window = 0;

			bool bNeedToStretch = false;
			int  xImage = m_image.GetWidth();
			int  yImage = m_image.GetHeight();

			if( rc.Width() < m_image.GetWidth() || rc.Height() < m_image.GetHeight() )
			{
				bNeedToStretch = true;

				tan_x_y_image = (double)m_image.GetWidth() / (double)m_image.GetHeight();
				tan_x_y_window = (double)rc.Width() / (double)rc.Height();

				if( tan_x_y_image > tan_x_y_window ) // 横向占满
				{
					xImage = rc.Width();
					yImage = (int)((double)xImage/tan_x_y_image);
				}
				else   // 纵向占满
				{
					yImage = rc.Height();
					xImage = (int)(yImage*tan_x_y_image);
				}
			}

			// 计算图片显示位置
			int xDisplayPos = (rc.Width()-xImage)/2;
			int yDisplayPos = (rc.Height()-yImage)/2;

			if( bNeedToStretch )
			{
				m_image.Draw(dc.m_hDC, xDisplayPos, yDisplayPos, xImage, yImage, 0,0, m_image.GetWidth(), m_image.GetHeight() );
			}
			else
			{
				m_image.Draw(dc.m_hDC, xDisplayPos, yDisplayPos );
			}
		}
		break;

	default:
		m_image.Draw(dc.m_hDC, 0, 0);
		break;
	}
}

CImageEditorDlg::CImageEditorDlg() : IWindow(CREATE_IMPL_TRUE)
{
	m_pSkin = NULL;
	m_pImageDisplayView = NULL;
	m_pListView = NULL;
	m_pBtnAdd = m_pBtnDel = m_pBtnNext = m_pBtnPrev = NULL;
	m_pHwndHost = NULL;
}
CImageEditorDlg::~CImageEditorDlg()
{
	m_pSkin = NULL;
	SAFE_DELETE(m_pImageDisplayView);
}


void CImageEditorDlg::OnInitDialog()
{
	m_pImageDisplayView = new CImagePreview;
	m_pImageDisplayView->Create( GetHWND() );
	m_pImageDisplayView->ShowWindow(SW_SHOW);

	m_pBtnAdd = (IButton*)FindObject(_T("btn_add"));
	m_pBtnDel = (IButton*)FindObject(_T("btn_del"));
// 	m_pBtnPrev = (IButton*)FindObject(_T("btn_prev"));
// 	m_pBtnNext = (IButton*)FindObject(_T("btn_next"));
	m_pHwndHost = (IHwndHost*)FindObject(_T("imagepreview"));
	if (m_pHwndHost)
	{
		m_pHwndHost->Attach(m_pImageDisplayView->m_hWnd);
	}

	m_pListView = (IListBox*)FindObject(_T("imagelistview"));
    if (m_pListView)
    {
        m_pListView->DBClickEvent().connect(this,
            &CImageEditorDlg::OnItemDBClick);

        m_pListView->SelectChangedEvent().connect(this,
            &CImageEditorDlg::OnItemChanged);

        m_pListView->KeyDownEvent().connect(this,
            &CImageEditorDlg::OnListKeyDown);
    }

    if (m_pBtnAdd)
    {
        m_pBtnAdd->ClickedEvent().connect(this,
            &CImageEditorDlg::OnBtnAdd);
    }

    if (m_pBtnDel)
    {
        m_pBtnDel->ClickedEvent().connect(this,
            &CImageEditorDlg::OnBtnRemove);
    }
    if (m_pBtnPrev)
    {
        m_pBtnPrev->ClickedEvent().connect(this,
            &CImageEditorDlg::OnBtnLeft);
    }
    if (m_pBtnNext)
    {
        m_pBtnNext->ClickedEvent().connect(this,
            &CImageEditorDlg::OnBtnRight);
    }
}

//
//	设置数据源
//
void CImageEditorDlg::AttachSkin(ISkinRes* h)
{
	if (m_pSkin == h)
		return;

	// changed notify
	m_pSkin = h;

	Reload();
}

bool CImageEditorDlg::Reload()
{
	if (!m_pSkin)
		return false;
	if (!m_pListView)
		return false;

	IImageRes& pImageRes = m_pSkin->GetImageRes();

	m_pListView->RemoveAllItem();

	// 获取该皮肤中的所有图片
	long nCount = pImageRes.GetImageCount();
	for (long i = 0; i < nCount; i++)
	{
		IImageResItem*  pInfo = pImageRes.GetImageResItem(i);
		if (pInfo)
		{
            ICustomListItem* pItem = m_pListView->InsertCustomItem(
                TEXT("imageview.list"), i);

            UI::ILabel* pLabelId = (UI::ILabel*)pItem->FindControl(TEXT("id"));
            UI::ILabel* pLabelPath = (UI::ILabel*)pItem->FindControl(TEXT("path"));

            pLabelId->SetText(pInfo->GetId());
            pLabelPath->SetText(pInfo->GetPath());
			pItem->SetData((LPARAM)pInfo);
		}
	}

	m_pImageDisplayView->Attach(NULL, NULL);

	// 将新增的图片显示出来
	m_pListView->SelectItem(m_pListView->GetFirstItem(), false);

	return true;
}

void CImageEditorDlg::OnBtnAdd(UI::IButton*)
{
	if (!m_pSkin || !m_pListView)
		return;

	CAddImageDlg  dlg(m_pSkin);
	dlg.DoModal();
}

void CImageEditorDlg::OnBtnRemove(UI::IButton*)
{
	if (!m_pSkin || !m_pListView)
		return ;

    IListItemBase* pSelItem = (IListItemBase*)m_pListView->GetFirstSelectItem();
     if (!pSelItem)
         return;
    
     IImageResItem* pImageResItem = (IImageResItem*)pSelItem->GetData();
     if (!pImageResItem)
         return;

     m_pImageDisplayView->Attach(NULL, NULL);  
     UE_ExecuteCommand(DeleteImageItemCommand::CreateInstance(m_pSkin, pImageResItem));
}

void CImageEditorDlg::OnBtnRight(UI::IButton*)
{
// 	int nCount = m_listctrl.GetItemCount();
// 	if( 0 == nCount )
// 		return;
// 
// 	int nIndex = m_listctrl.GetSelectedIndex();
// 	if( -1 == nIndex )
// 		nIndex = 0;
// 	else
// 		nIndex++;
// 
// 	if( nIndex >= nCount )
// 		nIndex = nCount-1;
// 
// 	m_listctrl.SelectItem(nIndex);
}
void CImageEditorDlg::OnBtnLeft(UI::IButton*)
{
// 	int nCount = m_listctrl.GetItemCount();
// 	if( 0 == nCount )
// 		return;
// 
// 	int nIndex = m_listctrl.GetSelectedIndex();
// 	nIndex--;
// 
// 	if( nIndex <= 0 )
// 		nIndex = 0;
// 
// 	m_listctrl.SelectItem(nIndex);
}

//
//	显示当前被选中的图片
//
void  CImageEditorDlg::OnItemChanged(IListCtrlBase*)
{
    ICustomListItem* pListViewItem = 
        (ICustomListItem*)m_pListView->GetFirstSelectItem();
    if (!pListViewItem)
        return;

    UI::ILabel* pLabel = (UI::ILabel*)pListViewItem->FindControl(TEXT("path"));
    m_pImageDisplayView->Attach(m_pSkin, pLabel->GetText());
}

void  CImageEditorDlg::OnItemDBClick(IListCtrlBase*, IListItemBase*)
{
    ICustomListItem* pListViewItem = (ICustomListItem*)
        m_pListView->GetFirstSelectItem();
	if (!pListViewItem)
		return;

	CAddImageDlg dlg(m_pSkin,true);
	dlg.SetImageInfo((IImageResItem*)pListViewItem->GetData());
	if (IDOK == dlg.DoModal())
	{
        UI::ILabel* pLabel = (UI::ILabel*)
            pListViewItem->FindControl(TEXT("path"));

        pLabel->SetText(dlg.m_strPath.c_str());

// 		m_pImageDisplayView->Invalidate(TRUE);
// 		m_pImageDisplayView->UpdateWindow();
	}
}

void  CImageEditorDlg::OnListKeyDown(IListCtrlBase* pList, UINT nChar, bool&)
{
    UI::ICustomListItem* pItem = (UI::ICustomListItem*)
            pList->GetFirstSelectItem();
    if (!pItem)
        return;

    if (nChar != TEXT('C') || !Util::IsKeyDown(VK_CONTROL))
        return;

    UI::ILabel* pLabel = (UI::ILabel*)pItem->FindControl(TEXT("id"));
    UIASSERT(pLabel);

    CString strTexst(pLabel->GetText());

    if (!OpenClipboard(NULL))
        return;

    EmptyClipboard();

    int nSize = sizeof(TCHAR) * (strTexst.GetLength() + 1);
    HGLOBAL hBlock = GlobalAlloc(GMEM_MOVEABLE, nSize);
    if (hBlock)
    {
        void* p = GlobalLock(hBlock);
        memcpy(p, strTexst, nSize);
        GlobalUnlock(hBlock);
    }
    SetClipboardData(CF_UNICODETEXT, hBlock);

    CloseClipboard();
    if (hBlock)
        GlobalFree(hBlock);
}

LRESULT  CImageEditorDlg::OnAddImage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	IImageResItem* pImageResItem = (IImageResItem*)wParam;
	if (!pImageResItem)
		return 0;
	if (!m_pListView)
		return 0;

    ICustomListItem* pItem = m_pListView->InsertCustomItem(
        TEXT("imageview.list"), m_pListView->GetItemCount());

    UI::ILabel* pLabelId = (UI::ILabel*)pItem->FindControl(TEXT("id"));
    UI::ILabel* pLabelPath = (UI::ILabel*)pItem->FindControl(TEXT("path"));

    pLabelId->SetText(pImageResItem->GetId());
    pLabelPath->SetText(pImageResItem->GetPath());
    pItem->SetData((LPARAM)pImageResItem);

	// 将新增的图片显示出来
	m_pListView->SelectItem(pItem, false);
	return 0;
}

LRESULT  CImageEditorDlg::OnDeleteImage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (!m_pListView)
		return 0;

	IListItemBase* pItem = m_pListView->GetFirstItem();
	while (pItem)
	{
		if (pItem->GetData() == wParam)
		{
			m_pListView->RemoveItem(pItem);
			break;
		}
		pItem = pItem->GetNextItem();
	}
	return 0;
}

long*  CImageEditorDlg::GetKey()
{
	return (long*)&m_pSkin->GetImageRes();
}
