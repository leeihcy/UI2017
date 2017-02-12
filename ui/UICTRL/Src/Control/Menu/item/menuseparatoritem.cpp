#include "stdafx.h"
#include "menuseparatoritem.h"

namespace UI
{
//////////////////////////////////////////////////////////////////////////

MenuSeparatorItemShareData::MenuSeparatorItemShareData(IMenuSeparatorItemShareData* p):MessageProxy(p)
{
    m_pListCtrl = NULL;
    m_pIMenuSeparatorItemShareData = p;

    m_pSeparatorRender = NULL;
    nIconGutterWidth = 0;
    nHeight = 3;
}

MenuSeparatorItemShareData::~MenuSeparatorItemShareData()
{
    SAFE_RELEASE(m_pSeparatorRender);
}

void  MenuSeparatorItemShareData::OnSerialize(SERIALIZEDATA* pData)
{
	IMapAttribute*  pMapAttrib = pData->pMapAttrib;
    if (!m_pListCtrl)
        return;
    if (!pMapAttrib)
        return;

	if (pData->IsReload())
	{
		SAFE_RELEASE(m_pSeparatorRender);
	}

	AttributeSerializerWrap as(pData, TEXT("SeparatorItem"));
	as.AddLong(XML_MENU_SEPARATORHEIGHT, nHeight);
	as.AddRenderBase(
		XML_MENU_SEPARATOR_RENDEER_PREFIX, 
		m_pListCtrl,
		m_pSeparatorRender);
}

//////////////////////////////////////////////////////////////////////////


MenuSeparatorItem::MenuSeparatorItem(IMenuSeparatorItem* p):MessageProxy(p)
{
    m_pIMenuSeparatorItem = p;
    m_pShareData = NULL;
}

void  MenuSeparatorItem::GetDesiredSize(SIZE* pSize)
{
    pSize->cx = 0;
    pSize->cy = m_pShareData->nHeight;
}

void  MenuSeparatorItem::OnInitialize()
{
    DO_PARENT_PROCESS(IMenuSeparatorItem, IListItemBase);
    m_pIMenuSeparatorItem->SetItemType(UI_LISTITEM_TYPE_MENU_SEPARATOR);

    IListCtrlBase*  pListCtrl = m_pIMenuSeparatorItem->GetIListCtrlBase();
    IMenuSeparatorItemShareData* p = static_cast<IMenuSeparatorItemShareData*>(
        pListCtrl->GetItemTypeShareData(UI_LISTITEM_TYPE_MENU_SEPARATOR));
    if (NULL == p)
    {
        p = IMenuSeparatorItemShareData::CreateInstance(pListCtrl->GetSkinRes());
        pListCtrl->SetItemTypeShareData(UI_LISTITEM_TYPE_MENU_SEPARATOR, p);
         
        m_pShareData = p->GetImpl();

        m_pShareData->m_pListCtrl = m_pIMenuSeparatorItem->GetIListCtrlBase();
        m_pShareData->nIconGutterWidth = UISendMessage(pListCtrl, UI_WM_MENU_GETICONGUTTERWIDTH);

        IMapAttribute*  pMapAttrib = NULL;
        pListCtrl->GetMapAttribute(&pMapAttrib);

		SERIALIZEDATA data = {0};
        data.pUIApplication = pListCtrl->GetUIApplication();
		data.pMapAttrib = pMapAttrib;
		data.nFlags = SERIALIZEFLAG_LOAD;
		UISendMessage(m_pShareData->GetIIMenuSeparatorItemShareData(), UI_MSG_SERIALIZE, (WPARAM)&data);
        SAFE_RELEASE(pMapAttrib);
    }
    else
    {
        m_pShareData = p->GetImpl();
    }
}

void  MenuSeparatorItem::OnPaint(IRenderTarget* pRenderTarget)
{
    IListCtrlBase*  pListCtrl = m_pIMenuSeparatorItem->GetIListCtrlBase();

    CRect  rcParent;
    m_pIMenuSeparatorItem->GetParentRect(&rcParent);

    if (m_pShareData->m_pSeparatorRender)
    {
        rcParent.left += m_pShareData->nIconGutterWidth;
        m_pShareData->m_pSeparatorRender->DrawState(pRenderTarget, &rcParent, 0);
    }
}

}