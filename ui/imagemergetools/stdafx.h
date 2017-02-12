// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // 从 Windows 头中排除极少使用的资料
// Windows 头文件:
#include <windows.h>

#pragma warning(disable:4996)
#pragma warning(disable:4005)

// C 运行时头文件
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

#include <atlbase.h>
#include "atlapp.h"
#include "atlcrack.h"
#include "atlctrls.h"
#include "atldlgs.h"
#include "..\UISDK\Inc\inc.h"
#pragma comment(lib, "UISDK.lib")
#include "..\UISDK\Inc\Util\iimage.h"


#pragma comment(linker, "\"/manifestdependency:type='Win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='X86' publicKeyToken='6595b64144ccf1df' language='*'\"")
