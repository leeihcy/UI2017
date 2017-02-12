#ifndef _UI_IHYPERLINK_H_
#define _UI_IHYPERLINK_H_
#include "ibutton.h"

namespace UI
{
    
class HyperLink;
interface UICTRL_API_UUID(008EBF97-5E38-4DC9-82EE-95D9A9D2EB19) 
IHyperLink : public IButton
{
    UI_DECLARE_INTERFACE(HyperLink);
};
}

#endif  // _UI_IHYPERLINK_H_