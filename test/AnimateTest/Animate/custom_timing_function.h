#pragma once
#include "animate_base.h"

// 自定义的动画函数， f 为 当前时间比例，返回位移比例

#define PI 3.141592653f
float __stdcall TimingFuction(float f)
{
	return sin(PI * f * 2);
}

const float duration = 800;
class CustomTimingFuctionAnimate : public Animate
{
public:
	virtual int  Type() override {
		return AnimateType_CustomTimingFunction;
	}

	virtual void Init() override
	{
		m_bDestroying = false;
		m_pPanel = (UI::IPanel*)m_pWindow->FindObject(L"panel");
	}

	virtual void Action() override
	{
		UIA::IStoryboard* storyboard = CreateStoryboard();
		UIA::IFromToTimeline* x = storyboard->CreateTimeline(0)
			->SetParam(10, UI::ScaleByDpi(380), duration);
		x->SetEaseType(UIA::linear);
		x->SetAutoReverse(true);

		UIA::IFromToTimeline* y = storyboard->CreateTimeline(1)
			->SetParam(0, -UI::ScaleByDpi(200), duration);
		y->SetAutoReverse(true);
		y->SetCustomTimingFuction(TimingFuction);

		// 占满CPU，软件渲染+移动形动画
		// storyboard->BeginFullCpu();

		storyboard->Begin();
	}

	virtual void Release() override
	{
		m_bDestroying = true;
		ClearStoryboard();
	}

	virtual void OnAnimateStart(UIA::IStoryboard*) override
	{
		UI::OBJSTYLE s = { 0 };
		s.layer = 1;
		m_pPanel->ModifyObjectStyle(&s, 0);
	}

	virtual void OnAnimateEnd(UIA::IStoryboard*, UIA::E_ANIMATE_END_REASON e) override
	{
		UI::OBJSTYLE s = { 0 };
		s.layer = 1;
		m_pPanel->ModifyObjectStyle(0, &s);

		if (!m_bDestroying)
			Destroy(this);
	}

	virtual UIA::E_ANIMATE_TICK_RESULT OnAnimateTick(UIA::IStoryboard* story) override
	{
		int x = story->GetTimeline(0)->GetCurrentIntValue();
		int y = story->GetTimeline(1)->GetCurrentIntValue();

		// 		m_pPanel->SetObjectPos(x, y, 0, 0, SWP_NOSIZE);
		// 		m_pWindow->GetWindowRender()->InvalidateNow();

		m_pPanel->SetTranslate(x, y, 0);

		return UIA::ANIMATE_TICK_RESULT_CONTINUE;
	}

private:
	UI::IPanel*  m_pPanel;
	bool  m_bDestroying;
};