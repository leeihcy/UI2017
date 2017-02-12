#pragma once
#include <usp10.h>
#include "selection.h"

namespace UI
{
namespace RT
{
class Element;
class Line;

class Run
{
public:
	Run(Element*, int w);
	~Run();

    Element*  GetElement();
	void  Draw(HDC hDC, RECT* prcLine);
	void  SetRange(uint nStart, uint nEnd);
    void  GetRange(uint* pnStart, uint* pnEnd);
	void  SetStart(uint nStart);
	void  SetEnd(uint nEnd);
    void  AddContentWidth(uint width);
    uint  GetStart() const;
    uint  GetEnd() const;
    uint  GetLength() const;
    void  SetPosInLine(int x);
	uint  GetWidth();
	int  GetXInLine();

	Run*  GetNextRun();
	Run*  GetPrevRun();
	void  SetNextRun(Run* p);
	void  SetPrevRun(Run* p);

	void  OnAddToLine(Line* p);
	Line*  GetLine();

    CaretPos SelectionHitTestByDocPoint(POINT ptDoc);
    void  GetRectInDoc(LPRECT);

    

private:
	Element*  m_pElement;
	Line*  m_pLine;

	Run*  m_pNextRun;
	Run*  m_pPrevRun;

	uint  m_nStartIndexInElement;
	uint  m_nEndIndexInElement;

    int   m_xInLine;
    uint  m_nWidth;
};

}
}