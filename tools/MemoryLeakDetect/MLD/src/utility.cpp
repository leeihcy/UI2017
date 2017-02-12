#include "stdafx.h"
#include "utility.h"

void  get_module_name(HMODULE hModule, TCHAR szName[MAX_PATH])
{
	if (!hModule)
	{
		memset(szName, 0, sizeof(szName));
		return;
	}

// 	GetModuleFileName(hModule, szName, MAX_PATH);
// 	TCHAR* p = _tcsrchr(szName, '\\');
// 	if (p)
// 	{
// 		_tcscpy_s(szName, MAX_PATH, p+1);
// 	}
     
    GetModuleBaseName(GetCurrentProcess(), hModule, szName, MAX_PATH);
}

HMODULE  module_from_address(PVOID pv)
{
	MEMORY_BASIC_INFORMATION  mbi;
	return ((VirtualQuery(pv, &mbi, sizeof(mbi))!=0) ?
		(HMODULE)mbi.AllocationBase : NULL);
}

// 检测一个按键是否按下，如CTRL,SHIFT,ALT
bool is_key_down( UINT vk )
{
	return ( ( GetAsyncKeyState( vk ) ) != 0 );  // 或者 ( GetKeyState( VK_CONTROL ) < 0 ) ??
}