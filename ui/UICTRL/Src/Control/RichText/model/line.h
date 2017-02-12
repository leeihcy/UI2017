#pragma once
#include "run.h"

namespace UI
{
namespace RT
{
class Page;
class Paragraph;

class Line 
{
public:
	Line(Page* pPage, Paragraph& para, int nDefaultHeight);
	~Line();

	void  Draw(HDC hDC, RECT* prcPage, LPCRECT rcClip);
    void  AppendRun(Run*, uint height);
    void  AddContentWidth(uint width);
	Run*  GetFirstRun();
	Run*  GetLastRun();
	bool  HasRun();
	Line*  GetNextLine();
	Line*  GetPrevLine();
	void  SetNextLine(Line*);
	void  SetPrevLine(Line*);

    Paragraph&  GetParagraph();
    Node*  GetChatMessage();
    Node*  GetChatMessageBubble();

	Run*  RunHitTest(POINT ptInLine, __out POINT* ptInRun);
	void  GetContentRectInPage(RECT* prc);
    void  GetFullRectInPage(RECT* prc);
    void  GetContentRectInDoc(RECT* prc);

    void  SetPosition(uint xInPage, uint yInPage);
    void  SetPositionXInPage(uint);
    void  SetPositionYInPage(uint);
    uint  GetPositionXInPage();
    uint  GetPositionYInPage();
    
    void  SetHeight(uint nHeight);
    uint  GetHeight();
    uint  GetRunsWidth();
	
	void  SetFirstParaLine(bool b);

private:
	Page*  m_pPage;

    // 该行属于哪个段，通过model tree来向上遍历有缺陷。如一个空行，没有first run时，遍历不了
    Paragraph&  m_paragraph;

	Line*  m_pNextLine;
    Line*  m_pPrevLine;
    Run*   m_pFirstRun;

    // 在page content rect中的位置
    int   m_xInPageContent;     
    int   m_yInPageContent;  

    // 整行文字所需要的高度。不包含行间距
    uint  m_nHeight;     
    // 该行所允许的宽度，受限于page，超出时换行
    uint  m_nLimitWidth; // 未使用
    // 本行所有runs的总宽度
	uint  m_nRunsWidth;  

	bool  m_bFirstParaLine; // 段首行
};

}
}