#pragma once
#include "animate_base.h"

// Î»ÒÆ¶¯»­

class MoveLeftToRightAnimate : public Animate
{
public:
	virtual int  Type() override {
		return AnimateType_MoveLeftToRight;
	}

	virtual void Init() override
	{
		
	}

	virtual void Action() override
	{
		UI::IPanel* pPanel = (UI::IPanel*)m_pWindow->FindObject(L"panel");

		UI::LayerAnimateParam param;
		param.SetFinishCallback(
			[pPanel](const UI::LayerAnimateFinishParam& param)
		{
			RECT rcParent;
			pPanel->GetParentRect(&rcParent);
			if (param.endreason == UIA::ANIMATE_END_NORMAL)
				pPanel->TranslateToParent(dpi(200), rcParent.top, DefaultLayerAnimateParam);
			else
				pPanel->TranslateToParent(dpi(200), rcParent.top);
		});
		pPanel->TranslateTo(dpi(150), 0, 0, &param);
	}

	virtual void Release() override
	{
		// Í£Ö¹¶¯»­
		UI::IPanel* pPanel = (UI::IPanel*)m_pWindow->FindObject(L"panel");
		pPanel->TranslateTo(0, 0, 0);
	}
};