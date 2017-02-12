#include "stdafx.h"
#include "HtmlNode_U.h"

HtmlNode* HtmlNode_U::CreateInstance(HtmlParser* pParser)
{
	HtmlNode_U* p = new HtmlNode_U(); 
	p->SetHtmlParser(pParser);
	p->AddRef();
	return p;
}

void  HtmlNode_U::ParseTag(PARSETAGDATA* pData)
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
	}
}

bool  HtmlNode_U::FillCharFormat(CharFormat* pcf)
{
	pcf->bUnderline = true;
	return true;
}