#include "stdafx.h"
#include "UIEditorCtrl\Inc\itoolboxtreeview.h"
#include "UIEditorCtrl\Src\ToolBoxTreeView\toolboxtreeitem.h"

namespace UI
{
UI_IMPLEMENT_INTERFACE_ACROSSMODULE(ToolBoxTreeItem, NormalTreeItem);

void  IToolBoxTreeItem::BindObject(IObjectDescription* p)
{
	m_pImpl->BindObject(p);
}

}