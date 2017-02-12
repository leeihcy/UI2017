#include "stdafx.h"
#include "message.h"
#include "line.h"
#include "layout_context.h"
#include "doc.h"
#include "paragraph.h"
#include "..\UISDK\Inc\Util\struct.h"

// 气泡的显示位置完全由内容来决定。因为内容可以没有气泡。
// 内容的位置算好了，气泡的位置就出来了

// 气泡图片与内容之间的间距
const int LEFT_BUBBLE_LEFT_PADDING = UI::ScaleByDpi(25);
const int LEFT_BUBBLE_RIGHT_PADDING = UI::ScaleByDpi(20);
const int RIGHT_BUBBLE_LEFT_PADDING = UI::ScaleByDpi(20);
const int RIGHT_BUBBLE_RIGHT_PADDING = UI::ScaleByDpi(25);
const int BUBBLE_TOP_PADDING = UI::ScaleByDpi(10);
const int BUBBLE_BOTTOM_PADDING = UI::ScaleByDpi(10);

// 头像大小
const int PORTRAIT_SIZE = UI::ScaleByDpi(36);
// 左气泡：头像的右侧与内容的间距  / 右气泡：内容与头像的左侧的间距
const int PORTRAIT_OFFSET_X = UI::ScaleByDpi(30);
const int PORTRAIT_OFFSET_Y = UI::ScaleByDpi(0);

// 内容相对于控件边缘的缩进
const int CONTENT_NEAR_INDENT = UI::ScaleByDpi(75);
const int CONTENT_FAR_INDENT = UI::ScaleByDpi(150);
// 消息上下空间，用于两个消息之间拉开距离
const int MESSAGE_TOP_SPACE = UI::ScaleByDpi(20);
const int MESSAGE_BOTTOM_SPACE = UI::ScaleByDpi(20);

namespace UI {
namespace RT {
	
ChatMessage::ChatMessage(Doc&  doc) : m_doc(doc)
{
	m_bDrawBubbleBackground = true;
	m_nLeftIndent = 0;
	m_nRightIndent = 0;
	m_nTopIndent = MESSAGE_TOP_SPACE;
	m_nBottomIndent = MESSAGE_BOTTOM_SPACE;

    SetAlign(RT::HALIGN_left);
}

void ChatMessage::SetAlign(RT::HALIGN align)
{
    m_hAlign = align;

    m_nLeftIndent = 0;
    m_nRightIndent = 0;

    switch (m_hAlign)
    {
    case RT::HALIGN_left:
    {
        m_nLeftIndent = CONTENT_NEAR_INDENT;
        m_nRightIndent = CONTENT_FAR_INDENT;
    }
    break;

    case RT::HALIGN_right:
    {
        m_nLeftIndent = CONTENT_FAR_INDENT;
        m_nRightIndent = CONTENT_NEAR_INDENT;
    }
    break;

    case RT::HALIGN_center:
    {
        m_nLeftIndent = CONTENT_NEAR_INDENT;
        m_nRightIndent = CONTENT_NEAR_INDENT;
    }
    break;
    }
}

void ChatMessage::SetPortrait(const CCacheImage& portrait)
{
	m_portrait = portrait;
}

HANDLE  ChatMessage::EnterBubble()
{
    Doc* pDoc = GetDocNode();
    UIASSERT(pDoc);
    if (!pDoc)
        return nullptr;

    ChatMessageBubble* bubble = new ChatMessageBubble(*this);
    AddChildNode(bubble);
    
	// 默认就添加一个para，用于解决出现空内容的时候，
	// 气泡没有分行，导致和上一行内容重叠问题
	Paragraph* para = new Paragraph;
	bubble->AddChildNode(para);

    Selection& s = pDoc->GetSelection();
    s.SetAnchorFocus(para, CaretPos(0));

    return (HANDLE)bubble;
}

void  ChatMessage::LeaveBubble(HANDLE handle)
{
    ChatMessageBubble* bubble = (ChatMessageBubble*)handle;
	if (!bubble)
		return;

    Doc* pDoc = GetDocNode();
    UIASSERT(pDoc);
    if (!pDoc)
        return;

    Selection& s = pDoc->GetSelection();
    s.SetAnchorFocus(bubble, -1);
}

void ChatMessage::PreDraw(HDC hDC, LPCRECT rcClip)
{
	RECT rcTemp;
	if (IntersectRect(&rcTemp, &m_rcPortrait, rcClip))
	{
		if (m_portrait)
		{
			m_portrait->Draw(hDC, &m_rcPortrait, ImageCache::DrawNormal);
		}
		else
		{
#ifdef _DEBUG
			FillRect(hDC, &m_rcPortrait, (HBRUSH)GetStockObject(GRAY_BRUSH));
#endif
		}
	}

    Node::PreDraw(hDC, rcClip);
}

void ChatMessage::PreLayoutMultiLine(MultiLineLayoutContext* context)
{
    context->yLine += m_nTopIndent;
    context->nLeftIndent += m_nLeftIndent;
    context->nRightIndent += m_nRightIndent;

    // TODO: 在这里设置每个段落的对齐方式？？解决消息头对齐方式与message不统一的问题
    Node* p = GetChildNode();
    while (p)
    {
        if (p->GetNodeType() == NodeTypeParagraph)
        {
            switch (m_hAlign)
            {
            case HALIGN_right:
                static_cast<Paragraph*>(p)->SetAlign(HALIGN_paraRight);
                break;

            case HALIGN_center:
                static_cast<Paragraph*>(p)->SetAlign(HALIGN_paraCenter);
                break;

            default:
                static_cast<Paragraph*>(p)->SetAlign(HALIGN_left);
                break;
            }            
        }
        p = p->GetNextNode();
    }
}

//
// message参数布局的情况下，将禁用Paragraph的布局功能。参见Paragraph的布局逻辑
//
void ChatMessage::PostLayoutMultiLine(MultiLineLayoutContext* context)
{
    m_rcPortrait.SetRect(
		0, 
		PORTRAIT_OFFSET_Y, 
		PORTRAIT_SIZE, 
		PORTRAIT_SIZE);

    Line* pFirstLine = context->pLine;
    Line* pLine = context->pLine;
	UIASSERT(pLine);
	if (!pLine)
		return;

    while (pLine)
    {
        if (pLine->GetChatMessage() != this)
            break;

        pFirstLine = pLine;
        pLine = pLine->GetPrevLine();
    }

	// 头像与气泡顶部对齐
    m_rcPortrait.OffsetRect(
		0, pFirstLine->GetPositionYInPage() - BUBBLE_TOP_PADDING);


    // 找到第一行，用于设置头像的纵向起始位置
    if (m_hAlign == RT::HALIGN_right)
    {
        m_rcPortrait.OffsetRect(
			context->nPageContentWidth - context->nRightIndent
			+ PORTRAIT_OFFSET_X, 0);
    }
	else
	{
		m_rcPortrait.OffsetRect(
			CONTENT_NEAR_INDENT - PORTRAIT_OFFSET_X - PORTRAIT_SIZE, 0);
	}

    context->yLine += m_nBottomIndent;
    context->nLeftIndent -= m_nLeftIndent;
    context->nRightIndent -= m_nRightIndent;
}


ChatMessageBubble::ChatMessageBubble(ChatMessage& message) : m_message(message)
{
	m_nBubbleTopSpace = 0/*BUBBLE_TOP_PADDING*/;
	// m_nBubbleBottomSpace = 0;
}

void ChatMessageBubble::PreDraw(HDC hDC, LPCRECT rcClip)
{
//     if (!m_bDrawBubbleBackground)
//         return;

	// 空结点，不画
	if (!GetChildNode())
		return;

#if 0
    FillRect(hDC, &m_rcBubble, (HBRUSH)GetStockObject(GRAY_BRUSH));
#else

	CRect rcBubble = m_rcContent;

	rcBubble.top -= BUBBLE_TOP_PADDING;
	rcBubble.bottom += BUBBLE_BOTTOM_PADDING;

    ImageWrap* image = nullptr;
    HALIGN hAlign = m_message.m_hAlign;
    if (hAlign == RT::HALIGN_left)
    {
        image = &m_message.m_doc.m_imageLeftBubble;

		rcBubble.left -= LEFT_BUBBLE_LEFT_PADDING;
		rcBubble.right += LEFT_BUBBLE_RIGHT_PADDING;
    }
    else if (hAlign == RT::HALIGN_right)
    {
        image = &m_message.m_doc.m_imageRightBubble;

		rcBubble.left -= RIGHT_BUBBLE_LEFT_PADDING;
		rcBubble.right += RIGHT_BUBBLE_RIGHT_PADDING;
    }

	RECT rcTemp;
	if (!IntersectRect(&rcTemp, &rcBubble, rcClip))
		return;

    if (image)
    {
        C9Region r;
        r.Set(15);

        image->Draw(hDC,
            rcBubble.left, rcBubble.top,
            rcBubble.Width(), rcBubble.Height(),
            0, 0,
            image->GetWidth(), image->GetHeight(),
            &r);
    }
#endif
}

void ChatMessageBubble::PreLayoutMultiLine(MultiLineLayoutContext* context)
{
    context->yLine += m_nBubbleTopSpace;
}

void ChatMessageBubble::OnLButtonDBClick(UINT nFlags, POINT ptDoc, bool& bHandled)
{
	__super::OnLButtonDBClick(nFlags, ptDoc, bHandled);

#if 0  // TODO: 有BUG，在后面的LBUTTONUP中又会将FOCUS修改为鼠标下的位置
	Doc* doc = GetDocNode();
	if (!doc)
		return;

	Selection& selection = doc->GetSelection();
	selection.SetAnchor(this, 0);

	Node* pLast = this;
	Node* pTemp = nullptr;
	while (pLast) 
	{
		pTemp = pLast->GetLastChildNode();
		if (!pTemp)
			break;;
		pLast = pTemp;
	}
	UIASSERT(pLast);
	selection.SetFocus(pLast, -1);

	bHandled = true;
#endif
}

void ChatMessageBubble::PostLayoutMultiLine(MultiLineLayoutContext* context)
{
    SetRectEmpty(&m_rcContent);

    // 获取到气泡内容首行

    Line* pLastLine = context->pLine;
    Line* pFirstLine = pLastLine;

    int nLineMaxWidth = 0;

    Line* pLine = context->pLine;
    UIASSERT(pLine);
	if (!pLine)
		return;

    while (pLine)
    {
        if (pLine->GetChatMessageBubble() != this)
            break;

        int width = pLine->GetRunsWidth();
        if (width > nLineMaxWidth)
            nLineMaxWidth = width;

        pFirstLine = pLine;
        pLine = pLine->GetPrevLine();
    }

    m_rcContent.left = 0;
    m_rcContent.right = nLineMaxWidth;
    m_rcContent.bottom =
        pLastLine->GetPositionYInPage() + pLastLine->GetHeight();
    m_rcContent.top = pFirstLine->GetPositionYInPage();

    Doc* pDoc = GetDocNode();
    if (pDoc)
    {
        m_rcContent.OffsetRect(
            pDoc->GetStyle().page_padding.left,
            pDoc->GetStyle().page_padding.top);
    }
    int leftIndex = 0;
    HALIGN hAlign = m_message.m_hAlign;
    if (hAlign == RT::HALIGN_left)
    {
        leftIndex = context->nLeftIndent;
    }
    else if (hAlign == RT::HALIGN_right)
    {
        leftIndex =
            context->nPageContentWidth -
            nLineMaxWidth -
            context->nRightIndent;
    }
    else if (hAlign == RT::HALIGN_center)
    {
        leftIndex = context->nLeftIndent +
            (context->nPageContentWidth -
            nLineMaxWidth -
            context->nLeftIndent -
            context->nRightIndent) / 2;
    }

    pLine = pFirstLine;
    while (1)
    {
        // 横向每一行自己调整
        pLine->SetPositionXInPage(leftIndex);
        if (pLine == pLastLine)
            break;

        pLine = pLine->GetNextLine();
    }

    m_rcContent.OffsetRect(leftIndex, 0);

    // context->yLine += m_nBubbleBottomSpace;
}

}}

