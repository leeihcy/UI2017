// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
//

#pragma once

#include "targetver.h"

#include <Windows.h>
#include <stdio.h>
#include <tchar.h>
#include <atlbase.h>
#include <atlstr.h>
#include <string>
using namespace std;
#pragma warning(disable:4996)


#ifdef _UNICODE
typedef wstring String;
#else
typedef string  String;
#endif