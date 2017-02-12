#pragma once
#include <objidl.h>

class MallocSpy : public IMallocSpy
{
public:
    MallocSpy();
    ~MallocSpy();

    virtual HRESULT STDMETHODCALLTYPE QueryInterface( 
        /* [in] */ REFIID riid,
        /* [iid_is][out] */ 
        __RPC__deref_out  void **ppvObject);

    virtual ULONG STDMETHODCALLTYPE AddRef( void);

    virtual ULONG STDMETHODCALLTYPE Release( void);

    virtual SIZE_T STDMETHODCALLTYPE PreAlloc( 
        /* [in] */ SIZE_T cbRequest);

    virtual void *STDMETHODCALLTYPE PostAlloc( 
        /* [in] */ void *pActual);

    virtual void *STDMETHODCALLTYPE PreFree( 
        /* [in] */ void *pRequest,
        /* [in] */ BOOL fSpyed);

    virtual void STDMETHODCALLTYPE PostFree( 
        /* [in] */ BOOL fSpyed);

    virtual SIZE_T STDMETHODCALLTYPE PreRealloc( 
        /* [in] */ void *pRequest,
        /* [in] */ SIZE_T cbRequest,
        /* [out] */ void **ppNewRequest,
        /* [in] */ BOOL fSpyed);

    virtual void *STDMETHODCALLTYPE PostRealloc( 
        /* [in] */ void *pActual,
        /* [in] */ BOOL fSpyed);

    virtual void *STDMETHODCALLTYPE PreGetSize( 
        /* [in] */ void *pRequest,
        /* [in] */ BOOL fSpyed);

    virtual SIZE_T STDMETHODCALLTYPE PostGetSize( 
        /* [in] */ SIZE_T cbActual,
        /* [in] */ BOOL fSpyed);

    virtual void *STDMETHODCALLTYPE PreDidAlloc( 
        /* [in] */ void *pRequest,
        /* [in] */ BOOL fSpyed);

    virtual int STDMETHODCALLTYPE PostDidAlloc( 
        /* [in] */ void *pRequest,
        /* [in] */ BOOL fSpyed,
        /* [in] */ int fActual);

    virtual void STDMETHODCALLTYPE PreHeapMinimize( void);

    virtual void STDMETHODCALLTYPE PostHeapMinimize( void);

private:
    long  m_lRef;

    // PostAlloc中得不到本次分配内存的大小，通过tls来获取
    DWORD  m_dwTls;
};