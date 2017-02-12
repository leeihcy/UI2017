#ifndef _UI_ITOOLBOXTREEVIEW_H_
#define _UI_ITOOLBOXTREEVIEW_H_
#include "UICTRL\Inc\Interface\itreeview.h"

namespace UI
{

class ToolBoxTreeItem;
interface UIAPI IToolBoxTreeItem : public INormalTreeItem
{
	void  BindObject(IObjectDescription* p);
    UI_DECLARE_INTERFACE_ACROSSMODULE(ToolBoxTreeItem);   
};


}
#endif  