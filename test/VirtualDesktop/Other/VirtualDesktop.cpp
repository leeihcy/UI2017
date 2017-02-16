#include "stdafx.h"
#include "VirtualDesktop.h"
#include "..\Src\dock_window.h"

int APIENTRY _tWinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPTSTR    lpCmdLine,
	int       nCmdShow)
{
	UI::IUIApplication* pUIApp = NULL;
	UI::CreateUIApplication(&pUIApp);
	UICTRL_RegisterUIObject(pUIApp);

	pUIApp->EnableGpuComposite();

	// ¶¥¼¶×ÊÔ´
	TCHAR szResPath[MAX_PATH] = { 0 };
	UI::Util::PathInBin(L"VirtualDesktopSkin", szResPath);
	UI::ISkinRes* pRootSkinRes = pUIApp->LoadSkinRes(szResPath);

	{
		DockWindow m;
		m.Create(pRootSkinRes);
		pUIApp->MsgHandleLoop();
	}
	pUIApp->Release();

	return 0;
}