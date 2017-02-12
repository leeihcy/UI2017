#pragma once
#include "Inc\Interface\irichtext.h"
#include "..\selection.h"
#include "..\node.h"

namespace UI
{
namespace RT
{
class Run;
class Doc;
class Unit;
class Line;

// 
//  
//  目前只给提供两种element，textelement/objectelelement
//  
//  element 的职责：
//  1. unit内容划分（\r\n)
//  2. unit多字体样式划分
//  3. 选区记录
//
class Element : public Node
{
public:
    Element();
	virtual ~Element();

public:
    virtual void  Draw(HDC hDC, Run* run, RECT* prcRun) PURE;
	virtual uint  GetWidth() PURE;
	virtual uint  GetHeight() PURE;

    virtual void LayoutSingleLine(SingleLineLayoutContext*) override;

public:
	void  AddRun(Run* p);
	void  ClearRun();
	void  GetRelativeLines(list<Line*>& listLines);
    Line*  GetFirstLine();
    Line*  GetLastLine();

protected:
    void  DrawSelectionBackground(HDC hDC, RECT* prc);

protected:
	list<Run*>  m_listRuns;
};


}
}