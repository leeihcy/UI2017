#pragma once

namespace UI
{
    class RichText;
    interface IScrollBarManager;

namespace RT
{
	class Doc;
	class Page;
	class Line;
	class Run;
    class Element;
    class Unit;
	class Node;
}

class MouseKey
{
public:
    MouseKey(RT::Doc& pDoc);
	~MouseKey();

public:
    void  Init(RichText*);
    void  Clear();

    bool  IsPageHover(RT::Page*  pPage);
    bool  IsLineHover(RT::Line*  pLine);
    bool  IsRunHover(RT::Run*  pRun);
    bool  IsElementHover(RT::Element*  pElement);

	void  OnMouseMove(UINT nFlags, POINT ptDoc);
    void  OnLButtonDown(UINT nFlags, POINT ptDoc);
    void  OnLButtonUp(UINT nFlags, POINT ptDoc);
	void  OnLButtonDBClick(UINT nFlags, POINT ptDoc);
	void  OnMouseLeave();
	BOOL  OnSetCursor();
    BOOL  OnKeyDown(UINT nChar);
    // BOOL  OnMouseWheel(UINT nFlags, short zDelta);

	void  HitTest(POINT ptDoc, 
		RT::Page** ppHoverPage, 
		RT::Line** ppHoverLine, 
		RT::Run** ppHoverRun);

	void  OnNodeRemove(RT::Node*);

private:
    void  doc_point_2_page_point(const POINT& ptDoc, POINT& ptPage);
    RT::Run*  get_nearest_run(const POINT& ptDoc);

private:
	RT::Doc&  m_doc;
	IUIApplication*  m_pUIApp;  // 用于tooltip

	RT::Page*  m_pHoverPage;
    RT::Line*  m_pHoverLine;
    RT::Run*   m_pHoverRun;
    RT::Element*   m_pHoverElement;
    RT::Element*   m_pPressElement;

	// 鼠标按下
	bool  m_bLButtonPressDown;

//  RT::Unit*   m_pHoverUnit;
// 	RT::Unit*   m_pPressUnit;
};

}