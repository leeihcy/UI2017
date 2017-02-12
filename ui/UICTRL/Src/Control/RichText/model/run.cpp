#include "stdafx.h"
#include "run.h"
#include "element\element.h"
#include "line.h"
#include "element\textelement.h"

using namespace UI;
using namespace UI::RT;

Run::Run(Element* p, int w)
{
	m_pLine = NULL;
	m_pElement = p;
	m_nStartIndexInElement = 0;
	m_nEndIndexInElement = 0;
	m_pNextRun = m_pPrevRun = NULL;
    m_xInLine = 0;
    m_nWidth = w;
}

Run::~Run()
{

}

Run*  Run::GetNextRun()
{	
	return m_pNextRun;
}
Run*  Run::GetPrevRun()
{
	return m_pPrevRun;
}
void  Run::SetNextRun(Run* p)
{
	m_pNextRun = p;
}
void  Run::SetPrevRun(Run* p)
{
	m_pPrevRun = p;
}

void  Run::OnAddToLine(Line* p)
{
	m_pLine = p;
	m_pElement->AddRun(this);
}

Line*  Run::GetLine()
{
	return m_pLine;
}

void  Run::SetRange(uint nStart, uint nEnd)
{
    UIASSERT(nEnd >= nStart);

	m_nStartIndexInElement = nStart;
	m_nEndIndexInElement = nEnd;
}

void  Run::GetRange(uint* pnStart, uint* pnEnd)
{
    if (pnStart)
        *pnStart = m_nStartIndexInElement;
    if (pnEnd)
        *pnEnd = m_nEndIndexInElement;
}

void  Run::SetStart(uint nStart)
{
	m_nStartIndexInElement = nStart;
    m_nEndIndexInElement = nStart;
}

void  Run::SetEnd(uint nEnd)
{
	m_nEndIndexInElement = nEnd;
}
void  Run::AddContentWidth(uint width)
{
    m_nWidth += width;
}

uint  Run::GetStart() const
{
    return m_nStartIndexInElement;
}
uint  Run::GetEnd() const
{
    return m_nEndIndexInElement;
}

uint  Run::GetLength() const
{
    return m_nEndIndexInElement - m_nStartIndexInElement + 1;
}

void  Run::SetPosInLine(int x)
{
    m_xInLine = x;
}

uint  Run::GetWidth()
{
	return m_nWidth;
}

int  Run::GetXInLine()
{
	return m_xInLine;
}

void  Run::Draw(HDC hDC, RECT* prcLine)
{
    RECT  rcRun = *prcLine;
    rcRun.left += m_xInLine;
    rcRun.right = rcRun.left + m_nWidth;

    // 单行模式下面
    if (rcRun.left >= prcLine->right)
        return;

    if (rcRun.right > prcLine->right)
        rcRun.right = prcLine->right;

	m_pElement->Draw(
		hDC,
		this,
		&rcRun);
}

Element*  Run::GetElement()
{
    return m_pElement;
}

void  Run::GetRectInDoc(LPRECT prc)
{
    m_pLine->GetContentRectInDoc(prc);
    prc->left += m_xInLine;
    prc->right = prc->left + m_nWidth;
}

CaretPos Run::SelectionHitTestByDocPoint(POINT ptDoc)
{
    RECT rc = { 0 };
    GetRectInDoc(&rc);

    // 允许y方向上超出，例如单行edit，允许右侧值相等。PtInRect右侧不包含
//     if (!::PtInRect(&rc, ptDoc))
//         return -1;
    if (ptDoc.x <= rc.left)
        return CaretPos(m_nStartIndexInElement);
    if (ptDoc.x >= rc.right)
        return CaretPos(m_nEndIndexInElement+1);

    if (m_pElement->GetNodeType() != NodeTypeText)
	{
		int middle = rc.left + ((rc.right-rc.left)>>1);
		if (ptDoc.x <= middle)
			return CaretPos(0);
		else
			return CaretPos(1);
	}

    uint width = ptDoc.x - rc.left;
    CaretPos to = static_cast<TextElement*>(m_pElement)->SelectionHitTest(
        m_nStartIndexInElement, width);

    return to;
}