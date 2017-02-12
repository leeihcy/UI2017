#pragma once
#include "htmlparser.h"

namespace UI
{

class HtmlNode_BR : public HtmlNode
{
public:
	virtual HTMLTAGTYPE  GetTagType() override { return TAG_BR; }
	virtual void  ParseTag(PARSETAGDATA* pData) override;
	static HtmlNode* CreateInstance(HtmlParser* pParser);
};

}