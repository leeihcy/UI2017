#pragma once
#include "animate_base.h"

// Î»ÒÆ¶¯»­

class AlphaAnimate : public Animate
{
public:
	virtual int  Type() override {
		return AnimateType_Alpha;
	}

	virtual void Init() override
	{
	}

	virtual void Action() override
	{
		UI::IPanel*  pPanel = (UI::IPanel*)m_pWindow->FindObject(L"panel");

		UI::LayerAnimateParam param;
		param.SetEaseType(UIA::linear);
		param.SetDuration(800);
		param.SetFinishCallback(
		[pPanel](const UI::LayerAnimateFinishParam& param)
		{
			UI::LayerAnimateParam param2;
			param2.SetDuration(800);
			param2.SetEaseType(UIA::ease_in);
			pPanel->SetOpacity(255, &param2);
		});
		pPanel->SetOpacity(0, &param);
	}

	virtual void Release() override
	{

	}
};