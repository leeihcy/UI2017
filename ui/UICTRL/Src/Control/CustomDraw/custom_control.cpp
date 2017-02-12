#include "stdafx.h"
#include "custom_control.h"
#include "custom_control_desc.h"

namespace UI
{
CustomControl::CustomControl(ICustomControl* p) :
	m_pICustomControl(p), MessageProxy(p)
{
	p->SetDescription(CustomControlDescription::Get());
	
	OBJSTYLE s = {0};
	s.default_reject_all_mouse_msg = 1;  // 默认不接收鼠标消息

	OBJSTYLE sRemove = { 0 };
	sRemove.default_tabstop = 1;     // 没有焦点
	p->ModifyObjectStyle(&s, &sRemove);
}

CustomControl::~CustomControl()
{

}

void CustomControl::OnPaint( IRenderTarget* pRenderTarget )
{
	draw.emit(m_pICustomControl, pRenderTarget);
}

LRESULT CustomControl::OnMouseMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	MSG msg;
	memset(&msg, 0, sizeof(&msg));
	msg.message = uMsg;
	msg.wParam = wParam;
	msg.lParam = lParam;

	mouse.emit(m_pICustomControl, &msg);
	return 0;
}

}