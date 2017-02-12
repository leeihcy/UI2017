#include "stdafx.h"
#include "Inc\Interface\itreeview.h"
#include "treeitem.h"
#include "treeview.h"
#include "normaltreeitem.h"

using namespace UI;

UI_IMPLEMENT_INTERFACE_ACROSSMODULE(TreeItem, ListItemBase)
UI_IMPLEMENT_INTERFACE_ACROSSMODULE(TreeView, ListCtrlBase)
UI_IMPLEMENT_INTERFACE(NormalTreeItem, TreeItem)
UI_IMPLEMENT_INTERFACE_ACROSSMODULE(NormalTreeItemShareData, ListItemShareData);

IRenderBase*  ITreeView::GetExpandIconRender()  
{
    return __pImpl->GetExpandIconRender(); 
}
ITreeItem*  ITreeView::InsertNormalItem(UITVITEM* pItem, IListItemBase* pParent, IListItemBase* pInsertAfter)
{
    return __pImpl->InsertNormalItem(pItem, pParent, pInsertAfter); 
}
ITreeItem*  ITreeView::InsertNormalItem(LPCTSTR szText, IListItemBase* pParent, IListItemBase* pInsertAfter)
{ 
    return __pImpl->InsertNormalItem(szText, pParent, pInsertAfter); 
}

int  INormalTreeItemShareData::GetImageWidth()
{
    return __pImpl->GetImageWidth();
}
int  INormalTreeItemShareData::GetExpandIconWidth()
{
    return __pImpl->GetExpandIconWidth();
}
IRenderBase*  INormalTreeItemShareData::GetExpandIconRender()
{
	return __pImpl->pExpandIconRender;
}