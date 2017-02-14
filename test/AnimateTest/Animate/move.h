#pragma once
#include "animate_base.h"

// Î»ÒÆ¶¯»­

class MoveLeftToRightAnimate : public Animate
{
public:
	virtual void Init() override
	{
		UI::IPanel* pPanel = (UI::IPanel*)m_pWindow->FindObject(L"panel");

		UI::LayerAnimateParam param;
		param.finishCallback = 
		[pPanel](const UI::LayerAnimateFinishParam& param) 
		{
// 			if (param.endreason == UIA::ANIMATE_END_NORMAL)
// 				pPanel->SetTranslate(0, 0, 0, nullptr);
// 			else
// 				pPanel->SetTranslate(0, 0, 0);
		};
		pPanel->SetTranslate(UI::ScaleByDpi(350), 0, 0, &param);
	}

	virtual void Release() override
	{
		UI::IPanel* pPanel = (UI::IPanel*)m_pWindow->FindObject(L"panel");
		pPanel->SetTranslate(0, 0, 0);
	}
};