#pragma once
#include "UIEditorCtrl\Inc\iprojecttreeview.h"

namespace UI
{

class ProjectTreeView : public MessageProxy
{
public:
    ProjectTreeView(IProjectTreeView*);
    ~ProjectTreeView();

    UI_BEGIN_MSG_MAP()
        UIMSG_DROPTARGETEVENT(OnDropTargetEvent)
        UIMSG_NOTIFY_CODE_FROM(UI_LCN_DRAWITEM, m_pIProjectTreeView, OnDrawItem)
        // UIMSG_WM_SETATTRIBUTE(SetAttribute)
        UIMSG_QUERYINTERFACE(ProjectTreeView)
        UIMSG_INITIALIZE(OnInitialize)
    UI_END_MSG_MAP_CHAIN_PARENT_Ixxx(ProjectTreeView, ITreeView)


public:
    void  OnInitialize();
    void  SetAttribute(IMapAttribute* pMapAttr, bool bReload);
    void  OnDropTargetEvent(
                DROPTARGETEVENT_TYPE eType, DROPTARGETEVENT_DATA* pData);
    LRESULT  OnDrawItem(WPARAM w, LPARAM l);

public:
    ISkinTreeViewItem*  InsertSkinItem(
            const TCHAR* szText, 
            IListItemBase* pParent = UITVI_ROOT, 
            IListItemBase* pInsertAfter = UITVI_LAST);
    IControlTreeViewItem*  InsertControlItem(
            const TCHAR* szText, IListItemBase* pParent = UITVI_ROOT, 
            IListItemBase* pInsertAfter = UITVI_LAST);

protected:
    IProjectTreeView*   m_pIProjectTreeView;
    ITextRenderBase*    m_pSkinItemText;
    IListItemBase*      m_pDragDropHitItem;  // 拖拽过程中，鼠标下的item
//    POINT               m_ptItemDragDropHit; // 拖拽过程中，鼠标在item中的位置
};

}