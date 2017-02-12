#pragma once
#include "htmlparser.h"

namespace UI
{

class HtmlNode_I : public HtmlNode
{
public:
	virtual HTMLTAGTYPE  GetTagType() override { return TAG_B; }
	virtual void  ParseTag(PARSETAGDATA* pData) override;
	virtual bool  FillCharFormat(CharFormat* pcf) override;

	static HtmlNode* CreateInstance(HtmlParser* pParser);
};

}