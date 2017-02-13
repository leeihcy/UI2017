#pragma once
class Animate;

class LoginWindow :
	public UI::IWindowDelegate
{
public:
	LoginWindow();
	~LoginWindow();

	void  Create(UI::ISkinRes* p);
	void  Destroy();

private:
	void  ShowAnimate(long type);
	void  OnKeydown(UINT key);
	virtual BOOL  OnWindowMessage(UINT msg, WPARAM wParam, LPARAM, LRESULT& lResult);

private:
	UI::ICustomWindow*  m_pWindow;
	UI::IPanel*  m_pPanel;
};