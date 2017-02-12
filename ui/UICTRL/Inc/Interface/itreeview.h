#ifndef _UI_ITREEVIEW_H_
#define _UI_ITREEVIEW_H_
#include "..\..\..\UISDK\Inc\Interface\ilistitembase.h"
#include "..\..\..\UISDK\Inc\Interface\ilistctrlbase.h"

namespace UI
{
#define LISTITEM_TYPE_TREEITEM           136121826
#define LISTITEM_TYPE_TREEITEM_COMPOUND  136121827
#define LISTITEM_TYPE_TREEITEM_NORMAL    136121828

#define UITVIF_TEXT   TVIF_TEXT	
#define UITVIF_IMAG   TVIF_IMAGE
#define UITVIF_PARAM  TVIF_PARAM

// 获取展开/收缩图标renderbase
// WPARAM:  IRenderBase**
#define UI_TVM_GETEXPANDICON_RENDER  136131631


#define TREEVIEWCTRL_EXPANDCOLLAPSEICON_RENDER_STATE_EXPAND_NORMAL    (RENDER_STATE_NORMAL|0)
#define TREEVIEWCTRL_EXPANDCOLLAPSEICON_RENDER_STATE_EXPAND_HOVER     (RENDER_STATE_HOVER|1)
#define TREEVIEWCTRL_EXPANDCOLLAPSEICON_RENDER_STATE_EXPAND_PRESS     (RENDER_STATE_PRESS|2);
#define TREEVIEWCTRL_EXPANDCOLLAPSEICON_RENDER_STATE_EXPAND_DISABLE   (RENDER_STATE_DISABLE|3)
#define TREEVIEWCTRL_EXPANDCOLLAPSEICON_RENDER_STATE_COLLAPSE_NORMAL  (RENDER_STATE_SELECTED|RENDER_STATE_NORMAL|4)
#define TREEVIEWCTRL_EXPANDCOLLAPSEICON_RENDER_STATE_COLLAPSE_HOVER   (RENDER_STATE_SELECTED|RENDER_STATE_HOVER|5)
#define TREEVIEWCTRL_EXPANDCOLLAPSEICON_RENDER_STATE_COLLAPSE_PRESS   (RENDER_STATE_SELECTED|RENDER_STATE_PRESS|6)
#define TREEVIEWCTRL_EXPANDCOLLAPSEICON_RENDER_STATE_COLLAPSE_DISABLE (RENDER_STATE_SELECTED|RENDER_STATE_DISABLE|7)


struct UITVITEM
{
    UINT         mask;
    TCHAR*       pszText;
    int          cchTextMax;   // Set时忽略
    IRenderBase*  pIconRender;  
    LPARAM       pData;
};


class TreeItem;
interface UICTRL_API ITreeItem : public IListItemBase
{
    UI_DECLARE_INTERFACE_ACROSSMODULE(TreeItem);      
};


class TreeView;
interface UICTRL_API_UUID(A3821E3C-61C2-4933-AAEC-887A3D130132)
ITreeView : public IListCtrlBase
{
    IRenderBase*  GetExpandIconRender();

    ITreeItem*  InsertNormalItem(
                    UITVITEM* pItem, 
                    IListItemBase* pParent = UITVI_ROOT, 
                    IListItemBase* pInsertAfter = UITVI_LAST);
    ITreeItem*  InsertNormalItem(
                    LPCTSTR szText, 
                    IListItemBase* pParent = UITVI_ROOT, 
                    IListItemBase* pInsertAfter = UITVI_LAST);

	UI_DECLARE_INTERFACE_ACROSSMODULE(TreeView);
};

//////////////////////////////////////////////////////////////////////////

class NormalTreeItemShareData;
interface UICTRL_API INormalTreeItemShareData : public IListItemShareData
{
    int  GetImageWidth();
    int  GetExpandIconWidth();
	IRenderBase*  GetExpandIconRender();

	UI_DECLARE_INTERFACE_ACROSSMODULE(NormalTreeItemShareData);
};

class NormalTreeItem;
interface UICTRL_API INormalTreeItem : public ITreeItem
{
    UI_DECLARE_INTERFACE(NormalTreeItem);      
};

}

#endif 