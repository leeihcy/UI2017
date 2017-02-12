#include "stdafx.h"
#include "uicursor.h"

namespace UI 
{

UICursor::UICursor()
{
    m_dwRef = 0;
    m_hCursor = NULL;
    m_pOutRef = NULL;
}
UICursor::~UICursor()
{
    *m_pOutRef = NULL;
    m_pOutRef = NULL;

    UIASSERT(NULL == m_hCursor);
}

void UICursor::CreateInstance2(const String strCurFilePath, UICursor** ppOutRef)
{
    if (NULL == ppOutRef)
        return;

    UICursor* pCursor = new UICursor();

    bool bRet = pCursor->Load(strCurFilePath.c_str());
    if (false == bRet)
    {
        //	UI_LOG_WARN(_T("%s LoadCursor Failed. filepath=%s"), FUNC_NAME, strCurFilePath.c_str());
        SAFE_DELETE(pCursor);
        *ppOutRef = NULL;
    }
    else 
    {
        *ppOutRef = pCursor;
        pCursor->AddRef();
    }
}
void UICursor::CreateInstance2(LPTSTR nCursorID, HINSTANCE hInstance, UICursor** ppOutRef)
{
    if (NULL == ppOutRef)
        return;

    UICursor* pCursor = new UICursor();
    bool bRet = pCursor->Load(nCursorID, hInstance);
    if (false == bRet)
    {
        //	UI_LOG_WARN(_T("%s LoadCursor Failed. cursor id=%s, hInstance=%x"), FUNC_NAME, nCursorID, hInstance);
        SAFE_DELETE(pCursor);
        *ppOutRef = NULL;
    }
    else
    {
        *ppOutRef = pCursor;
        pCursor->AddRef();
    }
}
HCURSOR UICursor::GetCursor()
{ 
    return m_hCursor; 
}

bool UICursor::Load(const TCHAR* szCursorFile)
{
    if (NULL == szCursorFile)
        return false;

    if (m_hCursor)
    {
        DestroyCursor(m_hCursor);
        m_hCursor = NULL;
    }

    m_hCursor = (HCURSOR)::LoadImage(NULL, szCursorFile, IMAGE_CURSOR, 0,0, LR_DEFAULTSIZE|LR_LOADFROMFILE);
    return m_hCursor!=NULL ? true:false;
}

bool  UICursor::Load(LPTSTR nCursorID, HINSTANCE hInstance)
{
    if (m_hCursor)
    {
        DestroyCursor(m_hCursor);
        m_hCursor = NULL;
    }

    m_hCursor = ::LoadCursor(hInstance, nCursorID );
    return m_hCursor!=NULL ? true:false;
}
void  UICursor::Attach(HCURSOR hCursor)
{
    UIASSERT( NULL != hCursor );
    m_hCursor = hCursor;
}
HCURSOR  UICursor::Detach()
{
    UIASSERT( NULL!= m_hCursor );
    HCURSOR hOldCursor = m_hCursor;
    m_hCursor = NULL;

    return hOldCursor;
}
long  UICursor::AddRef()
{
    m_dwRef++;
    return m_dwRef;
}
long  UICursor::Release()
{
    m_dwRef--;
    if (0 >= m_dwRef )
    {
        if (m_hCursor)
        {
            DestroyCursor(m_hCursor);
            m_hCursor = NULL;
        }
        m_dwRef = 0;

        delete this;
        return 0;
    }
    return m_dwRef;
}
}