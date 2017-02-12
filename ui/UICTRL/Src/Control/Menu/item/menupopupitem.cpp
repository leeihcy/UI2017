#include "stdafx.h"
#include "menupopupitem.h"


MenuPopupItemShareData::MenuPopupItemShareData(IMenuPopupItemShareData* p):MessageProxy(p)
{
    m_pIMenuPopupItemShareData = p;
    m_pListCtrl = NULL;
    m_pPopupTriangleRender = NULL;

    nIconGutterWidth = 0;
    nPopupTriangleWidth = 0;
    nTextIndentLeft = 4;
    nTextIndentRight = 4;
}
MenuPopupItemShareData::~MenuPopupItemShareData()
{
    SAFE_RELEASE(m_pPopupTriangleRender);
}


void  MenuPopupItemShareData::OnSerialize(SERIALIZEDATA* pData)
{
	AttributeSerializerWrap as(pData, TEXT("PopupItem"));
	as.AddRenderBase(XML_MENU_TRAIANGLE_RENDER_PREFIX, m_pListCtrl, m_pPopupTriangleRender);
	as.AddLong(XML_MENU_LEFT_TEXT_INDENT, nTextIndentLeft);
	as.AddLong(XML_MENU_RIGHT_TEXT_INDENT, nTextIndentRight);
}

//////////////////////////////////////////////////////////////////////////

MenuPopupItem::MenuPopupItem(IMenuPopupItem* p):MessageProxy(p)
{
    m_pIMenuPopupItem = p;
    m_pShareData = NULL;
}

void  MenuPopupItem::GetDesiredSize(SIZE* pSize)
{
    pSize->cx = 0;

    ITextRenderBase* pTextRender = m_pIMenuPopupItem->GetIListCtrlBase()->GetTextRender();
    if (pTextRender)
    {
        pSize->cx = pTextRender->GetDesiredSize(m_pIMenuPopupItem->GetText()).cx;
        pSize->cx += m_pShareData->nIconGutterWidth + m_pShareData->nPopupTriangleWidth +
                     m_pShareData->nTextIndentLeft + m_pShareData->nTextIndentRight;
    }

    pSize->cy = m_pIMenuPopupItem->GetIListCtrlBase()->GetItemHeight();
}

void  MenuPopupItem::OnInitialize()
{
    DO_PARENT_PROCESS(IMenuPopupItem, IListItemBase);
    m_pIMenuPopupItem->SetItemType(UI_LISTITEM_TYPE_MENU_POPUP);

    IListCtrlBase*  pListCtrl = m_pIMenuPopupItem->GetIListCtrlBase();
    IMenuPopupItemShareData* p = static_cast<IMenuPopupItemShareData*>(
        pListCtrl->GetItemTypeShareData(UI_LISTITEM_TYPE_MENU_POPUP));
    if (NULL == p)
    {
        p = IMenuPopupItemShareData::CreateInstance(pListCtrl->GetSkinRes());
        pListCtrl->SetItemTypeShareData(UI_LISTITEM_TYPE_MENU_POPUP, p);

        m_pShareData = p->GetImpl();
        m_pShareData->m_pListCtrl = m_pIMenuPopupItem->GetIListCtrlBase();
        m_pShareData->nIconGutterWidth = UISendMessage(pListCtrl, UI_WM_MENU_GETICONGUTTERWIDTH);
        m_pShareData->nPopupTriangleWidth = UISendMessage(pListCtrl, UI_WM_MENU_GETPOPUPTRIANGLEWIDTH);

        IMapAttribute*  pMapAttrib = NULL;
        pListCtrl->GetMapAttribute(&pMapAttrib);

		SERIALIZEDATA data = {0};
        data.pUIApplication = pListCtrl->GetUIApplication();
		data.pMapAttrib = pMapAttrib;
		data.nFlags = SERIALIZEFLAG_LOAD;
		UISendMessage(m_pShareData->GetIMenuPopupItemShareData(), UI_MSG_SERIALIZE, (WPARAM)&data);
        SAFE_RELEASE(pMapAttrib);
    }
    else
    {
        m_pShareData = p->GetImpl();
    }
}

void  MenuPopupItem::OnPaint(IRenderTarget* pRenderTarget)
{
    IListCtrlBase*  pListCtrl = m_pIMenuPopupItem->GetIListCtrlBase();

    CRect  rcParent;
    m_pIMenuPopupItem->GetParentRect(&rcParent);

    UINT nItemState = m_pIMenuPopupItem->GetItemState();
    UINT nRenderState = RENDER_STATE_NORMAL|0;

    if (nItemState & OSB_DISABLE)
    {
        nRenderState = RENDER_STATE_DISABLE|3;
    }
    else if (nItemState & OSB_PRESS && nItemState & OSB_HOVER)
    {
        nRenderState = RENDER_STATE_PRESS|2;
    }
    else if (nItemState & OSB_HOVER)
    {
        nRenderState = RENDER_STATE_HOVER|1;
    }

    IRenderBase* pRender = pListCtrl->GetForeRender();
    if (pRender)
    {
        pRender->DrawState(pRenderTarget, &rcParent, nRenderState);
    }

    CRect rcText = rcParent;
    ITextRenderBase* pTextRender = pListCtrl->GetTextRender();
    if (!pTextRender)
        pTextRender = pListCtrl->CreateDefaultTextRender();
    if (pTextRender)
    {
        rcText.left += m_pShareData->nIconGutterWidth + m_pShareData->nTextIndentLeft;
        rcText.right -= m_pShareData->nPopupTriangleWidth + m_pShareData->nTextIndentRight;

        pTextRender->DrawState(pRenderTarget, &rcText, nRenderState, m_pIMenuPopupItem->GetText());
    }


    if (m_pShareData->m_pPopupTriangleRender)
    {
        CRect rcItem = rcParent;
        rcItem.left = rcItem.right - m_pShareData->nPopupTriangleWidth;
        m_pShareData->m_pPopupTriangleRender->DrawState(pRenderTarget, &rcItem, nRenderState);
    }
}

