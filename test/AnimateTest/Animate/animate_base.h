#pragma once
#include "ui\UISDK\Inc\Interface\ianimate.h"

enum AnimateType
{
	AnimateType_CustomTimingFunction = 0,
	AnimateType_MoveLeftToRight,
	AnimateType_Alpha,

	//-----------------------
	AnimateType_Count,
};

class Animate : public UIA::IAnimateEventCallback
{
public:
	virtual ~Animate();

	static  Animate*  Create(long type, UI::IWindow* p);
	static  void  Quit();
	static  void  Destroy(Animate*);

	void  SetWindowPtr(UI::IWindow* p);

public:

	virtual void  Init() {}
	virtual void  Release() {}

	virtual void  OnAnimateStart(
		UIA::IStoryboard*) { }

	virtual void  OnAnimateEnd(
		UIA::IStoryboard*, 
		UIA::E_ANIMATE_END_REASON e) { }

	virtual UIA::E_ANIMATE_TICK_RESULT  OnAnimateTick(
		UIA::IStoryboard* s) { return UIA::ANIMATE_TICK_RESULT_CONTINUE; }

protected:
	UIA::IStoryboard*  CreateStoryboard();
	void  ClearStoryboard();

protected:
	UI::IWindow*  m_pWindow;

	static Animate*  s_pCurrentAnimate;
};