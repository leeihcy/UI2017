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
		param.SetFinishCallback(
		[pPanel](const UI::LayerAnimateFinishParam& param)
		{
			pPanel->SetOpacity(255);
		});
		pPanel->SetOpacity(0, &param);
	}

	virtual void Release() override
	{

	}
};