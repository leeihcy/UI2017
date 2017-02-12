#pragma once
#include "UIEditorCtrl\Inc\itoolboxtreeview.h"

namespace UI
{

class ToolBoxTreeItem : public MessageProxy
{
public:
    ToolBoxTreeItem(IToolBoxTreeItem* p);

    UI_BEGIN_MSG_MAP()
        MSG_WM_LBUTTONDOWN(OnLButtonDown)
        MSG_WM_LBUTTONUP(OnLButtonUp)
        MSG_WM_MOUSEMOVE(OnMouseMove)
    UI_END_MSG_MAP_CHAIN_PARENT_Ixxx(ToolBoxTreeItem, INormalTreeItem)


    void  OnMouseMove(UINT nFlags, POINT point);
    void  OnLButtonDown(UINT nFlags, POINT point);
    void  OnLButtonUp(UINT nFlags, POINT point);

	void  BindObject(IObjectDescription* p);

protected:
    void  DoDrag(POINT pt);
    HBITMAP  CreateDragBitmap(int* pWidth, int* pHeight);

private:
    IToolBoxTreeItem*  m_pIToolBoxTreeItem;
	IObjectDescription*  m_pObjectDescription;

    POINT  m_ptLastLButtonDown;

public:
    static UINT   s_nLayoutItemDragCF;  // ÍÏ×§¸ñÊ½
};


}