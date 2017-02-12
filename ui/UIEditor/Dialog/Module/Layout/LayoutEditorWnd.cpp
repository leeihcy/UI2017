#include "StdAfx.h"
#include "LayoutEditorWnd.h"
#include "UISDK\Inc\Util\iimage.h"
#include "UISDK\Inc\Interface\ilayout.h"
#include "UICTRL\Inc\Interface\imenu.h"
#include "Dialog\Framework\PropertyDialog.h"
#include "Business\Project\ProjectData\projectdata.h"
#include "Business\Command\AddObjectCommand.h"
#include "Business\Command\Base\commandhistorymgr.h"
#include "Business\Command\SetObjectPosCommand.h"

CLayoutEditorDlg::CLayoutEditorDlg() : IWindow(CREATE_IMPL_TRUE)
{
	m_pSkin = NULL;
	m_hBkBrush = NULL;
	m_pWindow = NULL;
	m_pCurSelectObject = NULL;
    m_hCursorHand = NULL;
    m_hCursorHandPress = NULL;

    m_nZoom = 100;
    m_bCenter = false;
    m_eMouseAction = MOUSE_ACTION_NONE;
    m_ptLastClick.x = m_ptLastClick.y = 0;
    m_ptLastMouseMove.x = m_ptLastMouseMove.y = 0;
	m_nWindowRenderAlpha = 255;

	SetRectEmpty(&m_rcWindowDraw);
    m_rectObjectSave.SetRectEmpty();
}

CLayoutEditorDlg::~CLayoutEditorDlg()
{
	SAFE_DELETE_GDIOBJECT(m_hBkBrush);

	if (m_pWindow)
	{
		::DestroyWindow(m_pWindow->GetHWND());
		SAFE_RELEASE(m_pWindow);
	}
    if (m_hCursorHand)
    {
        DestroyCursor(m_hCursorHand);
        m_hCursorHand = NULL;
    }
    if (m_hCursorHandPress)
    {
        DestroyCursor(m_hCursorHandPress);
        m_hCursorHandPress = NULL;
    }
}

HBRUSH createTransparentBrush()
{
	HBRUSH  brush = nullptr;
	HBITMAP hBitmap = ::LoadBitmap( _Module.m_hInst, MAKEINTRESOURCE(IDB_TRANSPARENT_BK) );
	if(hBitmap)
	{
		brush = ::CreatePatternBrush(hBitmap);
		::DeleteObject(hBitmap);
	}

	return brush;
}
HBRUSH createImageBrush(LPCTSTR szFile)
{
	HBRUSH  brush = nullptr;

	UI::ImageWrap image;
	if (image.Load(szFile))
	{
		brush = ::CreatePatternBrush((HBITMAP)image.GetHBITMAP());
		image.Destroy();
	}
	return brush;
}

void CLayoutEditorDlg::OnInitialize()
{
	m_hBkBrush = createTransparentBrush();

    this->SetDroppable(true);

	UI::OBJSTYLE s = {0};
	s.receive_dragdrop_event = 1;
    this->ModifyObjectStyle(&s, 0);

	UpdateTitle();
    m_recttracker.SetInfo(GetHWND(), static_cast<IRectTrackerRsp*>(this));
}

void CLayoutEditorDlg::OnPaint(IRenderTarget* pRenderTarget)
{
    CRect rc;
    this->GetClientRectInObject(&rc);
    if (rc.Width() == 0 || rc.Height() == 0)
        return;

    HDC hDC = pRenderTarget->GetHDC();
    ::SetStretchBltMode(hDC, COLORONCOLOR);

#if 0
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
#else

    PrepareDC(hDC);

    CDCHandle dc(hDC);
    DoPaintBkgnd(dc);
    DoPaint(dc);
    dc.Detach();

    UnPrepareDC(hDC);

    // pRenderTarget->Save(TEXT("d:\\aaa.png"));
#endif
}
void CLayoutEditorDlg::OnEraseBkgnd(IRenderTarget* pRenderTarget)
{

}

void CLayoutEditorDlg::DoPaintBkgnd(CDCHandle& dc)
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

void CLayoutEditorDlg::DoPaint(CDCHandle& dc)
{
	if (NULL == m_pWindow)
		return;

	CRect  rc;
	this->GetClientRectInObject(&rc);

	// 绘制窗口预览图
    bool bAlpha = m_pWindow->GetWindowRender()->GetRequireAlphaChannel();

	// 在GDI RenderLibraryType下面时没有alpha
	if (bAlpha)
	{
		m_pWindow->AlphaBlendMemBitmap(
			dc.m_hDC, &m_rcWindowDraw, m_nWindowRenderAlpha);  
	}
	else
	{
		m_pWindow->BitBltMemBitmap(dc.m_hDC, &m_rcWindowDraw);
	}

	m_recttracker.Draw(dc.m_hDC);
}

LRESULT  CLayoutEditorDlg::OnRefresh(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    Invalidate();

    // 更新窗口大小，TODO:有没有其它方法
    if (m_pWindow)
    {
        int nWidth = m_pWindow->GetWidth();
        int nHeight = m_pWindow->GetHeight();
        int nLeft = m_rcWindowDraw.left;
        int nTop = m_rcWindowDraw.top;

        RECT rc = {nLeft, nTop, nLeft + nWidth, nTop + nHeight};
        SetWindowDrawRect(&rc);
    }

    return 0;
}

LRESULT CLayoutEditorDlg::OnBtnClickPreView(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (NULL == m_pWindow)
		return 0;
	
	IObject* p = GetEditUIApplication()->CreateUIObjectByName(
			m_pWindow->GetDescription()->GetTagName(),
            m_pWindow->GetSkinRes());
	if (p)
	{
        IWindow* pWindow = (IWindow*)p->QueryInterface(__uuidof(IWindow));
        if (pWindow)
        {
            // pWindow->SetUserData(GetHWND());   // 用于通知窗口销毁

            // 这里不用能Create/Show。
            // 因为预览的窗口中可能存在动画，因此要用到该皮肤自己的app MsgLoop。而DoModal里正好调用了该函数
		    pWindow->DoModal(/*g_pGlobalData->m_pProjectData->m_pEditUIApp, */
				m_pWindow->GetId(), GetActiveWindow(), false);  
        }
	}
	SAFE_RELEASE(p);

    return 0;
}

void CLayoutEditorDlg::OnSize(UINT nType, int cx, int cy)
{
    SetMsgHandled(FALSE);

    if (!m_pWindow)
        return;

    if (!m_bCenter)
        return;

    int nWindowWidth = m_pWindow->GetWidth();
    int nWindowHeight = m_pWindow->GetHeight();

    RECT rc = {
        (cx-nWindowWidth)/2,
        (cy-nWindowHeight)/2,
        m_rcWindowDraw.left + nWindowWidth,
        m_rcWindowDraw.top + nWindowHeight,
    };
    SetWindowDrawRect(&rc);

}

LRESULT CLayoutEditorDlg::OnPrevViewWindowDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	IWindow* pWindow = (IWindow*)wParam;
	SAFE_RELEASE(pWindow);
	return 0;
}

IObject* CLayoutEditorDlg::CreateObject(LPCTSTR szNodeName, LPCTSTR szNodeID)
{
    IObject* p = NULL;
    if (!szNodeName || !szNodeName[0])
        p = GetEditUIApplication()->CreateUIObjectByName(TEXT("Window"), m_pSkin); 
    else
        p = GetEditUIApplication()->CreateUIObjectByName(szNodeName, m_pSkin);

    if (NULL == p)
        return NULL;

    switch (p->GetDescription()->GetMajorType())
    {
    case OBJ_WINDOW:
        {
            m_pWindow = (IWindow*)p->QueryInterface(__uuidof(IWindow));
            if (m_pWindow)
            {
                m_pWindow->Create(szNodeID, g_pGlobalData->m_pMainFrame->m_hWnd);
            }

            IWindowRender* pWindowRender = m_pWindow->GetWindowRender();
            pWindowRender->SetCommitListener(
                static_cast<IWindowCommitListener*>(this));
        }
        break;
    }

	if (m_pWindow)
	{
		::EnableWindow(m_pWindow->GetHWND(), FALSE);

        RECT  rc = {0, 0, m_pWindow->GetWidth(), m_pWindow->GetHeight()};
        ::OffsetRect(&rc, 10, 10);
        SetWindowDrawRect(&rc);
		return m_pWindow;
	}

	return NULL;
}

void CLayoutEditorDlg::PreCommit(LPCRECT dirtyarray, int count)
{

}
void CLayoutEditorDlg::PostCommit(LPCRECT dirtyarray, int count)
{
    this->Invalidate();
}

void**  CLayoutEditorDlg::QueryInterface(REFIID riid)
{
	if (::IsEqualIID(IID_ILayoutMDIClientView, riid))
	{
		return (void**)static_cast<ILayoutMDIClientView*>(this);
	}
    return IMDIClientView::QueryInterface(riid);
}

bool CLayoutEditorDlg::ShowLayoutNode(LayoutTreeItemData* pData)
{
	if (NULL == pData)
		return false;

	if (NULL == m_pWindow && NULL == pData->m_pObject)
	{
		// 创建窗口
		pData->m_pObject = this->CreateObject(pData->m_strNodeName.c_str(), pData->m_strNodeID.c_str());
		if (pData->m_pObject)
		{
			GetUIEditor()->SetObjectHTREEITEM(pData->m_pObject, pData->m_pListItem);
		}

        if (m_pWindow)
		{
			// 加载所有子结点到树中显示
            g_pGlobalData->m_pProjectTreeDialog->LoadObjectChildNode(m_pWindow);
		}
	}
	else
	{
		if (pData->GetDataType() == TREEITEM_DATA_TYPE_LAYOUT_OBJECT ||
            pData->GetDataType() == TREEITEM_DATA_TYPE_LAYOUT_WINDOW)
		{
			SelectObject(pData->m_pObject);
		}
	}

	return true;
}

// bool CLayoutEditorDlg::GetLayoutNodeInfo(__out TCHAR* szNodeName, int nNodeNameSize, __out TCHAR* szNodeID, int nNodeIdSize)
// {
// 	if (NULL == m_pWindow)
// 		return false;
// 	
// 	if (NULL == szNodeID || NULL == szNodeName)
// 		return false;
// 
// 	_tcsncpy(szNodeName, m_pWindow->GetObjectName(), nNodeNameSize-1);
// 	_tcsncpy(szNodeID, m_pWindow->GetId(), nNodeIdSize-1);
// 	return true;
// }

void  CLayoutEditorDlg::OnObjectDeleteInd(IObject* p)
{
	if (m_pCurSelectObject == p)
	{
		SelectObject(NULL);
	}
}

void  CLayoutEditorDlg::OffsetWindowDrawRect(int x, int y)
{
	CRect rc = m_rcWindowDraw;
	::OffsetRect(&rc, x, y);
	SetWindowDrawRect(&rc);

	{
		TCHAR szOffset[128] = _T("");
		_stprintf(szOffset, _T("Offset: %d, %d  Width: %d, %d"), 
			m_rcWindowDraw.left, m_rcWindowDraw.top,
			m_rcWindowDraw.right-m_rcWindowDraw.left,
			m_rcWindowDraw.bottom-m_rcWindowDraw.top);
		g_pGlobalData->SetStatusText2(szOffset);

	}
	// 
	//                 CRect rcInvalidate;
	//                 UnionRect(&rcInvalidate, &m_rcWindowDraw, &rcOld);
	//                 ::InflateRect(&rcInvalidate, 2,2);  // 窗口被选中时的虚线
	//                 ::InvalidateRect(GetHWND(), &rcInvalidate, TRUE);

	Invalidate();
}

void CLayoutEditorDlg::OnMouseMove(UINT nFlags, CPoint point)
{
    if (nFlags & MK_LBUTTON)
    {
        switch (m_eMouseAction)
        {
        case MOUSE_ACTION_MOVE_WINDOW_DRAW_POS:
            {
                POINT  ptLast = m_ptLastMouseMove;
                POINT  ptNow = point;

                HDC hDC = ::GetDC(GetHWND());
                PrepareDC(hDC);
                ::DPtoLP(hDC, &ptLast, 1);
                ::DPtoLP(hDC, &ptNow, 1);
                UnPrepareDC(hDC);
                ::ReleaseDC(GetHWND(), hDC);

                int x = ptNow.x - ptLast.x;
                int y = ptNow.y - ptLast.y;

                OffsetWindowDrawRect(x, y);
            }
            break;
        }
    }

    m_ptLastMouseMove = point;
}
void CLayoutEditorDlg::OnLButtonDown(UINT nFlags, CPoint p)
{
	if (NULL == m_pWindow)
		return;

    ::SetCapture(GetHWND());
    if (GetFocus() != GetHWND())  // MDI Child View在失焦后有时点击仍然拿不到键盘消息，消息跑MainFrame中去了
    {
        ::SetFocus(GetHWND());
    }
    m_ptLastClick = p;

    switch (m_eMouseAction)
    {
    case MOUSE_ACTION_MOVE_WINDOW_DRAW_POS:
        {
            OnSetCursor(0,0,0);
        }
        break;

    default:
        {
            DP2LP(&p);

            POINT pt = p;
            if (!PtInRect(&m_rcWindowDraw, pt))
            {
                SelectObject(m_pWindow);
                return;
            }

            pt.x -= m_rcWindowDraw.left;
            pt.y -= m_rcWindowDraw.top;

            IObject* pObjSelect = m_pWindow->GetObjectByPos(m_pWindow, &pt, true);
            if (NULL == pObjSelect)
            {
                pObjSelect = m_pWindow;
            }

            this->SelectObject(pObjSelect);
        }
        break;
    }
}

void CLayoutEditorDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
    if (GetHWND() == GetCapture())
    {
        ReleaseCapture();
    }

    if (m_eMouseAction == MOUSE_ACTION_MOVE_WINDOW_DRAW_POS)
    {
        if (!Util::IsKeyDown(VK_SPACE))
        {
            m_eMouseAction = MOUSE_ACTION_NONE;
        }
        OnSetCursor(NULL, 0, 0);
    }
}
void CLayoutEditorDlg::OnKillFocus(IObject* pNewFocusObj)
{
    OnCancelMode();
}
void CLayoutEditorDlg::OnCancelMode()
{
    if (GetHWND() == GetCapture())
    {
        ReleaseCapture();

        if (m_eMouseAction == MOUSE_ACTION_MOVE_WINDOW_DRAW_POS)
        {
            m_eMouseAction = MOUSE_ACTION_NONE;
            OnSetCursor(NULL, 0, 0);
        }  
    }
}

void CLayoutEditorDlg::OnRButtonUp(UINT nFlags, CPoint point)
{
	// 选中
	OnLButtonDown(nFlags, point);

	// 菜单
	if (NULL == m_pCurSelectObject)
		return;

	const UINT MENU_ID_PROPERTY = 1;
	const UINT MENU_ID_IMAGEBKG = 2;
	const UINT MENU_ID_TRANSBKG = 3;

	HMENU hMenu = CreatePopupMenu();
	::AppendMenu(hMenu, MF_STRING, MENU_ID_PROPERTY, _T("属性"));
	::AppendMenu(hMenu, MF_SEPARATOR, 0, 0);
	::AppendMenu(hMenu, MF_STRING, MENU_ID_IMAGEBKG, _T("效果图"));
	::AppendMenu(hMenu, MF_STRING, MENU_ID_TRANSBKG, _T("透明背景图"));

	GetCursorPos(&point);
	int nRet = TrackPopupMenu(hMenu, TPM_RETURNCMD, point.x, point.y, 0, GetHWND(), NULL);
	DestroyMenu(hMenu);

	if (0 == nRet)
		return;

	switch (nRet)
	{
	case MENU_ID_PROPERTY:
		{
			m_PropertyHandler.AttachObject(m_pSkin, m_pCurSelectObject);
		}
		break;
	case MENU_ID_IMAGEBKG:
		{
			CFileDialog dlg(TRUE);
			if (IDOK == dlg.DoModal())
			{
				if (m_hBkBrush)
					DeleteObject(m_hBkBrush);

				m_hBkBrush = createImageBrush(dlg.m_szFileName);
				Invalidate();
			}
		}
		break;
	case MENU_ID_TRANSBKG:
		{
			if (m_hBkBrush)
				DeleteObject(m_hBkBrush);
			m_hBkBrush = createTransparentBrush();
			Invalidate();
		}
	}
}

void CLayoutEditorDlg::SelectObject(IObject* pObject)
{
	if (m_pCurSelectObject == pObject)
		return;

	while (pObject)
	{
		// 同步树控件中的选择项
		if (g_pGlobalData->m_pProjectTreeDialog->SelectLayoutObjectNode(pObject))
			break;
		
		// 有可能这个控件是UI内部创建的，不在XML中配置
		pObject = pObject->GetParentObject();
	}
	SetCurSelectObject(pObject);

	// 同步属性控件中的选择项
	m_PropertyHandler.AttachObject(m_pSkin, m_pCurSelectObject);

    Invalidate();
	UpdateTitle();
}

void CLayoutEditorDlg::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
    if (VK_SPACE == nChar && 
        m_eMouseAction == MOUSE_ACTION_NONE && 
        !Util::IsKeyDown(VK_LBUTTON))    
    {
        m_eMouseAction = MOUSE_ACTION_MOVE_WINDOW_DRAW_POS;
        OnSetCursor(NULL, 0, 0);
    }
	else if (VK_LEFT == nChar)
	{
		OffsetWindowDrawRect(-1, 0);
	}
	else if (VK_RIGHT == nChar)
	{
		OffsetWindowDrawRect(1, 0);

	}
	else if (VK_UP == nChar)
	{
		OffsetWindowDrawRect(0, -1);

	}
	else if (VK_DOWN == nChar)
	{
		OffsetWindowDrawRect(0, 1);
	}
}
void CLayoutEditorDlg::OnKeyUp( UINT nChar, UINT nRepCnt, UINT nFlags )
{
    if (m_eMouseAction == MOUSE_ACTION_MOVE_WINDOW_DRAW_POS)
    {
        if (!Util::IsKeyDown(MK_LBUTTON))
        {
            m_eMouseAction = MOUSE_ACTION_NONE;
            OnSetCursor(NULL, 0, 0);
        }
    }
}

BOOL CLayoutEditorDlg::OnSetCursor(HWND wnd, UINT nHitTest, UINT message)
{
     HCURSOR hCursor = ::LoadCursor(NULL, IDC_ARROW);
     if (m_eMouseAction == MOUSE_ACTION_MOVE_WINDOW_DRAW_POS)
     {
         if(Util::IsKeyDown(MK_LBUTTON))    
         {
              if (!m_hCursorHandPress)
                 m_hCursorHandPress = ::LoadCursor(_Module.m_hInst, MAKEINTRESOURCE(IDC_CURSOR_HAND_PRESS));

             hCursor = m_hCursorHandPress;
         }
         else
         {
             if (!m_hCursorHand)
                 m_hCursorHand = ::LoadCursor(_Module.m_hInst, MAKEINTRESOURCE(IDC_CURSOR_HAND));

             hCursor = m_hCursorHand;
         }         
     }
     ::SetCursor(hCursor);

    return TRUE;
}

BOOL CLayoutEditorDlg::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
    if (Util::IsKeyDown(VK_MENU))
    {
        int nOldZoom = m_nZoom;
        if (zDelta > 0)
        {
             m_nZoom += 50;

            if (m_nZoom > 1600)
                m_nZoom = 1600;
        }
        else
        {
            m_nZoom -= 50;
           
            if (m_nZoom < 100)
                m_nZoom = 100; 
        }

        if (m_nZoom != nOldZoom)
        {
            TCHAR szText[32] = _T("");
            _stprintf(szText, _T("Zoom: %d%%"), m_nZoom);
            g_pGlobalData->SetStatusText2(szText);

            Invalidate();
        }
    }
	else
	{
		bool ctrldown = Util::IsKeyDown(VK_CONTROL);

		int old = m_nWindowRenderAlpha;
		if (zDelta > 0)
		{
			if (ctrldown)
			{
				m_nWindowRenderAlpha = 255;
			}
			else
			{
				m_nWindowRenderAlpha += 10;

				if (m_nWindowRenderAlpha > 255)
					m_nWindowRenderAlpha = 255;
			}
		}
		else
		{
			if (ctrldown)
			{
				m_nWindowRenderAlpha = 0;
			}
			else
			{
				m_nWindowRenderAlpha -= 10;

				if (m_nWindowRenderAlpha < 0)
					m_nWindowRenderAlpha = 0;
			}
		}
		if (old !=  m_nWindowRenderAlpha)
		{
			Invalidate();
		}

	}
    return TRUE;
}

// MDI失焦，鼠标点击了其它MDI窗口
void CLayoutEditorDlg::OnMDIActivate(bool bActivate)
{
	if (!bActivate)	
	{
		SelectObject(NULL);
	}
}

long*  CLayoutEditorDlg::GetKey()
{
	return (long*)m_pWindow;
}

void  CLayoutEditorDlg::PrepareDC(HDC hDC)
{
    SaveDC(hDC);
    ::SetMapMode(hDC, MM_ISOTROPIC);
    ::SetViewportExtEx(hDC, m_nZoom, m_nZoom, NULL);
    ::SetWindowExtEx(hDC, 100, 100, NULL);
}
void  CLayoutEditorDlg::UnPrepareDC(HDC hDC)
{
    RestoreDC(hDC, -1);
}

// 查找容器对象用于插入
IObject*  CLayoutEditorDlg::GetContainerObject(IObject* pCurObj)
{
    if (NULL == pCurObj)
        return NULL;

    IObject* pContainerObj = pCurObj;
    while (1)
    {
        IObjectDescription* pDesc = pContainerObj->GetDescription();
        if (pDesc)
        {

            OBJ_TYPE eObjType = pDesc->GetMajorType();
            if (eObjType == OBJ_PANEL || eObjType == OBJ_WINDOW)
                break;
        }
        pContainerObj = pContainerObj->GetParentObject();
    }
    return pContainerObj;
}

void CLayoutEditorDlg::OnDropTargetEvent(DROPTARGETEVENT_TYPE eType, DROPTARGETEVENT_DATA* pData)
{
    UINT  nLayoutItemDragCF = GetToolBoxItemDragClipboardFormat();  // 拖拽格式
    switch (eType)
    {
    case _Drop:
        {
            if (NULL == pData->pDataObj)
                break;

            if (NULL == m_pCurSelectObject)
                break;
            
            FORMATETC etc = 
            {
                nLayoutItemDragCF,
                NULL, DVASPECT_CONTENT, 0, TYMED_HGLOBAL
            };

            STGMEDIUM medium = {0};
            medium.tymed = TYMED_HGLOBAL;
            pData->pDataObj->GetData(&etc, &medium);
            if (TYMED_HGLOBAL == medium.tymed)
            {
                IObjectDescription* pObjDesc = *(IObjectDescription**)GlobalLock(medium.hGlobal);
                ICommand*  pCommand = NULL;

				POINT  pt = {pData->pt.x, pData->pt.y};
				::ScreenToClient(GetHWND(), &pt);
				DP2LP(&pt);
				pt.x -= m_rcWindowDraw.left;
				pt.y -= m_rcWindowDraw.top;

				if (m_pCurSelectObject)
				{
					RECT rcParent;
					m_pCurSelectObject->GetClientRectInWindow(&rcParent);

					pt.x -= rcParent.left;
					pt.y -= rcParent.top;
				}
				pCommand = AddObjectCommand::CreateInstance(m_pSkin, m_pCurSelectObject, pObjDesc, pt.x, pt.y);
                UE_ExecuteCommand(pCommand);

                GlobalUnlock(medium.hGlobal);
            }
            ::ReleaseStgMedium(&medium);
        }
        break;

    case _DragOver:
    case _DragEnter:
        {
            (*pData->pdwEffect) = DROPEFFECT_NONE;
            if (NULL == pData->pDataObj)
                break;

            FORMATETC etc = 
            {
                nLayoutItemDragCF,
                NULL, DVASPECT_CONTENT, 0, TYMED_HGLOBAL
            };
            
            if (FAILED(pData->pDataObj->QueryGetData(&etc)))
                break;

            POINT pt = {pData->pt.x, pData->pt.y};
            ::ScreenToClient(GetHWND(), &pt);
            DP2LP(&pt);

            if (!PtInRect(&m_rcWindowDraw, pt))
                break;

            pt.x -= m_rcWindowDraw.left;
            pt.y -= m_rcWindowDraw.top;

            IObject* pObjSelect = m_pWindow->GetObjectByPos(m_pWindow, &pt, true);
            if (NULL == pObjSelect)
            {
                pObjSelect = m_pWindow;
            }

            IObject* pContainerObj = GetContainerObject(pObjSelect);
            if (!pContainerObj)
                break;
            this->SelectObject(pContainerObj);
			(*pData->pdwEffect) = DROPEFFECT_MOVE;
        }
        break;

    case _DragLeave:
        break;
    }
}

void  CLayoutEditorDlg::SetCurSelectObject(IObject*  pSelectObj)
{
    if (m_pCurSelectObject == pSelectObj)
        return;

    m_pCurSelectObject = pSelectObj;
    UpdateRectTracker();
}
void  CLayoutEditorDlg::SetWindowDrawRect(RECT* prc)
{
    CopyRect(&m_rcWindowDraw, prc);
    UpdateRectTracker();
}
void  CLayoutEditorDlg::UpdateRectTracker()
{
    if (!m_pCurSelectObject)
    {
		m_recttracker.Update(NULL, 0);
		return;
	}

	CRect  rc;
	m_pCurSelectObject->GetWindowRect(&rc);
	::OffsetRect(&rc, m_rcWindowDraw.left, m_rcWindowDraw.top);

	// 目前只支持canvas布局下的控件移动
	UINT nMask = 0;
	if (m_pCurSelectObject->GetParentObject())
	{
		IObject*  pParent = m_pCurSelectObject->GetParentObject();
		IPanel*  pPanel = (IPanel*)pParent->QueryInterface(__uuidof(IPanel));
		if (pPanel)
		{
			ILayout*  pLayout = pPanel->GetLayout();
			if (pLayout->GetLayoutType() == UI::LAYOUT_TYPE_CANVAS)
			{
				nMask |= recttrackermask_sizemove;
			}
		}
	}

	// 容器不支持中间拖拽
	if (m_pCurSelectObject->GetChildObject())
		nMask |= recttrackermask_iscontainer;

	m_recttracker.Update(&rc, nMask);
}

void  CLayoutEditorDlg::UpdateTitle()
{
	HWND  hWndParent = GetParent(GetHWND());
	if (!hWndParent)
		return;

	String  strTitle(_T("layout - "));

	if (m_pSkin)
	{
		strTitle.append(m_pSkin->GetName());
		strTitle.append(_T(" - "));
	}
	if (m_pWindow)
	{
		strTitle.append(m_pWindow->GetId());
		strTitle.append(_T(" : "));
	}

	if (m_pCurSelectObject)
	{
		strTitle.append(m_pCurSelectObject->GetId());
	}

	::SetWindowText(hWndParent, strTitle.c_str());
}

void  CLayoutEditorDlg::OnRectTrackerDragStart()
{
    if (!m_pCurSelectObject)
        return;

    m_pCurSelectObject->GetParentRect(&m_rectObjectSave);
}
void  CLayoutEditorDlg::OnRectTrackerDragEnd()
{
	if (!m_pCurSelectObject)
		return;

	CRect  rcNow;
	m_pCurSelectObject->GetParentRect(&rcNow);
	if (rcNow != m_rectObjectSave)
	{
		UE_ExecuteCommand(SetObjectPosCommand::CreateInstance(
				m_pCurSelectObject, &m_rectObjectSave, &rcNow));

	}
    m_rectObjectSave.SetRectEmpty();
}

void  CLayoutEditorDlg::DP2LP(POINT* p)
{
    if (!p)
        return;

    HDC hDC = GetDC(GetHWND());
    PrepareDC(hDC);
    DPtoLP(hDC, (POINT*)p, 1);
    UnPrepareDC(hDC);
    ReleaseDC(GetHWND(), hDC);
}

// 拖拽移动控件
void  CLayoutEditorDlg::OnRectTrackerDrag(RectTrackerHit eHit, int xDelta, int yDelta)
{
    if (!m_pCurSelectObject)
        return;

    IObject*  pParent = m_pCurSelectObject->GetParentObject();
    if (!pParent)
        return;

    CRect rcTry = m_rectObjectSave;
	switch (eHit)
	{
	case hitTopLeft:
		rcTry.left += xDelta;
		rcTry.top += yDelta;
		break;

	case hitTop:
		rcTry.top += yDelta;
		break;

	case hitTopRight:
		rcTry.right += xDelta;
		rcTry.top += yDelta;
		break;

	case hitLeft:
		rcTry.left += xDelta;
		break;

	case hitRight:
		rcTry.right += xDelta;
		break;

	case hitBottomLeft:
		rcTry.left += xDelta;
		rcTry.bottom += yDelta;
		break;

	case hitBottom:
		rcTry.bottom += yDelta;
		break;

	case hitBottomRight:
		rcTry.right += xDelta;
		rcTry.bottom += yDelta;
		break;

	case hitMiddle:
		rcTry.OffsetRect(xDelta, yDelta);
		break;

	default:
		return;
	}
	rcTry.NormalizeRect();

    // 将位置限制到父对象范围之内 

	CRect  rcParent;
	pParent->GetObjectClientRect(&rcParent);

	if (eHit == hitMiddle)
    {
        int xOffset = 0;
        int yOffset = 0;
        
        if (rcTry.right > rcParent.right)
            xOffset -= rcTry.right-rcParent.right;
        else if (rcTry.left < rcParent.left)
            xOffset += rcParent.left - rcTry.left;

        if (rcTry.bottom > rcParent.bottom)
            yOffset -= rcTry.bottom-rcParent.bottom;
        else if (rcTry.top < rcParent.top)
            yOffset += rcParent.top - rcTry.top;

        rcTry.OffsetRect(xOffset, yOffset);
    }
	else
	{
		if (rcTry.right > rcParent.right)
			 rcTry.right = rcParent.right;
		else if (rcTry.left < rcParent.left)
			rcTry.left = rcParent.left;

		if (rcTry.bottom > rcParent.bottom)
			rcTry.bottom = rcParent.bottom;
		else if (rcTry.top< rcParent.top)
			rcTry.top = rcParent.top;
	}
    m_pCurSelectObject->SetObjectPos(&rcTry, 0);

    UE_RefreshUIWindow(m_pCurSelectObject);
    InvalidateRect(GetHWND(), NULL, TRUE);

    // 更新recttracker位置
    UpdateRectTracker();
}
