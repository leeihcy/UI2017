#include "stdafx.h"
#include "print.h"

void  mld_print(LPCTSTR szText)
{
    ::OutputDebugString(szText);
    //::OutputDebugStringA("\r\n");
}
void  mld_print(LPCSTR szText)
{
    ::OutputDebugStringA(szText);
    //::OutputDebugStringA("\r\n");
}

void  __cdecl mld_printf(LPCTSTR szFormat, ...)
{
    va_list argList;                           
    va_start(argList, szFormat);       

    int nLength = _vsctprintf( szFormat, argList ) + 1;
    if (nLength < 128)
    {
        TCHAR szBuffer[128];
        _vstprintf_s(szBuffer, nLength, szFormat, argList);
        mld_print(szBuffer);
    }
    else
    {
        TCHAR* pszFormantStack = (TCHAR*)_malloca(nLength*sizeof(TCHAR));
        _vstprintf_s(pszFormantStack, nLength, szFormat, argList);
        mld_print(pszFormantStack);
        _freea(pszFormantStack);
    }
     
    va_end(argList); 
}

void  __cdecl mld_printf(LPCSTR szFormat, ...)
{
    va_list argList;                           
    va_start(argList, szFormat);       

    int nLength = _vscprintf( szFormat, argList ) + 1;
    if (nLength < 128)
    {
        char szBuffer[128];
        vsprintf_s(szBuffer, nLength, szFormat, argList);
        mld_print(szBuffer);
    }
    else
    {
        char* pszFormantStack = (char*)_malloca(nLength*sizeof(char));
        vsprintf_s(pszFormantStack, nLength, szFormat, argList);
        mld_print(pszFormantStack);
        _freea(pszFormantStack);
    }

    va_end(argList); 
}