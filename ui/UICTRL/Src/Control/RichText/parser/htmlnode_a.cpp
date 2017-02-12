#include "stdafx.h"
#include "htmlnode_a.h"
#include "..\model\unit\link_unit.h"

HtmlNode_A::HtmlNode_A()
{
	m_pLinkUnit = NULL;
}

HtmlNode* HtmlNode_A::CreateInstance(HtmlParser* pParser)
{
	HtmlNode_A* p = new HtmlNode_A(); 
	p->SetHtmlParser(pParser);
	p->AddRef();
	return p;
}

void  HtmlNode_A::SetLinkUnit(RT::LinkUnit* p)
{
	m_pLinkUnit = p;
    if (m_pLinkUnit)
        m_pLinkUnit->SetId(m_strId.c_str());
}
RT::LinkUnit*  HtmlNode_A::GetLinkUnit()
{
	return m_pLinkUnit;
}

void  HtmlNode_A::ParseTag(PARSETAGDATA* pData)
{
	if (pData->bStartBacklash)
	{
		// 取消堆栈上的一个<a>
		m_pHtmlParser->RemoveLastTagNode(GetTagType());
	}
	else
	{
		// 没有闭合，放入堆栈中，等待下一个</a>
		if (!pData->bEndBacklash)
			m_pHtmlParser->AddNode(this);

        PARSETAGDATA::_Iter iter = pData->mapProp.find(TEXT("id"));
        if (iter != pData->mapProp.end())
            m_strId = iter->second;
	}
}