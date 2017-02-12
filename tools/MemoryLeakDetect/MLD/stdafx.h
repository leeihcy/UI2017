// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>
#include <process.h>
#include <crtdbg.h>
#include <map>
#include <list>
#include <assert.h>
#include <tchar.h>
#include <atlbase.h>
#include <string>
#include <set>
#include <algorithm>
#include <windowsx.h>
#include <psapi.h>
#pragma comment(lib, "Psapi.lib")
using namespace std;

#ifdef _UNICODE
typedef wstring String;
#else
typedef string  String;
#endif

extern HINSTANCE  g_hInstance;

#include "3rd\_dbgint.h"