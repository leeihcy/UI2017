#include "stdafx.h"
#include "link.h"
#include "doc.h"
#include "..\richtext.h"
#include "..\UISDK\Inc\Util\struct.h"
#include "paragraph.h"
#include "font.h"

using namespace UI;
using namespace UI::RT;

Link::Link()
{
	m_nType = 0;
	m_wParam = m_lParam = 0;
	m_pcf = nullptr;
}

void  Link::SetId(LPCTSTR szId)
{
    if (szId)
        m_strId = szId;
    else
        m_strId.clear();
}
LPCTSTR  Link::GetId()
{
    return m_strId.c_str();
}


BOOL  Link::OnSetCursor()
{
	static HCURSOR sHandCursor = ::LoadCursor(NULL, IDC_HAND);
	SetCursor(sHandCursor);
	return TRUE;
}

void  Link::OnClick(UINT nFlags, POINT ptDoc)
{
    Doc* pDoc = GetDocNode();
    if (!pDoc)
        return;

    RichText* pControl = pDoc->GetControl();
    if (!pControl)
        return;

    pControl->link_clicked.emit(
        pControl->GetIRichText(),
        static_cast<ILink*>(this));

    Node::OnClick(nFlags, ptDoc);
}

// LPCTSTR  Link::GetToolTipText()
// {
// 	return m_strToolTip.c_str(); 
// }

void  Link::SetToolTip(LPCTSTR szToolTip)
{
    if (szToolTip)
        m_strToolTip = szToolTip;
    else
        m_strToolTip.clear();
}

LPCTSTR  Link::GetToolTip()
{
    return m_strToolTip.c_str();
}

LPCTSTR  Link::GetReference()
{
    return m_strRefefence.c_str();
}
void  Link::SetReference(LPCTSTR text)
{
    if (text)
        m_strRefefence = text;
    else
        m_strRefefence.clear();
}

void  Link::SetWParam(WPARAM w)
{
	m_wParam = w;
}
void  Link::SetLParam(LPARAM l)
{
	m_lParam = l;
}
WPARAM  Link::GetWParam()
{
	return m_wParam;
}
LPARAM  Link::GetLParam()
{
	return m_lParam;
}

#if 0
void  Link::DrawText(HDC hDC, RECT* prc, LPCTSTR szText,
        ULONG nLen, const CharFormat* pcf)
{
    __super::DrawText(hDC, prc, szText, nLen, pcf);

    const Style& style = GetDoc().GetStyle();

    // 划线
    if (style.draw_link_underline)
    {
        if (IsPress() ||/*&&*/ IsHover())
        {
            HFONT hfont = pcf->hFont;
            if (!hfont)
                return;

            COLORREF color = RGB(0, 0, 0);
            if (pcf->bTextColor)
                color = pcf->textColor;

            int y = prc->bottom - pcf->nFontDescent + 1;

            HPEN hPen = CreatePen(PS_SOLID, 1, color);
            HPEN hOldPen = (HPEN)SelectObject(hDC, hPen);
            MoveToEx(hDC, prc->left, y, NULL);
            LineTo(hDC, prc->right, y);
            SelectObject(hDC, hOldPen);
            DeleteObject(hPen);
        }
    }
}
#endif

const CharFormat*  Link::GetCharFormat()
{
	if (m_pcf)
		return m_pcf;

    Doc* pDoc = GetDocNode();
    if (!pDoc)
        return nullptr;

    return pDoc->GetDefaultLinkCharFormat();
}

void  Link::OnMouseEnter()
{
	Doc* doc = GetDocNode();
	if (!doc)
		return;

	if (doc->GetStyle().draw_link_underline)
	{
		doc->InvalidRegion(nullptr);
	}

    if (m_strToolTip.empty())
        return;

    RichText* control = doc->GetControl();
    UIASSERT(control);

    IUIApplication* pUIApp = control->GetIRichText()->GetUIApplication();
    UIASSERT(pUIApp);

    TOOLTIPITEM item = { 0 };
    item.szToolTipContent = GetToolTip();
    pUIApp->ShowToolTip(&item);
	
}

void  Link::OnMouseLeave()
{
	Doc* doc = GetDocNode();
	if (!doc)
		return;

	if (doc->GetStyle().draw_link_underline)
	{
		doc->InvalidRegion(nullptr);
	}

    if (m_strToolTip.empty())
        return;


    RichText* control = doc->GetControl();
    UIASSERT(control);

    IUIApplication* pUIApp = control->GetIRichText()->GetUIApplication();
    UIASSERT(pUIApp);

    pUIApp->HideToolTip();
}

UI::RT::Link::~Link()
{
	SAFE_DELETE(m_pcf);
}

int UI::RT::Link::GetType()
{
	return m_nType;
}

void UI::RT::Link::SetType(int n)
{
	m_nType = n;
}

UI::RT::IParagraph* UI::RT::Link::GetParagraph()
{
	Paragraph* para = __super::GetParagraphNode();
	if (!para)
		return nullptr;

	return static_cast<RT::IParagraph*>(para);
}

void UI::RT::Link::SetCharFormat(const CharFormat* pcf)
{
	if (!pcf)
	{
		SAFE_DELETE(m_pcf);
		return;
	}

	if (!m_pcf)
		m_pcf = new CharFormat;

	memcpy(m_pcf, pcf, sizeof(CharFormat));

	// 缓存该字体
	FontCache::Instance().CacheFont(m_pcf); 
}
