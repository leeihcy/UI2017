#pragma once
#include "symbol.h"


void  leak_report(bool bQuitDebugMode);

void  lock_handle_message(UINT uMsg, WPARAM wParam, LPARAM lParam);

typedef void (*pfnInvokeMLDRes)();
void  suspend_thread_call(pfnInvokeMLDRes);



extern CRITICAL_SECTION  g_thread_cs;
class Lock
{
public:
    Lock()
    {
        EnterCriticalSection(&g_thread_cs);
    }
    ~Lock()
    {
        LeaveCriticalSection(&g_thread_cs);
    }
};
#define SCOPE_LOCK  Lock __lock__;
