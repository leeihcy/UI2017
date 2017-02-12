#ifndef _UI_IRADIOBUTTON_H_
#define _UI_IRADIOBUTTON_H_
#include "ibutton.h"

namespace UI
{

class RadioButton;
interface __declspec(uuid("73CEB09D-D3B3-43F0-8C76-7AEC2350DA7A"))
UICTRL_API IRadioButton : public IButton
{
    UI_DECLARE_INTERFACE(RadioButton)
};

}

#endif