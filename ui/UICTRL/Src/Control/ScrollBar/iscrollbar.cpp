#include "stdafx.h"
#include "systemscrollbar.h"

using namespace UI;

UI_IMPLEMENT_INTERFACE_ACROSSMODULE(SystemScrollBar, Control)
UI_IMPLEMENT_INTERFACE(SystemHScrollBar, SystemScrollBar)
UI_IMPLEMENT_INTERFACE(SystemVScrollBar, SystemScrollBar)

void  ISystemScrollBar::SetIScrollBarMgr(IScrollBarManager* p)
{
    __pImpl->SetIScrollBarMgr(p); 
}
void  ISystemScrollBar::SetAutoSetBindObjFocus(bool b)
{
    __pImpl->SetAutoSetBindObjFocus(b);
}

void  ISystemScrollBar::SetFloatOnClientRegion(bool b)
{
    __pImpl->SetFloatOnClientRegion(b);
}
void  ISystemScrollBar::SetKeepHoldNonClientRegion(bool b)
{
    __pImpl->SetKeepHoldNonClientRegion(b);
}