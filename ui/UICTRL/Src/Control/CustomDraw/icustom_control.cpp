#include "stdafx.h"
#include "inc\interface\icustom_control.h"
#include "custom_control.h"

namespace UI
{

UI_IMPLEMENT_INTERFACE_ACROSSMODULE(CustomControl, Control)

signal2<ICustomControl*, IRenderTarget*>& ICustomControl::DrawEvent()
{
	return __pImpl->draw;
}


UI::signal2<ICustomControl*, MSG*>& ICustomControl::MouseEvent()
{
	return __pImpl->mouse;
}

}