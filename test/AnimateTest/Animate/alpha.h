#pragma once
#include "animate_base.h"

// Î»ÒÆ¶¯»­

class AlphaAnimate : public Animate
{
public:
	virtual void Init() override
	{
		m_pPanel = (UI::IPanel*)m_pWindow->FindObject(L"panel");
		UI::LayerAnimateParam param = { 0 };
		param.bBlock = true;
		m_pPanel->SetOpacity(0, param);
		m_pPanel->SetOpacity(255);
	}

	virtual void Release() override
	{

	}

private:
	UI::IPanel*  m_pPanel;
};