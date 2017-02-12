#include "stdafx.h"
#include "unit.h"
#include "element.h"
#include "..\doc.h"
#include "..\run.h"
#include "..\line.h"
#include "..\layout_context.h"

using namespace UI;
using namespace UI::RT;

Unit::Unit()
{
    m_bHover = false;
    m_bPress = false;
	m_leftPadding = m_rightPadding = 0;
}
Unit::~Unit()
{
}

void  Unit::SetHover(bool b)
{
    m_bHover = b;
    Invalidate();
}
void  Unit::SetPress(bool b)
{
    m_bPress = b;
	Invalidate();
}
bool  Unit::IsHover()
{
    return m_bHover;
}
bool  Unit::IsPress()
{
    return m_bPress;
}

void  Unit::PreLayout()
{
// 	Element* pElement = m_pFirstElement;
// 	while (pElement)
// 	{
// 		pElement->ClearRun();
// 		pElement = pElement->GetNextElement();
// 	}
}

void  Unit::Invalidate()
{
#if 0
    if (m_pDoc.GetPageMode() == PAGE_MODE_SINGLELINE)
    {
        m_pDoc.InvalidRegion(NULL);
    }
    else
    {
        list<Line*>  listDirtyLine;

        Element* pElement = m_pFirstElement;
        while (pElement)
        {
			pElement->GetRelativeLines(listDirtyLine);
            pElement = pElement->GetNextElement();
        }

		// 整理成map，过滤重复
		map<Line*, bool>  mapDirtyLine;
		{
			list<Line*>::iterator iter = listDirtyLine.begin();
			for (; iter != listDirtyLine.end(); ++iter)
			{
				mapDirtyLine[(*iter)] = 1;
			}
		}

		// 刷新
		map<Line*, bool>::iterator iter = mapDirtyLine.begin();
		for (; iter != mapDirtyLine.end(); ++iter)
		{
			RECT rcPage;
			iter->first->GetContentRectInDoc(&rcPage);
			m_pDoc.InvalidRegion(&rcPage);
		}
    }
#endif
}