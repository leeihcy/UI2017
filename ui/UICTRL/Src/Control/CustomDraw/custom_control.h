#pragma once
#include "Inc\Interface\icustom_control.h"

namespace UI
{

class CustomControl : public MessageProxy
{
public:
	CustomControl(ICustomControl*);
	~CustomControl();

	UI_BEGIN_MSG_MAP()
		UIMSG_PAINT(OnPaint)
		MESSAGE_RANGE_HANDLER_EX(WM_MOUSEFIRST, WM_MOUSELAST, OnMouseMessage)
	UI_END_MSG_MAP_CHAIN_PARENT_Ixxx(CustomControl, IControl)

private:
	void  OnPaint(IRenderTarget* pRenderTarget);
	LRESULT OnMouseMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

private:
	ICustomControl*  m_pICustomControl;
public:
	signal2<ICustomControl*, IRenderTarget*> draw;
	signal2<ICustomControl*, MSG*> mouse;
};

}