// UIBuilder.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "Other\UIEditor.h"
#include "Dialog\Framework\MainFrame.h"


// 全局变量:
CAppModule      _Module;
CGlobalData*    g_pGlobalData = NULL;

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	_CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
	
	g_pGlobalData = new CGlobalData;
	if (!g_pGlobalData->Init())
	{
		SAFE_DELETE(g_pGlobalData);
		return 0;
	}

	_Module.Init(NULL, hInstance);
	AtlAxWinInit();   // for webbrowser
	CMessageLoop  theLoop;
	_Module.AddMessageLoop(&theLoop);

	{
		// 窗口
		CMainFrame  frameWnd;
		frameWnd.Create( NULL, NULL, _T("UI Editor"), WS_OVERLAPPEDWINDOW );
		g_pGlobalData->m_hWndMainFrame = frameWnd.m_hWnd;
        frameWnd.HandleCmdLine(lpCmdLine);

		frameWnd.ShowWindow(SW_SHOWMAXIMIZED);
        //frameWnd.ShowWindow(SW_SHOW);

        g_pGlobalData->m_pMyUIApp->MsgHandleLoop();
// 		theLoop.Run();
	}

	SAFE_DELETE(g_pGlobalData);

	_Module.RemoveMessageLoop();
	_Module.Term();
	return (int) 0;
}

