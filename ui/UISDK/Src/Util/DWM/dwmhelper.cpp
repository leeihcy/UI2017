#include "stdafx.h"
#include "dwmhelper.h"

DwmHelper::DwmHelper()
{
    pDwmExtendFrameIntoClientArea = NULL;
    pDwmEnableBlurBehindWindow = NULL;
    pDwmIsCompositionEnabled = NULL;
    pDwmGetWindowAttribute = NULL;
    pDwmSetWindowAttribute = NULL;
    pDwmDefWindowProc = NULL;

    m_hModule = LoadLibrary(_T("dwmapi.dll"));
    if (NULL == m_hModule)
        return;

    pDwmExtendFrameIntoClientArea = (funcDwmExtendFrameIntoClientArea)GetProcAddress(m_hModule, "DwmExtendFrameIntoClientArea");
    pDwmEnableBlurBehindWindow = (funcDwmEnableBlurBehindWindow)GetProcAddress(m_hModule, "DwmEnableBlurBehindWindow");
    pDwmIsCompositionEnabled = (funcDwmIsCompositionEnabled)GetProcAddress(m_hModule, "DwmIsCompositionEnabled");
    pDwmGetWindowAttribute = (funcDwmGetWindowAttribute)GetProcAddress(m_hModule, "DwmGetWindowAttribute");
    pDwmSetWindowAttribute = (funcDwmSetWindowAttribute)GetProcAddress(m_hModule, "DwmSetWindowAttribute");
	pDwmDefWindowProc = (funcDwmDefWindowProc)GetProcAddress(m_hModule, "DwmDefWindowProc");
}
DwmHelper::~DwmHelper()
{
    if (m_hModule)
    {
        FreeLibrary(m_hModule);
        m_hModule = NULL;
    }
}

DwmHelper*  DwmHelper::GetInstance()
{
    static DwmHelper  dwm;
    return &dwm;
}

bool  DwmHelper::IsEnable()
{ 
    if (NULL == m_hModule)
        return false;

    if (NULL == pDwmIsCompositionEnabled)
        return false;

    BOOL b = FALSE;
    pDwmIsCompositionEnabled(&b);

    return b ? true:false;
};

// 开启/禁用窗口显示、隐藏、最化小、还原时的动画效果
void UI::DwmHelper::EnableTransition(HWND hWnd, bool b)
{
    if (!hWnd)
        return;
    if (!pDwmSetWindowAttribute)
        return;

    BOOL bEnable = b?FALSE:TRUE;
    pDwmSetWindowAttribute(hWnd, DWMWA_TRANSITIONS_FORCEDISABLED,
        &bEnable, sizeof(bEnable));
}