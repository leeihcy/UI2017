#pragma once
#include "Inc\Interface\isliderctrl.h"

namespace UI
{
interface IButton;

// 滑块控件(带按钮的滚动条)
class SliderCtrl : public MessageProxy
{
public:
	SliderCtrl(ISliderCtrl* p);
	~SliderCtrl();

	UI_BEGIN_MSG_MAP()
		UIMSG_PAINT(OnPaint)
		UIMSG_SIZE(OnSize)
		MSG_WM_LBUTTONDOWN(OnLButtonDown)
		MSG_WM_KEYDOWN(OnKeyDown)
		MSG_WM_KEYUP(OnKeyUp)
		UIMSG_STATECHANGED(OnStateChanged)
		//UIMSG_STYLECHANGED(OnStyleChanged)
        UIMSG_QUERYINTERFACE(SliderCtrl)
        UIMSG_SERIALIZE(OnSerialize)
		UIMSG_INITIALIZE(OnObjectLoaded)

		// 按钮的事件
	UIALT_MSG_MAP(1)   
		MSG_WM_LBUTTONDOWN(OnBtnLButtonDown)	
		MSG_WM_MOUSEMOVE(OnBtnMouseMove)	
		MSG_WM_LBUTTONUP(OnBtnLButtonUp)
		MSG_WM_KEYDOWN(OnKeyDown)
		MSG_WM_KEYUP(OnKeyUp)

    UIALT_MSG_MAP(UIALT_CALLLESS)
        UIMSG_CREATEBYEDITOR(OnCreateByEditor)

	UI_END_MSG_MAP_CHAIN_PARENT_Ixxx(SliderCtrl, IControl)

public:
	void  OnSerialize(SERIALIZEDATA* pData);
    void  OnCreateByEditor(CREATEBYEDITORDATA* pData);

    int   SetPos(int nPos, bool bUpdate=true);
    int   SetPage(int nPage, bool bUpdate=true);
    void  SetRange(int nLower, int nUpper, bool bUpdate=true);
    void  SetScrollInfo(LPCSCROLLINFO lpsi, bool bUpdate=true);

    void  SetLine(int nLine){ m_nLine = nLine; }
    int   GetPos(){ return m_nCur; }
    int   GetPage(){ return m_nPage; }
    int   GetLine(){ return m_nLine; }
    void  GetRange(int& nLower, int& nUpper){ nLower = m_nMin; nUpper = m_nMax; }
    int   GetRange() { return m_nMax - m_nMin; }

    void  SetDirectionType(PROGRESSBAR_SCROLL_DIRECTION_TYPE eType) { m_eDirectionType = eType; }
    PROGRESSBAR_SCROLL_DIRECTION_TYPE  GetDirectionType() { return m_eDirectionType; }

	void  UpdateButtonRect();
	int   SetTickFreq(int nFreq);

protected:
	void  OnObjectLoaded();
	void  OnPaint(IRenderTarget* pRenderTarget);
	void  OnLButtonDown(UINT nFlags, POINT point);
	void  OnBtnLButtonDown(UINT nFlags, POINT point);
	void  OnBtnMouseMove(UINT nFlags, POINT point);
	void  OnBtnLButtonUp(UINT nFlags, POINT point);
	void  OnMove(POINT pt );
	void  OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags );
	void  OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	void  OnSize(UINT nType, int cx, int cy );
	void  OnStyleChanged(int nStyleType, LPSTYLESTRUCT lpStyleStruct);
	void  OnStateChanged(UINT nMask);

	void  UpdateTicksData();
	void  UpdateUIData(bool bCalBeginEnd=false, bool bUpdateButtonRect=true);
	void  CalcCurPosByUITrackPos(int nWindowPos);

	int   WindowPoint2UITrackPos(const POINT* ptWindow);
	
    int   FixCurValue(int nCur);

    void  show_tooltip();
    void  update_tooltip();
    void  hide_tooltip();

	void  firePosChanged(int pos, int type);

protected:
    ISliderCtrl*  m_pISliderCtrl;

    int  m_nMin;        // 最小值
    int  m_nMax;        // 最大值
    int  m_nCur;        // 当前位置
    int  m_nLine;       // 每次按键时，滚动的距离
    int  m_nPage;       // 一个页面的大小
    PROGRESSBAR_SCROLL_DIRECTION_TYPE   m_eDirectionType;       // 滚动条方向,类型

	// 界面显示数据
	int  m_nBegin;     // 对应m_nMin，进度条开始绘制的坐标，PADDING + half of button size
	int  m_nEnd;       // 对应m_nMax，进度条结束绘制的地方，Control Size - (PADDING + half of button size )
	int  m_nTrack;     // 对应m_nCur，当前按钮所在的坐标，middle of button

	// show
	IButton*  m_pButton;    // 拖拽用的按钮控件，子类负责实例化
	int  m_nDiffFix;   // 误差消除，拖动时，鼠标离core m_nTrack的偏差

	// 刻度绘制相关
	IRenderBase*  m_pTickRender; // 刻度绘制
	int  m_nFreq;       // auto tick时，指定显示刻度的频率
	struct TickInfo
	{
		int    nPos;              // 显示哪一个刻度
		CRect  rcRect;            // 该刻度最后要显示位置，（在大小、参数改变后，需要重新计算）
	};
	vector<TickInfo*>  m_vTickInfo;

    // TOOLTIP
    HWND  m_hToolTip;
    TOOLINFO m_toolinfo;

public:
	signal<SliderPosChangedParam*>  PosChanged;
};
}