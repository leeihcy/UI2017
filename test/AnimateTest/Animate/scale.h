#pragma once
#include "animate_base.h"

// Î»ÒÆ¶¯»­

class ScaleAnimate : public Animate
{
public:
	virtual int  Type() override {
		return AnimateType_Scale;
	}

	virtual void Init() override
	{
		UI::IPanel* pPanel = (UI::IPanel*)m_pWindow->FindObject(L"panel");
	}

	virtual void Action() override
	{
		UI::IPanel* pPanel = (UI::IPanel*)m_pWindow->FindObject(L"panel");

		UI::LayerAnimateParam param;
		param.SetDuration(500);
		param.SetFinishCallback( [pPanel](const UI::LayerAnimateFinishParam& param) 
		{
			pPanel->ScaleTo(1, 1, DefaultLayerAnimateParam);
		});
		pPanel->ScaleTo(3,3, &param);
	}

	virtual void Release() override
	{
	}
};

