#pragma once
#include "htmlparser.h"

namespace UI
{

class HtmlNode_FONT : public HtmlNode
{
public:
	HtmlNode_FONT();

	virtual HTMLTAGTYPE  GetTagType() override { return TAG_FONT; }
	virtual void  ParseTag(PARSETAGDATA* pData) override;
	virtual bool  FillCharFormat(CharFormat* pcf) override;

	static HtmlNode* CreateInstance(HtmlParser* pParser);

private:
	bool  m_bDefineColor;
	bool  m_bDefineHeight;
	bool  m_bDefineFace;

	COLORREF  m_color;
	long  m_size;
	String  m_face;
	
};

}