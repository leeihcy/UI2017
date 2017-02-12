// imagemergetools.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "imagemergetools.h"
#include "MainDlg.h"

CAppModule  _Module;

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	_Module.Init(NULL, hInstance);

// 	CMessageLoop theLoop;
// 	_Module.AddMessageLoop(&theLoop);

	CMainDlg dlg; 
	dlg.DoModal();

// 	dlg.Create(NULL);
// 	dlg.ShowWindow(SW_SHOW);
// 
// 	theLoop.Run();
// 	_Module.RemoveMessageLoop();
	_Module.Term();
	return 0;
}