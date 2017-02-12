#include "stdafx.h"
#include "hook.h"
#include <tlhelp32.h>
#include <imagehlp.h>
#include "print.h"
#include "utility.h"
#include "symbol.h"
#include "wnd.h"
#include "thread.h"
#pragma comment(lib, "Dbghelp.lib")

CHookManager  g_hookMgr;

void  ReplaceIATEntryInOneModule(
                                 PCSTR pszCalleeModuleName, 
                                 PROC pfnCurrent,
                                 PROC pfnNew,
                                 HMODULE hModCaller
                                 );


CApiHook::CApiHook(
                   PSTR pszCalleeModName, 
                   PROC pfnCurrent, 
                   PROC pfnNew)
{
    m_pszCalleeModName = pszCalleeModName;
    m_pfnOrig = pfnCurrent;
    m_pfnHook = pfnNew;

    g_hookMgr.AddHook(*this);
}

CApiHook::~CApiHook()
{
    m_pszCalleeModName = NULL;
    m_pfnOrig = NULL;
    m_pfnHook = NULL;
}

void  CApiHook::UnHookModule(HMODULE hModule)
{
    if (!m_pszCalleeModName)
        return;

    ReplaceIATEntryInOneModule(
        m_pszCalleeModName,
        m_pfnOrig, 
        m_pfnOrig,
        hModule);
}

void  CApiHook::HookModule(HMODULE hModule)
{
    if (!hModule)
        return;

    ReplaceIATEntryInOneModule(
        m_pszCalleeModName,
        m_pfnOrig,
        m_pfnHook,
        hModule);
}


// 调用方式:
// PROC  pfnOrig = GetProcAddress(GetModuleHandle("Kernel32",
//      "ExitProcess");
// HMODULE hmodCaller = GetModuleHandle("DataBase.exe");
// 
// ReleaseIATEntryInOneModule(
//      "Kernel32.dll",  // Module containing the function(ANSI)
//      pfnOrig,         // Address of funcction in callee
//      MyExitProcess,   // Address of new function to be called
//      hmodCaller);     // Handle of module that should call the
//                       // new function
//
void  ReplaceIATEntryInOneModule(
                                PCSTR pszCalleeModuleName, 
                                PROC pfnCurrent,
                                PROC pfnNew,
                                HMODULE hModCaller)
{
    ULONG   ulSize = 0;
    PIMAGE_IMPORT_DESCRIPTOR  pImportDesc =
        (PIMAGE_IMPORT_DESCRIPTOR)
        ::ImageDirectoryEntryToData(hModCaller, TRUE, 
        IMAGE_DIRECTORY_ENTRY_IMPORT, &ulSize);

    if (NULL == pImportDesc)
        return;  // This module has no import section.

    // Find the importdescriptor containing references
    // to callee's functions.
    for (; pImportDesc->Name; pImportDesc++)
    {
        PSTR pszModName = (PSTR)
            ((PBYTE)hModCaller + pImportDesc->Name);
        if (0 == lstrcmpiA(pszModName, pszCalleeModuleName))
            break;
    }

    if (0 == pImportDesc->Name)
        // This module doesn't import any function from this callee.
        return;

    // Get call's import address table(IAT)
    // for the callee's funcions.
    PIMAGE_THUNK_DATA pThunk = (PIMAGE_THUNK_DATA)
        ((PBYTE)hModCaller + pImportDesc->FirstThunk);

    // Replace current functoin address with new function address.
    for (; pThunk->u1.Function; pThunk++)
    {
        // Get the address of the function address.
        PROC*  ppfn = (PROC*)&pThunk->u1.Function;

        // Is this the function we're looking for?
        BOOL fFound = (*ppfn == pfnCurrent);

        // See the sample code for some stricky Window 98
        // stuff that goes here;
        if (fFound)
        {
            // The address match; change the import section address.
            // 不加VirtualProtect的话,会报0x3e6 内存位置访问无效。的错误
            DWORD protect;
            VirtualProtect(ppfn, sizeof(PROC), PAGE_EXECUTE_READWRITE, &protect);
            //::WriteProcessMemory(GetCurrentProcess(), ppfn, &pfnNew, sizeof(pfnNew), NULL);
            *ppfn = pfnNew;
            VirtualProtect(ppfn, sizeof(PROC), protect, &protect);
            return;  // We did it; get out.
        }
    }

    // If we get to here, the function is not in the callee's
    // import section.
}

// NOTE: This function must NOT be inlined
FARPROC  GetProcAddressRaw(HMODULE hmod, PCSTR pszProcName)
{
    return (::GetProcAddress(hmod, pszProcName));
}


//////////////////////////////////////////////////////////////////////////

HMODULE WINAPI Hook_LoadLibraryA(LPCSTR lpFileName)
{
    HMODULE hModule = LoadLibraryA(lpFileName);
    if (hModule)
        g_hookMgr.OnLoadLibrary(hModule);

    return hModule;
}

HMODULE WINAPI Hook_LoadLibraryW(LPCWSTR lpFileName)
{
    HMODULE hModule = LoadLibraryW(lpFileName);
    if (hModule)
        g_hookMgr.OnLoadLibrary(hModule);

    return hModule;
}

HMODULE WINAPI Hook_LoadLibraryExA(LPCSTR lpFileName, HANDLE hFile, DWORD dwFlags)
{
    HMODULE hModule = LoadLibraryExA(lpFileName, hFile, dwFlags);
    if (hModule)
        g_hookMgr.OnLoadLibrary(hModule);

    return hModule;
}

HMODULE WINAPI Hook_LoadLibraryExW(LPCWSTR lpFileName, HANDLE hFile, DWORD dwFlags)
{
    HMODULE hModule = LoadLibraryExW(lpFileName, hFile, dwFlags);
    if (hModule)
        g_hookMgr.OnLoadLibrary(hModule);

    return hModule;
}

CHookManager::CHookManager()
{
    InitializeCriticalSection(&m_csModuleMap);
}
CHookManager::~CHookManager()
{
    DeleteCriticalSection(&m_csModuleMap);
}

PVOID WINAPI Hook_HeapAlloc/*Hook_RtlAllocateHeap*/(PVOID,ULONG,SIZE_T);
LPVOID WINAPI Hook_HeapReAlloc(HANDLE, DWORD,LPVOID,SIZE_T);
BOOL WINAPI Hook_HeapFree(HANDLE,DWORD,LPVOID);
BOOL WINAPI Hook_HeapDestroy(HANDLE hHeap);

void  CHookManager::Init()
{
    if (m_apiHookList.size() > 0)
        return;

    GetModuleFileName(NULL, m_szAppDir, MAX_PATH);
    TCHAR* p =_tcsrchr(m_szAppDir, '\\');
    *(p) = 0;
    *(p+1) = 0;

    // 放开上一级目录，允许其它DLL在EXE的上一级目录下面
    TCHAR* p2 =_tcsrchr(m_szAppDir, '\\');
    if (p2)
        *(p2+1) = 0;
    else
        *p = '\\';

    CApiHook hook0(
        "Kernel32.dll",
        (PROC)LoadLibraryA,
        (PROC)Hook_LoadLibraryA);

    {
    CApiHook hook(
        "Kernel32.dll",
        (PROC)LoadLibraryW,
        (PROC)Hook_LoadLibraryW);
    }

//     PROC pfnRtlAllocateHeap = GetProcAddress(
//         GetModuleHandle(TEXT("ntdll.dll")),
//         "RtlAllocateHeap");
//     assert(pfnRtlAllocateHeap);
// 
//     CApiHook hook1(
//         "ntdll.dll",
//         pfnRtlAllocateHeap,
//         (PROC)Hook_RtlAllocateHeap,
//         TRUE);

    CApiHook hook1(
        "Kernel32.dll",
        (PROC)HeapAlloc,
        (PROC)Hook_HeapAlloc);

    CApiHook hook2(
        "Kernel32.dll",
        (PROC)HeapFree,
        (PROC)Hook_HeapFree);

    CApiHook hook3(
        "Kernel32.dll",
        (PROC)HeapReAlloc,
        (PROC)Hook_HeapReAlloc);

	CApiHook hook4(
		"Kernel32.dll",
		(PROC)HeapDestroy,
		(PROC)Hook_HeapDestroy);

    HookAllModules();

    // com hook
    // MallocSpy这里不能使用指针new出来，可能是因为这个new的heap在退出时先销毁了,
    // 导致MallocSpy内容无效，出现崩溃。
    m_oComHook.AddRef();
    
    // BSTR系统内部有缓存，使用spy拦截会出现误报。先关闭掉
    //CoRegisterMallocSpy(&m_oComHook);
}

void  CHookManager::Uninit()
{
    // 全局类，就不卸载了
    //UnHookAllModules();

    m_apiHookList.clear();

    CoRevokeMallocSpy();
    m_oComHook.Release();
}

void  CHookManager::AddHook(const CApiHook& hook)
{
    m_apiHookList.push_back(hook);
}

// 注：隐式加载的dll不走loadlibrary（内部走ZwMapViewOfSectiont..）
//     因此一个dll加载时，可能附带把其依赖的dll也加载起来，这些dll
//     目前只能通过再枚举一次来实现HOOK。
void  CHookManager::OnLoadLibrary(HMODULE hModule)
{
    EnterCriticalSection(&m_csModuleMap);

    HookAllModules();

    LeaveCriticalSection(&m_csModuleMap);
}

// 只测试应用程序目录下的DLL是否有内存泄露
// 返回true，表示该模块可HOOK
// 返回false，表示该模块不可HOOK
MODULE_TYPE  CHookManager::GetModuleType(HMODULE hModule)
{
    if (!hModule)
        return MODULE_SYSTEM;

    // 将自己去静态加载msvcr，并且不去拦截mld模块，这样在本模块中就可
    // 以随意分配内存了
    // 另外g_hInstance的初始化比memoryleakdetect构造函数还晚。所以下面还要
    // 判断dll名字是否是mld.dll
    if (g_hInstance && hModule == g_hInstance)
        return MODULE_MLD;

    TCHAR szText[MAX_PATH] = {0};
    GetModuleFileName(hModule, szText, MAX_PATH);

	TCHAR* p = _tcsrchr(szText, '\\');
    if (p)
    {
        // HACK: 
        // 1. 对于动态加载msvcr的模块，分配内存是由msvcr调用的。
        // 2. 对于静态加载msvcr的模块，分配内存是由该模块调用的。
        // 3. GetModuleHandle("msvcr90d.dll")将返回NULL。据说是MS的一种什么版本区分机制导致的。
        // 因此这里对msvcr***.dll进行过滤

	    if (_tcsnicmp(p+1, TEXT("msvcr"), 5) == 0)
		    return MODULE_CRT;

        if (_tcsicmp(p+1, TEXT("mld.dll")) == 0)
            return MODULE_MLD;

        if (_tcsicmp(p+1, TEXT("gdiplus.dll")) == 0)
            return MODULE_GDIPLUS;
    }
   
    if (_tcsstr(szText, m_szAppDir) == szText)
    {
        return MODULE_APPLICATION;
    }

    return MODULE_SYSTEM;
}

// 是否是所测试程序自已的模块
bool  CHookManager::IsAppModule(HMODULE hModule)
{
    if (!hModule)
        return false;

    bool bRet = false;

    EnterCriticalSection(&m_csModuleMap);
    map<HMODULE, MODULE_TYPE>::iterator iter = m_mapAllModules.find(hModule);
    if (iter != m_mapAllModules.end() && iter->second == MODULE_APPLICATION)
        bRet = true;
    LeaveCriticalSection(&m_csModuleMap);

    return bRet;
}


long  Function_For_GetThisModule_FromAddress()
{
    return 0;
}

void  CHookManager::HookModule(HMODULE hModule)
{
    list<CApiHook>::iterator iter = m_apiHookList.begin();
    for ( ; iter != m_apiHookList.end(); ++iter)
    {
        (*iter).HookModule(hModule);
    }
}
void  CHookManager::UnHookModule(HMODULE hModule)
{
    list<CApiHook>::iterator iter = m_apiHookList.begin();
    for ( ; iter != m_apiHookList.end(); ++iter)
    {
        (*iter).UnHookModule(hModule);
    }
}

void  CHookManager::ClearModules()
{
    EnterCriticalSection(&m_csModuleMap);
    m_mapAllModules.clear();
    LeaveCriticalSection(&m_csModuleMap);
}
void  CHookManager::AddModule(HMODULE hModule, MODULE_TYPE e)
{
    EnterCriticalSection(&m_csModuleMap);
    m_mapAllModules[hModule] = e;
    LeaveCriticalSection(&m_csModuleMap);
}
void  CHookManager::CopyModuleMap(map<HMODULE, MODULE_TYPE>& m)
{
    EnterCriticalSection(&m_csModuleMap);
    m.insert(m_mapAllModules.begin(), m_mapAllModules.end());
    LeaveCriticalSection(&m_csModuleMap);
}
void  CHookManager::HookAllModules()
{
    HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, 
        GetCurrentProcessId());
    if(snap == INVALID_HANDLE_VALUE)
        return;

    // 有可能一些模块被卸载了，这里重新整理一份m_setAllModule
    map<HMODULE, MODULE_TYPE>  mapOldAllModule;
    CopyModuleMap(mapOldAllModule);
    ClearModules();

    MODULEENTRY32 me = { sizeof(me), 0 };
    BOOL bRet = Module32Next(snap, &me);
    do 
    {
        HMODULE hModule = me.hModule;

        // 已经HOOK过了
        map<HMODULE, MODULE_TYPE>::iterator iter = mapOldAllModule.find(hModule);
        if (iter != mapOldAllModule.end())
        {
            AddModule(hModule, iter->second);
            continue;
        }

        MODULE_TYPE eType = g_hookMgr.GetModuleType(me.hModule);
        if (eType == MODULE_APPLICATION || eType == MODULE_GDIPLUS)
        {
            HookModule(hModule);
            set_appmodulelist_dirty();

            mld_printf(L"MLD: hook application module: 0x%08X  %s\r\n",
                me.hModule, me.szModule);
        }
        else if (eType == MODULE_CRT)
        {
            HookModule(hModule);
            mld_printf(L"MLD: hook application module: 0x%08X  %s\r\n",
                me.hModule, me.szModule);
        }

        AddModule(hModule, eType);
    } 
    while (bRet = Module32Next(snap, &me));

    CloseHandle(snap);
}

void  CHookManager::UnHookAllModules()
{
    HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, 
        GetCurrentProcessId());
    if(snap == INVALID_HANDLE_VALUE)
        return;

    MODULEENTRY32 me = { sizeof(me), 0 };
    BOOL bRet = Module32Next(snap, &me);
    do 
    {
        UnHookModule(me.hModule);
    } 
    while (bRet = Module32Next(snap, &me));

    CloseHandle(snap);

    ClearModules();
}