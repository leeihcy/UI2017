#pragma once
#include "htmlparser.h"

namespace UI
{

class HtmlNode_IMG : public HtmlNode
{
public:
	virtual HTMLTAGTYPE  GetTagType() override 
	{ 
		return TAG_IMG; 
	}
	virtual void  ParseTag(PARSETAGDATA* pData) override;
	static HtmlNode*  CreateInstance(HtmlParser* pParser);
};

}