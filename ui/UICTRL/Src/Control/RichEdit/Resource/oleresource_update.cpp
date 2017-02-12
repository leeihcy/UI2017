#include "stdafx.h"
#include "oleresource_update.h"
using namespace UI;

namespace UI
{

REOleResUpdateManager  g_REOleResUpdateManager;

bool  RegisterREOleResUpdate(LPCTSTR  szMD5, IREOleResUpdateCallback* pCallback, long lData)
{
	if (!szMD5)
		return false;

	REOleMD5Res* pRes = g_REOleResUpdateManager.Find(szMD5);
	if (!pRes)
	{
		pRes = g_REOleResUpdateManager.Add(szMD5);
	}
	pRes->Add(pCallback, lData);
	return true;
}

bool  UnRegisterREOleResUpdate(LPCTSTR  szMD5, IREOleResUpdateCallback* pCallback)
{
	if (!szMD5)
		return false;

	REOleMD5Res* pRes = g_REOleResUpdateManager.Find(szMD5);
	if (!pRes)
		return false;

	return pRes->Delete(pCallback);
}

}

namespace UI
{
	extern "C"
	void UICTRL_API UpdateREOleRes(LPCTSTR  szMD5, REOleLoadStatus e, LPCTSTR szPath)
	{
		g_REOleResUpdateManager.OnREOleResUpdate(szMD5, e, szPath);
	}
}

//////////////////////////////////////////////////////////////////////////

REOleMD5Res::REOleMD5Res(LPCTSTR szMD5)
{
	if (szMD5)
		m_strMD5 = szMD5;
}

bool  REOleMD5Res::IsEqual(LPCTSTR szMD5)
{
	if (szMD5 && 0 == m_strMD5.compare(szMD5))
		return true;
	return false;
}

bool  REOleMD5Res::IsEmpty()
{
	return m_listItem.empty();
}

void  REOleMD5Res::Add(IREOleResUpdateCallback* pCallback, long lData)
{
	REOleResUpdateItem  item = {0};
	item.pCallback = pCallback;
	item.lData = lData;

	m_listItem.push_back(item);
}

bool  REOleMD5Res::Delete(IREOleResUpdateCallback* pCallback)
{
	list<REOleResUpdateItem>::iterator iter = m_listItem.begin();
	for (; iter != m_listItem.end(); iter++)
	{
		if (iter->pCallback == pCallback)
		{
			m_listItem.erase(iter);
			return true;
		}
	}
	return false;
}

void  REOleMD5Res::OnUpdate(REOleLoadStatus e, LPCTSTR szPath)
{
	list<REOleResUpdateItem>::iterator iter = m_listItem.begin();
	for (; iter != m_listItem.end(); iter++)
	{
		iter->pCallback->OnREOleResUpdate(e, szPath, iter->lData);
	}
}

//////////////////////////////////////////////////////////////////////////

REOleResUpdateManager::REOleResUpdateManager()
{
	
}
REOleResUpdateManager::~REOleResUpdateManager()
{
	Clear();
}

REOleMD5Res*  REOleResUpdateManager::Add(LPCTSTR  szMD5)
{
	if (!szMD5)
		return NULL;

	if (Find(szMD5))
		return NULL;

	REOleMD5Res* p = new REOleMD5Res(szMD5);
	m_listMD5Res.push_back(p);
	return p;
}
void  REOleResUpdateManager::Delete(LPCTSTR  szMD5)
{
	if (!szMD5)
		return;

	list<REOleMD5Res*>::iterator iter = m_listMD5Res.begin();
	for (; iter != m_listMD5Res.end(); iter++)
	{
		if ((*iter)->IsEqual(szMD5))
		{
			REOleMD5Res* pRes = *iter;

			m_listMD5Res.erase(iter);
			delete pRes;

			return;
		}
	}
}
REOleMD5Res*  REOleResUpdateManager::Find(LPCTSTR  szMD5)
{
	if (!szMD5)
		return NULL;

	list<REOleMD5Res*>::iterator iter = m_listMD5Res.begin();
	for (; iter != m_listMD5Res.end(); iter++)
	{
		if ((*iter)->IsEqual(szMD5))
			return (*iter);
	}
	return NULL;
}
void  REOleResUpdateManager::Clear()
{
	list<REOleMD5Res*>::iterator iter = m_listMD5Res.begin();
	for (; iter != m_listMD5Res.end(); iter++)
	{
		REOleMD5Res*  p = *iter;
		delete p;
	}
	m_listMD5Res.clear();
}

void  REOleResUpdateManager::OnREOleResUpdate(LPCTSTR  szMD5, REOleLoadStatus e, LPCTSTR szPath)
{
	list<REOleMD5Res*>::iterator iter = m_listMD5Res.begin();
	for (; iter != m_listMD5Res.end(); iter++)
	{
		REOleMD5Res* pRes = *iter;
		if (pRes->IsEqual(szMD5))
		{
			pRes->OnUpdate(e, szPath);

			// TODO: 失败允许重试，其它情况呢
			if (e == REOleLoadStatus_SUCCESS)
			{
				m_listMD5Res.erase(iter);
				delete pRes;
			}
			return;
		}
	}
}