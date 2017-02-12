#include "stdafx.h"
#include "Inc\Interface\irichtext.h"
#include "richtext.h"
#include "model\paragraph.h"

using namespace UI;

UI_IMPLEMENT_INTERFACE_ACROSSMODULE(RichText, Control)

void  IRichText::Clear()
{
    __pImpl->Clear();
}
void  IRichText::SetText(LPCTSTR szText)
{
    __pImpl->SetText(szText);
}

void  IRichText::SelectEnd()
{
	__pImpl->GetDoc().SelectEnd();
}
void  IRichText::SelectBegin()
{
	__pImpl->GetDoc().SelectBegin();
}
void  IRichText::ScrollEnd()
{
	__pImpl->ScrollEnd();
}

void  IRichText::ReplaceText(LPCTSTR szText)
{
	__pImpl->AppendText(szText);
}
void  IRichText::ReplaceTextEx(LPCTSTR szText, CharFormat* pcf)
{
    __pImpl->AppendText2(szText, pcf);
}
void  IRichText::ReplaceImage(LPCTSTR szPath)
{
	__pImpl->ReplaceImage(szPath);
}
void  IRichText::ReplaceCacheImage(const CCacheImage& image)
{
	__pImpl->ReplaceCacheImage(image);
}
RT::ILink*  IRichText::ReplaceLink(LPCTSTR text)
{
	return __pImpl->ReplaceLink(text);
}
RT::ILink*  IRichText::ReplaceLinkEx(LPCTSTR text, CharFormat* pcf)
{
    return __pImpl->ReplaceLink2(text, pcf);
}
void  IRichText::GetDefaultCharFormat(CharFormat* pcf)
{
    __pImpl->GetDefaultCharFormat(pcf);
}
void  IRichText::SetDefaultCharFormat(CharFormat* pcf)
{
	__pImpl->GetDoc().SetDefaultFont(pcf);
}
void  IRichText::GetDefaultLinkCharFormat(CharFormat* pcf)
{
	__pImpl->GetDefaultLinkCharFormat(pcf);
}
void  IRichText::SetDefaultLinkCharFormat(CharFormat* pcf)
{
	__pImpl->GetDoc().SetDefaultLinkFont(pcf);
}
void  IRichText::LoadHtml(LPCTSTR szHtml)
{
	__pImpl->LoadHtml(szHtml);
}


void  IRichText::SetPageMode(PageMode e)
{
    __pImpl->SetPageMode(e);
}
PageMode IRichText::GetPageMode()
{
    return __pImpl->GetPageMode();
}
void  IRichText::SetEditMode(RT::EDITABLE e)
{
	__pImpl->GetDoc().SetEditMode(e);
}
RT::EDITABLE  IRichText::GetEditMode()
{
	return __pImpl->GetDoc().GetEditMode();
}

// RT::ISelection*  IRichText::GetSelection()
// {
//     return __pImpl->GetDoc().GetSelection();
// }

RT::IParagraph*  IRichText::GetSelectionAnchorParagraph()
{
    RT::Paragraph* para = __pImpl->GetDoc().GetSelectionAnchorParagraph();
    if (!para)
        return nullptr;

    return static_cast<RT::IParagraph*>(para);
}

void  IRichText::SetStyle_PageContentVAlign(
        RT::VALIGN e)
{
    __pImpl->GetDoc().GetStyle().page_content_valign = e;
}
void  IRichText::SetStyle_PageContentHAlign(
        RT::HALIGN e)
{
    __pImpl->GetDoc().GetStyle().page_content_halign = e;
}
void  IRichText::SetStyle_DrawLinkUnderline(bool b)
{
    __pImpl->GetDoc().GetStyle().draw_link_underline = b;
}

void  IRichText::SetStyle_LineSpace(int linespace)
{
	__pImpl->GetDoc().SetStyle_LineSpace(linespace);
}
// RT::IParagraph*  IRichText::AppendParagraphStart()
// {
//     return __pImpl->GetDoc().BeginParagraph();
// }
// void  IRichText::AppendParagraphEnd(RT::IParagraph* handle)
// {
//     __pImpl->GetDoc().EndParagraph((Paragraph*)handle);
// }

RT::IChatMessage*  IRichText::BeginInsertMessage()
{
    return __pImpl->GetDoc().BeginInsertMessage();
}
void  IRichText::EndInsertMessage(RT::IChatMessage* h)
{
    __pImpl->GetDoc().EndInsertMessage(h);
}
void  IRichText::DeleteParagraph(RT::IParagraph* p)
{
	if (!p)
		return;

	__pImpl->GetDoc().DeleteParagraph(static_cast<RT::Paragraph*>(p));
}

#if _MSC_VER >= 1800
signal<IRichText*, RT::ILink*>&  IRichText::LinkClickedEvent()
{
    return __pImpl->link_clicked;
}
#else
signal2<IRichText*, RT::ILink*>&  IRichText::LinkClickedEvent()
{
	return __pImpl->link_clicked;
}
#endif

signal2<IRichText*, RT::IImageElement*>&  IRichText::ImageDBClickEvent()
{
	return __pImpl->image_dbclick;
}
//////////////////////////////////////////////////////////////////////////

// IRichTextLinkUnit::IRichTextLinkUnit(RT::LinkUnit* p)
// {
// 	m_pImpl = p;
// }
// LPCTSTR  IRichTextLinkUnit::GetId()
// {
// 	return m_pImpl->GetId();
// }

