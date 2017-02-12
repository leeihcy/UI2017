#include "stdafx.h"
#include "password_edit.h"
#include "edit_desc.h"
#include <atlenc.h>

PasswordEdit::PasswordEdit(IPasswordEdit* p) : Edit(p)
{
	m_pIPasswordEdit = p;
	m_pIPasswordEdit->SetDescription(PasswordEditDescription::Get());

	m_eMode = PASSWORDEDIT_MODE_PASSWORD;
	m_hHook = NULL;
	m_nTimerId = 0;
	m_nCreateThreadId = GetCurrentThreadId();
    m_bShowDefaultIng = false;
}

PasswordEdit::~PasswordEdit()
{
	
}

void  PasswordEdit::FinalRelease()
{
	SetMsgHandled(FALSE);

	KillTimer();
	UnInstallHook();
}


void  PasswordEdit::OnInitialize()
{
	__super::OnInitialize();

	OBJSTYLE  style = { 0 };
	style.enable_ime = 1;

	m_pIPasswordEdit->ModifyObjectStyle(0, &style);
}

#define DEFULAT_PASSWORD_CHAR_COUNT 6
TCHAR GetPasswordChar()
{
	return TEXT('*');
}

void  PasswordEdit::OnSetFocus(IObject* pOldFocusObj)
{
	SetMsgHandled(FALSE);
	InstallHook();
	SetTimer();

#define PASSWORD_EDIT_HOOK_TIMER_ELAPSE 500

	if (GetKeyState(VK_CAPITAL) & 0x1)
	{
		NotifyCapsLockChanged();
	}

	// 默认密码状态下，点击进入编辑后，清空，让用户重新输入
	if (m_bShowDefaultIng)
	{
		SetText(NULL);
	}
}

void  PasswordEdit::OnKillFocus(IObject* pNewFocusObj)
{
	SetMsgHandled(FALSE);
	UnInstallHook();
	KillTimer();

    UpdatePasswordCharInEdit();
}

static PasswordEdit* s_pActivePasswordEdit = NULL;

void  PasswordEdit::InstallHook()
{
	if (!m_hHook)
	{
		s_pActivePasswordEdit = this;
		m_hHook = ::SetWindowsHookEx(WH_KEYBOARD_LL, (HOOKPROC)PasswordEdit::LLKeyboardProc, g_hInstance, NULL);
	}

}

void  PasswordEdit::UnInstallHook()
{
	if (m_hHook)
	{
		if (s_pActivePasswordEdit == this)
			s_pActivePasswordEdit = NULL;

		::UnhookWindowsHookEx(m_hHook);
		m_hHook = NULL;
	}
}

void  PasswordEdit::SetTimer()
{
	UIASSERT(0 == m_nTimerId);
	IUIApplication* pUIApp = m_pIPasswordEdit->GetUIApplication();
	m_nTimerId = pUIApp->SetTimer(PASSWORD_EDIT_HOOK_TIMER_ELAPSE, m_pIPasswordEdit);
}

void  PasswordEdit::KillTimer()
{
	if (m_nTimerId)
	{
		IUIApplication* pUIApp = m_pIPasswordEdit->GetUIApplication();
		pUIApp->KillTimer(m_nTimerId);
	}
}

void  PasswordEdit::OnTimer(UINT_PTR nIDEvent, LPARAM lParam)
{
	if (nIDEvent == m_nTimerId)
	{
		SetMsgHandled(TRUE);

		UnInstallHook();
		InstallHook();
		return ;
	}

	SetMsgHandled(FALSE);
}

bool IsPasswordChar(UINT ch)
{
	return (ch >= 0x21 && ch <= 0x7E);
}

bool IsMovePosKey(UINT ch)
{
	switch (ch)
	{
	case VK_PRIOR:
	case VK_NEXT:
	case VK_END:
	case VK_HOME:
	case VK_LEFT:
	case VK_UP:
	case VK_RIGHT:
	case VK_DOWN:
		return true;
	}
	return false;
}

bool IsFunctionKey(UINT ch)
{
	switch (ch)
	{
	case VK_RETURN:
	case VK_ESCAPE:
	case VK_TAB:
	case VK_DELETE:
	case VK_BACK:
	case 0x41://Ctrl+A
		return true;
	}
	return false;
}

void Base64Encode(const char* src, string& out)
{
	out.clear();
	if (!src || !src[0])
		return;

	int length = strlen(src);
	int destlen = Base64EncodeGetRequiredLength(length);

	char* pOut = new char[destlen];
	memset(pOut, 0, sizeof(char)*destlen);
	
	BOOL bRet = ATL::Base64Encode((const BYTE*)src, length, pOut, &destlen);
	UIASSERT(bRet);

	out = pOut;
	delete[] pOut;
}

void Base64Decode(const char* src, string& out)
{
	out.clear();
	if (!src || !src[0])
		return;

	int length = strlen(src);
	int destlen = Base64DecodeGetRequiredLength(length);

	BYTE* pOut = new BYTE[destlen];
	memset(pOut, 0, sizeof(BYTE)*destlen);

	BOOL bRet = ATL::Base64Decode(src, length, pOut, &destlen);
	UIASSERT(bRet);

	out = (char*)pOut;
	delete[] pOut;
}

LRESULT CALLBACK PasswordEdit::LLKeyboardProc(int code, WPARAM wParam, LPARAM lParam)
{
	PKBDLLHOOKSTRUCT pStruct = NULL;

	if (!s_pActivePasswordEdit)
		return 0L;

	if (code == HC_ACTION)
	{
		BYTE keyState[256] = { 0 };
		unsigned short uAscii[4] = { 0 };
		BYTE tInput[2] = { 0 };
		pStruct = (PKBDLLHOOKSTRUCT)lParam;
		if ((UINT)wParam == WM_KEYDOWN)
		{
			if (GetAsyncKeyState(VK_CONTROL) & 0x8000)
				return TRUE;

			GetKeyboardState(keyState);
			keyState[VK_SHIFT] = (BYTE)GetKeyState(VK_SHIFT);
			keyState[VK_CONTROL] = (BYTE)GetKeyState(VK_CONTROL);
			keyState[VK_NUMLOCK] = (BYTE)GetKeyState(VK_NUMLOCK);

			if (!/*s_pActivePasswordEdit->*/IsMovePosKey((UINT)pStruct->vkCode))
			{
				int iRet = ToAscii((UINT)pStruct->vkCode, (UINT)pStruct->scanCode, keyState, uAscii, 0);
				if (iRet == 1)
				{
					tInput[0] = (BYTE)uAscii[0];
				}
				else if (iRet == 0)
				{
					return iRet;
				}
				if (/*s_pActivePasswordEdit->*/IsPasswordChar((UINT)tInput[0]))
				{
					string str((char*)tInput);
					s_pActivePasswordEdit->SetHasInputContent(true);
					s_pActivePasswordEdit->OnHookPasswordChar(str);

					return TRUE;
				}
				else
				{
					if (!/*s_pActivePasswordEdit->*/IsFunctionKey((UINT)pStruct->vkCode))
						return TRUE;
				}
			}

		}
		else if ((UINT)wParam == WM_KEYUP)
		{
			GetKeyboardState(keyState);
			keyState[VK_SHIFT] = (BYTE)GetKeyState(VK_SHIFT);
			keyState[VK_CONTROL] = (BYTE)GetKeyState(VK_CONTROL);
			keyState[VK_NUMLOCK] = (BYTE)GetKeyState(VK_NUMLOCK);

			// 在密码框回车时，有可能要弹“请输入密码”气泡，这时再发送
			// 大小写状态通知，会导致气泡立刻又被关闭了
			if (VK_RETURN != pStruct->vkCode)
			{
				s_pActivePasswordEdit->NotifyCapsLockChanged();
			}

			if (!/*s_pActivePasswordEdit->*/IsMovePosKey((UINT)pStruct->vkCode))
			{
				int iRet = ToAscii((UINT)pStruct->vkCode, (UINT)pStruct->scanCode, keyState, uAscii, 0);
				if (iRet == 1)
				{
					tInput[0] = (BYTE)uAscii[0];
				}
				else if (iRet == 0)
				{
					return iRet;
				}
				if (/*s_pActivePasswordEdit->*/IsPasswordChar((UINT)tInput[0]))
				{
					return TRUE;
				}
			}
		}
	}

	return CallNextHookEx(s_pActivePasswordEdit->m_hHook, code, wParam, lParam);
}

void PasswordEdit::OnHookPasswordChar(string strText)
{
	string strEncode;
	Base64Encode(strText.c_str(), strEncode);

	int nSelStart = 0;
	int nSelLength = 0;
	__super::GetSel2(nSelStart, nSelLength);

	if (nSelLength > 0 || m_bShowDefaultIng)
	{
		if (m_bShowDefaultIng)
		{
			MapErase(0, m_rawPassword.size());
			m_bShowDefaultIng = false;
			nSelStart = 0;
		}
		else
		{
			MapErase(nSelStart, nSelLength);
		}
	}

	int nLimit = __super::GetLimit();  // -1 表示没限制
	if (nLimit > 0 && ((int)m_rawPassword.size() >= nLimit))
		return;

	MapInsert(nSelStart, strEncode);
	nSelStart += 1;

	UpdatePasswordCharInEdit();
	__super::SetSel(nSelStart, nSelStart);
}


void  PasswordEdit::MapInsert(int nKey, string value)
{
	ATLASSERT(nKey >= 0 && nKey <= (int)m_rawPassword.size());

	// move element
	int oldSize = m_rawPassword.size();
	for (int i = oldSize; i > nKey; i--)
	{
		m_rawPassword[i] = m_rawPassword[i - 1];
	}

	m_rawPassword[nKey] = value;
}

void  PasswordEdit::MapErase(int startKey, int count)
{
	ATLASSERT(startKey + count <= (int)m_rawPassword.size());

	// forward move element
	int oldSize = m_rawPassword.size();
	for (int i = startKey; i < oldSize - count; i++)
	{
		m_rawPassword[i] = m_rawPassword[i + count];
	}

	// delete last element
	while (count >= 1)
	{
		m_rawPassword.erase(m_rawPassword.size() - 1);
		--count;
	}
}


// 设置密码框中显示的*数量
void PasswordEdit::UpdatePasswordCharInEdit()
{
    // 默认密码状态下显示几个*
    if (m_bShowDefaultIng)
    {
        String strText;
        strText.append(DEFULAT_PASSWORD_CHAR_COUNT, GetPasswordChar());
        SetText(strText.c_str());
    }
    else
    {
        String strEditText;
        GetPasswordCharInEdit(strEditText);
        __super::SetText(strEditText.c_str());
    }
}

// 获取密码框中显示的*数量
void  PasswordEdit::GetPasswordCharInEdit(String& str)
{
	if (m_eMode == PASSWORDEDIT_MODE_PASSWORD)
	{
		str.clear();
		str.insert(0, (int)m_rawPassword.size(), GetPasswordChar());
	}
	else if (m_eMode == PASSWORDEDIT_MODE_PLAIN)
	{
		GetRealPassword(str);
	}
}

LRESULT  PasswordEdit::OnSetRealPassword(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LPCTSTR szNewPassword = (LPCTSTR)wParam;
	m_rawPassword.clear();

    if (!szNewPassword)
    {
        m_bShowDefaultIng = true;
    }
    else
    {
        int nLength = 0;
        if (szNewPassword)
            nLength = _tcslen(szNewPassword);

        for (int i = 0; i < nLength; i++)
        {
            char b[2] = { LOBYTE(szNewPassword[i]), 0 };
            string strEncode;
            Base64Encode(b, strEncode);
            m_rawPassword[i] = strEncode;
        }

        // 	m_bHasInputContent = nLength > 0 ? true : false;
        m_bShowDefaultIng = false;
    }

	UpdatePasswordCharInEdit();

	SetSel(-1, 0);
	return 0;
}

LRESULT  PasswordEdit::OnGetRealPassword(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	bool* pbIsDefault = (bool*)wParam;
    return (LRESULT)GetRealPassword(pbIsDefault);
}
LPCTSTR  PasswordEdit::GetRealPassword(bool* pbIsDefault)
{
	if (GetCurrentThreadId() != m_nCreateThreadId)
        return TEXT("");

	if (pbIsDefault)
		*pbIsDefault = m_bShowDefaultIng;

	if (!m_bShowDefaultIng)
	{
		String& strText = GetTempBufferString();
		GetRealPassword(strText);

        return strText.c_str();
	}

    return TEXT("");
}

// 获取真实密码
void  PasswordEdit::GetRealPassword(String& str)
{
    for (int i = 0; i < (int)m_rawPassword.size(); i++)
    {
        string strOutBuf;
        Base64Decode(m_rawPassword[i].data(), strOutBuf);

        str.append((LPCTSTR)CA2T(strOutBuf.c_str()));
    }
}


LRESULT  PasswordEdit::IgnoreMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return 0;
}

void  PasswordEdit::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	switch (nChar)
	{
	case 'A':
	{
		if (IsKeyDown(VK_CONTROL))
		{
			__super::SetSel(0, -1);
		}
	}
	break;

	case VK_DELETE:
	{
		int nSelStart = 0;
		int nSelLength = 0;

		if (m_bShowDefaultIng)
		{
			// TODO: m_bDeleteFlag = true;
			m_bShowDefaultIng = false;
		}
		else
		{

			__super::GetSel2(nSelStart, nSelLength);
			if (nSelStart < (int)m_rawPassword.size())
			{
				MapErase(nSelStart, nSelLength == 0 ? 1 : nSelLength);
			}
		}

		UpdatePasswordCharInEdit();
		__super::SetSel2(nSelStart, 0);
		break;
	}
	break;

	case VK_BACK:
	{
		int nSelStart = 0;
		int nSelLength = 0;

		if (m_rawPassword.size() > 0)
		{
			__super::GetSel2(nSelStart, nSelLength);

			if (nSelStart > 0 && nSelLength == 0)
			{
				nSelStart = nSelStart - 1;
				MapErase(nSelStart);
			}
			else if (nSelLength > 0)
			{
				MapErase(nSelStart, nSelLength);
			}
		}
		if (m_bShowDefaultIng)
		{
			// TODO: m_bDeleteFlag = true;
			m_bShowDefaultIng = false;
		}
		UpdatePasswordCharInEdit();
		__super::SetSel2(nSelStart, 0);
	}
	break;

	default:
	{
		// 继续由richedit去处理
		if (IsMovePosKey(nChar) || IsFunctionKey(nChar))
		{
			SetMsgHandled(FALSE);
			return;
		}
	}
	break;
	}
}
void  PasswordEdit::NotifyCapsLockChanged()
{
	UIMSG  msg;
	msg.message = UI_MSG_NOTIFY;
	msg.nCode = UI_PASSWORDEDIT_NM_CAPSLOCK_CHANGED;
	msg.pMsgFrom = m_pIPasswordEdit;
	msg.wParam = (WPARAM)m_pIPasswordEdit;
	msg.lParam = (LPARAM)(GetKeyState(VK_CAPITAL) & 0x1);  // 直接调用Util::IsKeyDown拿到的结果不对

	long lRet = m_pIPasswordEdit->DoNotify(&msg);
	if (!lRet)
	{
		// DO Self Process
	}
}

LRESULT PasswordEdit::OnSwitchMode(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	PASSWORDEDIT_MODE eNewMode = (PASSWORDEDIT_MODE)wParam;
	if (eNewMode == m_eMode)
		return 0;

	m_eMode = eNewMode;
	UpdatePasswordCharInEdit();
	return 0;
}