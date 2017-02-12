#include "stdafx.h"
#include "HtmlNode_DEL.h"

HtmlNode* HtmlNode_DEL::CreateInstance(HtmlParser* pParser)
{
	HtmlNode_DEL* p = new HtmlNode_DEL(); 
	p->SetHtmlParser(pParser);
	p->AddRef();
	return p;
}

void  HtmlNode_DEL::ParseTag(PARSETAGDATA* pData)
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

bool  HtmlNode_DEL::FillCharFormat(CharFormat* pcf)
{
	pcf->bDeleteline = true;
	return true;
}