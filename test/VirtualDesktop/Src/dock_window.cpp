#include "stdafx.h"
#include "dock_window.h"
#include "..\..\..\ui\UISDK\Src\Util\DWM\dwmhelper.h"

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
	m_pWindow->EnableGpuComposite(true);
	m_pWindow->Create(L"dock");

	 m_pWindow->ShowWindow();
}

BOOL DockWindow::OnWindowMessage(UINT msg, WPARAM wParam, LPARAM, LRESULT& lResult)
{
	if (msg == WM_DESTROY)
	{
		PostQuitMessage(0);
	}
	else if (msg == WM_SIZE)
	{
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

