#include "stdafx.h"
#include "layout_context.h"
#include "doc.h"
#include "page.h"
#include "element\element.h"
#include "line.h"
#include "run.h"

using namespace UI;
using namespace UI::RT;;

void  SingleLineLayoutContext::PushElement(Element* element)
{
    uint nChars = element->GetCharacters();
    int nRunWidth = element->GetWidth();
    int nRunHeight = element->GetHeight();

    Run* pRun = new Run(element, nRunWidth);
    this->pLine->AppendRun(pRun, nRunHeight);

    uint nCharIndexs = nChars - 1; // 数量转索引
    pRun->SetRange(0, nCharIndexs);
    pRun->SetPosInLine(this->xLineCursor);

    this->xLineCursor += nRunWidth;
}

void  MultiLineLayoutContext::BeginLayoutElement(Element* p)
{
    this->pElement = p;
    UIASSERT(pRun == nullptr);
}
void  MultiLineLayoutContext::EndLayoutElement(Element* p)
{
    this->pElement = p;
    pRun = nullptr;
}
void  MultiLineLayoutContext::PushChar(uint Index, int width)
{
    UIASSERT(pElement);

    checkNewLine();
    
    // 能放下
    int remainWidth = getLineRemainWidth();
    if (width <=  remainWidth || !pLine->GetFirstRun())
    {
        if (!pRun)
        {
            pRun = new Run(pElement, 0);
            pLine->AppendRun(pRun, pElement->GetHeight());
            pRun->SetStart(Index);
            pRun->SetPosInLine(xLineOffset);
        }

        pLine->AddContentWidth(width);
        pRun->AddContentWidth(width);
        xLineOffset += width;

        pRun->SetEnd(Index);
    }
    // 放不下，新起一行
    else
    {
        SetLineFinish();
        PushChar(Index, width);
    }
}
void  MultiLineLayoutContext::checkNewLine()
{
    if (!pLine)
    {
        AppendNewLine();
        return;
    }

    if ((int)xLineOffset >= GetLineContentWidth())
    {
        SetLineFinish();
        AppendNewLine();
    }
}

void  MultiLineLayoutContext::AppendNewLine()
{
	if (pLine)
	{
		SetLineFinish();
	}
    pRun = nullptr;

    UIASSERT(pParagraph);
    pLine = new Line(pPage, *pParagraph,
        pDoc->GetDefaultCharFormat()->nFontHeight);

    pPage->AppendLine(pLine);
    xLineOffset = 0;

    pLine->SetPosition(xLineOffset/*+nLeftIndent*/, yLine);
}

void  MultiLineLayoutContext::SetLineFinish()
{
    if (pLine)
    {
        yLine += pLine->GetHeight();
        pLine = NULL;
    }
    pRun = nullptr;
    
    yLine += nLineSpace;
    xLineOffset = 0;
}

int   MultiLineLayoutContext::GetLineContentWidth()
{
    return nPageContentWidth - nLeftIndent - nRightIndent;
}
int   MultiLineLayoutContext::getLineRemainWidth()
{
    return GetLineContentWidth() - xLineOffset;
}

