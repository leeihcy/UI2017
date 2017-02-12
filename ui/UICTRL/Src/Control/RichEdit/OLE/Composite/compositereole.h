#pragma once
#include "..\reoleimpl.h"

// 复合OLE，内部由许多元素组成
namespace UI
{
class OleElement;

class CompositeREOle : public REOleImpl
{
public:
    typedef list<OleElement*> OLEELEMENTLIST;
    typedef list<OleElement*>::iterator OLEELEMENTITER;

    CompositeREOle();
    ~CompositeREOle();

public:
    bool  AddElement(OleElement* p);
    bool  IsOleElementExist(OleElement* p);
    bool  FindOleElement(OleElement* p, OLEELEMENTITER& iter);
    bool  RemoveElement(OleElement* p);
    bool  DestroyElement(OleElement* p);

    void  ControlPoint2OlePoint(POINT* ptControl, POINT* ptOle);
    OleElement*  HitTest(POINT ptInOle);

public:
    virtual LRESULT  ProcessMessage(
		UINT msg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) override;

protected:
    virtual void  OnSizeChanged(int cx, int cy) override;
    virtual void  OnDraw(HDC hDC, RECT* prc) override;

protected:
    virtual void  DrawBkg(HDC hDC){};
    virtual void  DrawForegnd(HDC hDC){};

    LRESULT  OnMouseMove(WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    LRESULT  OnMouseLeave(WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    LRESULT  OnLButtonDown(WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    LRESULT  OnLButtonDBClick(WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    LRESULT  OnLButtonUp(WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    LRESULT  OnSetCursor(WPARAM wParam, LPARAM lParam, BOOL& bHandled);

private:
    OLEELEMENTLIST  m_elementList;
    OleElement*  m_pHoverElement;
    OleElement*  m_pPressElement;
};

}