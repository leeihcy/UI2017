#include "stdafx.h"
#include "statusbar.h"
#include "UICTRL\Inc\Interface\ilabel.h"

CStatusBar::CStatusBar() : IWindow(CREATE_IMPL_TRUE)
{
    m_pLabel = NULL;   
    m_nTimerId = 0;
}
CStatusBar::~CStatusBar()
{

}


void  CStatusBar::OnInitialize()
{
    IWindow::nvProcessMessage(GetCurMsg(), 0, false);

    m_pLabel = (ILabel*)FindObject(_T("statustext"));
}

void  CStatusBar::OnEraseBkgnd(IRenderTarget* pRenderTarget)
{
    CRect  rc;
    this->GetClientRectInObject(&rc);

    UI::Color cBack(241,237,237,255);
    UI::Color cTopLine(145,145,145,255);
    UI::Color cBorder(0xFFFFFFFF);

    rc.top++;
    pRenderTarget->Rectangle(&rc, &cBorder, &cBack, 1, false);
    
    rc.top--;
    IRenderPen* pPen = pRenderTarget->CreateSolidPen(1, &cTopLine);
    pRenderTarget->DrawLine(rc.left, rc.top, rc.right, rc.top, pPen);
    SAFE_RELEASE(pPen);
}

void  CStatusBar::SetStatusText(LPCTSTR  szText)
{
    if (!m_pLabel)
        return;

    m_pLabel->SetText(szText);
}
void  CStatusBar::SetStatusText2(LPCTSTR  szText, int nDuration)
{
    if (!m_pLabel)
        return;

    if (0 != m_nTimerId)
    {
        KillTimer(GetHWND(), m_nTimerId);
        m_nTimerId = 0;
    }
    m_pLabel->SetText(szText);
    m_nTimerId = ::SetTimer(GetHWND(), 1, nDuration, NULL);
}

void  CStatusBar::OnTimer(UINT_PTR nIDEvent, LPARAM lParam)
{
    KillTimer(GetHWND(), nIDEvent);
    m_nTimerId = 0;

    m_pLabel->SetText(_T("Ready"));
}