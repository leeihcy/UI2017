#include "stdafx.h"
#include "htmlnode_img.h"

HtmlNode* HtmlNode_IMG::CreateInstance(HtmlParser* pParser)
{
	HtmlNode_IMG* p = new HtmlNode_IMG(); 
	p->SetHtmlParser(pParser);
	p->AddRef();
	return p;
}

// 规定： <img/> 必须是个单独的标签，末尾带上结束符
void  HtmlNode_IMG::ParseTag(PARSETAGDATA* pData)
{
	// </img> 无效
	if (pData->bStartBacklash)
		return;

	// <img> 无效
	if (!pData->bEndBacklash)
		return;

	String strPath;
	String strAlt;

	PARSETAGDATA::_Iter iter = pData->mapProp.find(TEXT("src"));
	if (iter == pData->mapProp.end())
		return;

	strPath = iter->second;
	iter = pData->mapProp.find(TEXT("alt"));
	if (iter != pData->mapProp.end())
		strAlt = iter->second;

	m_pHtmlParser->AddImg(strPath.c_str());
}