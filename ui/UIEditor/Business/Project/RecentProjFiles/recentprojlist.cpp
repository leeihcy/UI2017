#include "StdAfx.h"
#include "RecentProjList.h"
#include <time.h>
#include "3rd\markup\markup.h"

bool RecentProjItemCompare(const RecentProjItem* o1, const RecentProjItem* o2)
{   
	return o1->nLastVisitTime > o2->nLastVisitTime;
}

CRecentProjList::CRecentProjList(void)
{
}

CRecentProjList::~CRecentProjList(void)
{
	Clear();
}

void  CRecentProjList::Clear()
{
	vector<RecentProjItem*>::iterator  iter = m_arrRecentProj.begin();
	vector<RecentProjItem*>::iterator  iterEnd = m_arrRecentProj.end();
	for (; iter != iterEnd; iter++)
	{
		RecentProjItem* pItem = *iter;
		SAFE_DELETE(pItem);
	}
	m_arrRecentProj.clear();
}

RecentProjItem* CRecentProjList::GetItem(LPCTSTR szProjPath)
{
	vector<RecentProjItem*>::iterator  iter = m_arrRecentProj.begin();
	vector<RecentProjItem*>::iterator  iterEnd = m_arrRecentProj.end();
	for (; iter != iterEnd; iter++)
	{
		RecentProjItem* pItem = *iter;
		if (NULL == pItem)
			continue;
		
		if (0 == _tcsicmp(pItem->szFilePath, szProjPath))
		{
			return pItem;
		}
	}

	return NULL;
}

bool  CRecentProjList::Save()
{
	TCHAR szConfigXmlPath[MAX_PATH] = _T("");
	::GetUIEditorConfigFilePath(szConfigXmlPath);

	CMarkup  xml;
	if (false == xml.Load(szConfigXmlPath))
	{
		if (false == CreateEmptyXmlFile(szConfigXmlPath, _T("config"), NULL))
			return false;

		if (false == xml.Load(szConfigXmlPath))
			return false;
	}

	bool bRet = false; 
	do 
	{
		if (false == xml.NextElem()) break;
		if (false == xml.IntoElem()) break;

		if (xml.NextElem(_T("recent_project_list")))  
		{
			xml.RemoveElem();
		}
		xml.InsertElem(_T("recent_project_list"));
		xml.IntoElem();
		
		vector<RecentProjItem*>::iterator  iter = m_arrRecentProj.begin();
		vector<RecentProjItem*>::iterator  iterEnd = m_arrRecentProj.end();
		for (; iter != iterEnd; iter++)
		{
			RecentProjItem* pItem = *iter;
			if (NULL == pItem)
				continue;

			if (false == xml.AddElem(_T("item")))  
				continue;

			xml.SetAttrib(_T("filepath"), pItem->szFilePath);
			xml.SetAttrib(_T("lastvisittime"), pItem->nLastVisitTime);
		}
		bRet = true;
	}
	while(0);

	if (bRet)
		bRet = xml.Save(szConfigXmlPath);

	return bRet;
}

bool CRecentProjList::Add(LPCTSTR szProjPath)
{
	time_t ltime = 0;
	time( &ltime );
	//printf( "Time in seconds since UTC 1/1/70:\t%ld\n", ltime );

	RecentProjItem* pItem = this->GetItem(szProjPath);
	if (NULL == pItem)
	{
		pItem = new RecentProjItem;
		_tcscpy(pItem->szFilePath, szProjPath);
		pItem->nLastVisitTime = (long)ltime;

		m_arrRecentProj.push_back(pItem);
	}
	else
	{
		pItem->nLastVisitTime = (long)ltime;
	}

	std::sort(m_arrRecentProj.begin(), m_arrRecentProj.end(), RecentProjItemCompare);
	return this->Save();
}
bool CRecentProjList::Remove(LPCTSTR szProjPath)
{
	return false;
}
bool CRecentProjList::Load()
{
	TCHAR szConfigXmlPath[MAX_PATH] = _T("");
	::GetUIEditorConfigFilePath(szConfigXmlPath);

	CMarkup  xml;
	if (false == xml.Load(szConfigXmlPath))
		return false;

	this->Clear();

	bool bNeedSave = false;
	bool bRet = false;
	do 
	{
		if (false == xml.NextElem()) break;
		if (false == xml.IntoElem()) break;
		
		if (false == xml.NextElem(_T("recent_project_list")))  break;
		if (false == xml.IntoElem()) break;
		
		while(xml.NextElem())
		{
			String strFilePath = xml.GetAttrib(_T("filepath"));
			if (!PathFileExists(strFilePath.c_str()))
			{
				bNeedSave = true;
				xml.RemoveElem();
				continue;
			}
			String strVisitTime = xml.GetAttrib(_T("lastvisittime"));

			RecentProjItem* pItem = new RecentProjItem;
			_tcscpy(pItem->szFilePath, strFilePath.c_str());
			GetPathFileName(pItem->szFilePath, pItem->szFileName);
			pItem->nLastVisitTime = _ttoi(strVisitTime.c_str());

			m_arrRecentProj.push_back(pItem);
		}

		bRet = true;
	}
	while(0);

	if (false == bRet)
	{
		this->Clear();
		return false;
	}

	if (bNeedSave)  // 有可能是删除了不存在的文件导致的修改
	{
		xml.Save(szConfigXmlPath);
	}
	return true;
}

RecentProjItem* CRecentProjList::GetItemByIndex(int n)
{
	if (n < 0 || n > (int)m_arrRecentProj.size())
		return NULL;

	return m_arrRecentProj[n];
}