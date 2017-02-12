#include "stdafx.h"
#include "line.h"
#include "page.h"
#include "element\element.h"
#include "paragraph.h"

using namespace UI;
using namespace UI::RT;

// 新增一个行时，需要指定一个默认行高。这样在给一个空行添加一个回车时
// 不用再多判断一下给该行设置为默认行高。
Line::Line(Page* pPage, Paragraph& para, int nDefaultHeight) :
    m_paragraph(para)
{
    m_nHeight = nDefaultHeight;
	m_pPage = pPage;

	m_pNextLine = nullptr;
	m_pPrevLine = nullptr;
	m_pFirstRun = nullptr;

	m_xInPageContent = 0;     
	m_yInPageContent= 0;

	m_nHeight = 0;     
	m_nLimitWidth = -1; // 未使用
	m_nRunsWidth = 0;  
	m_bFirstParaLine = false; // 段首行
}
Line::~Line()
{
	Run* p = m_pFirstRun;
	while (p)
	{
		Run* pSave = p;
		p = p->GetNextRun();

		delete pSave;
	}
	m_pFirstRun = NULL;
}

// 该行中的内容范围
void  Line::GetContentRectInPage(RECT* prc)
{
	prc->left = m_xInPageContent;
	prc->top = m_yInPageContent;
	prc->right = prc->left + m_nRunsWidth;
	prc->bottom = prc->top + m_nHeight;
}

// 整行范围
void  Line::GetFullRectInPage(RECT* prc)
{
    RECT rcPage;
    m_pPage->GetRectInDoc(&rcPage);

    prc->left = m_xInPageContent;
    prc->top = m_yInPageContent;
    prc->right = prc->left + rcPage.right - rcPage.left;
    prc->bottom = prc->top + m_nHeight;
}

void  Line::GetContentRectInDoc(RECT* prc)
{
// 	GetRectInPage(prc);
// 	m_pPage->PageRect2DocRect(prc, prc);

    m_pPage->GetContentRectInDoc(prc);
    prc->left += m_xInPageContent;
    prc->top += m_yInPageContent;

    prc->right = prc->left + m_nRunsWidth;
    prc->bottom = prc->top + m_nHeight;
}

void  Line::Draw(HDC hDC, RECT* prcPageContent, LPCRECT rcClip)
{
    RECT rcLine;
    rcLine.left = prcPageContent->left + m_xInPageContent;
    rcLine.top = prcPageContent->top + m_yInPageContent;
    rcLine.right = prcPageContent->right;
    rcLine.bottom = rcLine.top + m_nHeight;

	RECT rcTemp;
	if (!IntersectRect(&rcTemp, &rcLine, rcClip))
		return;

	Run* p = m_pFirstRun;
	while (p)
	{
		p->Draw(hDC, &rcLine);
		p = p->GetNextRun();
	}
}

Run*  Line::GetFirstRun()
{
	return m_pFirstRun;
}
Run*  Line::GetLastRun()
{
	Run* p = m_pFirstRun;
	while (p && p->GetNextRun())
		p = p->GetNextRun();

	return p;
}

bool  Line::HasRun()
{
	return m_pFirstRun?true:false;
}

Line*  Line::GetNextLine()
{
	return m_pNextLine;
}
Line*  Line::GetPrevLine()
{
	return m_pPrevLine;
}
void  Line::SetNextLine(Line* p)
{
	m_pNextLine = p;
}
void  Line::SetPrevLine(Line* p)
{
	m_pPrevLine = p;
}

void  Line::AppendRun(Run* run, uint height)
{
    if (!run)
        return;

    if (m_pFirstRun)
    {
        Run* pLastRun = GetLastRun();
        pLastRun->SetNextRun(run);
        run->SetPrevRun(pLastRun);
    }
    else
    {
        m_pFirstRun = run;
    }

    run->OnAddToLine(this);

    if (height > m_nHeight)
        m_nHeight = height;
}

void  Line::AddContentWidth(uint width)
{
    m_nRunsWidth += width;
}
/*
Run*  Line::AppendRun(Element* pElement, uint nRunWidth, uint nRunHeight)
{
	if (!pElement)
		return NULL;

	Run* pRun = new Run(pElement, nRunWidth);
	if (m_pFirstRun)
	{
		Run* pLastRun = GetLastRun();
		pLastRun->SetNextRun(pRun);
		pRun->SetPrevRun(pLastRun);
	}
	else
	{
		m_pFirstRun = pRun;
	}
	
	pRun->OnAdd2Line(this);

	if (nRunHeight > m_nHeight)
		m_nHeight = nRunHeight;

	m_nRunsWidth += nRunWidth;
	return pRun;
}
*/

void  Line::SetPosition(uint xInPage, uint yInPage)
{
    m_xInPageContent = xInPage;
    m_yInPageContent = yInPage;
}
void  Line::SetPositionXInPage(uint v)
{
    m_xInPageContent = v;
}
void  Line::SetPositionYInPage(uint v)
{
    m_yInPageContent = v;
}
uint  Line::GetPositionXInPage()
{
    return m_xInPageContent;
}
uint  Line::GetPositionYInPage()
{
    return m_yInPageContent;
}

void  Line::SetHeight(uint nHeight)
{
    m_nHeight = nHeight;
}

uint  Line::GetHeight()
{
    return m_nHeight;
}

uint  Line::GetRunsWidth()
{
    return m_nRunsWidth;
}

void  Line::SetFirstParaLine(bool b)
{
	m_bFirstParaLine = b;
}

Run*  Line::RunHitTest(POINT ptInLine, __out POINT* ptInRun)
{
	int left = 0;
	int right = 0;

	Run* p = m_pFirstRun;
	while (p)
	{
		left = p->GetXInLine() + m_xInPageContent;
		right = left + p->GetWidth();
		if (ptInLine.x >= left && ptInLine.x <= right)
		{
			if (ptInRun)
			{
				ptInRun->x = ptInLine.x - left;
				ptInRun->y = ptInLine.y;
			}
			return p;
		}

		p = p->GetNextRun();
	}

	return NULL;
}

Paragraph&  Line::GetParagraph()
{
    return m_paragraph;

    // 有bug
//     if (!m_pFirstRun)
//         return nullptr;
// 
//     Node* pNode = m_pFirstRun->GetElement();
//     if (!pNode)
//         return nullptr;
// 
//     return pNode->GetParagraphNode();
}

Node*  Line::GetChatMessage()
{
    return m_paragraph.GetAncestorNodeByType(NodeTypeChatMessage);
}
Node*  Line::GetChatMessageBubble()
{
    return m_paragraph.GetAncestorNodeByType(NodeTypeChatMessageBubble);
}