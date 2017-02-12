#include "stdafx.h"
#include "reoletimer.h"
#include "..\reoleapi.h"

using namespace UI;

// Ê±¼ä¼ä¾à
#define REOLE_TIMER_ELAPSE  100

namespace UI
{
    REOleTimer  g_reoleTimer;
}

REOleTimer::REOleTimer()
{
    m_nTimerId = 0;
}
REOleTimer::~REOleTimer()
{
    KillTimer();
}


VOID CALLBACK TimerProc(
    HWND hwnd,
    UINT uMsg,
    UINT_PTR idEvent,
    DWORD dwTime
)
{
    g_reoleTimer.OnTimer();
}

void  REOleTimer::StartTimer()
{
    if (m_nTimerId)
    {
        KillTimer();
    }
    m_nTimerId = ::SetTimer(NULL, 1, REOLE_TIMER_ELAPSE, TimerProc);
}
void  REOleTimer::KillTimer()
{
    if (m_nTimerId)
    {
        ::KillTimer(NULL, m_nTimerId);
        m_nTimerId = 0;
    }
}

void  REOleTimer::OnTimer()
{
    _MyIter iter = m_items.begin();
	for (; iter != m_items.end(); iter++)
	{
		iter->pCallback->OnREOleTimer();
	}
}

bool  REOleTimer::Register(IREOleTimerCallback*  pCallback)
{
	if (!pCallback)
		return false;
	if (FindItem(pCallback))
		return false;

	REOleTimerItem item = {0};
	item.pCallback = pCallback;
	m_items.push_back(item);

	if (0 == m_nTimerId)
	{
		StartTimer();
	}

	return true;
}

bool  REOleTimer::UnRegister(IREOleTimerCallback*  pCallback)
{
	_MyIter iter = m_items.end();
	if (!FindItem(pCallback, iter))
		return false;

	m_items.erase(iter);

	if (m_items.empty())
	{
		KillTimer();
	}
	return true;
}

bool  REOleTimer::FindItem(IREOleTimerCallback*  pCallback, __out _MyIter& iter)
{
	iter = m_items.begin();
	for (; iter != m_items.end(); iter++)
	{
		if ((*iter).pCallback == pCallback)
			return true;
	}

	return false;
}

REOleTimerItem*  REOleTimer::FindItem(IREOleTimerCallback*  pCallback)
{
	_MyIter iter = m_items.end();
	if (!FindItem(pCallback, iter))
		return NULL;

	return &(*iter);
}


namespace UI
{

bool  RegisterREOleTimer(IREOleTimerCallback*  pCallback)
{
	return g_reoleTimer.Register(pCallback);
}
bool  UnRegisterREOleTimer(IREOleTimerCallback*  pCallback)
{
	return g_reoleTimer.UnRegister(pCallback);
}

}