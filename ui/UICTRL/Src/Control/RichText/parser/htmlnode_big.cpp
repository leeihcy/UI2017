#include "stdafx.h"
#include "HtmlNode_BIG.h"

HtmlNode* HtmlNode_BIG::CreateInstance(HtmlParser* pParser)
{
	HtmlNode_BIG* p = new HtmlNode_BIG(); 
	p->SetHtmlParser(pParser);
	p->AddRef();
	return p;
}

void  HtmlNode_BIG::ParseTag(PARSETAGDATA* pData)
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

bool  HtmlNode_BIG::FillCharFormat(CharFormat* pcf)
{
	int g_fontSize[] = {8,9,10,11,12,14,16,18,20,22,24,26,28,36,48,72};
	int nSize = sizeof(g_fontSize)/sizeof(int);
	for (int i = 0; i < nSize; i++)
	{
		if (g_fontSize[i] < pcf->lFontSize)
			continue;

		if ((i+1) < nSize)
			pcf->lFontSize = g_fontSize[i+1];
		else
			pcf->lFontSize = g_fontSize[i];

		break;
	}
	return true;
}