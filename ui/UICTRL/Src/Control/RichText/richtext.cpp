#include "stdafx.h"
#include "richtext.h"
//#include "parser\htmlparser.h"
#include "richtext_desc.h"
#include "..\UISDK\Inc\Util\util.h"
#include "..\UISDK\Inc\Interface\ilayout.h"
#include "..\UISDK\Inc\Interface\iscrollbarmanager.h"

using namespace UI;



RichText::RichText(IRichText* p) :MessageProxy(p)
{
	m_pIRichText = p;
    m_bNeedLayout = false;

	p->SetDescription(RichTextDescription::Get());
}

RichText::~RichText()
{

}

void  RichText::OnInitialize()
{
	DO_PARENT_PROCESS(IRichText, IControl);
	m_doc.Init(this);

	m_vscrollbarCreator.Initialize(
		m_pIRichText, 
		m_doc.GetScrollBarManager());
}

void  RichText::try_layout()
{
    if (!m_bNeedLayout)
        return;
    
    CRect rc;
    m_pIRichText->GetClientRectInObject(&rc);
    int  w = rc.Width();
    int  h = rc.Height();

    if (w < 0)
        w = 0;
    if (h < 0)
        h = 0;
    m_doc.LayoutAll(w, h);

    SetLayoutDirty(false);
}

void  RichText::OnPaint(IRenderTarget* pRT)
{
    if (m_bNeedLayout)
    {
        try_layout();
    }

	HDC hDC = pRT->GetHDC();

	RECT rcClip = {0};
	::GetClipBox(hDC, &rcClip);

	m_doc.Render2DC(hDC, &rcClip);

    // m_caret.OnControlPaint(m_pIRichText, pRT);

    Util::FixRenderTargetClipRegionAlpha(pRT);
}

void  RichText::OnSize(UINT nType, int cx, int cy)
{
    SetMsgHandled(FALSE);
    SetLayoutDirty(true);
}

void  RichText::OnMouseMove(UINT nFlags, POINT point)
{
	POINT ptDoc = {0};
	m_pIRichText->WindowPoint2ObjectClientPoint(&point, &ptDoc, true);

    m_doc.GetMouseKey().OnMouseMove(nFlags, ptDoc);
	return ;
}

void  RichText::OnMouseLeave()
{
    m_doc.GetMouseKey().OnMouseLeave();
	return ;
}


BOOL  RichText::OnSetCursor(HWND hWnd, UINT nHitTest, UINT message)
{
    if (!m_doc.GetMouseKey().OnSetCursor())
        SetMsgHandled(FALSE);
    return TRUE;
}

void  RichText::OnLButtonDown(UINT nFlags, POINT point)
{
    POINT ptDoc = { 0 };
    m_pIRichText->WindowPoint2ObjectClientPoint(&point, &ptDoc, true);

    m_doc.GetMouseKey().OnLButtonDown(nFlags, ptDoc);
}
void  RichText::OnLButtonDBClick(UINT nFlags, POINT point)
{
	POINT ptDoc = { 0 };
	m_pIRichText->WindowPoint2ObjectClientPoint(&point, &ptDoc, true);

	m_doc.GetMouseKey().OnLButtonDBClick(nFlags, ptDoc);
}

void  RichText::OnLButtonUp(UINT nFlags, POINT point)
{
    POINT ptDoc = { 0 };
    m_pIRichText->WindowPoint2ObjectClientPoint(&point, &ptDoc, true);

    m_doc.GetMouseKey().OnLButtonUp(nFlags, ptDoc);
}

void  RichText::OnSetFocus(IObject* pOldFocusObj)
{
//     if (m_eEditMode == RICHEDIT_EDIT_MODE_DISABLE)
//         return;
// 
//     m_caret.CreateCaret(m_pIRichText, NULL, 1,
//         17/*GetCaretHeight()*/,
//         CARET_TYPE_UNKNOWN,
//         (ICaretUpdateCallback*)COORD_CONTROLCLIENT);
//     //this->UpdateCaretByPos();
//     m_caret.SetCaretPos(m_pIRichText, 0, 0);
//     m_caret.ShowCaret(m_pIRichText);
}

void  RichText::OnKillFocus(IObject* pNewFocusObj)
{
//     if (m_eEditMode == RICHEDIT_EDIT_MODE_DISABLE)
//         return;
// 
//     m_caret.DestroyCaret(m_pIRichText, true);
}

void  RichText::LoadHtml(LPCTSTR szHtml)
{
// 	if (!szHtml)
// 		return;
// 
// 	Clear();
// 
// 	HtmlParser parse(m_doc);
// 	parse.Parse(szHtml);
// 
//     SetLayoutDirty(true);
// 
//     ILayout*  pLayout = (ILayout*)UISendMessage(
//         m_pIRichText->GetParentObject(), UI_MSG_GETLAYOUT);
//     if (pLayout)
//     {
//         pLayout->ChildObjectContentSizeChanged(m_pIRichText);
//     }
}

void  RichText::Clear()
{
    m_doc.Clear();
}

void  RichText::SetText(LPCTSTR szText)
{
    Clear();
    AppendText(szText);
}

void  RichText::AppendText(LPCTSTR szText)
{
	m_doc.ReplaceText(szText);
    SetLayoutDirty(true);
    m_pIRichText->Invalidate();
}

void  RichText::AppendText2(LPCTSTR szText, CharFormat* pcf)
{
    m_doc.ReplaceTextEx(szText, pcf);
    SetLayoutDirty(true);
    m_pIRichText->Invalidate();
}

void  RichText::ReplaceImage(LPCTSTR szPath)
{
    m_doc.ReplaceImage(szPath);
	SetLayoutDirty(true);
    m_pIRichText->Invalidate();
}

void  RichText::ReplaceCacheImage(const CCacheImage& image)
{               
	if (m_doc.ReplaceCacheImage(image))
	{
		SetLayoutDirty(true);
		m_pIRichText->Invalidate();
	}
}

RT::ILink*  RichText::ReplaceLink(LPCTSTR szText)
{
    UIASSERT(szText);
    RT::ILink* link = m_doc.ReplaceLink(szText);

    SetLayoutDirty(true);
    m_pIRichText->Invalidate();

    return link;
}

RT::ILink*  RichText::ReplaceLink2(LPCTSTR szText, CharFormat* pcf)
{
    UIASSERT(szText);
    RT::ILink* link = m_doc.ReplaceLinkEx(szText, pcf);

	SetLayoutDirty(true);
    m_pIRichText->Invalidate();

    return link;
}

void  RichText::GetDefaultCharFormat(CharFormat* pcf)
{
    if (!pcf)
        return;

    m_doc.CopyDefaultCharFormat(pcf);
}

void  RichText::GetDefaultLinkCharFormat(CharFormat* pcf)
{
	if (!pcf)
		return;

	m_doc.CopyDefaultLinkCharFormat(pcf);
}

void  RichText::SetLayoutDirty(bool b)
{
	if (m_bNeedLayout == b)
		return;

    m_bNeedLayout = b;
	if (m_bNeedLayout)
	{
		m_doc.ReleasePage();
	}
}

PageMode  RichText::GetPageMode()
{
    return m_doc.GetPageMode();
}
void  RichText::SetPageMode(PageMode e)
{
    if (m_doc.GetPageMode() != e)
    {
        m_doc.SetPageMode(e);
        SetLayoutDirty(true);
    }
}

void  RichText::GetDesiredSize(SIZE* pSize)
{
    if (m_bNeedLayout)
    {
        // TODO: 如果是多行的，要限制当前宽度
        int widthLimit = 0x7FFFFFFF;
        if (m_doc.GetPageMode() == PageMode_multiline)
        {
            CRect rc;
            m_pIRichText->GetClientRectInObject(&rc);
            if (rc.Width() > 0)
            {
                widthLimit = rc.Width();
            }
        }
        

        // 仅测量，没有设置标识位。因为此时控件的大小没有定，没法决定page大小
        m_doc.LayoutAll(widthLimit, 0x7FFFFFFF);
    }

    *pSize = m_doc.GetDesiredSize();
}

void UI::RichText::OnSerialize(SERIALIZEDATA* pData)
{
    DO_PARENT_PROCESS(IRichText, IControl);
    UI::AttributeSerializerWrap as(pData, TEXT("RichText"));

    as.AddEnum(XML_RICHTEXT_EDITABLE, &m_doc,
        memfun_cast<pfnLongSetter>(&RT::Doc::SetEditMode),
        memfun_cast<pfnLongGetter>(&RT::Doc::GetEditMode))
        ->AddOption((long)RT::EDITABLE_Disable,  XML_RICHTEXT_EDITABLE_DISABLE)
        ->AddOption((long)RT::EDITABLE_Readonly, XML_RICHTEXT_EDITABLE_READONLY)
        ->AddOption((long)RT::EDITABLE_Enable,   XML_RICHTEXT_EDITABLE_ENABLE);

    as.AddEnum(XML_RICHTEXT_PAGE_ALIGN_HORZ, *(long*)&m_doc.GetStyle().page_content_halign)
        ->AddOption((long)RT::HALIGN_left, XML_ALIGN_LEFT)
        ->AddOption((long)RT::HALIGN_right, XML_ALIGN_RIGHT)
        ->AddOption((long)RT::HALIGN_center, XML_ALIGN_CENTER);
    as.AddEnum(XML_RICHTEXT_PAGE_ALIGN_VERT, *(long*)m_doc.GetStyle().page_content_valign)
        ->AddOption((long)RT::VALIGN_top, XML_ALIGN_TOP)
        ->AddOption((long)RT::VALIGN_bottom, XML_ALIGN_BOTTOM)
        ->AddOption((long)RT::VALIGN_center, XML_ALIGN_CENTER);
}

void  RichText::OnCreateByEditor(CREATEBYEDITORDATA* pData)
{
	DO_PARENT_PROCESS_MAPID(IRichText, IControl, UIALT_CALLLESS);
	m_vscrollbarCreator.CreateByEditor(pData, m_pIRichText);
}


void  RichText::ScrollEnd()
{
	IScrollBarManager* pScroll = m_doc.GetScrollBarManager();
	if (!pScroll)
		return;

	// 需要先得到最新的尺寸才能滚动到最后面
	if (m_bNeedLayout)
	{
		try_layout();
	}

	pScroll->SetVScrollPos(
		pScroll->GetVScrollRange());
}