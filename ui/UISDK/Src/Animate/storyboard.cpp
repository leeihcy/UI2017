#include "stdafx.h"
#include "storyboard.h"
#include "timeline.h"
using namespace UIA;

Storyboard::Storyboard()
{
    m_pAnimateMgr = NULL;
    m_pIStoryboard = NULL;
    m_pNotify = NULL;
    m_nID = 0;
    m_pWParam = m_pLParam = 0;
    m_lDelayBeginTick = 0;
	m_ppRef = nullptr;
}
Storyboard::~Storyboard()
{
	if (m_ppRef)
		*m_ppRef = nullptr;

    DestroyAllTimeline();
	SAFE_DELETE(m_pIStoryboard);
}

IStoryboard*  Storyboard::GetIStoryboard()
{ 
	if (!m_pIStoryboard)
		m_pIStoryboard = new IStoryboard(this);

	return m_pIStoryboard;
}

void  Storyboard::SetAnimateMgr(AnimateManager*  p)
{
    m_pAnimateMgr = p;
}

bool  Storyboard::AddTimeline(Timeline* p)
{
    if (NULL == p)
        return false;

    if (FindTimeline(p->GetId()))
    {
        UIASSERT(0);
        return false;
    }

    m_listTimeline.push_back(p);
    return true;
}

bool  Storyboard::DestroyTimeline(int nTimelineId)
{
    TimelineIter iter = m_listTimeline.begin();
    for (; iter != m_listTimeline.end(); iter++)
    {
        Timeline*  p = *iter;
        if (p->GetId() == nTimelineId)
        {
            m_listTimeline.erase(iter);
            SAFE_DELETE(p);
            return true;
        }
    }
    return false;
}

IdleTimeline*  Storyboard::CreateIdleTimeline(int nTimelineId)
{
	IdleTimeline* p = new IdleTimeline();
	p->SetAnimateMgr(m_pAnimateMgr);
	p->SetId(nTimelineId);

	if (false == AddTimeline(p))
	{
		SAFE_DELETE(p);
		return NULL;
	}

	return p;
}

FromToTimeline*  Storyboard::CreateTimeline(int nId)
{
	FromToTimeline* p = new FromToTimeline;
	p->SetAnimateMgr(m_pAnimateMgr);
	p->SetId(nId);

	if (!AddTimeline(p))
	{
		delete p;
		return NULL;
	}

	return p;
}

KeyFrameTimeline*  Storyboard::CreateKeyFrameTimeline(int nId)
{
	KeyFrameTimeline* p = new KeyFrameTimeline;
	p->SetAnimateMgr(m_pAnimateMgr);
	p->SetId(nId);

	if (!AddTimeline(p))
	{
		delete p;
		return NULL;
	}

	return p;
}

void  Storyboard::DestroyAllTimeline()
{
    TimelineIter iter = m_listTimeline.begin();
    for (; iter != m_listTimeline.end(); iter++)
    {
        Timeline*  p = *iter;
        SAFE_DELETE(p);
    }
    m_listTimeline.clear();
}

Timeline*  Storyboard::GetTimeline(unsigned int nIndex)
{
    if (nIndex >= m_listTimeline.size())
        return NULL;

    return m_listTimeline[nIndex];
}
Timeline*  Storyboard::FindTimeline(int nTimelineId)
{
    TimelineIter iter = m_listTimeline.begin();
    for (; iter != m_listTimeline.end(); iter++)
    {
        if ((*iter)->GetId() == nTimelineId)
            return *iter;
    }

    return NULL;
}

void  Storyboard::Begin()
{
    m_lDelayBeginTick = 0;
    m_pAnimateMgr->AddStoryboard(this);
}
void  Storyboard::BeginBlock()
{
    m_lDelayBeginTick = 0;
    m_pAnimateMgr->AddStoryboardBlock(this);
}

void  Storyboard::BeginFullCpu()
{
	m_lDelayBeginTick = 0;
	m_pAnimateMgr->AddStoryboardFullCpu(this);
}

// 延迟一段时候后再开始动画。
void  Storyboard::BeginDelay(long lElapse)
{
    m_lDelayBeginTick = GetTickCount() + lElapse;
    m_pAnimateMgr->AddStoryboard(this);
}

bool  Storyboard::IsDelayWaiting()
{
    return m_lDelayBeginTick?true:false;
}
void  Storyboard::ClearDelayWaiting()
{
    m_lDelayBeginTick = 0;
}

void  Storyboard::UpdateDelayWaiting()
{
    if (!m_lDelayBeginTick)
        return;

    if (GetTickCount() < m_lDelayBeginTick)
        return;

    m_lDelayBeginTick = 0;
}

void  Storyboard::SetEventListener(IAnimateEventCallback* pNotify)
{
    m_pNotify = pNotify;
}
IAnimateEventCallback*  Storyboard::GetEventListener()
{
    return m_pNotify;
}

void  Storyboard::OnAnimateStart()
{
    TimelineIter iter = m_listTimeline.begin();
    for (; iter != m_listTimeline.end(); iter++)
    {
        (*iter)->OnAnimateStart();
    }

	NotifyStart();
}

void  Storyboard::SetFinish()
{
    TimelineIter iter = m_listTimeline.begin();
    for (; iter != m_listTimeline.end(); iter++)
    {
        (*iter)->SetFinish();
    }
}

bool  Storyboard::IsFinish()
{   
    TimelineIter iter = m_listTimeline.begin();
    for (; iter != m_listTimeline.end(); iter++)
    {
        if (false == (*iter)->IsFinish())
            return false;
    }
    return true;
}

void  Storyboard::Cancel()
{
    m_pAnimateMgr->CancelStoryboard(this);
    // this 已被销毁
    return;
}

bool  Storyboard::OnTick()
{
    unsigned int nSize = m_listTimeline.size();
    for (unsigned int i = 0; i < nSize; i++)
    {
        m_listTimeline[i]->OnTick();
    }

    // 防止Notify过程中对象被销毁，先保存一份bFinish
    bool bFinish = IsFinish();

	E_ANIMATE_TICK_RESULT eResult = NotifyTick();  
    if (ANIMATE_TICK_RESULT_CANCEL == eResult)
        bFinish = true;

    return bFinish;
}

E_ANIMATE_TICK_RESULT  Storyboard::NotifyTick()
{
	if (m_pNotify)
		return m_pNotify->OnAnimateTick(m_pIStoryboard);

    return ANIMATE_TICK_RESULT_CONTINUE;
}
void  Storyboard::NotifyStart()
{
	if (m_pNotify)
		m_pNotify->OnAnimateStart(m_pIStoryboard);
}
void  Storyboard::NotifyEnd()
{
	if (m_pNotify)
		m_pNotify->OnAnimateEnd(m_pIStoryboard, ANIMATE_END_NORMAL);
}
void  Storyboard::NotifyCancel()
{
	if (m_pNotify)
		m_pNotify->OnAnimateEnd(m_pIStoryboard, ANIMATE_END_CANCEL);
}
void  Storyboard::NotifyDiscard()
{
	if (m_pNotify)
		m_pNotify->OnAnimateEnd(m_pIStoryboard, ANIMATE_END_DISCARD);
}
void  Storyboard::NotifyReverse()
{
	if (m_pNotify)
		m_pNotify->OnAnimateReverse(m_pIStoryboard);
}
void  Storyboard::NotifyRepeat()
{
	if (m_pNotify)
		m_pNotify->OnAnimateRepeat(m_pIStoryboard);
}

void UIA::Storyboard::SetRef(Storyboard** pp)
{
	if (pp)
	{
		UIASSERT(!m_ppRef);
	}
	m_ppRef = pp;
}
