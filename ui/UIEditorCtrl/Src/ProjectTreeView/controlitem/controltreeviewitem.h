#pragma once
#include "UIEditorCtrl\Inc\iprojecttreeview.h"

namespace UI
{
class ControlTreeViewItem : public MessageProxy
{
public:
    ControlTreeViewItem(IControlTreeViewItem* p);
    ~ControlTreeViewItem();

    UI_BEGIN_MSG_MAP()
        MSG_WM_LBUTTONDOWN(OnLButtonDown)
        MSG_WM_MOUSEMOVE(OnMouseMove)
    UI_END_MSG_MAP_CHAIN_PARENT_Ixxx(ControlTreeViewItem, INormalTreeItem)


public:
    void  OnMouseMove(UINT nFlags, POINT point);
    void  OnLButtonDown(UINT nFlags, POINT point);
    void  DoDrag(POINT point);
    HBITMAP  CreateDragBitmap(int* pWidth, int* pHeight);

public:
    IControlTreeViewItem*  m_pIControlTreeViewItem;
    POINT  m_ptLastLButtonDown;
    static UINT  s_nControlDragCF;
};
}