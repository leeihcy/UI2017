#include "stdafx.h"
#include "object_unit.h"
#include "..\layout_context.h"
#include "..\line.h"

using namespace RT;

#if 0
BOOL ObjectUnit::OnSetCursor()
{
    SetCursor(LoadCursor(nullptr, IDC_ARROW));
    return TRUE;
}

void ObjectUnit::LayoutMultiLine(MultiLineLayoutContext* pContext)
{
    if (!pContext->pPage || !pContext->pLine)
    {
        UIASSERT(0);
        return;
    }

    // 当前页面内容大小
    SIZE sizePageContent = {
        pContext->nPageContentWidth, pContext->nPageContentHeight
    };

    // 当前行剩余宽度
    int nLineRemain = pContext->nPageContentWidth - pContext->xLineOffset;

    // 元素剩余宽度
    SIZE  nObjSize = this->GetLayoutSize(sizePageContent, nLineRemain);

    // 本行能够放不下该元素
    if (nObjSize.cx > nLineRemain)
    {
        // 这已经是一个空行，强制放
        if (pContext->pLine->HasRun())
        {
            // 放不下，另起一行
            pContext->AppendNewLine();
        }
    }

    Run* pRun = pContext->pLine->AppendRun(&GetElement(), nObjSize.cx, nObjSize.cy);
    pRun->SetRange(0, 0);
    pRun->SetPosInLine(pContext->xLineOffset);

    // 该行放满
    pContext->xLineOffset += nObjSize.cx;
    if (pContext->xLineOffset >= pContext->nPageContentWidth)
        pContext->SetLineFinish();
}
#endif