#pragma once
#include "edit.h"
#include "Inc\Interface\iedit.h"

namespace UI
{
class PasswordEdit : public Edit
{
public:
	PasswordEdit(IPasswordEdit*);
	~PasswordEdit();

	UI_BEGIN_MSG_MAP()
		UIMSG_TIMER(OnTimer)
		MSG_WM_KEYDOWN(OnKeyDown)
		UIMSG_HANDLER_EX(WM_CHAR, IgnoreMessage)
		UIMSG_HANDLER_EX(WM_KEYUP, IgnoreMessage)
		UIMSG_SETFOCUS(OnSetFocus)
		UIMSG_KILLFOCUS(OnKillFocus)
		UIMSG_HANDLER_EX(UI_PASSWORDEDIT_MSG_GETREALPASSWORD, OnGetRealPassword)
		UIMSG_HANDLER_EX(UI_PASSWORDEDIT_MSG_SETREALPASSWORD, OnSetRealPassword)
		UIMSG_HANDLER_EX(UI_PASSWORDEDIT_MSG_SWITCHMODE, OnSwitchMode)
		UIMSG_INITIALIZE(OnInitialize)
		UIMSG_FINALRELEASE(FinalRelease)
	UI_END_MSG_MAP_CHAIN_PARENT(Edit)

public:
	void  GetRealPassword(BSTR* pbstr, bool* pbIsDefault);


protected:
	void  FinalRelease();
	void  OnInitialize();
	void  OnSetFocus(IObject* pOldFocusObj);
	void  OnKillFocus(IObject* pNewFocusObj);
	void  OnTimer(UINT_PTR nIDEvent, LPARAM lParam);
	LRESULT  OnSetRealPassword(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT  OnGetRealPassword(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT  IgnoreMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT  OnSwitchMode(UINT uMsg, WPARAM wParam, LPARAM lParam);
	void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

	// 不弹菜单 
	virtual void virtualShowContentMenu(CPoint point) override {}

protected:
	void  InstallHook();
	void  UnInstallHook();
	void  SetTimer();
	void  KillTimer();

	void  MapErase(int startKey, int count = 1);
	void  MapInsert(int nKey, string value);

	void  UpdatePasswordCharInEdit();
	void  GetPasswordCharInEdit(String& str);
    void  GetRealPassword(String& str);
    LPCTSTR  GetRealPassword(bool* pbIsDefault);

	void  OnHookPasswordChar(string str);
	void  SetHasInputContent(bool) {};
	void  NotifyCapsLockChanged();
	static LRESULT CALLBACK LLKeyboardProc(int code, WPARAM wParam, LPARAM lParam);


protected:
	IPasswordEdit*  m_pIPasswordEdit;
	HHOOK  m_hHook;
	UINT_PTR  m_nTimerId;
	DWORD  m_nCreateThreadId;    // 创建控件的线程id

	PASSWORDEDIT_MODE  m_eMode;  // 原文模式，直接显示密码（即普通edit模式）

    bool  m_bShowDefaultIng;     // 显示默认密码
	map<int, string>  m_rawPassword;
};
}