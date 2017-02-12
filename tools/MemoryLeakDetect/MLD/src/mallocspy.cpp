#include "stdafx.h"
#include "mallocspy.h"
#include "thread.h"
#include "wnd.h"
#define COM_HEAP  (HANDLE)0xFFFFFFFF

void  OnMalloc(HANDLE  hHeap, LPVOID lpMem, SIZE_T dwBytes);
void  OnFree(HANDLE  hHeap, LPVOID lpMem);

MallocSpy::MallocSpy()
{
    m_lRef = 0;
    m_dwTls = TlsAlloc();
}
MallocSpy::~MallocSpy()
{
    TlsFree(m_dwTls);
    m_dwTls = 0;
}

HRESULT  MallocSpy::QueryInterface(REFIID riid, void **ppvObject)
{
    if (IsEqualIID(riid, IID_IUnknown))
    {
        AddRef();
        *ppvObject = static_cast<IUnknown*>(this);
        return S_OK;
    }
    else if (IsEqualIID(riid, IID_IMallocSpy))
    {
        AddRef();
        *ppvObject = static_cast<IMallocSpy*>(this);
        return S_OK;
    }

    return E_NOINTERFACE;
}

ULONG  MallocSpy::AddRef( void)
{
    InterlockedIncrement(&m_lRef);
    return m_lRef;
}

ULONG  MallocSpy::Release( void)
{
    InterlockedDecrement(&m_lRef);
    if (m_lRef <= 0)
    {
        // 不是new出来的。而是一个全局变量
        //delete this;
        return 0;
    }
    return m_lRef;
}


SIZE_T  MallocSpy::PreAlloc(SIZE_T cbRequest)
{
    TlsSetValue(m_dwTls, (LPVOID)cbRequest);
    return cbRequest;
}

void * MallocSpy::PostAlloc(void *pActual)
{
    OnMalloc(COM_HEAP, pActual, (SIZE_T)TlsGetValue(m_dwTls));
    return pActual;
}

void * MallocSpy::PreFree(void *pRequest, BOOL fSpyed)
{
    TlsSetValue(m_dwTls, (LPVOID)pRequest);
    return pRequest;
}

void  MallocSpy::PostFree(BOOL fSpyed)
{
    OnFree(COM_HEAP, (LPVOID)TlsGetValue(m_dwTls));
}

SIZE_T  MallocSpy::PreRealloc(
    void *pRequest,
    SIZE_T cbRequest,
    /* [out] */ void **ppNewRequest,
    BOOL fSpyed)
{
    OnFree(COM_HEAP, pRequest);
    TlsSetValue(m_dwTls, (LPVOID)cbRequest);

    *ppNewRequest = pRequest;
    return cbRequest;
}

void * MallocSpy::PostRealloc(void *pActual, BOOL fSpyed)
{
    OnMalloc(COM_HEAP, pActual, (SIZE_T)TlsGetValue(m_dwTls));

    return pActual;
}

void * MallocSpy::PreGetSize(void *pRequest,BOOL fSpyed)
{
    return pRequest;
}

SIZE_T  MallocSpy::PostGetSize(SIZE_T cbActual, BOOL fSpyed)
{
    return cbActual;
}

void * MallocSpy::PreDidAlloc(void *pRequest, BOOL fSpyed)
{
    return pRequest;
}

int  MallocSpy::PostDidAlloc(void *pRequest, BOOL fSpyed, int fActual)
{
    return fActual;
}

void  MallocSpy::PreHeapMinimize(void)
{

}

void  MallocSpy::PostHeapMinimize(void)
{

}