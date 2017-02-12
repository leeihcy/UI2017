#pragma once
#include "..\UISDK\Inc\Interface\ilistctrlbase.h"


// 菜单， [非固定行高]
//    滚动条：  横向NONE，纵向AUTO
//    布  局：  横向取max width，或者client width，纵向依次排列
//    AUTOSIZE: 横向max width, 纵向total height

namespace UI
{
class MenuItemLayout : public IListCtrlLayout
{
public:
    MenuItemLayout();

    static void  CreateInstance(MenuItemLayout** pp) { *pp = new MenuItemLayout(); }
    virtual void  SetIListCtrlBase(IListCtrlBase* p);
    virtual void  Arrange(IListItemBase* pStartToArrange, SIZE* pSizeContent);
    virtual void  Measure(SIZE* pSize);
    virtual void  Release() { delete this; }

protected:
    IListCtrlBase*  m_pIListCtrlBase;
};

}