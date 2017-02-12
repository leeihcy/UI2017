#pragma once

void  get_module_name(HMODULE hModule, TCHAR szName[MAX_PATH]);
HMODULE  module_from_address(PVOID pv);
bool is_key_down(UINT vk);