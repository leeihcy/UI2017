#pragma once
#include "htmlparser.h"

namespace UI
{

namespace RT
{
	class LinkUnit;
}

class HtmlNode_A : public HtmlNode
{
public:
	HtmlNode_A();

	virtual HTMLTAGTYPE  GetTagType() override 
	{ 
		return TAG_A; 
	}

	virtual void  ParseTag(PARSETAGDATA* pData) override;
	static HtmlNode* CreateInstance(HtmlParser* pParser);

	void  SetLinkUnit(RT::LinkUnit* p);
	RT::LinkUnit*  GetLinkUnit();

private:
	RT::LinkUnit*  m_pLinkUnit;
    String  m_strId;
};

}