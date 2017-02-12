#include "stdafx.h"
#include "privateheap.h"
#if 0
// 未使用。
// 不去HOOK mld模块，并且mld静态链接msvcr。
HANDLE  g_mldheap = NULL;

LPVOID  mld_new(size_t size)
{
    if (!g_mldheap)
        g_mldheap = HeapCreate(0, 0, 0);
    
    return HeapAlloc(g_mldheap, 0, size);
}
void  mld_delete(LPVOID p)
{
    if (!g_mldheap)
        return;

    HeapFree(g_mldheap, 0, p);
}

void  destroy_heap()
{
    if (g_mldheap)
    {
        HeapDestroy(g_mldheap);
        g_mldheap = NULL;
    }
}
#endif