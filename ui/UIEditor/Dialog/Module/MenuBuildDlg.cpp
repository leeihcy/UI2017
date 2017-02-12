#include "StdAfx.h"
#include "MenuBuildDlg.h"
#include "Dialogs.h"
#include "Dialog\Framework\PropertyDialog.h"
#include "Business\Project\ProjectData\projectdata.h"
#include "UISDK\Inc\Util\iimage.h"
#include "Business\Command\AddObjectCommand.h"
#include "Business\Command\Base\commandhistorymgr.h"
#include "UICTRL\Inc\Interface\imenu.h"
#include "UISDK\Inc\Interface\ilayout.h"
#include "Business\Command\SetObjectPosCommand.h"

//////////////////////////////////////////////////////////////////////////

CMenuViewDlg::CMenuViewDlg() : IWindow(CREATE_IMPL_TRUE)
{
	m_pSkin = NULL;
	m_hBkBrush = NULL;
	m_pMenu = NULL;

	SetRectEmpty(&m_rcWindowDraw);
}

CMenuViewDlg::~CMenuViewDlg()
{
	SAFE_DELETE_GDIOBJECT(m_hBkBrush);

	if (m_pMenu)
	{
//         IWindow*  pWindow = m_pMenu->GetWindowObject();
//         if (pWindow)
//         {
//             ::DestroyWindow(pWindow->GetHWND());
//         }
		UIDestroyMenu(m_pMenu);
		m_pMenu = NULL;
	}
}

void CMenuViewDlg::OnInitialize()
{
	HBITMAP hBitmap = ::LoadBitmap( _Module.m_hInst, MAKEINTRESOURCE(IDB_TRANSPARENT_BK) );
	if( NULL != hBitmap )
	{
		m_hBkBrush = ::CreatePatternBrush(hBitmap);
		::DeleteObject(hBitmap);
	}

	long lStyle = GetClassLong(GetHWND(), GCL_STYLE);
	::SetClassLong(GetHWND(), GCL_STYLE, lStyle | CS_VREDRAW|CS_HREDRAW);

    this->SetDroppable(true);

	UI::OBJSTYLE s = {0};
	s.receive_dragdrop_event = 1;
    this->ModifyObjectStyle(&s, 0);

	UpdateTitle();
    m_recttracker.SetInfo(GetHWND(), NULL);
}

void CMenuViewDlg::OnPaint(IRenderTarget* pRenderTarget)
{
    HDC hDC = pRenderTarget->GetHDC();
    ::SetStretchBltMode(hDC, COLORONCOLOR);

	CRect rc;
	this->GetClientRectInObject(&rc);

	HDC  hMemDC = ::CreateCompatibleDC(NULL);

    ImageWrap image;
    image.Create(rc.Width(), rc.Height(), 32, ImageWrap::createAlphaChannel);
    HBITMAP hBitmap  = image.Detach();
	HBITMAP hOldBmp  = (HBITMAP)::SelectObject(hMemDC,hBitmap);

	CDCHandle dc(hMemDC);
	DoPaintBkgnd(dc);
    DoPaint(dc);

    PrepareDC(hDC);    
 	::BitBlt( hDC, 0,0, rc.Width(), rc.Height(), hMemDC, 0,0, SRCCOPY );
    UnPrepareDC(hDC);

	::SelectObject(hMemDC,hOldBmp);
	::DeleteObject(hBitmap);
	::DeleteDC(hMemDC);
}
void CMenuViewDlg::OnEraseBkgnd(IRenderTarget* pRenderTarget)
{

}

void CMenuViewDlg::DoPaintBkgnd(CDCHandle& dc)
{
	CRect  rc;
	this->GetClientRectInObject(&rc);

	// 透明背景
	HBRUSH hOldBrush = dc.SelectBrush(m_hBkBrush);
	dc.PatBlt(0,0, rc.right, rc.bottom, PATCOPY);
	dc.SelectBrush(hOldBrush);


	// 黑色边框
	HPEN hPen = ::CreatePen(PS_SOLID, 1, RGB(0,0,0) );
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
}

void CMenuViewDlg::DoPaint(CDCHandle& dc)
{
    if (!m_pMenu)
        return;

    IWindowBase*  pWindow = m_pMenu->GetWindowObject();
	if (NULL == pWindow)
		return;

	CRect  rc;
	this->GetClientRectInObject(&rc);

	// 绘制窗口预览图
    //bool bAlpha = m_pMenu->GetGraphicsRenderLibraryType()==GRAPHICS_RENDER_LIBRARY_TYPE_GDI?false:true;
	pWindow->AlphaBlendMemBitmap(dc.m_hDC, &m_rcWindowDraw, 255);  // 在GDI RenderLibraryType下面时没有alpha

	m_recttracker.Draw(dc.m_hDC);
}

LRESULT  CMenuViewDlg::OnRefresh(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    ::InvalidateRect(GetHWND(), NULL, TRUE);
    return 0;
}

LRESULT CMenuViewDlg::OnBtnClickPreView(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    return 0;
}

void CMenuViewDlg::OnSize(UINT nType, int cx, int cy)
{
    SetMsgHandled(FALSE);
}

LRESULT CMenuViewDlg::OnPrevViewWindowDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return 0;
}

IObject* CMenuViewDlg::CreateObject(LPCTSTR szNodeName, LPCTSTR szNodeID)
{
	UI::LoadMenuData data = {
		g_pGlobalData->m_pProjectData->m_pEditUIApp->GetDefaultSkinRes(), 
		0, szNodeID};

    m_pMenu = UILoadMenu(&data);
	if (!m_pMenu)
	{
		MsgBox(_T("创建菜单失败，检查是否添加依赖控件"));
		return NULL;
	}
    //m_pMenu->TrackPopupMenu(0, 0, 0, NULL);
    //m_pWindow = m_pMenu->GetPopupWindow();
    IWindow* pWindow = m_pMenu->CreateMenuWindow();

	if (pWindow)
	{
		::EnableWindow(pWindow->GetHWND(), FALSE);
		::SendMessage(pWindow->GetHWND(), WM_PAINT, 0, 0);

        RECT  rc = {0, 0, pWindow->GetWidth(), pWindow->GetHeight()};
        ::OffsetRect(&rc, 10, 10);
        SetWindowDrawRect(&rc);

		// ::ShowWindow(m_pWindow->GetHWND(), SW_SHOWNOACTIVATE);
		// ::UpdateWindow(m_pWindow->GetHWND());  // 强制调用WM_PAINT创建MemBitmap 
		// m_pWindow->HideWindow();               // 只需要membitmap就够了

		return m_pMenu;
	}
	return NULL;
}

void**  CMenuViewDlg::QueryInterface(REFIID riid)
{
	if (::IsEqualIID(IID_IMenuMDIClientView , riid))
	{
		return (void**)static_cast<IMenuMDIClientView*>(this);
	}
    return IMDIClientView::QueryInterface(riid);
}

bool CMenuViewDlg::ShowLayoutNode(LayoutTreeItemData* pData)
{
	if (NULL == pData)
		return false;

	if (NULL == m_pMenu && NULL == pData->m_pObject)
	{
		// 创建窗口
		pData->m_pObject = this->CreateObject(pData->m_strNodeName.c_str(), pData->m_strNodeID.c_str());
		if (pData->m_pObject)
		{
			g_pGlobalData->m_pProjectData->m_uieditor.SetObjectHTREEITEM(pData->m_pObject, pData->m_pListItem);
		}
	}

	return true;
}


// MDI失焦，鼠标点击了其它MDI窗口
void CMenuViewDlg::OnMDIActivate(bool bActivate)
{
	if (!bActivate)	
	{
        int a= 0;
	}
}

long*  CMenuViewDlg::GetKey()
{
	return (long*)m_pMenu;
}

void  CMenuViewDlg::PrepareDC(HDC hDC)
{
}
void  CMenuViewDlg::UnPrepareDC(HDC hDC)
{
}

void  CMenuViewDlg::SetWindowDrawRect(RECT* prc)
{
    CopyRect(&m_rcWindowDraw, prc);
}
void  CMenuViewDlg::UpdateTitle()
{
	HWND  hWndParent = GetParent(GetHWND());
	if (!hWndParent)
		return;

	String  strTitle(_T("menu - "));

	if (m_pSkin)
	{
		strTitle.append(m_pSkin->GetName());
		strTitle.append(_T(" - "));
	}
	if (m_pMenu)
	{
		strTitle.append(m_pMenu->GetId());
	}

	::SetWindowText(hWndParent, strTitle.c_str());
}

void  CMenuViewDlg::DP2LP(POINT* p)
{
    if (!p)
        return;

    HDC hDC = GetDC(GetHWND());
    PrepareDC(hDC);
    DPtoLP(hDC, (POINT*)p, 1);
    UnPrepareDC(hDC);
    ReleaseDC(GetHWND(), hDC);
}