#pragma once
#include "Inc\Interface\itreeview.h"

namespace UI
{
class TreeItem : public MessageProxy
{
public:
    TreeItem(ITreeItem* p);
    ~TreeItem();

    UI_BEGIN_MSG_MAP()
    UI_END_MSG_MAP_CHAIN_PARENT_Ixxx(TreeItem, IListItemBase)

public:
    ITreeItem*  GetITreeItem() { return m_pITreeItem; }

    void  SetExpand(bool b);
    
protected:

protected:
    LRESULT  OnBtnExpandCollapse(BOOL& bHandled);
    void  CreateExpandCollapseCheckBtn();

protected:
    ITreeItem*  m_pITreeItem;
};

}