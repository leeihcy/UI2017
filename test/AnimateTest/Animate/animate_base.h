#pragma once
#include "ui\UISDK\Inc\Interface\ianimate.h"

enum AnimateType
{
	AnimateType_List = 0,

	AnimateType_RotateX = 1,
	AnimateType_RotateY = 2,
	AnimateType_RotateZ = 3,
	AnimateType_Alpha = 4,
	AnimateType_Scale = 5,
	AnimateType_MoveLeftToRight = 6,
	AnimateType_CustomTimingFunction = 7,


	//-----------------------
	AnimateType_Count,
};

class Animate : public UIA::IAnimateEventCallback
{
public:
	virtual ~Animate();
	virtual int  Type() = 0;

	static  Animate*  Create(long type, UI::IWindow* p);
	static  void  Quit();
	static  void  Destroy(Animate*);

	void  SetWindowPtr(UI::IWindow* p);

public:

	virtual void  Init() {}
	virtual void  Release() {}
	virtual void  Action() = 0;

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