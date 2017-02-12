#pragma once
#include "Inc\Interface\imenu.h"

namespace UI
{

class MenuStringItemShareData : public MessageProxy 
{
public:
    MenuStringItemShareData(IMenuStringItemShareData* p);
    ~MenuStringItemShareData();

	UI_BEGIN_MSG_MAP()
		UIMSG_SERIALIZE(OnSerialize)
    UI_END_MSG_MAP_CHAIN_PARENT_Ixxx(MenuStringItemShareData, IListItemShareData)


protected:
	void  OnSerialize(SERIALIZEDATA* pData);

public:
    IMenuStringItemShareData*  m_pIMenuStringItemShareData;

    IRenderBase*  m_pRadioIconRender;
    IRenderBase*  m_pCheckIconRender;
    IRenderBase*  m_pRadioCheckIconBkRender;

    int  nIconGutterWidth;
    int  nPopupTriangleWidth;
    int  nTextIndentLeft;
    int  nTextIndentRight;
};

class MenuStringItem : public MessageProxy
{
public:
    MenuStringItem(IMenuStringItem* p);

	UI_BEGIN_MSG_MAP()
        UIMSG_PAINT(OnPaint)
        UIMSG_GETDESIREDSIZE(GetDesiredSize)
        UIMSG_INITIALIZE(OnInitialize)
    UI_END_MSG_MAP_CHAIN_PARENT_Ixxx(MenuStringItem, IListItemBase)


protected:
    void  GetDesiredSize(SIZE* pSize);
    void  OnPaint(IRenderTarget* pRenderTarget);
    void  OnInitialize();

private:
    IMenuStringItem*  m_pIMenuStringItem;
    MenuStringItemShareData*  m_pShareData;

    int nMenuIconGutter;
};


}