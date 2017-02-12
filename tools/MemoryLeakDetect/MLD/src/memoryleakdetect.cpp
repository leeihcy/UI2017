#include "stdafx.h"
#include "memoryleakdetect.h"
#include "thread.h"
#include "..\3rd\_dbgint.h"
#include "hook.h"
#include "symbol.h"
#include "wnd.h"
#include "print.h"

__declspec(dllexport) MemoryLeakDetect g;
int __cdecl MLD_CRT_ALLOC_HOOK(int, void *, size_t, int, long, const unsigned char *, int);

MemoryLeakDetect::MemoryLeakDetect()
{
    InitializeCriticalSection(&g_thread_cs);
	dwMainThreadId = GetCurrentThreadId();
    symbol_init();
	g_hookMgr.Init(); 
    bPause = false;
}

MemoryLeakDetect::~MemoryLeakDetect()
{
	leak_report(true);
	symbol_uninit();
    DeleteCriticalSection(&g_thread_cs);

    clear_map_heaps();
}

void MemoryLeakDetect::clear_map_heaps()
{
    map<HANDLE, HeapAllocs*>::iterator iter = map_heaps.begin();
    for (; iter != map_heaps.end(); ++iter)
    {
        delete iter->second;
    }
    map_heaps.clear();
}

HeapAllocs::HeapAllocs()
{
    hHeap = NULL;
}

HeapAllocs::~HeapAllocs()
{
    map<LPVOID, AllocInfo*>::iterator iter = map_allocs.begin();
    for (; iter != map_allocs.end(); ++iter)
    {
        delete iter->second;
    }
    map_allocs.clear();
}

// allocType:
// #define _HOOK_ALLOC     1
// #define _HOOK_REALLOC   2
// #define _HOOK_FREE      3
// 
// blockType:
// #define _FREE_BLOCK      0
// #define _NORMAL_BLOCK    1
// #define _CRT_BLOCK       2
// #define _IGNORE_BLOCK    3
// #define _CLIENT_BLOCK    4
// #define _MAX_BLOCKS      5
//
// int __cdecl MLD_CRT_ALLOC_HOOK(
//         int allocType, 
//         void *userData, 
//         size_t size, 
//         int blockType, 
//         long requestNumber, 
//         const unsigned char *filename, 
//         int lineNumber)
// {
//     if ( blockType == _CRT_BLOCK )   // Ignore internal C runtime library allocations
//         return TRUE;
// 
//     if (allocType == _HOOK_ALLOC)
//     {
//         g.OnMalloc(requestNumber);   
//     }
//     else if (allocType == _HOOK_FREE)
//     {
//         g.OnFree(pHdr(userData)->lRequest);
//     }
//     else if (allocType == _HOOK_REALLOC)
//     {
//         g.OnMalloc(requestNumber);  
//         g.OnFree(pHdr(userData)->lRequest);
//     }
//     return TRUE;
// }
// 

#define GET_RETURN_ADDRESS(context)  *(context.fp + 1)
void  OnMalloc(HANDLE  hHeap, LPVOID lpMem, SIZE_T dwBytes)
{
    if (g.bPause)
        return;

	AllocInfo* pInfo = new AllocInfo;
	memset(pInfo, 0, sizeof(AllocInfo));
	pInfo->lpMem = lpMem;
    pInfo->dwBytes = dwBytes;
    
    // TODO: 目前无法得知当前分配的内存是否是CRT的。有可能是直接调用HeapAlloc，
    // 如CString::Alloc
    //pInfo->dwBytes -= sizeof(_CrtMemBlockHeader) - nNoMansLandSize;

	get_stack_framepoint(pInfo->framepointArray);

    lock_handle_message(THREAD_MSG_ID_MALLOC, (WPARAM)hHeap, (LPARAM)pInfo);
}
void  OnFree(HANDLE  hHeap, LPVOID lpMem)
{
    lock_handle_message(THREAD_MSG_ID_FREE, (WPARAM)hHeap, (LPARAM)lpMem);
}

void  OnHeapDestroy(HANDLE hHeap)
{
	lock_handle_message(THREAD_MSG_ID_HEAPDESTROY, (WPARAM)hHeap, 0);
}


PVOID WINAPI Hook_HeapAlloc/*Hook_RtlAllocateHeap*/( 
                IN PVOID  hHeap,
                IN ULONG  Flags,
                IN SIZE_T  Size
    )
{
    PVOID p = ::HeapAlloc(hHeap, Flags, Size);

    OnMalloc(hHeap, p, Size);
    return p;
}


BOOL WINAPI Hook_HeapFree(
  __in HANDLE hHeap,
  __in DWORD dwFlags,
  __in LPVOID lpMem
)
{
    OnFree(hHeap, lpMem);
    return ::HeapFree(hHeap, dwFlags, lpMem);
}


LPVOID WINAPI Hook_HeapReAlloc(
  __in HANDLE hHeap,
  __in DWORD dwFlags,
  __in LPVOID lpMem,
  __in SIZE_T dwBytes
)
{
    OnFree(hHeap, lpMem);
    LPVOID p = ::HeapReAlloc(hHeap, dwFlags, lpMem, dwBytes);
    OnMalloc(hHeap, p, dwBytes);

    return p;
}

BOOL WINAPI Hook_HeapDestroy(HANDLE hHeap)
{
	OnHeapDestroy(hHeap);
	return HeapDestroy(hHeap);
}

namespace MLD
{
extern "C"
void __declspec(dllexport) PauseLeakDetect(bool bPause)
{
    g.bPause = bPause;
}
}