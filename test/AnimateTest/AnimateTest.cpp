//
// libo 2017/2/8 
// 动画效率提升测试
//

#include "stdafx.h"
#include "AnimateTest.h"

#include "UI\UISDK\Inc\inc.h"
#include "UI\UICTRL\Inc\inc.h"
#include "ui\UISDK\Inc\Interface\iwindow.h"
#include "ui\UISDK\Inc\Interface\icustomwindow.h"
#include "ui\UISDK\Inc\Util\util.h"
#include "ui\UISDK\Inc\Interface\ianimate.h"
#pragma comment(lib,"uisdk.lib")
#pragma comment(lib,"uictrl.lib")


#define PI 3.141592653f
float __stdcall TimingFuction(float f)
{
	return sin(PI * f * 2);
}


class LoginWindow : 
	public UI::IWindowDelegate, 
	public UIA::IAnimateEventCallback
{
public:
	LoginWindow()
	{
		m_pWindow = nullptr;
	}
	~LoginWindow()
	{
		if (m_pWindow)
			m_pWindow->Release();
	}
	void  Create(UI::ISkinRes* p)
	{
		m_pWindow = UI::ICustomWindow::CreateInstance(p);
		m_pWindow->SetWindowMessageCallback(this);
		m_pWindow->EnableGpuComposite(true);
		m_pWindow->Create(L"login"); 

		m_pPanel = (UI::IPanel*)m_pWindow->FindObject(L"panel");
		//m_pPanel->EnableLayer(true);
		m_pWindow->ShowWindow();
	}

	virtual BOOL  OnWindowMessage(UINT msg, WPARAM wParam, LPARAM, LRESULT& lResult)
	{
		if (msg == WM_DESTROY)
		{
			UIA::IAnimateManager* mgr = m_pWindow->GetUIApplication()->GetAnimateMgr();
			mgr->ClearStoryboardByNotify(static_cast<UIA::IAnimateEventCallback*>(this));

			PostQuitMessage(0);
		}
		else if (msg == WM_MOUSEWHEEL)
		{
			
		}
		else if (msg == WM_NCLBUTTONDBLCLK)
		{
			UIA::IAnimateManager* mgr = m_pWindow->GetUIApplication()->GetAnimateMgr();
			//mgr->SetFps(100);

			UIA::IStoryboard* storyboard = mgr->CreateStoryboard(this);
			UIA::IFromToTimeline* timeline = storyboard->CreateTimeline(0)
				->SetParam(10,UI::ScaleByDpi(380), 800);
			timeline->SetRepeateCount(2);	
			timeline->SetEaseType(UIA::linear);
			timeline->SetAutoReverse(true);

			timeline = storyboard->CreateTimeline(1)
				->SetParam(UI::ScaleByDpi(200),0, 800);
			timeline->SetRepeateCount(2);
			timeline->SetAutoReverse(true);
			timeline->SetCustomTimingFuction(TimingFuction);

			// 占满CPU，软件渲染+移动形动画
			// storyboard->BeginFullCpu();

			storyboard->Begin();
		}
		return FALSE;
	}

	void  Destroy()
	{
		if (m_pWindow)
		{
			m_pWindow->DestroyWindow();
		}
	}

	virtual void  OnAnimateStart(UIA::IStoryboard*) 
	{
		m_pPanel->EnableLayer(true);
	}
	virtual void  OnAnimateEnd(UIA::IStoryboard*, UIA::E_ANIMATE_END_REASON e) 
	{
		m_pPanel->EnableLayer(false);
	}
	virtual UIA::E_ANIMATE_TICK_RESULT  OnAnimateTick(UIA::IStoryboard* s) 
	{ 
		int x = s->GetTimeline(0)->GetCurrentIntValue();
		int y = s->GetTimeline(1)->GetCurrentIntValue();
		
// 		m_pPanel->SetObjectPos(x, y, 0, 0, SWP_NOSIZE);
// 		m_pWindow->GetWindowRender()->InvalidateNow();
		
		m_pPanel->GetLayer()->SetTranslate(x, y, 0);
		m_pWindow->GetWindowRender()->InvalidateNow();

		return UIA::ANIMATE_TICK_RESULT_CONTINUE; 
	};

private:
	UI::ICustomWindow*  m_pWindow;
	UI::IPanel*  m_pPanel;
};

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UI::IUIApplication* pUIApp = NULL;
	UI::CreateUIApplication(&pUIApp);
	UICTRL_RegisterUIObject(pUIApp);

	pUIApp->EnableGpuComposite();

	// 顶级资源
	TCHAR szResPath[MAX_PATH] = { 0 };
	UI::Util::PathInBin(L"AniamteTestSkin", szResPath);
	UI::ISkinRes* pRootSkinRes = pUIApp->LoadSkinRes(szResPath);

	{
		LoginWindow m;
		m.Create(pRootSkinRes);
		m.Destroy();
	}

	//pUIApp->MsgHandleLoop();
	pUIApp->Release();

	return 0;
}