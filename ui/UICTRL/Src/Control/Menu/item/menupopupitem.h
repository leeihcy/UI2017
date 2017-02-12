#pragma once
#include "Inc\Interface\imenu.h"

namespace UI
{

class MenuPopupItemShareData : public MessageProxy 
{
public:
    MenuPopupItemShareData(IMenuPopupItemShareData* p);
    ~MenuPopupItemShareData();
    
	UI_BEGIN_MSG_MAP()
		UIMSG_SERIALIZE(OnSerialize)
    UI_END_MSG_MAP_CHAIN_PARENT_Ixxx(MenuPopupItemShareData, IListItemShareData)

    IMenuPopupItemShareData*  GetIMenuPopupItemShareData() { return m_pIMenuPopupItemShareData; }

protected:
	void  OnSerialize(SERIALIZEDATA* pData);

public:
    IMenuPopupItemShareData*  m_pIMenuPopupItemShareData;
    IListCtrlBase*   m_pListCtrl;
    IRenderBase*     m_pPopupTriangleRender;

    int  nIconGutterWidth;
    int  nPopupTriangleWidth;
    int  nTextIndentLeft;
    int  nTextIndentRight;
};

class MenuPopupItem : public MessageProxy
{
public:
    MenuPopupItem(IMenuPopupItem* p);

	UI_BEGIN_MSG_MAP()
        UIMSG_PAINT(OnPaint)
        UIMSG_GETDESIREDSIZE(GetDesiredSize)
        UIMSG_INITIALIZE(OnInitialize)
    UI_END_MSG_MAP_CHAIN_PARENT_Ixxx(MenuPopupItem, IListItemBase)

    IMenuPopupItem*  GetIMenuPopupItem() { return m_pIMenuPopupItem; }

protected:
    void  GetDesiredSize(SIZE* pSize);
    void  OnPaint(IRenderTarget* pRenderTarget);
    void  OnInitialize();

private:
    IMenuPopupItem*  m_pIMenuPopupItem;
    MenuPopupItemShareData*  m_pShareData;

    int nMenuIconGutter;
};


}