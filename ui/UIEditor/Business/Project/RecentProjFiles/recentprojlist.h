#pragma once

struct RecentProjItem
{
	TCHAR   szFilePath[MAX_PATH];
	TCHAR   szFileName[MAX_PATH];
	long    nLastVisitTime;

	friend bool operator <(const RecentProjItem& o1, const RecentProjItem& o2);
	friend bool operator >(const RecentProjItem& o1, const RecentProjItem& o2);
	
};


class CRecentProjList
{
public:
	CRecentProjList(void);
	~CRecentProjList(void);

	bool   Add(LPCTSTR szProjPath);
	bool   Remove(LPCTSTR szProjPath);
	bool   Load();
	int    GetItemCount() { return (int)m_arrRecentProj.size(); }
	RecentProjItem* GetItemByIndex(int n);

protected:
	void   Clear();
	RecentProjItem*   GetItem(LPCTSTR szProjPath);
	bool   Save();

public:
	vector<RecentProjItem*>  m_arrRecentProj;
};
