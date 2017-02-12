#pragma once
#include "Inc\Interface\imenu.h"

namespace UI
{

class MenuSeparatorItemShareData : public MessageProxy 
{
public:
    MenuSeparatorItemShareData(IMenuSeparatorItemShareData* p);
    ~MenuSeparatorItemShareData();
    
	UI_BEGIN_MSG_MAP()
		UIMSG_SERIALIZE(OnSerialize)
    UI_END_MSG_MAP_CHAIN_PARENT_Ixxx(MenuSeparatorItemShareData, IListItemShareData)

    IMenuSeparatorItemShareData*  GetIIMenuSeparatorItemShareData() { return m_pIMenuSeparatorItemShareData; }

protected:
	void  OnSerialize(SERIALIZEDATA* pData);

public:
    IMenuSeparatorItemShareData*  m_pIMenuSeparatorItemShareData;
    IListCtrlBase*  m_pListCtrl;
    IRenderBase*    m_pSeparatorRender;

	long  nIconGutterWidth;
    long  nHeight;
};

class MenuSeparatorItem : public MessageProxy
{
public:
    MenuSeparatorItem(IMenuSeparatorItem* p);

	UI_BEGIN_MSG_MAP()
        UIMSG_PAINT(OnPaint)
        UIMSG_GETDESIREDSIZE(GetDesiredSize)
        UIMSG_INITIALIZE(OnInitialize)
    UI_END_MSG_MAP_CHAIN_PARENT_Ixxx(MenuSeparatorItem, IListItemBase)

protected:
    void  GetDesiredSize(SIZE* pSize);
    void  OnPaint(IRenderTarget* pRenderTarget);
    void  OnInitialize();

private:
    IMenuSeparatorItem*  m_pIMenuSeparatorItem;
    MenuSeparatorItemShareData*  m_pShareData;

    int nMenuIconGutter;
};


}