#include "stdafx.h"
#include "gesturehelper.h"

namespace UI
{
GestureHelper*  GetGestureInstance()
{
    static GestureHelper dwm;
    return &dwm;
}

GestureHelper::GestureHelper()
{
    pSetGestureConfig = NULL;
    pGetGestureInfo = NULL;
    pCloseGestureInfoHandle = NULL;
	pBeginPanningFeedback = NULL;
	pEndPanningFeedback = NULL;
	pUpdatePanningFeedback = NULL;

    m_hModule = LoadLibrary(_T("User32.dll"));
    if (NULL == m_hModule)
        return;

	m_hModuleUxTheme = LoadLibrary(_T("UxTheme.dll"));
	if (NULL == m_hModuleUxTheme)
	{
		FreeLibrary(m_hModule);
		return;
	}

    pSetGestureConfig = (pfnSetGestureConfig)
			GetProcAddress(m_hModule, "SetGestureConfig");
    pGetGestureInfo = (pfnGetGestureInfo)
			GetProcAddress(m_hModule, "GetGestureInfo");
    pCloseGestureInfoHandle = (pfnCloseGestureInfoHandle)
			GetProcAddress(m_hModule, "CloseGestureInfoHandle");
	pBeginPanningFeedback = (pfnBeginPanningFeedback)
		GetProcAddress(m_hModuleUxTheme, "BeginPanningFeedback");
	pEndPanningFeedback = (pfnEndPanningFeedback)
		GetProcAddress(m_hModuleUxTheme, "EndPanningFeedback");
	pUpdatePanningFeedback = (pfnUpdatePanningFeedback)
		GetProcAddress(m_hModuleUxTheme, "UpdatePanningFeedback");
}

GestureHelper::~GestureHelper()
{
    if (m_hModule)
    {
        FreeLibrary(m_hModule);
        m_hModule = NULL;
    }
	if (m_hModuleUxTheme)
	{
		FreeLibrary(m_hModuleUxTheme);
		m_hModuleUxTheme = NULL;
	}
}

bool GestureHelper::IsValid()
{
    return pSetGestureConfig ? true:false;
}



}