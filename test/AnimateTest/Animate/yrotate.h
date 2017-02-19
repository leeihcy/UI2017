#pragma once
#include "animate_base.h"


class RotateYAnimate : public Animate
{
public:
	virtual int  Type() override {
		return AnimateType_RotateY;
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
		param.SetFinishCallback([pPanel](const UI::LayerAnimateFinishParam& param) 
		{
		});
		pPanel->RotateYBy(360, &param);
	}

	virtual void Release() override
	{
	}
};

