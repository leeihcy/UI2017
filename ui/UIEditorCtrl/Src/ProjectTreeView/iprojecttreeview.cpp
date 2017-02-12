#include "stdafx.h"
#include "Inc\iprojecttreeview.h"
#include "Src\ProjectTreeView\projecttreeview.h"
#include "Src\ProjectTreeView\skinitem\skintreeviewitem.h"
#include "Src\ProjectTreeView\controlitem\controltreeviewitem.h"

namespace UI
{

UI_IMPLEMENT_INTERFACE_ACROSSMODULE(ProjectTreeView, TreeView)
UI_IMPLEMENT_INTERFACE_ACROSSMODULE(SkinTreeViewItem, NormalTreeItem)
UI_IMPLEMENT_INTERFACE_ACROSSMODULE(ControlTreeViewItem, NormalTreeItem)

ISkinTreeViewItem*  IProjectTreeView::InsertSkinItem(
const TCHAR* szText, IListItemBase* pParent, IListItemBase* pInsertAfter)
{
    return __pImpl->InsertSkinItem(szText, pParent, pInsertAfter);    
}
IControlTreeViewItem*  IProjectTreeView::InsertControlItem(
    const TCHAR* szText, IListItemBase* pParent, IListItemBase* pInsertAfter)
{
    return __pImpl->InsertControlItem(szText, pParent, pInsertAfter);    
}

void  ISkinTreeViewItem::SetTextRender(ITextRenderBase**  ppTextRender)
{
    __pImpl->SetTextRender(ppTextRender);
}
}