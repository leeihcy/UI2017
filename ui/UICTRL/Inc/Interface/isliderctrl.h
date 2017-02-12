#include "..\..\..\UISDK\Inc\Interface\icontrol.h"
#ifndef ISLIDERCTRL_H_955F5E3E_3091_4ca4_96CD_52929B078315
#define ISLIDERCTRL_H_955F5E3E_3091_4ca4_96CD_52929B078315

namespace UI
{
	interface ISliderCtrl;

#define SLIDER_STYLE_POINT_LEFT         0x0100    // 滑块按钮箭头指针左侧 默认为无指向
#define SLIDER_STYLE_POINT_TOP          0x0100    // 滑块按钮箭头指针上侧
#define SLIDER_STYLE_POINT_RIGHT        0x0200    // 滑块按钮箭头指针右侧
#define SLIDER_STYLE_POINT_BOTTOM       0x0200    // 滑块按钮箭头指针下侧
#define SLIDER_STYLE_SHOWTICKS          0x0400    // 显示刻度，默认只显示第一个和最后一个值的刻度
#define SLIDER_STYLE_AUTOTICKS          0x0800    // 自动显示刻度，默认每增一个值显示一个刻度

	struct SliderPosChangedParam
	{
		ISliderCtrl* pCtrl;
		int pos;
		int type;
	};
//
//	滑动条位置改变通知
//
//		message:UI_WM_NOTIFY
//		code:   UI_TRBN_POSCHANGED
//		wparam: new Pos
//		lParam: scroll type, SB_LEFT SB_RIGHT SB_THUMBTRACK SB_ENDSCROLL ...
//		pMsgFrom: this
//
#define UI_TRBN_POSCHANGED  130172216

// void OnTrbnPosChanged( int nPos, int nScrollType );
#define UIMSG_TRBN_POSCHANGED_ID(OBJID, func)         \
    if (uMsg == UI_WM_NOTIFY  &&                      \
        code == UI_TRBN_POSCHANGED &&                 \
        NULL != pMsgFrom &&                           \
        0 == _tcscmp(((IObject*)pMsgFrom)->GetId(), OBJID)) \
    {                                                 \
        SetMsgHandled(TRUE);                          \
        func ((int)wParam, (int)lParam);              \
        if (IsMsgHandled())                           \
            return TRUE;                              \
    }

// void OnTrbnPosChanged( int nPos, int nScrollType );
#define UIMSG_TRBN_POSCHANGED( p, func )              \
    if (uMsg == UI_WM_NOTIFY  &&                      \
        code == UI_TRBN_POSCHANGED &&                 \
        NULL != pMsgFrom &&                           \
        (pMsgFrom) == (UI::IMessage*)p)               \
    {                                                 \
        SetMsgHandled(TRUE);                          \
        func( (int)wParam, (int)lParam );             \
        if (IsMsgHandled())                           \
            return TRUE;                              \
    }

// 拖拽时，向外部请求要显示的提示文本
// wParam: nCur Pos
// lParam: buffer [64]
#define UI_TRBN_DRAGTOOLTIPS_REQ  150291238

// void  OnSliderDragTooltipReq(long lPos, TCHAR szBuffer[64]);
#define UI_NOTIFY_SLIDERDRAGTOOLTIP_REQ( p, func )    \
    if (uMsg == UI_WM_NOTIFY  &&                      \
        code == UI_TRBN_DRAGTOOLTIPS_REQ &&           \
        NULL != pMsgFrom &&                           \
        (pMsgFrom) == (UI::IMessage*)p)               \
    {                                                 \
        SetMsgHandled(TRUE);                          \
        func( (int)wParam, (TCHAR*)lParam );          \
        if (IsMsgHandled())                           \
            return TRUE;                              \
    }



class SliderCtrl;
interface __declspec(uuid("73C1B080-7257-4F43-A8DB-C8FBADAF265F")) UICTRL_API ISliderCtrl : public IControl
{
    int   SetPos(int nPos, bool bUpdate=true);
    int   SetPage(int nPage, bool bUpdate=true);
    void  SetRange(int nLower, int nUpper, bool bUpdate=true);
    void  SetScrollInfo(LPCSCROLLINFO lpsi, bool bUpdate=true);

    void  SetLine(int nLine);
    int   GetPos();
    int   GetPage();
    int   GetLine();
    void  GetRange(int& nLower, int& nUpper);
    int   GetRange();

    void  SetDirectionType(PROGRESSBAR_SCROLL_DIRECTION_TYPE eType);
    PROGRESSBAR_SCROLL_DIRECTION_TYPE  GetDirectionType();
    int   SetTickFreq(int nFreq);

	signal<SliderPosChangedParam*>&  PosChangedEvent();
	UI_DECLARE_INTERFACE_ACROSSMODULE(SliderCtrl);
};

}

#endif // ISLIDERCTRL_H_955F5E3E_3091_4ca4_96CD_52929B078315