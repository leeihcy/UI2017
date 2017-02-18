#pragma once
#include "animate_base.h"


class RotateXAnimate : public Animate
{
public:
	virtual int  Type() override {
		return AnimateType_RotateX;
	}

	virtual void Init() override
	{
		UI::IPanel* pPanel = (UI::IPanel*)m_pWindow->FindObject(L"panel");
	}

	virtual void Action() override
	{
		UI::IPanel* pPanel = (UI::IPanel*)m_pWindow->FindObject(L"panel");

		UI::LayerAnimateParam param;
		param.SetDuration(600);
		param.finishCallback = 
		[pPanel](const UI::LayerAnimateFinishParam& param) 
		{
		};
		pPanel->RotateXBy(360, &param);
	}

	virtual void Release() override
	{
	}
};

