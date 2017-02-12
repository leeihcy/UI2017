#pragma once
#include "treeitem.h"
#include "Inc\Interface\itreeview.h"

namespace UI
{
// 操作系统样式的树结点，里面的元素都是绘制出来的
class NormalTreeItemShareData : public MessageProxy
{
public:
    NormalTreeItemShareData(INormalTreeItemShareData* p);
    ~NormalTreeItemShareData();

    UI_BEGIN_MSG_MAP()
    UI_END_MSG_MAP_CHAIN_PARENT_Ixxx(
        NormalTreeItemShareData, IListItemShareData)

public:
    int  GetImageWidth()
    {
        return nImageWidth;
    }
    int  GetExpandIconWidth()
    {
        return nExpandIconWidth;
    }
protected:

public:
    INormalTreeItemShareData*  m_pINormalTreeItemShareData;
    IRenderBase*  pExpandIconRender;

    int  nExpandIconWidth;
    int  nImageWidth;
};

class NormalTreeItem : public TreeItem
{
public:
    NormalTreeItem(INormalTreeItem* p);

    UI_BEGIN_MSG_MAP()
        UIMSG_PAINT(OnPaint)
        UIMSG_GETDESIREDSIZE(GetDesiredSize)
        UIMSG_INITIALIZE(OnInitialize)
        MSG_WM_LBUTTONDOWN(OnLButtonDown)
        MSG_WM_LBUTTONDBLCLK(OnLButtonDblClk)
    UI_END_MSG_MAP_CHAIN_PARENT_Ixxx(NormalTreeItem, ITreeItem)

protected:
    void  OnPaint(IRenderTarget* pRenderTarget);
    void  OnInitialize();
    void  OnLButtonDown(UINT nFlags, POINT point);
    void  OnLButtonDblClk(UINT nFlags, POINT point);
    void  GetDesiredSize(SIZE* pSize);

protected:
    void  GetItemExpandIconRect(CRect* prc);
    void  DrawLine(IRenderTarget* pRenderTarget);
    void  DrawFullVLine(HDC hDC, const CRect* prc);
    void  DrawFullHLine(HDC hDC, const CRect* prc);
    void  DrawHalfVTopLine(HDC hDC, const CRect* prc);
    void  DrawHalfVBottomLine(HDC hDC, const CRect* prc);
    void  DrawHalfHLine(HDC hDC, const CRect* prc);

private:
    INormalTreeItem*  m_pINormalTreeItem;
protected:
    NormalTreeItemShareData*  m_pShareData;
};
}