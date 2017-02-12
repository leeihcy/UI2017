#pragma once
#include "icaret.h"

//////////////////////////////////////////////////////////////////////////
// 对外部提供一套光标解决方案，当需要在分层窗口上面显示一个光标时，
// 将创建一个CCaretWindow，当需要在普通窗口上面显示一个光标时，则
// 调用系统的API
// 
//  2016.6.17 11:08
//
namespace UI
{

class CCaret
{
public:
	CCaret();
    ~CCaret();

	BOOL  CreateCaret(IObject* pObj, HBITMAP hbmp, int nWidth, int nHeight, 
        CARET_TYPE eType = CARET_TYPE_UNKNOWN, 
        ICaretUpdateCallback* pCallback = NULL);
	void  DestroyCaret(IObject* pObj, bool bRedraw);
	void  SetCaretPos(IObject* pObj, int x, int y, bool bRedraw=true);
	void  ShowCaret(IObject* pObj, bool bRedraw=true);
	void  HideCaret(IObject* pObj, bool bRedraw=true);
    void  OnControlPaint(IObject* pObj, IRenderTarget* p);
//     void  PreCommitWindowBuffer(HDC hMemDC);
//     void  PostCommitWindowBuffer(HDC hMemDC);

    CARET_TYPE  GetCaretType();

public:
    static ICaret*  m_pCaret;  // 全局唯一
    static POINT  m_ptLast;
};

}