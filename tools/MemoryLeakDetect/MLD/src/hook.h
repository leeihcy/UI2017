#pragma once
#include "mallocspy.h"

class CApiHook
{
public:
    CApiHook(
            PSTR pszCalleeModName, 
            PROC pfnCurrent, 
            PROC pfnNew
        );
    ~CApiHook();
    void  UnHookModule(HMODULE hModule);
    void  HookModule(HMODULE hModule);


private:
    PCSTR  m_pszCalleeModName;
    PROC   m_pfnOrig; 
    PROC   m_pfnHook;
};

// TODO: 没有处理模块被卸载(freelibrary)的情况
enum MODULE_TYPE
{
    MODULE_SYSTEM,
    MODULE_CRT,
    MODULE_GDIPLUS,
    MODULE_MLD,
    MODULE_APPLICATION,    
};
class CHookManager
{
public:
    CHookManager();
    ~CHookManager();

    void  Init();
    void  Uninit();

    void  OnLoadLibrary(HMODULE hModule);
    bool  IsAppModule(HMODULE hModule);

    void  AddHook(const CApiHook& hook);

    void  CopyModuleMap(map<HMODULE, MODULE_TYPE>& m);

private:
    MODULE_TYPE  GetModuleType(HMODULE hModule);
    void  HookModule(HMODULE hModule);
    void  UnHookModule(HMODULE hModule);

    void  HookAllModules();
    void  UnHookAllModules();
    
    void  ClearModules();
    void  AddModule(HMODULE, MODULE_TYPE);

private:
    // 只在初始化时write了，平时都是read，不加锁
    list<CApiHook>  m_apiHookList;

    TCHAR  m_szAppDir[MAX_PATH];

    CRITICAL_SECTION  m_csModuleMap;
    map<HMODULE, MODULE_TYPE>  m_mapAllModules;  // 所有加载的模块

    // COM hook
    MallocSpy  m_oComHook;
};

extern CHookManager  g_hookMgr;