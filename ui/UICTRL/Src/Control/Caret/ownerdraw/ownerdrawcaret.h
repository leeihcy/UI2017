#pragma once
#include "UISDK\Control\Src\Control\Caret\icaret.h"
#include "UISDK\Kernel\Inc\Interface\imessage.h"

namespace UI
{

class OwnerDrawCaret : public ICaret
{
public:
    OwnerDrawCaret();
    ~OwnerDrawCaret();

    virtual BOOL  Create(IObject* pObj, HWND hWndParent, HBITMAP hbmp, int nWidth, int nHeight);
    virtual void  Destroy();
    virtual void  Show();
    virtual void  Hide();
    virtual void  SetPos(int x, int y);
    virtual void  OnControlPaint(IRenderTarget* p){};

//     virtual void  PreCommitWindowBuffer(HDC hMemDC){};
//     virtual void  PostCommitWindowBuffer(HDC hMemDC);

    virtual CARET_TYPE  GetType() { return CARET_TYPE_OWNERDRAW; }
    void  DoPaint();

protected:
    void  CreateTimer();
    void  DestroyTimer();

    void  SetPosBase(int x, int y);

private:
    HWND   m_hWnd;
    IObject*  m_pObject;
    POINT  m_ptLast;
    SIZE   m_sizeCaret;

    CRect  m_rcDraw;
    UINT_PTR  m_nTimerId;
    bool   m_bVisible;
};

}