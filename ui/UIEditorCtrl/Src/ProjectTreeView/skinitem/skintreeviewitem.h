#pragma once
#include "UIEditorCtrl\Inc\iprojecttreeview.h"

namespace UI
{
class SkinTreeViewItem : public MessageProxy
{
public:
    SkinTreeViewItem(ISkinTreeViewItem*);
    ~SkinTreeViewItem();

    UI_BEGIN_MSG_MAP()
        UIMSG_PAINT(OnPaint)
    UI_END_MSG_MAP_CHAIN_PARENT_Ixxx(SkinTreeViewItem, INormalTreeItem)


public:
    void  SetTextRender(ITextRenderBase**  pTextRender);
    void  OnPaint(IRenderTarget* pRenderTarget);

public:
    ISkinTreeViewItem*  m_pISkinTreeViewItem;
    ITextRenderBase**   m_ppTextRender;
};
}