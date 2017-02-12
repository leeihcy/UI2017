#include "stdafx.h"
#include "HtmlNode_I.h"

HtmlNode* HtmlNode_I::CreateInstance(HtmlParser* pParser)
{
	HtmlNode_I* p = new HtmlNode_I(); 
	p->SetHtmlParser(pParser);
	p->AddRef();
	return p;
}

void  HtmlNode_I::ParseTag(PARSETAGDATA* pData)
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

bool  HtmlNode_I::FillCharFormat(CharFormat* pcf)
{
	pcf->bItalic = true;
	return true;
}