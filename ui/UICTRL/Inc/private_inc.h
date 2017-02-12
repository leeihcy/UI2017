#pragma once
#include "../common/define.h"

typedef CRect CRegion4;

#ifndef uint
typedef unsigned int uint;
#endif

bool IsKeyDown(UINT vk);
LPTSTR  GetTempBuffer(int nMaxSize = 255);
UI::String&  GetTempBufferString();