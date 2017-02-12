#pragma once
#include <stack>
#include "Inc\Interface\irichtext.h"

namespace UI
{
namespace RT{
	class Doc;
}

// 支持：
//  <a> <b> <i> <u> <br> <img> <font> <big> <small> <del> <s>
//  < > & " ' 符号转码

// 不支持:
//  . 注释 <! -- >

enum HTMLTAGTYPE
{
	TAG_UNKNOWN,
	TAG_A,    // <a>
	TAG_B,    // <b>
	TAG_I,    // <i>
	TAG_U,    // <u>
	TAG_BR,   // <br>
	TAG_IMG,  // <img>
	TAG_FONT, // <font>
	TAG_DEL,  // <del>
	TAG_BIG,  // <big>
	TAG_SMALL,// <small>
};
class HtmlNode;
class HtmlParser;
typedef HtmlNode* (*pfnCreateHtmlNode)(HtmlParser*);

class HtmlNode_A;

struct HTMLTAG
{
	HTMLTAGTYPE  eType;
	String  strName;
	pfnCreateHtmlNode  pCreator;
};

struct PARSETAGDATA
{
	String  strTagName;    // 属性标签
	map<String, String>  mapProp;  // 属性列表
	bool  bStartBacklash;  // </a> 表示是个结束标签
	bool  bEndBacklash;    // <a/> 表示是个封闭标签

	typedef map<String, String>::iterator _Iter;
};

class HtmlNode
{
public:
	HtmlNode()
	{
		m_lRef = 0;
	}
	virtual ~HtmlNode() {};
	virtual HTMLTAGTYPE  GetTagType() PURE;
	virtual void  ParseTag(PARSETAGDATA* pData) PURE;
	virtual bool  FillCharFormat(CharFormat* pcf) 
	{
		return false; 
	}

	long  AddRef() 
	{ 
		return ++m_lRef;
	}
	long  Release() 
	{
		--m_lRef; 
		if (0 == m_lRef)
		{
			delete this; 
			return 0;
		} 
		return m_lRef; 
	}

protected:
	void  SetHtmlParser(HtmlParser* p)
	{
		m_pHtmlParser = p; 
	}

protected:
	HtmlParser*  m_pHtmlParser;
private:
	long  m_lRef;
};

class HtmlParser
{
public:
	HtmlParser(RT::Doc& doc);
	~HtmlParser();
	bool  Parse(LPCTSTR szText);

public:
	void  RegiterTag(HTMLTAGTYPE eType, LPCTSTR szTag, pfnCreateHtmlNode p);
	HTMLTAG*  GetTagInfo(LPCTSTR szTag);

	void  AddText(LPCTSTR szText);
	bool  AddImg(LPCTSTR szImgPath);

	void  AddNode(HtmlNode* p);
	void  RemoveLastTagNode(HTMLTAGTYPE);

private:
	void  AddText(LPCTSTR pStart, LPCTSTR pEnd);
	LPCTSTR  ParseTag(LPCTSTR pStart);

	HtmlNode_A*  GetLastLinkContext();
	void  GetCharFormatOnStack(CharFormat* pcf);

private:
	RT::Doc&  m_refDoc;

	// 字体堆栈
	list<HtmlNode*>  m_listParentNode;
	list<HTMLTAG>   m_listTagInfo;
	
	// 遍历深度
	long  m_lDepth;  
};

}

LPCTSTR  ReadNextWord(LPCTSTR pStart);
LPCTSTR  SkipWhiteSpace(LPCTSTR pStart);
LPCTSTR  ReadText(LPCTSTR pStart);
LPCTSTR  SkipTagContent(LPCTSTR pStart);
LPCTSTR  SkipOutOfTag(LPCTSTR pStart);
