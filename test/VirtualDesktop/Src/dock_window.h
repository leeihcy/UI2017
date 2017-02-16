#pragma once
class Animate;

class DockWindow :
	public UI::IWindowDelegate
{
public:
	DockWindow();
	~DockWindow();

	void  Create(UI::ISkinRes* p);
	void  Destroy();

private:
	virtual BOOL  OnWindowMessage(UINT msg, WPARAM wParam, LPARAM, LRESULT& lResult);

private:
	UI::ICustomWindow*  m_pWindow;
};