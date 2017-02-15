#pragma once
#include "animate_base.h"

// Î»ÒÆ¶¯»­

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
		param.SetDuration(300);
		param.finishCallback = 
		[pPanel](const UI::LayerAnimateFinishParam& param) 
		{
		};
		pPanel->RotateYBy(90, &param);
	}

	virtual void Release() override
	{
	}
};

