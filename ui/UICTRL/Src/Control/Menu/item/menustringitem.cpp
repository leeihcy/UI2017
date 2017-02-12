#include "stdafx.h"
#include "menustringitem.h"

namespace UI
{

MenuStringItemShareData::MenuStringItemShareData(IMenuStringItemShareData* p):MessageProxy(p)
{
    m_pIMenuStringItemShareData = p;

    nIconGutterWidth = 0;
    nPopupTriangleWidth = 0;
    nTextIndentLeft = 4;
    nTextIndentRight = 4;

    m_pCheckIconRender = NULL;
    m_pRadioIconRender = NULL;
    m_pRadioCheckIconBkRender = NULL;
}
MenuStringItemShareData::~MenuStringItemShareData()
{
    SAFE_RELEASE(m_pCheckIconRender);
    SAFE_RELEASE(m_pRadioIconRender);
    SAFE_RELEASE(m_pRadioCheckIconBkRender);
}

void  MenuStringItemShareData::OnSerialize(SERIALIZEDATA* pData)
{
	IListCtrlBase* pListCtrl = m_pIMenuStringItemShareData->GetListCtrlBase();

	AttributeSerializerWrap as(pData, TEXT("StringItem"));
	as.AddRenderBase(XML_MENU_RADIO_ICON_RENDER_PREFIX, pListCtrl, m_pRadioIconRender);
	as.AddRenderBase(XML_MENU_CHECK_ICON_RENDER_PREFIX, pListCtrl, m_pCheckIconRender);
	as.AddRenderBase(XML_MENU_RADIOCHECK_ICONBK_RENDER_PREFIX, pListCtrl, m_pRadioCheckIconBkRender);
	as.AddLong(XML_MENU_LEFT_TEXT_INDENT, nTextIndentLeft)
		->SetDefault(4);
	as.AddLong(XML_MENU_RIGHT_TEXT_INDENT, nTextIndentRight)
		->SetDefault(4);
}
//////////////////////////////////////////////////////////////////////////

MenuStringItem::MenuStringItem(IMenuStringItem* p):MessageProxy(p)
{
    m_pIMenuStringItem = p;
    m_pShareData = NULL;
}

void  MenuStringItem::GetDesiredSize(SIZE* pSize)
{
    pSize->cx = 0;

    ITextRenderBase* pTextRender = m_pIMenuStringItem->
        GetIListCtrlBase()->GetTextRenderDefault();
    if (pTextRender)
    {
        pSize->cx = pTextRender->GetDesiredSize(m_pIMenuStringItem->GetText()).cx;
        pSize->cx += m_pShareData->nIconGutterWidth + m_pShareData->nPopupTriangleWidth +
                     m_pShareData->nTextIndentLeft + m_pShareData->nTextIndentRight;
    }

    pSize->cy = m_pIMenuStringItem->GetIListCtrlBase()->GetItemHeight();
}

void  MenuStringItem::OnInitialize()
{
    DO_PARENT_PROCESS(IMenuStringItem, IListItemBase);
    m_pIMenuStringItem->SetItemType(UI_LISTITEM_TYPE_MENU_STRING);

    IListCtrlBase*  pListCtrl = m_pIMenuStringItem->GetIListCtrlBase();
    IMenuStringItemShareData* p = static_cast<IMenuStringItemShareData*>(
        pListCtrl->GetItemTypeShareData(UI_LISTITEM_TYPE_MENU_STRING));
    if (NULL == p)
    {
        p = IMenuStringItemShareData::CreateInstance(pListCtrl->GetSkinRes());
        pListCtrl->SetItemTypeShareData(UI_LISTITEM_TYPE_MENU_STRING, p);

        m_pShareData = p->GetImpl();
        m_pShareData->nIconGutterWidth = UISendMessage(pListCtrl, UI_WM_MENU_GETICONGUTTERWIDTH);
        m_pShareData->nPopupTriangleWidth = UISendMessage(pListCtrl, UI_WM_MENU_GETPOPUPTRIANGLEWIDTH);
    }
    else
    {
        m_pShareData = p->GetImpl();
    }
}

void  MenuStringItem::OnPaint(IRenderTarget* pRenderTarget)
{
    IListCtrlBase*  pListCtrl = m_pIMenuStringItem->GetIListCtrlBase();

    CRect  rcParent;
    m_pIMenuStringItem->GetParentRect(&rcParent);

    UINT nItemState = m_pIMenuStringItem->GetItemState();
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

        pTextRender->DrawState(pRenderTarget, &rcText, nRenderState, m_pIMenuStringItem->GetText());
    }


    //////////////////////////////////////////////////////////////////////////
    // Í¼±ê
    // »æÖÆÍ¼±ê
    if (m_pShareData->nIconGutterWidth)
    { 
        CRect rcIcon = rcParent;
        rcIcon.right = rcIcon.left + m_pShareData->nIconGutterWidth;

        IRenderBase*  pIconRender = m_pIMenuStringItem->GetIconRender();
        IRenderBase*  pRadioCheckIconBkRender = m_pShareData->m_pRadioCheckIconBkRender;
        if (m_pIMenuStringItem->IsChecked())
        {
            if (pRadioCheckIconBkRender)
                pRadioCheckIconBkRender->DrawState(pRenderTarget, &rcIcon, nRenderState);

            if (m_pShareData->m_pCheckIconRender && NULL == pIconRender)
                m_pShareData->m_pCheckIconRender->DrawState(pRenderTarget, &rcIcon, nRenderState);
        }
        else if (m_pIMenuStringItem->IsRadioChecked())
        {
            if (pRadioCheckIconBkRender)
                pRadioCheckIconBkRender->DrawState(pRenderTarget, &rcIcon, nRenderState);

            if (m_pShareData->m_pRadioIconRender && NULL == pIconRender)
                m_pShareData->m_pRadioIconRender->DrawState(pRenderTarget, &rcIcon, nRenderState);
        }

        if (pIconRender)
        {
           pIconRender->DrawState(pRenderTarget, &rcIcon, nRenderState);
        }

    }
}


}