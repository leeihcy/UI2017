#include "stdafx.h"
#include "animate_base.h"
#include "custom_timing_function.h"
#include "move.h"
#include "alpha.h"

Animate*  Animate::s_pCurrentAnimate = nullptr;


Animate::~Animate()
{
	ClearStoryboard();
}

void  Animate::Quit()
{
	if (s_pCurrentAnimate)
	{
		Animate::Destroy(s_pCurrentAnimate);
		s_pCurrentAnimate = nullptr;
	}
}

Animate* Animate::Create(long type, UI::IWindow* pWindow)
{
	if (s_pCurrentAnimate)
	{
		Animate::Destroy(s_pCurrentAnimate);
		s_pCurrentAnimate = nullptr;
	}
	if (!pWindow)
		return nullptr;

#define MAPCREATOR(x,y) case x: p = new y; break;

	Animate* p = nullptr;
	switch (type)
	{
MAPCREATOR(AnimateType_CustomTimingFunction, CustomTimingFuctionAnimate);
MAPCREATOR(AnimateType_MoveLeftToRight, MoveLeftToRightAnimate);
MAPCREATOR(AnimateType_Alpha, AlphaAnimate);
	}

	if (p)
	{
		s_pCurrentAnimate = p;
		p->SetWindowPtr(pWindow);
		p->Init();
	}
	return p;
}

void Animate::Destroy(Animate* a)
{
	if (a)
	{
		if (a == s_pCurrentAnimate)
		{
			s_pCurrentAnimate = nullptr;
		}

		a->Release();
		delete a;
	}
}

void Animate::SetWindowPtr(UI::IWindow* p)
{
	m_pWindow = p;
}

UIA::IStoryboard* Animate::CreateStoryboard()
{
	UIA::IAnimateManager* mgr = m_pWindow->GetUIApplication()->GetAnimateMgr();
	UIA::IStoryboard* storyboard = mgr->CreateStoryboard(
		static_cast<UIA::IAnimateEventCallback*>(this));

	return storyboard;
}

void Animate::ClearStoryboard()
{
	UIA::IAnimateManager* mgr = m_pWindow->GetUIApplication()->GetAnimateMgr();
	mgr->ClearStoryboardByNotify(
		static_cast<UIA::IAnimateEventCallback*>(this));
}
