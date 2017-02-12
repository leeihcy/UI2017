#pragma once
#include "Inc\Interface\itreeview.h"
#include "..\ScrollBar\vscrollbar_creator.h"
#include "..\ScrollBar\hscrollbar_creator.h"

namespace UI
{
class TreeView : public MessageProxy
{
public:
    TreeView(ITreeView* p);
    ~TreeView();

    UI_BEGIN_MSG_MAP()
        MSG_WM_KEYDOWN(OnKeyDown)
        UIMSG_HANDLER_EX(UI_TVM_GETEXPANDICON_RENDER, 
            OnGetExpandIconRender)
        UIMSG_SERIALIZE(OnSerialize)
        UIMSG_QUERYINTERFACE(TreeView)
        UIMSG_FINALCONSTRUCT(FinalConstruct)
		UIMSG_INITIALIZE(OnInitialize)
	UIALT_MSG_MAP(UIALT_CALLLESS)
		UIMSG_CREATEBYEDITOR(OnCreateByEditor)
    UI_END_MSG_MAP_CHAIN_PARENT_Ixxx(TreeView, IListCtrlBase)

public:
    ITreeItem*  InsertNormalItem(
            UITVITEM* pItem, 
            IListItemBase* pParent = UITVI_ROOT, 
            IListItemBase* pInsertAfter = UITVI_LAST);
    ITreeItem*  InsertNormalItem(
            LPCTSTR szText, 
            IListItemBase* pParent = UITVI_ROOT, 
            IListItemBase* pInsertAfter = UITVI_LAST);

    IRenderBase*  GetExpandIconRender() { return m_pExpandIcon; }

protected:
    HRESULT  FinalConstruct(ISkinRes* p);
    void  OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	void  OnSerialize(SERIALIZEDATA* pData);
	void  OnInitialize();
	void  OnCreateByEditor(CREATEBYEDITORDATA* pData);

    LRESULT  OnGetExpandIconRender(UINT uMsg, WPARAM wParam, LPARAM lParam);

	LPCTSTR  SaveExpandIconRender() ;
	void  LoadExpandIconRender(LPCTSTR szName) ;

protected:
    ITreeView*  m_pITreeView;
    IRenderBase*   m_pExpandIcon;    // 展开/收缩图标绘制

	VScrollbarCreator  m_vscrollbarCreator;
	HScrollbarCreator  m_hscrollbarCreator;
};
}