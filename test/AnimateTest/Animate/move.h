#pragma once
#include "animate_base.h"

// Î»ÒÆ¶¯»­

class MoveLeftToRightAnimate : public Animate
{
public:
	virtual void Init() override
	{
		m_pPanel = (UI::IPanel*)m_pWindow->FindObject(L"panel");
		m_pPanel->EnableLayer(true);

		UI::LayerAnimateParam param = { 0 };
		m_pPanel->SetTranslate(300, 0, 0, param);
	}

	virtual void Release() override
	{

	}

private:
	UI::IPanel*  m_pPanel;
};