#ifndef _UI_ICHECKBUTTON_H_
#define _UI_ICHECKBUTTON_H_
#include "ibutton.h"

namespace UI
{
class CheckButton;
struct __declspec(uuid("23F3CC63-179E-4108-86B2-A37D657EFFF7"))
UICTRL_API ICheckButton : public IButton
{
    UI_DECLARE_INTERFACE(CheckButton)
};

}

#endif // _UI_ICHECKBUTTON_H_