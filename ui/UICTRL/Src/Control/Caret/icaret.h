#pragma once

namespace UI
{

enum CARET_TYPE
{
    CARET_TYPE_UNKNOWN = 0,
    CARET_TYPE_API = 1,
    CARET_TYPE_OWNERDRAW = 2,
    CARET_TYPE_WINDOW = 3,
    CARET_TYPE_CONTROLDRAW = 4,
};

enum CoordType
{
	COORD_WINDOW,
	COORD_CONTROL,
	COORD_CONTROLCLIENT,
};

interface ICaretUpdateCallback
{
    // 用于实现RE的光标延时刷新。因为RE也是延时刷新的
    virtual void  CaretInvalidateRect(RECT* prc, int nCount) = 0;

    // 用于解决RichEdit下面，大斜体字光标位置显示不正确问题
    virtual bool  NeedFixItalicCaretOffset() = 0;
};


interface ICaret
{
    virtual ~ICaret() = 0{};
    virtual BOOL  Create(IObject* pObj, HWND hWndParent, 
        HBITMAP hbmp, int nWidth, int nHeight) = 0;
    virtual void  Destroy(bool bRedraw) = 0;
    virtual void  Show(bool bRedraw) = 0;
    virtual void  Hide(bool bRedraw) = 0;
    virtual void  SetPos(int x, int y, bool bRedraw) = 0;
    virtual void  OnControlPaint(IRenderTarget* p) = 0;

    virtual CARET_TYPE  GetType() = 0;
    virtual IObject*  GetObject() = 0;
};

}