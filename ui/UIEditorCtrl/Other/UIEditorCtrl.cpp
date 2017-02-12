// UIEditorCtrl.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "Src\ProjectTreeView\controlitem\controltreeviewitem.h"
#include "Src\ProjectTreeView\projecttreeview_desc.h"
#include "Src\PropertyCtrl\propertyctrl_desc.h"
#include "Src\ToolBoxTreeView\toolboxtreeitem.h"

const RECT g_rcDragImgPadding = { 10, 10, 10, 10 };

extern "C" 
{
    __declspec(dllexport) bool RegisterUIObject(UI::IUIApplication* p)
    {
        p->RegisterUIObject(ProjectTreeViewDescription::Get());
        p->RegisterUIObject(PropertyCtrlDescription::Get());

        return true;
    }

    // 可以直接由IM_UI工程调用的导出接口
    bool UIEditor_Ctrl_RegisterUIObject(UI::IUIApplication* p)
    {
        return RegisterUIObject(p);
    }

    UINT  GetToolBoxItemDragClipboardFormat()
    {
        return ToolBoxTreeItem::s_nLayoutItemDragCF;
    }

    UINT  GetProjectTreeControlDragClipboardFormat()
    {
        return ControlTreeViewItem::s_nControlDragCF;
    }
}
