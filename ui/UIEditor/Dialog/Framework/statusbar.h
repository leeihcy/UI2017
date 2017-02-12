#pragma once
#include "UISDK\Inc\Interface\iwindow.h"

namespace UI
{
    interface ILabel;
}
class CStatusBar : public IWindow
{
public:
    CStatusBar();
    ~CStatusBar();

    UI_BEGIN_MSG_MAP_Ixxx(CStatusBar)
        UIMSG_ERASEBKGND(OnEraseBkgnd)
        UIMSG_TIMER(OnTimer)
        UIMSG_INITIALIZE(OnInitialize)
    UI_END_MSG_MAP_CHAIN_PARENT(IWindow)

    void  SetStatusText(LPCTSTR  szText);
    void  SetStatusText2(LPCTSTR  szText, int nDuration=10000);

protected:
    void  OnInitialize();
    void  OnTimer(UINT_PTR nIDEvent, LPARAM lParam);
    void  OnEraseBkgnd(IRenderTarget* pRenderTarget);

private:
    ILabel*   m_pLabel;
    UINT_PTR  m_nTimerId;
};

