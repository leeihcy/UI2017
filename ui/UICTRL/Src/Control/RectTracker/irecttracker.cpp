#include "stdafx.h"
#include "recttracker.h"
#include "inc/Interface/irectracker.h"
#include "recttracker.h"

using namespace UI;

UI_IMPLEMENT_INTERFACE_ACROSSMODULE(RectTracker, Control);

void  IRectTracker::SetRsp(IRectTrackerRsp* prsp)
{
    __pImpl->SetRsp(prsp);
}
void  IRectTracker::SetRealRectInParent(RECT* prc)
{
    __pImpl->SetRealRectInParent(prc);
}
void  IRectTracker::SetHandleMask(UINT nMask)
{
    __pImpl->SetHandleMask(nMask);
}
void  IRectTracker::GetRealRectInWindow(RECT* prc)
{
    __pImpl->GetRealRectInWindow(prc);
}
void  IRectTracker::GetRealRectInControl(RECT* prc)
{
    __pImpl->GetRealRectInControl(prc);
}
void  IRectTracker::GetRealRectInParent(RECT* prc)
{
	__pImpl->GetRealRectInParent(prc);
}
int  IRectTracker::GetHandleSize()
{
    return __pImpl->GetHandleSize();
}
int  IRectTracker::GetLineSpace()
{
    return __pImpl->GetLineSpace();
}
void  IRectTracker::SetDrawBorder(bool b)
{
    __pImpl->SetDrawBorder(b);
}
void  IRectTracker::SetCursor(int nIndex, HCURSOR hCursor)
{
    __pImpl->SetCursor(nIndex, hCursor);
}

void IRectTracker::SetHandleMask(RectTrackerMask mask)
{
	if (__pImpl->GetHandleMask() == mask)
		return;

	__pImpl->SetHandleMask(mask);
	this->Invalidate();
}

signal2<IRectTracker*, LPCRECT>&  IRectTracker::RectChangedEvent()
{
	return __pImpl->rect_changed;
}
signal<IRectTracker*>&  IRectTracker::ClickEvent()
{
	return __pImpl->click;
}
