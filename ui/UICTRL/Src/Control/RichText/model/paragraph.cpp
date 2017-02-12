#include "stdafx.h"
#include "paragraph.h"
#include "line.h"
#include "page.h"
#include "font.h"
#include "doc.h"

using namespace RT;


Paragraph::Paragraph()
{
	m_hAlign = HALIGN_left;
	m_nTopSpace = 0;
	m_nBottomSpace = 0;
	m_nLeftIndent = 0;
	m_nRightIndent = 0;
	m_nLineSpace = 0;
	m_pcf = nullptr;  
}

Paragraph::~Paragraph()
{
    // SAFE_DELETE(m_pTail);
    SAFE_DELETE(m_pcf);
}

void Paragraph::PreLayoutMultiLine(MultiLineLayoutContext* context)
{
    context->pParagraph = this;
    context->yLine += m_nTopSpace;
    context->AppendNewLine();

    // 被message封装，则不处理
    if (GetParentNode() &&
        GetParentNode()->GetNodeType() == NodeTypeChatMessage)
    {
        return;
    }
    
    context->nLeftIndent += m_nLeftIndent;
    context->nRightIndent += m_nRightIndent;

	
}

void Paragraph::PostLayoutMultiLine(MultiLineLayoutContext* context)
{
	// 空白行，增加一个默认行高
	if (!GetChildNode())
	{
		Doc* doc = GetDocNode();
		if (doc)
		{
			const CharFormat* cf = doc->GetDefaultCharFormat();
			if (cf)
			{
				context->yLine += cf->nFontHeight;
			}
		}
	}

    context->yLine += m_nBottomSpace;
    context->pParagraph = nullptr;

    // 被message封装，则不处理
    if (GetAncestorNodeByType(NodeTypeChatMessageBubble))
    {
        return;
    }

    int nLineMaxWidth = 0;
    if (m_hAlign == HALIGN_paraRight ||
        m_hAlign == HALIGN_paraCenter)
    {
        // 这两种对齐方式需要统计内容的最大宽度
        Line* pLine = context->pLine;
        UIASSERT(pLine);

        while (pLine)
        {
            if (&pLine->GetParagraph() != this)
                break;

            int width = pLine->GetRunsWidth();
            if (width > nLineMaxWidth)
                nLineMaxWidth = width;

            pLine = pLine->GetPrevLine();
        }
    }

    // 居中和居右的情景下，每一行内容的左侧开始位置都不一样，单独处理
    if (m_hAlign == HALIGN_center)
    {
        Line* pLine = context->pLine;
        UIASSERT(pLine);
        while (pLine)
        {
            if (&pLine->GetParagraph() != this)
                break;

            int nLineW = pLine->GetRunsWidth();
            if (nLineW < context->GetLineContentWidth())
            {
                int leftIndex = context->nLeftIndent + (context->GetLineContentWidth() - nLineW) / 2;
                pLine->SetPositionXInPage(leftIndex);
            }

            pLine = pLine->GetPrevLine();
        }
    }
    else if (m_hAlign == HALIGN_right)
    {
        Line* pLine = context->pLine;
        UIASSERT(pLine);
        while (pLine)
        {
            if (&pLine->GetParagraph() != this)
                break;

            int nLineW = pLine->GetRunsWidth();
            int leftIndex =
                    context->nPageContentWidth - context->nRightIndent - nLineW;
            pLine->SetPositionXInPage(leftIndex);

            pLine = pLine->GetPrevLine();
        }
    }
    else
    {
        Line* pLine = context->pLine;
        UIASSERT(pLine);

        // 横向每一行自己调整
        int leftIndex = 0;
        switch (m_hAlign)
        {
        case HALIGN_left:
        {
            leftIndex = context->nLeftIndent;
        }
        break;

        case HALIGN_paraCenter:
        {
            if (nLineMaxWidth < context->GetLineContentWidth())
            {
                leftIndex =
                    context->nLeftIndent +
                    (context->GetLineContentWidth() - context->nLeftIndent -
                    context->nRightIndent) / 2;
            }
        }
        break;

        case HALIGN_paraRight:
        {
            leftIndex =
                context->nPageContentWidth - context->nRightIndent - nLineMaxWidth;
        }
        break;

        }

        while (pLine)
        {
            if (&pLine->GetParagraph() != this)
                break;

            pLine->SetPositionXInPage(leftIndex);

            pLine = pLine->GetPrevLine();
        }
    }

    context->nLeftIndent -= m_nLeftIndent;
    context->nRightIndent -= m_nRightIndent;
}

void Paragraph::SetAlign(RT::HALIGN align)
{
    m_hAlign = align;

//     switch (m_hAlign)
//     {
//     case RT::HALIGN::left:
//     {
//         m_rcMsgIndent.left = 10;
//         m_rcMsgIndent.right = 100;
//     }
//     break;
// 
//     case RT::HALIGN::right:
//     {
//         m_rcMsgIndent.left = 100;
//         m_rcMsgIndent.right = 10;
//     }
//     break;
// 
//     case RT::HALIGN::center:
//     {
//         m_rcMsgIndent.left = 50;
//         m_rcMsgIndent.right = 50;
//     }
//     break;
//     }
}

void  Paragraph::SetLeftIndent(int indent)
{
    m_nLeftIndent = indent;
}
void  Paragraph::SetRightIndent(int indent)
{
    m_nRightIndent = indent;
}
void  Paragraph::SetTopSpace(int space)
{
    m_nTopSpace = space;
}
void  Paragraph::SetBottomSpace(int space)
{
    m_nBottomSpace = space;
}

const CharFormat*  Paragraph::GetCharFormat()
{
    if (m_pcf)
        return m_pcf;

    return Node::GetCharFormat();
}

void  Paragraph::SetCharformat(CharFormat* pcf)
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

    // TODO: 让子结点重新更新，计算字体大小
}

void  Paragraph::ToString(
        CaretPos start, CaretPos end, int format, String& out)
{
    // 特殊处理：第一个para不加回车，后面的才需要加回车
    // TBD: 这面这个逻辑可能不靠谱
    bool bFirstParagraph = true;
    Node* p = this;
    while (p)
    {
        if (p->GetPrevNode())
        {
            bFirstParagraph = false;
            break;
        }
        p = p->GetParentNode();
    }

    if (!bFirstParagraph)
        out.append(TEXT("\r\n"));
}