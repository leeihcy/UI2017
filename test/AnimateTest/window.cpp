#include "stdafx.h"
#include "window.h"
#include "Animate\animate_base.h"


LoginWindow::LoginWindow()
{
	m_pWindow = nullptr;
}


LoginWindow::~LoginWindow()
{
	if (m_pWindow)
		m_pWindow->Release();
}

void LoginWindow::Create(UI::ISkinRes* p)
{
	m_pWindow = UI::ICustomWindow::CreateInstance(p);
	m_pWindow->SetWindowMessageCallback(this);
	m_pWindow->EnableGpuComposite(true);
	m_pWindow->Create(L"login");

	m_pPanel = (UI::IPanel*)m_pWindow->FindObject(L"panel");

	

	srand(GetTickCount());

	int nResult = rand() % 2;
	if (nResult == 0)
	{
		m_pPanel->TranslateXTo(dpi(-200));
		m_pPanel->TranslateXTo(dpi(0), DefaultLayerAnimateParam);
	}
	else if (nResult == 1)
	{
		m_pPanel->SetOpacity(0);
		m_pPanel->SetOpacity(255, DefaultLayerAnimateParam);
	}

	m_pWindow->ShowWindow();
}

BOOL LoginWindow::OnWindowMessage(UINT msg, WPARAM wParam, LPARAM, LRESULT& lResult)
{
	if (msg == WM_DESTROY)
	{
		Animate::Quit();
		PostQuitMessage(0);
	}
	else if (msg == WM_MOUSEWHEEL)
	{

	}
	else if (msg == WM_NCLBUTTONDBLCLK)
	{
		
	}
	else if (msg == WM_KEYDOWN)
	{
		OnKeydown(wParam);
	}
	return FALSE;
}

void LoginWindow::Destroy()
{
	if (m_pWindow)
	{
		m_pWindow->DestroyWindow();
	}
}


void LoginWindow::ShowAnimate(long type)
{
	Animate* p = Animate::Create(type, m_pWindow);
	if (p)
		p->Action();
}

void LoginWindow::OnKeydown(UINT key)
{
	if (key >= '0' && key <= '9')
	{
		int index = key - '0';
		ShowAnimate(index);
	}
// 	switch (key)
// 	{
// 	case '1':
// 		ShowAnimate(AnimateType_CustomTimingFunction);
// 		break;
// 
// 	default:
//		break;
// 	}
}
