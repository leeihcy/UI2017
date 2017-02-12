#pragma once
#include "..\icaret.h"

namespace UI
{

// 由控件自己调用绘制，每次timer时重新刷新整个控件。
// 这种类型的光标一般只用于需要滚动的Edit/RichEdit上面，
// 解决光标绘制裁剪的问题。因此每次闪烁都有一定的开销

// 光标位置为控件客户区域

class ControlDrawCaret : public ICaret
{
public:
    ControlDrawCaret(ICaretUpdateCallback* pCallback);
    ~ControlDrawCaret();

    virtual CARET_TYPE  GetType() { return CARET_TYPE_CONTROLDRAW; }
    virtual BOOL  Create(IObject* pObj, HWND hWndParent, HBITMAP hbmp, int nWidth, int nHeight);
    virtual void  Destroy(bool bRedraw);
    virtual void  SetPos(int x, int y, bool bRedraw);
    virtual void  Show(bool bRedraw);
    virtual void  Hide(bool bRedraw);
    virtual void  OnControlPaint(IRenderTarget* p);
    virtual IObject*  GetObject() { return m_pObject; }

    void  OnTimer();
protected:
    void  CreateTimer();
    void  DestroyTimer();
    void  UpdateControl();
    void  UpdateObjectEx(RECT* prcArray, int nCount);

private:
    HWND   m_hWnd;
    HBITMAP  m_hBitmap;
    UI::IRenderBitmap*  m_pRenderBitmap;

    IObject*  m_pObject;
    POINT  m_ptLast;
    SIZE   m_sizeCaret;
    SIZE   m_sizeItalicFontCaretOffset;  // 用于解决RichEdit下面，大斜体字光标位置显示不正确问题

    
    CoordType  m_eCoordType;
    ICaretUpdateCallback*  m_pCallback;

    CRect  m_rcDraw;
    UINT_PTR  m_nTimerId;
    bool   m_bOn;  // 当前是否调用了ShowCaret
    bool   m_bVisible; // 记录光标的闪烁状态

};

}