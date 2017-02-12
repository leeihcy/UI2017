#pragma once
#include "Inc\Interface\iricheditole.h"

// REOLE 中的一些全局函数

namespace UI
{
	// 静态资源 
	void  SetREOleResUIApplication(IUIApplication* pUIApp);
	

	// 定时器
	interface IREOleTimerCallback
	{
		virtual void  OnREOleTimer() PURE;
	};
	bool  RegisterREOleTimer(IREOleTimerCallback*  pCallback);
	bool  UnRegisterREOleTimer(IREOleTimerCallback*  pCallback);


	// 资源更新
	interface IREOleResUpdateCallback
	{
		virtual void  OnREOleResUpdate(REOleLoadStatus e, LPCTSTR szPath, long lData) PURE;
	};

	bool  RegisterREOleResUpdate(LPCTSTR  szMD5, IREOleResUpdateCallback* pCallback, long lData);
	bool  UnRegisterREOleResUpdate(LPCTSTR  szMD5, IREOleResUpdateCallback* pCallback);
}