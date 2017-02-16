#include "stdafx.h"
#include "dock_window.h"
#include "..\..\..\ui\UISDK\Src\Util\DWM\dwmhelper.h"
#include <dwmapi.h>
#pragma comment(lib, "Dwmapi.lib")

DockWindow::DockWindow()
{
	m_pWindow = nullptr;
}


DockWindow::~DockWindow()
{
	if (m_pWindow)
		m_pWindow->Release();
}

void DockWindow::Create(UI::ISkinRes* p)
{
	m_pWindow = UI::ICustomWindow::CreateInstance(p);
	m_pWindow->SetWindowMessageCallback(this);
	//m_pWindow->EnableGpuComposite(true);
	m_pWindow->Create(L"dock");

	

	HRESULT hr = S_OK;

	// Create and populate the Blur Behind structure
	DWM_BLURBEHIND bb = { 0 };

	// Enable Blur Behind and apply to the entire client area
	bb.dwFlags = DWM_BB_ENABLE;
	bb.fEnable = true;
	bb.hRgnBlur = NULL;

	// Apply Blur Behind
	// hr = DwmEnableBlurBehindWindow(m_pWindow->GetHWND(), &bb);
	 int a = 0;

	 MARGINS m = {300,30,30,30 };
	 DwmExtendFrameIntoClientArea(m_pWindow->GetHWND(), &m);

	 m_pWindow->ShowWindow();
}

BOOL DockWindow::OnWindowMessage(UINT msg, WPARAM wParam, LPARAM, LRESULT& lResult)
{
	if (msg == WM_DESTROY)
	{
		PostQuitMessage(0);
	}
	return FALSE;
}

void DockWindow::Destroy()
{
	if (m_pWindow)
	{
		m_pWindow->DestroyWindow();
	}
}

