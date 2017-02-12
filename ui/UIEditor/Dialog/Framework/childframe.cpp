#include "stdafx.h"
#include "childframe.h"
#include "Dialog\Framework\toolbar.h"
#include "Dialog\Framework\mainframe.h"

// {03E78E3A-0412-4370-ADBA-9750BF3B92CC}
const GUID IID_IMDIClietnView = 
{ 0x3e78e3a, 0x412, 0x4370, { 0xad, 0xba, 0x97, 0x50, 0xbf, 0x3b, 0x92, 0xcc } };

// {1875BBE6-7EBE-4f43-91FB-6D4FF005B1F2}
const GUID IID_IExplorerMDIClientView = 
{ 0x1875bbe6, 0x7ebe, 0x4f43, { 0x91, 0xfb, 0x6d, 0x4f, 0xf0, 0x5, 0xb1, 0xf2 } };

// {D2DCF52B-3F4D-4ba6-B6C9-1B417B231A21}
const GUID IID_ILogMDIClientView = 
{ 0xd2dcf52b, 0x3f4d, 0x4ba6, { 0xb6, 0xc9, 0x1b, 0x41, 0x7b, 0x23, 0x1a, 0x21 } };

// {EE098D3B-DD70-4575-A1F7-86473A1263C6}
const GUID IID_ILayoutMDIClientView = 
{ 0xee098d3b, 0xdd70, 0x4575, { 0xa1, 0xf7, 0x86, 0x47, 0x3a, 0x12, 0x63, 0xc6 } };

// {4036E387-4748-4d48-B751-4D0DC638D468}
static const GUID IID_IMenuMDIClientView = 
{ 0x4036e387, 0x4748, 0x4d48, { 0xb7, 0x51, 0x4d, 0xd, 0xc6, 0x38, 0xd4, 0x68 } };


CChildFrame::CChildFrame(CMainFrame* pMainFrame):m_pMainFrame(pMainFrame)
{
    //	m_bDirty = false;
    m_pClientView = NULL;
}

void CChildFrame::OnFinalMessage(HWND)
{
    if (m_pClientView)
    {
        m_pClientView->Destroy();
    }
    //	SAFE_DELETE(m_pClientView);
    delete this;
}

int CChildFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    SetMsgHandled(FALSE);
    return 0;
}

void CChildFrame::OnDestroy()
{
    if (m_pClientView)
    {
        long*  pKey = m_pClientView->GetKey();
        if (pKey)
        {
            g_pGlobalData->m_pCmdHistroyMgr->CloseElem(pKey);
            g_pGlobalData->m_pMainFrame->MDIActivateInd(pKey, false);
        }
    }
    m_pMainFrame->RemoveChildFrame(this);
    return;
}

void CChildFrame::SetClientView(IMDIClientView* pClientWnd)
{
    if (NULL == pClientWnd)
        return;

    SAFE_DELETE(m_pClientView);
    m_pClientView = pClientWnd;

    m_hWndClient = pClientWnd->GetHWND();

    this->UpdateTitle();
    this->UpdateLayout();

    // 切换到当前窗口的
    g_pGlobalData->m_pMainFrame->MDIActivateInd(m_pClientView->GetKey(), true);
}

//////////////////////////////////////////////////////////////////////////
//
// 设置标题栏
//
void CChildFrame::UpdateTitle()
{
    if (NULL == m_pClientView)
        return;

    ISkinRes* pSkin = m_pClientView->GetSkinRes();
    UI_RESOURCE_TYPE eType = m_pClientView->GetResourceType();
    if (NULL == pSkin || eType == UI_RESOURCE_NULL)
        return;

    String strTitle;

    switch(eType)
    {
    case UI_RESOURCE_IMAGE:
        strTitle.append(_T("image builder - "));
        break;

    case UI_RESOURCE_COLOR:
        strTitle.append(_T("color builder - "));
        break;

    case UI_RESOURCE_FONT:
        strTitle.append(_T("font builder - "));
        break;

    case UI_RESOURCE_LAYOUT:
    case UI_RESOURCE_MENU:
        return;  // 修改为由窗口自己设置

    case UI_RESOURCE_STYLE:
        strTitle.append(_T("style builder - "));
        break;

    case UI_RESOURCE_I18N:
        strTitle.append(_T("i18n builder - "));
        break;
    }

    TCHAR* pszName = NULL;
    int    nSize = 0;

    strTitle.append(pSkin->GetName());

    this->SetWindowText(strTitle.c_str());
}

LRESULT CChildFrame::OnForwardMsg(UINT, WPARAM, LPARAM lParam, BOOL&)
{
    LPMSG pMsg = (LPMSG)lParam;

    if(CMDIChildWindowImpl<CChildFrame>::PreTranslateMessage(pMsg))
        return TRUE;

    return FALSE/*m_view.PreTranslateMessage(pMsg)*/;
}

LRESULT CChildFrame::OnMDIActivate(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    SetMsgHandled(FALSE);
    if (m_pClientView)
    {
        bool  bActivate = false;
        if ((HWND)lParam == m_hWnd)
            bActivate = true;
        else if ((HWND)wParam == m_hWnd)
            bActivate = false;
        else
            return 0;

        m_pClientView->OnMDIActivate(bActivate);
        g_pGlobalData->m_pMainFrame->MDIActivateInd(
			m_pClientView->GetKey(), bActivate);
    }

    return 0;
}