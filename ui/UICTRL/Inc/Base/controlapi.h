#ifndef _UI_CONTROLAPI_H_
#define _UI_CONTROLAPI_H_

namespace UI
{
	interface IUIApplication;
// interface ITrayIcon;
}

extern "C" UICTRL_API 
bool  UICTRL_RegisterUIObject(UI::IUIApplication* p);
// extern "C" UICTRL_API
// bool  UICreateTrayIcon(UI::IUIApplication* pUIApplication, UI::ITrayIcon** ppOut);

#endif  