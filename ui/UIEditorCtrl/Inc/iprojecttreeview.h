#ifndef _UI_IPROJECTTREEVIEW_H_
#define _UI_IPROJECTTREEVIEW_H_
#include "..\..\UISDK\Inc\Util\struct.h"
#include "..\..\UISDK\Inc\Interface\ilistitembase.h"
#include "..\..\UICTRL\Inc\Interface\itreeview.h"

namespace UI
{
//
// ¿Ø¼þÍÏ×§ÊÂ¼þ  
// wParam: ProjectTreeViewControlItemDropData*
// lParam: NA.
// return: NA.
//    
#define UI_PROJECTTREEVIEW_NOTIFY_CONTROLITEM_DROPEVENT  1404141608
struct  ProjectTreeViewControlItemDropData
{
      DROPTARGETEVENT_TYPE   eType;
      DROPTARGETEVENT_DATA*  pData;
      IListItemBase*  pHitItem;
};



class SkinTreeViewItem;
interface UIAPI ISkinTreeViewItem : public INormalTreeItem
{
public:
    void  SetTextRender(ITextRenderBase**  pTextRender);

	UI_DECLARE_INTERFACE_ACROSSMODULE(SkinTreeViewItem)
};

class ControlTreeViewItem;
interface UIAPI IControlTreeViewItem : public INormalTreeItem
{
    UI_DECLARE_INTERFACE_ACROSSMODULE(ControlTreeViewItem)
};



class ProjectTreeView;
interface UIAPI_UUID(73C4E47F-03EF-4AD0-B2D5-74F8C4CAE6EA) IProjectTreeView
 : public ITreeView
{
public:
    ISkinTreeViewItem*  InsertSkinItem(
        const TCHAR* szText,
        IListItemBase* pParent = UITVI_ROOT, 
        IListItemBase* pInsertAfter = UITVI_LAST);
    IControlTreeViewItem*  InsertControlItem(
        const TCHAR* szText, 
        IListItemBase* pParent = UITVI_ROOT,
        IListItemBase* pInsertAfter = UITVI_LAST);

    UI_DECLARE_INTERFACE_ACROSSMODULE(ProjectTreeView)
};

}

#endif  // _UI_IPROJECTTREEVIEW_H_