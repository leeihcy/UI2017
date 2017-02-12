#pragma once
#include "..\OLE\reoleapi.h"

// 需要下载的图片资源，下载成功后更新图片

namespace UI
{

struct  REOleResUpdateItem
{
    IREOleResUpdateCallback*  pCallback;
    long  lData;
};

class REOleMD5Res
{
public:
	REOleMD5Res(LPCTSTR szMD5);

    bool  IsEqual(LPCTSTR szMD5);
	bool  IsEmpty();
	void  Add(IREOleResUpdateCallback* pCallback, long lData);
	bool  Delete(IREOleResUpdateCallback* pCallback);
	
	void  OnUpdate(REOleLoadStatus e, LPCTSTR szPath);

private:
	String  m_strMD5;
    list<REOleResUpdateItem>  m_listItem;
};

class REOleResUpdateManager
{
public:
	REOleResUpdateManager();
	~REOleResUpdateManager();

	REOleMD5Res*  Add(LPCTSTR  szMD5);
	void  Delete(LPCTSTR  szMD5);
	void  Clear();
	REOleMD5Res*  Find(LPCTSTR  szMD5);

	void  OnREOleResUpdate(LPCTSTR  szMD5, REOleLoadStatus e, LPCTSTR szPath);

private:
    list<REOleMD5Res*>  m_listMD5Res;
};

}