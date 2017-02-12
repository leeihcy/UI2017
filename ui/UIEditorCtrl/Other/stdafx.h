// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"
#pragma warning(disable:4996)

///#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>
#include <shobjidl.h>
#include <ShlGuid.h>
#include <shlobj.h>
#include <gdiplus.h>
#include <tchar.h>
#include <list>
#include <map>
#include <vector>
#pragma comment(lib, "gdiplus.lib")
using namespace std;

#include "UISDK\Inc\inc.h"
#include "UICTRL\Inc\inc.h"
#include "UIEditorCtrl\Inc\inc.h"

#pragma comment(lib,"uisdk.lib")
#pragma comment(lib,"uictrl.lib")

using namespace UI;

#include "common\define.h"

#include <atlbase.h>
#include <../3rd/wtl90/Include/atlapp.h>
#include <../3rd/wtl90/Include/atlcrack.h>

#define _WTL_NO_CSTRING
#include <../3rd/wtl90/Include/atlmisc.h>
using namespace WTL;

extern const RECT g_rcDragImgPadding;

#ifndef uint
typedef unsigned int uint;
#endif

typedef CRect CRegion4;