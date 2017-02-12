#include "stdafx.h"
#include "page.h"
#include "line.h"
#include "doc.h"
#include "..\UISDK\Inc\Util\util.h"

using namespace UI;
using namespace UI::RT;

Page::Page(Doc* pDoc)
{
	m_pDoc = pDoc;
	m_pNextPage = NULL;
	m_pPrevPage = NULL;
	m_pFirstLine = NULL;

    m_posInDoc.x = m_posInDoc.y = 0;
    m_sizePage.cx = m_sizePage.cy = 0;
	m_bVisible = true;
}

Page::~Page()
{
	Line* p = m_pFirstLine;
	while (p)
	{
		Line* pSave = p;
		p = p->GetNextLine();

		delete pSave;
	}
	m_pFirstLine = NULL;
}

Page*  Page::GetNextPage()
{
	return m_pNextPage;
}
Page*  Page::GetPrevPage()
{
	return m_pPrevPage;
}

void  Page::Draw(HDC hDC, LPCRECT rcClip)
{
    RECT rcPageContent = {0};
    GetContentRectInDoc(&rcPageContent);

	if (rcPageContent.top > rcClip->bottom || 
		rcPageContent.bottom < rcClip->top)
		return;

	Line* pLine = m_pFirstLine;
	while (pLine)
	{
		pLine->Draw(hDC, &rcPageContent, rcClip);
		pLine = pLine->GetNextLine();
	}
}

Line*  Page::GetFirstLine()
{
	return m_pFirstLine;
}

Line*  Page::GetLastLine()
{
	Line* p = m_pFirstLine;
	while (p && p->GetNextLine())
		p = p->GetNextLine();

	return p;
}

void  Page::AppendLine(Line* pLine)
{
	if (m_pFirstLine)
	{
		Line* pLastLine = GetLastLine();
		pLastLine->SetNextLine(pLine);
		pLine->SetPrevLine(pLastLine);
	}
	else
	{
		m_pFirstLine = pLine;
	}
}

void  Page::SetSize(int cx, int cy)
{
    m_sizePage.cx = cx;
    m_sizePage.cy = cy;
}

// 获取page内容区域(page可能有留白)
void  Page::GetContentRectInDoc(RECT* prc)
{
    if (!m_pDoc)
        return;

    const REGION4&  rPagePadding = m_pDoc->GetStyle().page_padding;
    prc->left = m_posInDoc.x;
    prc->top = m_posInDoc.y;
    prc->right = prc->left + m_sizePage.cx;
    prc->bottom = prc->top + m_sizePage.cy;

    Util::DeflatRect(prc, &rPagePadding);
}

void  Page::SetVisible(bool b)
{
	m_bVisible = b;
}
bool  Page::IsVisible()
{
	return m_bVisible;
}

Line*  Page::LineHitTest(POINT ptInPage, __out POINT* ptInLine)
{
	Line* pLine = m_pFirstLine;
	while (pLine)
	{
        int y1 = pLine->GetPositionYInPage();
        int y2 = y1 + pLine->GetHeight();
		if (ptInPage.y >= y1 && ptInPage.y <= y2)
		{
			if (ptInLine)
			{
				ptInLine->x = ptInPage.x;
				ptInLine->y = ptInPage.y - y1;
			}
			return pLine;
		}
		pLine = pLine->GetNextLine();
	}

	return NULL;
}

// 做选区检测时，返回一个当前最近的行。该位置允许超出了page范围
Line*  Page::NearestLineHitTest(POINT ptInDoc)
{
    CRect rcPage;
    GetContentRectInDoc(&rcPage);

    if (ptInDoc.y <= rcPage.top)
        return m_pFirstLine;

    if (ptInDoc.y >= rcPage.bottom)
        return GetLastLine();

    int yInPage = ptInDoc.y - rcPage.top;

    Line* pLine = m_pFirstLine;
    while (pLine)
    {
        int y1 = pLine->GetPositionYInPage();
        int y2 = y1 + pLine->GetHeight();
        if (yInPage >= y1 && yInPage <= y2)
        {
            return pLine;
        }

        // 在本行与上一行之间，直接返回上一行
        if (yInPage < y1)
        {
            pLine = pLine->GetPrevLine();
            return pLine;
        }
        pLine = pLine->GetNextLine();
    }

    // 未超出page，但超出了最后一行
    return GetLastLine();
}

void  Page::PageRect2DocRect(RECT* prcPage, RECT* prcDoc)
{
	RECT rc;
	CopyRect(&rc, prcPage);
	OffsetRect(&rc, m_posInDoc.x, m_posInDoc.y);
	CopyRect(prcDoc, &rc);
}

void  Page::GetRectInDoc(LPRECT prc)
{
    ::SetRect(prc, 
        m_posInDoc.x,
        m_posInDoc.y,
        m_posInDoc.x + m_sizePage.cx,
        m_posInDoc.y + m_sizePage.cy);
}