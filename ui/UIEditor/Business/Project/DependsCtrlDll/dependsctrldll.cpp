#include "stdafx.h"
#include "dependsctrldll.h"
#include "Business/Project/ProjectData/projectdata.h"
#include "Util/util.h"

CDependsCtrlDllMgr::CDependsCtrlDllMgr()
{
}

CDependsCtrlDllMgr::~CDependsCtrlDllMgr()
{
    vector<DependsCtrlDllItem*>::iterator iter = m_vStrDlls.begin();
    for (; iter != m_vStrDlls.end(); iter++)
    {
        DependsCtrlDllItem* p = *iter;
        SAFE_DELETE(p);
    }
    m_vStrDlls.clear();
}

bool CDependsCtrlDllMgr::AddCtrlDll(LPCTSTR szAbsolutePath)
{
    if (NULL == szAbsolutePath)
        return false;

    vector<DependsCtrlDllItem*>::iterator iter = m_vStrDlls.begin();
    for (; iter != m_vStrDlls.end(); iter++)
    {
        if (0 == _tcscmp((*iter)->strAbsolutePath.c_str(), szAbsolutePath))
            return false;
    }

    DependsCtrlDllItem* pItem = new DependsCtrlDllItem;
    pItem->strAbsolutePath = szAbsolutePath;

    TCHAR szRelativePath[MAX_PATH] = _T("");
    CalcRelativePathToFile(GetProjectData()->m_uiprojParse.GetUIProjPath(),
        szAbsolutePath, szRelativePath);
    pItem->strRelativePath = szRelativePath;

    m_vStrDlls.push_back(pItem);
    
    GetProjectData()->m_uiprojParse.SetDirty(true);
    return true;
}

bool CDependsCtrlDllMgr::DelCtrlDll(LPCTSTR szPath)
{
    if (NULL == szPath)
        return false;

    vector<DependsCtrlDllItem*>::iterator iter = m_vStrDlls.begin();
    for (; iter != m_vStrDlls.end(); iter++)
    {
        if (0 == _tcscmp((*iter)->strAbsolutePath.c_str(), szPath))
        {
            DependsCtrlDllItem* pItem = *iter;
            SAFE_DELETE(pItem);

            m_vStrDlls.erase(iter);
            GetProjectData()->m_uiprojParse.SetDirty(true);
            return true;
        }
    }

    return false;
}


void  CDependsCtrlDllMgr::OnLoadItem(LPCTSTR szPath)
{
    if (!szPath)
        return;

    DependsCtrlDllItem* pNewItem = new DependsCtrlDllItem;
    if (IsFullPath(szPath))
    {
        pNewItem->strAbsolutePath = szPath;
        pNewItem->strRelativePath = szPath;
    }
    else
    {
        TCHAR szUIProjDir[MAX_PATH] = _T("");
        TCHAR szAbsolutePath[MAX_PATH] = _T("");

        CalcFullPathByRelative(GetProjectData()->m_uiprojParse.GetUIProjPath(), szPath, szAbsolutePath);

        pNewItem->strRelativePath = szPath;
        pNewItem->strAbsolutePath = szAbsolutePath;
    }
    if (!PathFileExists(pNewItem->strAbsolutePath.c_str()))
    {
        delete pNewItem;

		GetProjectData()->m_uiprojParse.SetDirty(true);
        return;
    }
    m_vStrDlls.push_back(pNewItem);
}