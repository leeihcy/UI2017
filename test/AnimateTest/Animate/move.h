#pragma once
#include "animate_base.h"

// Î»ÒÆ¶¯»­

class TranslateLeftToRightAnimate : public Animate
{
public:
	virtual void Init() override
	{
		UI::IPanel* pPanel = (UI::IPanel*)m_pWindow->FindObject(L"panel");

		UI::LayerAnimateParam param;
		param.finishCallback =  
		[pPanel](const UI::LayerAnimateFinishParam& param) 
		{
			if (param.endreason == UIA::ANIMATE_END_NORMAL) 
				pPanel->TranslateXTo(-dpi(350), DefaultLayerAnimateParam);
			else
				pPanel->TranslateXTo(-dpi(350));
		};
		pPanel->TranslateTo(dpi(350), 0, 0, &param);
	}

	virtual void Release() override
	{
		// Í£Ö¹¶¯»­
		UI::IPanel* pPanel = (UI::IPanel*)m_pWindow->FindObject(L"panel");
		pPanel->TranslateTo(0, 0, 0);
	}
};


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
		param.finishCallback =
			[pPanel](const UI::LayerAnimateFinishParam& param)
		{
			RECT rcParent;
			pPanel->GetParentRect(&rcParent);
			if (param.endreason == UIA::ANIMATE_END_NORMAL)
				pPanel->TranslateToParent(dpi(10), rcParent.top, DefaultLayerAnimateParam);
			else
				pPanel->TranslateToParent(dpi(10), rcParent.top);
		};
		pPanel->TranslateTo(dpi(350), 0, 0, &param);
	}

	virtual void Release() override
	{
		// Í£Ö¹¶¯»­
		UI::IPanel* pPanel = (UI::IPanel*)m_pWindow->FindObject(L"panel");
		pPanel->TranslateTo(0, 0, 0);
	}
};