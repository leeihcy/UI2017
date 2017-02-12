#ifndef _UI_ISCROLLBAR_H_
#define _UI_ISCROLLBAR_H_
#include "..\..\..\UISDK\Inc\Interface\icontrol.h"

namespace UI
{
interface IScrollBarManager;
class SystemScrollBar;
interface ISystemScrollBar : public IControl
{
    void  SetIScrollBarMgr(IScrollBarManager* p);
    void  SetAutoSetBindObjFocus(bool b);
    void  SetFloatOnClientRegion(bool b);
    void  SetKeepHoldNonClientRegion(bool b);

	UI_DECLARE_INTERFACE_ACROSSMODULE(SystemScrollBar);
};


class SystemHScrollBar;
interface __declspec(uuid("F2DD83A8-4D20-4289-B7EF-A495383A009D"))
ISystemHScrollBar : public ISystemScrollBar
{
    UI_DECLARE_INTERFACE(SystemHScrollBar);
};
class SystemVScrollBar;
interface __declspec(uuid("FDDBC931-D9D3-4E0E-A6D1-F885B5CC3545"))
ISystemVScrollBar : public ISystemScrollBar
{
    UI_DECLARE_INTERFACE(SystemVScrollBar);
};

}

#endif // _UI_ISCROLLBAR_H_