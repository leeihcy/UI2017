#pragma once
#include "memoryleakdetect.h"


void symbol_init();
void symbol_uninit();

void get_stack_framepoint(UINT_PTR fpArray[MAXSTACKDEPTH]);
void trace_stack(UINT_PTR fpArray[MAXSTACKDEPTH]);
DWORD hash_stack_framepoint(UINT_PTR fpArray[MAXSTACKDEPTH]);
HMODULE get_appmodule_from_stack(UINT_PTR fpArray[MAXSTACKDEPTH]);

void load_module_symbol(HMODULE hModule);
