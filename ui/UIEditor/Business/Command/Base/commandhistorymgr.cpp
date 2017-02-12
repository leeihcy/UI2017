#include "stdafx.h"
#include "commandhistorymgr.h"
#include "icommand.h"
#include "Dialog/Framework/toolbar.h"


CCommandHistroy::CCommandHistroy()
{
	m_pKey = NULL;
    m_nMaxHistorySize = 100;
}

CCommandHistroy::~CCommandHistroy()
{
    Clear();
}

void  CCommandHistroy::Clear()
{
    {
        deque<ICommand*>::iterator iter = m_stack_undo.begin();
        deque<ICommand*>::iterator iterEnd = m_stack_undo.end();
        for (; iter != iterEnd; iter++ )
        {
            ICommand* p = *iter;
            SAFE_RELEASE(p);
        }
        m_stack_undo.clear();
    }

    clearRedo();

    NotifyUndoRedoCountChanged();
}

// 每次执行一个新Command时，将redo列表清空
void  CCommandHistroy::clearRedo()
{
    {
        deque<ICommand*>::iterator iter = m_stack_redo.begin();
        deque<ICommand*>::iterator iterEnd = m_stack_redo.end();
        for (; iter != iterEnd; iter++ )
        {
            ICommand* p = *iter;
            SAFE_RELEASE(p);
        }
        m_stack_redo.clear();
    }
}

HRESULT  CCommandHistroy::Execute(ICommand* p)
{
    if (NULL == p)
        return E_FAIL;

    int nRedoSize = m_stack_redo.size();
    int nUndoSize = m_stack_undo.size();
    if (nRedoSize + nRedoSize > m_nMaxHistorySize)
    {
        if (nUndoSize > 0)
        {
            ICommand* p = m_stack_undo.front();
            m_stack_undo.pop_front();
            SAFE_RELEASE(p);
        }
        else if (nRedoSize > 0)
        {
            ICommand* p = m_stack_redo.front();
            m_stack_redo.pop_front();
            SAFE_RELEASE(p);
        }
    }

    m_stack_undo.push_back(p);

    long lRet = p->execute();

    clearRedo();
    NotifyUndoRedoCountChanged();
    return lRet;
}

void  CCommandHistroy::Undo()
{
    if (m_stack_undo.size() > 0)
    {
        ICommand* p = m_stack_undo.back();
        m_stack_undo.pop_back();
        m_stack_redo.push_back(p);

        p->undo();
        NotifyUndoRedoCountChanged();
    }
}
void  CCommandHistroy::Redo()
{
    if (m_stack_redo.size() > 0)
    {
        ICommand* p = m_stack_redo.back();
        m_stack_redo.pop_back();
        m_stack_undo.push_back(p);

        p->execute();
        NotifyUndoRedoCountChanged();
    }
}

void  CCommandHistroy::NotifyUndoRedoCountChanged()
{
    if (g_pGlobalData && g_pGlobalData->m_pToolBar)
        g_pGlobalData->m_pToolBar->OnUndoRedoCountChanged(m_pKey
		/*(int)m_stack_undo.size(), (int)m_stack_redo.size()*/);
}

//////////////////////////////////////////////////////////////////////////

CCommandHistroyMgr::CCommandHistroyMgr() : IMessage(CREATE_IMPL_TRUE)
{
}
CCommandHistroyMgr::~CCommandHistroyMgr()
{
	_MyIter  iter = m_mapElement.begin();
	_MyIter  iterEnd = m_mapElement.end();

	for (; iter != iterEnd; iter++)
	{
		CCommandHistroy*  pElem = iter->second;
		pElem->Clear();

		delete pElem;
	}
	m_mapElement.clear();
}

HRESULT  CCommandHistroyMgr::Execute(ICommand* p)
{
    if (!p)
        return E_FAIL;

	long*  pKey = p->GetKey();

	_MyIter  iter = m_mapElement.find(pKey);
	if (iter != m_mapElement.end())
	{
		return iter->second->Execute(p);
	}
	else
	{
		CCommandHistroy*  pElem = new CCommandHistroy;
		pElem->m_pKey = pKey;
		m_mapElement[pKey] = pElem;

		return pElem->Execute(p);
	}
}

// 延迟执行命令。例如在菜单中执行一个删除命令，需要将这个菜单项也删除掉
void  CCommandHistroyMgr::PostExecute(ICommand* p)
{
	UIMSG  msg;
	msg.pMsgTo = static_cast<IMessage*>(this);
	msg.message = POST_EXECUTE_MSG;
	msg.lParam = (LPARAM)p;
	UIPostMessage(g_pGlobalData->m_pMyUIApp, &msg);
}
LRESULT  CCommandHistroyMgr::OnPostExecute(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	ICommand* p = (ICommand*)lParam;
	Execute(p);

	return 0;
}

void  CCommandHistroyMgr::Undo(long* pKey)
{
	CCommandHistroy*  pElem = GetElemByKey(pKey);
	if (pElem)
	{
		pElem->Undo();
	}
}
void  CCommandHistroyMgr::Redo(long* pKey)
{
	CCommandHistroy*  pElem = GetElemByKey(pKey);
	if (pElem)
	{
		pElem->Redo();
	}
}

bool  CCommandHistroyMgr::CloseElem(long* pKey)
{
	_MyIter  iter = m_mapElement.find(pKey);
	if (iter != m_mapElement.end())
	{
		CCommandHistroy*  pElem = iter->second;
		pElem->Clear();
		delete pElem;
		m_mapElement.erase(iter);
	}

	return true;
}

void  CCommandHistroyMgr::GetUndoRedoCount(long* pKey, int* pnUndo, int* pnRedo)
{
	_MyIter  iter = m_mapElement.find(pKey);
	if (iter != m_mapElement.end())
	{
		CCommandHistroy*  pElem = iter->second;

		if (pnUndo)
			*pnUndo = (int)pElem->m_stack_undo.size();
		if (pnRedo)
			*pnRedo = (int)pElem->m_stack_redo.size();
	}
}

CCommandHistroy*  CCommandHistroyMgr::GetElemByKey(long*  pKey)
{
	_MyIter  iter = m_mapElement.find(pKey);
	if (iter != m_mapElement.end())
	{
		CCommandHistroy*  pElem = iter->second;
		return pElem;
	}

	return NULL;
}