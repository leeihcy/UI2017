#pragma once
#include "edit.h"
#include "..\UISDK\Inc\Interface\iuiinterface.h"

/*
    即时编辑器
    
    与普通edit的区别：
    1. hover时会高亮，用于提醒用户点击一下
    2. 非编辑状态下，文本由不是edit绘制，而是instantedit绘制，并支持显示...
    3. 编辑状态下，回车键、失焦会触发acccept notify
    4. 编辑状态下，Esc键会触发cancel notify

    使用场景：
    1. 列表控件中列表项名称修改
    2. im软件中的心情短语等
*/

namespace UI
{

class InstantEdit :  public Edit, public IPreTranslateMessage
{
public:    
    InstantEdit(IInstantEdit* p);
    ~InstantEdit();

	UI_BEGIN_MSG_MAP()
        UIMSG_PAINT(OnPaint)
        MSG_WM_KEYDOWN(OnKeyDown)
        UIMSG_HANDLER_EX(WM_LBUTTONDOWN, OnLButtonDown)
		UIMSG_HANDLER_EX(WM_LBUTTONDBLCLK, OnLButtonDown)
		UIMSG_HANDLER_EX(WM_RBUTTONDOWN, OnRButtonDown)
		UIMSG_HANDLER_EX(WM_SETCURSOR, OnSetCursor)
        MSG_WM_KILLFOCUS(OnKillFocus)
        MSG_WM_SETFOCUS(OnSetFocus)
		UIMSG_SERIALIZE(OnSerialize)
		UIMSG_FINALRELEASE(FinalRelease)
    UI_END_MSG_MAP_CHAIN_PARENT(Edit)

	bool  StartEdit();
	void  CancelEdit();

private:
    LRESULT  OnSetCursor(UINT uMsg, WPARAM wParam, LPARAM lParam);
    LRESULT  OnLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam);
    LRESULT  OnRButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam);
    void  OnSetFocus(HWND wndFocus);
    void  OnKillFocus(HWND wndFocus);
    void  OnPaint(IRenderTarget* pRenderTarget);
    void  OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	void  OnSerialize(SERIALIZEDATA* pData);
	void  FinalRelease();

	virtual BOOL PreTranslateMessage(MSG* pMsg) override;

private:
	void  acceptEdit();
	void  endEdit();

private:
    IInstantEdit*  m_pIInstantEdit;
	Color*   m_pColorQuiet; // 未编辑时的颜色

    bool  m_bEditing;
	String  m_strTextBeforeEdit;
};


}