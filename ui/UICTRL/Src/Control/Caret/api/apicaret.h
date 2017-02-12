#pragma once
#include "..\icaret.h"

namespace UI
{

class ApiCaret : public ICaret
{
public:
    ApiCaret();
    ~ApiCaret();

    virtual BOOL  Create(IObject* pObj, HWND hWndParent, HBITMAP hbmp, int nWidth, int nHeight);
    virtual void  Destroy(bool bRedraw);
    virtual void  Show(bool bRedraw);
    virtual void  Hide(bool bRedraw);
    virtual void  SetPos(int x, int y,bool bRedraw);
    virtual void  OnControlPaint(IRenderTarget* p){};

    virtual IObject*  GetObject() {return m_pObject;}
//     virtual void  PreCommitWindowBuffer(HDC hMemDC);
//     virtual void  PostCommitWindowBuffer(HDC hMemDC);

    virtual CARET_TYPE  GetType() { return CARET_TYPE_API; }

private:
    HWND   m_hWnd;
    IObject*  m_pObject;
    POINT  m_ptLast;
    SIZE   m_sizeCaret;  // 光标创建时指定的大小
};

}