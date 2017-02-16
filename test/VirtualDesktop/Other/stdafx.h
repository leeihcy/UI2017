// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
//

#pragma once

#include "targetver.h"

// Windows 头文件:
#include <windows.h>

#pragma warning(disable:4244)
#pragma warning(disable:4302)
#pragma warning(disable:4838)

// C 运行时头文件
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

#include "UI\UISDK\Inc\inc.h"
#include "UI\UICTRL\Inc\inc.h"
#include "ui\UISDK\Inc\Interface\iwindow.h"
#include "ui\UISDK\Inc\Interface\icustomwindow.h"
#include "ui\UISDK\Inc\Util\util.h"
#include "ui\UISDK\Inc\Interface\ianimate.h"
#pragma comment(lib,"uisdk.lib")
#pragma comment(lib,"uictrl.lib")
