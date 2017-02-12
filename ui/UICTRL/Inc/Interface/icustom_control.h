#ifndef _INCLUDED_CUSTOMCONTROL_H_
#define _INCLUDED_CUSTOMCONTROL_H_
#include "..\..\..\UISDK\Inc\Interface\icontrol.h"

namespace UI
{

class CustomControl;
interface __declspec(uuid("97A2B9D4-2C1D-49C4-BE31-2A12A3D73740")) UICTRL_API ICustomControl : public IControl
{
	signal2<ICustomControl*, IRenderTarget*>& DrawEvent();
	signal2<ICustomControl*, MSG*>& MouseEvent();
	UI_DECLARE_INTERFACE_ACROSSMODULE(CustomControl)
};


}

#endif // _INCLUDED_CUSTOMCONTROL_H_