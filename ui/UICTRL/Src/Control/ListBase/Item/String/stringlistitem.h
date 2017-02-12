#pragma once
#include "UISDK\Control\Inc\Interface\istringlistitem.h"

namespace UI
{

class StringListItemShareData : public MessageProxy 
{
public:
	StringListItemShareData(IStringListItemShareData* p):MessageProxy(p)
    {
        m_rcItemPadding.SetRectEmpty();
        m_pIStringListItemShareData = p;
    }
    
    UI_BEGIN_MSG_MAP
    UI_END_MSG_MAP_CHAIN_PARENT_Ixxx(StringListItemShareData, IListItemTypeShareData)


    void  GetItemPadding(CRegion4* prc) { prc->CopyRect(&m_rcItemPadding); }
    void  SetItemPadding(CRegion4* prc) { m_rcItemPadding.CopyRect(prc); }

public:
    CRegion4  m_rcItemPadding;  
    IStringListItemShareData*  m_pIStringListItemShareData;
};


class StringListItem : public MessageProxy
{
public:
    StringListItem(IStringListItem* p);
    ~StringListItem();

    UI_BEGIN_MSG_MAP
        UIMSG_WM_PAINT(OnPaint)
        UIMSG_WM_GETDESIREDSIZE(GetDesiredSize)
        UIMSG_WM_QUERYINTERFACE(StringListItem)
        UIMSG_WM_INITIALIZE(OnInitialize)
    UI_END_MSG_MAP_CHAIN_PARENT_Ixxx(StringListItem, IListItemBase)

public:

protected:
    void  GetDesiredSize(SIZE* pSize);
    void  OnPaint(IRenderTarget* pRenderTarget);
    void  OnInitialize();

protected:
    IStringListItem*  m_pIStringListItem;
    StringListItemShareData*  m_pShareData;
};

}