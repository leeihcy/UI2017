#include "stdafx.h"
#include "system_button.h"
#include "button_desc.h"
#include "../UISDK/Inc/Interface/iwindow.h"

UI_IMPLEMENT_INTERFACE(SystemButton, Button);

SystemButton::SystemButton(ISystemButton* p) : Button(p)
{
    p->SetDescription(SystemButtonDescription::Get());
    m_eButtonType = SYSTEM_BUTTON_TYPE_UNKNOWN;
}

void SystemButton::FinalRelease()
{
	SetMsgHandled(FALSE);

	if (m_eButtonType == SYSTEM_BUTTON_TYPE_MAXIMIZE ||
		m_eButtonType == SYSTEM_BUTTON_TYPE_RESTORE)
	{
		IWindowBase* pWnd = m_pIButton->GetWindowObject();
		if (pWnd)
		{
			pWnd->SizeChangedEvent().disconnect(this);
		}
	}
}

void  SystemButton::OnSerialize(SERIALIZEDATA* pData)
{
    __super::OnSerialize(pData);

    AttributeSerializerWrap as(pData, TEXT("SystemButton"));
    as.AddEnum(XML_SYSTEM_BUTTON_TYPE, *(long*)&m_eButtonType)
        ->AddOption(SYSTEM_BUTTON_TYPE_MINIMIZE, XML_SYSTEM_BUTTON_TYPE_MINIMIZE)
        ->AddOption(SYSTEM_BUTTON_TYPE_MAXIMIZE, XML_SYSTEM_BUTTON_TYPE_MAXIMIZE)
        ->AddOption(SYSTEM_BUTTON_TYPE_RESTORE, XML_SYSTEM_BUTTON_TYPE_RESTORE)
        ->AddOption(SYSTEM_BUTTON_TYPE_CLOSE, XML_SYSTEM_BUTTON_TYPE_CLOSE)
        ->SetDefault(SYSTEM_BUTTON_TYPE_UNKNOWN);
}

void SystemButton::virtualOnClicked()
{
    HWND hWnd = m_pIButton->GetHWND();
    if (!hWnd)
        return;

	if (!clicked.empty())
	{
		clicked.emit(m_pIButton);
		return;
	}

    switch (m_eButtonType)
    {
    case SYSTEM_BUTTON_TYPE_MINIMIZE:
		::PostMessage(hWnd, WM_SYSCOMMAND, SC_MINIMIZE, 0);
        break;

    case SYSTEM_BUTTON_TYPE_MAXIMIZE:
		::PostMessage(hWnd, WM_SYSCOMMAND, SC_MAXIMIZE, 0);
        break;

    case SYSTEM_BUTTON_TYPE_RESTORE:
		::PostMessage(hWnd, WM_SYSCOMMAND, SC_RESTORE, 0);
        break;

    case SYSTEM_BUTTON_TYPE_CLOSE:
		::PostMessage(hWnd, WM_SYSCOMMAND, SC_CLOSE, 0);
        break;
    }
}

void  SystemButton::OnInitialize2()
{
	HWND hWnd = m_pIButton->GetHWND();
	if (!hWnd)
		return;

	m_pIButton->nvProcessMessage(GetCurMsg(), 0, 0); 

    OBJSTYLE s = { 0 };
    s.tabstop = 1;
    m_pIButton->ModifyObjectStyle(0, &s);

    if (m_eButtonType == SYSTEM_BUTTON_TYPE_MINIMIZE)
    {
		long lStyle = GetWindowLong(hWnd, GWL_STYLE);
        lStyle |= WS_MINIMIZEBOX;
		SetWindowLong(hWnd, GWL_STYLE, lStyle);
    }
    else if (m_eButtonType == SYSTEM_BUTTON_TYPE_MAXIMIZE)
    {
		long lStyle = GetWindowLong(hWnd, GWL_STYLE);
        lStyle |= WS_MAXIMIZEBOX;
		SetWindowLong(hWnd, GWL_STYLE, lStyle);
    }
    
	if (m_eButtonType == SYSTEM_BUTTON_TYPE_MAXIMIZE ||
		m_eButtonType == SYSTEM_BUTTON_TYPE_RESTORE)
	{
		if (::IsZoomed(hWnd))
		{
			if (m_eButtonType == SYSTEM_BUTTON_TYPE_MAXIMIZE)
				m_pIButton->SetVisible(false);
		}
		else
		{
			if (m_eButtonType == SYSTEM_BUTTON_TYPE_RESTORE)
				m_pIButton->SetVisible(false);
		}

		IWindowBase* pWnd = m_pIButton->GetWindowObject();
		if (pWnd)
		{
			pWnd->SizeChangedEvent().connect(
				this, &SystemButton::on_window_size);
		}
	}
}

void SystemButton::on_window_size(long type)
{
	if (type == SIZE_RESTORED)
	{
		if (m_eButtonType == SYSTEM_BUTTON_TYPE_MAXIMIZE)
		{
			m_pIButton->SetVisible(true);
		}
		else if (m_eButtonType == SYSTEM_BUTTON_TYPE_RESTORE)
		{
			m_pIButton->SetVisible(false);
		}
	}
	else if (type == SIZE_MAXIMIZED)
	{
		if (m_eButtonType == SYSTEM_BUTTON_TYPE_MAXIMIZE)
		{
			m_pIButton->SetVisible(false);
		}
		else if (m_eButtonType == SYSTEM_BUTTON_TYPE_RESTORE)
		{
			m_pIButton->SetVisible(true);
		}
	}
	
}