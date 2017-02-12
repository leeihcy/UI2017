#include "stdafx.h"
#include "symbol.h"
#include <imagehlp.h>
#include <intrin.h>
#include "print.h"
#include "utility.h"
#include "hook.h"
#pragma comment(lib, "dbghelp.lib")

// 用于记录哪些模块
// map<HMODULE, bool>  g_mapModuleSymbolLoaded;

void buildSymbolSearchPath(String& str)
{ 
	// Oddly, the symbol handler ignores the link to the PDB embedded in the
	// executable image. So, we'll manually add the location of the executable
	// to the search path since that is often where the PDB will be located.
	TCHAR directory [_MAX_DIR] = {0};
	TCHAR drive [_MAX_DRIVE] = {0};
	TCHAR path[MAX_PATH] = {0};
	LPTSTR p = NULL;

	HMODULE module = GetModuleHandleW(NULL);
	GetModuleFileName(module, path, MAX_PATH);
	p = _tcsrchr(path, '\\');
	*(p+1) = 0;

	str.append(path);


	// When the symbol handler is given a custom symbol search path, it will no
	// longer search the default directories (working directory, system root,
	// etc). But we'd like it to still search those directories, so we'll add
	// them to our custom search path.
	//
	// Append the working directory.
	str.append(TEXT(";.\\"));

	// Append Visual Studio 2010/2008 symbols cache directory.
	HKEY debuggerkey;
	WCHAR symbolCacheDir [MAX_PATH] = {0};
	LSTATUS regstatus = RegOpenKeyEx(HKEY_CURRENT_USER, L"Software\\Microsoft\\VisualStudio\\9.0\\Debugger", 0, KEY_QUERY_VALUE, &debuggerkey);
	if (regstatus == ERROR_SUCCESS)
	{
		DWORD valuetype;
		DWORD dirLength = MAX_PATH * sizeof(WCHAR);
		regstatus = RegQueryValueEx(debuggerkey, L"SymbolCacheDir", NULL, &valuetype, (LPBYTE)&symbolCacheDir, &dirLength);
		if (regstatus == ERROR_SUCCESS && valuetype == REG_SZ)
		{
			str.append(TEXT(";"));
			str.append(symbolCacheDir);
			str.append(TEXT("\\MicrosoftPublicSymbols;"));
			str.append(symbolCacheDir);
		}
		RegCloseKey(debuggerkey);
	}

	// Remove any quotes from the path. The symbol handler doesn't like them.
// 	SIZE_T  pos = 0;
// 	SIZE_T  length = wcslen(path);
// 	while (pos < length) {
// 		if (path[pos] == L'\"') {
// 			for (SIZE_T  index = pos; index < length; index++) {
// 				path[index] = path[index + 1];
// 			}
// 		}
// 		pos++;
// 	}
//
//	return path;
}

void symbol_init()
{
    // 符号加载修改为leak_report时，如果有模块没有加载符号，则为其加载。
    // 因此不需要延时加载符号：SYMOPT_DEFERRED_LOADS，SymInitializeW的
    // 第三个参数也只需要设置为FALSE，不需要立即加载。
    // 
    // SYMOPT_DEBUG，相当于windbg中的 !sym noisy 
    // 显示出加载符号的过程信息。
	SymSetOptions(/*SYMOPT_DEBUG |*/ SYMOPT_UNDNAME  | SYMOPT_LOAD_LINES);

	HANDLE hProcess = GetCurrentProcess();
	String strSymbolPath;
	buildSymbolSearchPath(strSymbolPath);

	if (!SymInitializeW(hProcess, strSymbolPath.c_str(), FALSE))
	{
		assert(0);
	}
}

void symbol_uninit()
{
	HANDLE hProcess = GetCurrentProcess();
	SymCleanup(hProcess);
}

void get_stack_framepoint(UINT_PTR fpArray[MAXSTACKDEPTH])
{
	UINT_PTR* framePointer = ((UINT_PTR*)_AddressOfReturnAddress()) - 1;
	int count = -1;
	while (count < MAXSTACKDEPTH)
	{
		if (*framePointer < (UINT_PTR)framePointer) 
		{
			if (*framePointer == NULL)
			{
				// Looks like we reached the end of the stack.
				break;
			}
			else 
			{
				// Invalid frame pointer. Frame pointer addresses should always
				// increase as we move up the stack.
				//m_status |= CALLSTACK_STATUS_INCOMPLETE;
				break;
			}
		}
		if (*framePointer & (sizeof(UINT_PTR*) - 1)) 
		{
			// Invalid frame pointer. Frame pointer addresses should always
			// be aligned to the size of a pointer. This probably means that
			// we've encountered a frame that was created by a module built with
			// frame pointer omission (FPO) optimization turned on.
			//m_status |= CALLSTACK_STATUS_INCOMPLETE;
			break;
		}

        // 实际使用中发现IsBadReadPtr这些在debug下会直接崩溃，干脆直接屏蔽
        if (*framePointer == 0xcccccccc)
        {
            break;
        }

		if (IsBadReadPtr((UINT*)*framePointer, sizeof(UINT_PTR*))) 
		{
			// Bogus frame pointer. Again, this probably means that we've
			// encountered a frame built with FPO optimization.
			//m_status |= CALLSTACK_STATUS_INCOMPLETE;
			break;
		}

        if (count >= 0)  // count = -1，不记录第一行。第一行是mld里的hook函数: OnMalloc
		    fpArray[count] = *(framePointer + 1);

		count++;
		framePointer = (UINT_PTR*)*framePointer;
	}
}

void load_module_symbol(HMODULE hModule)
{
	char szModuleFullPath[MAX_PATH] = {0};
	GetModuleFileNameA(hModule, szModuleFullPath, MAX_PATH);


	// 注：
	//   如果设置了 SymSetOptions SYMOPT_DEFERRED_LOADS，
	//   则调用两次SymLoadModule才会触发加载符号
	DWORD64 dwRet = SymLoadModuleEx(GetCurrentProcess(), NULL, 
		szModuleFullPath, NULL, (DWORD64)hModule, 0,
		NULL, 0);
	int a = 0;
}

HMODULE get_appmodule_from_stack(UINT_PTR fpArray[MAXSTACKDEPTH])
{
    for (int i = 0; i < MAXSTACKDEPTH; ++i)
    {
        if (!fpArray[i])
            break;

        HMODULE hAppModule = module_from_address((PVOID)fpArray[i]);
        if (!hAppModule)
            break;

        if (g_hookMgr.IsAppModule(hAppModule))
            return hAppModule;
    }

    return NULL;
}

void trace_stack(UINT_PTR fpArray[MAXSTACKDEPTH])
{
	HANDLE hProcess = GetCurrentProcess();

	TCHAR szModuleName[MAX_PATH] = {0};

	IMAGEHLP_LINEW64  sourceInfo = { 0 };
	sourceInfo.SizeOfStruct = sizeof(IMAGEHLP_LINE64);

    const int max_sym_name = 2*MAX_SYM_NAME;  // boost template class hengchange
	ULONG64 buffer[(sizeof(PSYMBOL_INFOW) +
		max_sym_name*sizeof(TCHAR) +
		sizeof(ULONG64) - 1) /
		sizeof(ULONG64)];
	PSYMBOL_INFOW pSymbol = (PSYMBOL_INFOW)buffer;
	pSymbol->SizeOfStruct = sizeof(SYMBOL_INFO);
	pSymbol->MaxNameLen = max_sym_name;

	for (int i = 0; i < MAXSTACKDEPTH; i++)
	{
		UINT_PTR framepoint = fpArray[i];
		if (NULL == framepoint)
			return;
		
        HMODULE hModule = module_from_address((PVOID)framepoint);

        DWORD displacement = 0;
        for (int j = 0; j < 2; j++)
        {
            // 获取代码行+偏移量
            sourceInfo.FileName = 0;
            if (SymGetLineFromAddrW64(hProcess, framepoint, 
                &displacement, &sourceInfo))
            {
                break;
            }
            else if (j == 0)
            {
				// 表明该模块还没有加载符号。先加载符号。
				load_module_symbol(hModule);
            }
            else
            {
                // 无符号下的偏移量
                displacement = (long)framepoint-(long)hModule;
            }
        }

		// 获取模块名
		get_module_name(hModule, szModuleName);

		// 获取函数名
        DWORD64 displacement64 = 0;
        pSymbol->Name[0] = 0;
		SymFromAddrW(hProcess, framepoint, &displacement64, pSymbol);

        // 输出：
        if (sourceInfo.FileName)
        {
            // 分两行输入，上一行是函数信息，下一行是代码行信息
            // 因为函数信息更重要，代码行只是用于双击定位，
            // 将代码行进行缩进，更方便的查看函数堆栈。
            mld_printf(TEXT("% 3d  %s!%s+0x%x\r\n % 80s %s(%d)\r\n"),
                i,
                szModuleName, 
                pSymbol->Name, 
                displacement,
                TEXT(" "),
                sourceInfo.FileName,
                sourceInfo.LineNumber);

//             mld_printf(TEXT("    %s(%d): %s!%s+0x%x\r\n"),
//                 sourceInfo.FileName,
//                 sourceInfo.LineNumber, 
//                 szModuleName, 
//                 pSymbol->Name, 
//                 displacement);
        }
        else
        {
            mld_printf(TEXT("% 3d  %s!%s+0x%x\r\n"),
                i,
                szModuleName, 
                pSymbol->Name,
                displacement);
        }
	}
}


// COPY FROM [VLD 2.3.0]
static const DWORD crctab[256] = {
    0x00000000, 0x77073096, 0xee0e612c, 0x990951ba, 0x076dc419, 0x706af48f,
    0xe963a535, 0x9e6495a3, 0x0edb8832, 0x79dcb8a4, 0xe0d5e91e, 0x97d2d988,
    0x09b64c2b, 0x7eb17cbd, 0xe7b82d07, 0x90bf1d91, 0x1db71064, 0x6ab020f2,
    0xf3b97148, 0x84be41de, 0x1adad47d, 0x6ddde4eb, 0xf4d4b551, 0x83d385c7,
    0x136c9856, 0x646ba8c0, 0xfd62f97a, 0x8a65c9ec, 0x14015c4f, 0x63066cd9,
    0xfa0f3d63, 0x8d080df5, 0x3b6e20c8, 0x4c69105e, 0xd56041e4, 0xa2677172,
    0x3c03e4d1, 0x4b04d447, 0xd20d85fd, 0xa50ab56b, 0x35b5a8fa, 0x42b2986c,
    0xdbbbc9d6, 0xacbcf940, 0x32d86ce3, 0x45df5c75, 0xdcd60dcf, 0xabd13d59,
    0x26d930ac, 0x51de003a, 0xc8d75180, 0xbfd06116, 0x21b4f4b5, 0x56b3c423,
    0xcfba9599, 0xb8bda50f, 0x2802b89e, 0x5f058808, 0xc60cd9b2, 0xb10be924,
    0x2f6f7c87, 0x58684c11, 0xc1611dab, 0xb6662d3d, 0x76dc4190, 0x01db7106,
    0x98d220bc, 0xefd5102a, 0x71b18589, 0x06b6b51f, 0x9fbfe4a5, 0xe8b8d433,
    0x7807c9a2, 0x0f00f934, 0x9609a88e, 0xe10e9818, 0x7f6a0dbb, 0x086d3d2d,
    0x91646c97, 0xe6635c01, 0x6b6b51f4, 0x1c6c6162, 0x856530d8, 0xf262004e,
    0x6c0695ed, 0x1b01a57b, 0x8208f4c1, 0xf50fc457, 0x65b0d9c6, 0x12b7e950,
    0x8bbeb8ea, 0xfcb9887c, 0x62dd1ddf, 0x15da2d49, 0x8cd37cf3, 0xfbd44c65,
    0x4db26158, 0x3ab551ce, 0xa3bc0074, 0xd4bb30e2, 0x4adfa541, 0x3dd895d7,
    0xa4d1c46d, 0xd3d6f4fb, 0x4369e96a, 0x346ed9fc, 0xad678846, 0xda60b8d0,
    0x44042d73, 0x33031de5, 0xaa0a4c5f, 0xdd0d7cc9, 0x5005713c, 0x270241aa,
    0xbe0b1010, 0xc90c2086, 0x5768b525, 0x206f85b3, 0xb966d409, 0xce61e49f,
    0x5edef90e, 0x29d9c998, 0xb0d09822, 0xc7d7a8b4, 0x59b33d17, 0x2eb40d81,
    0xb7bd5c3b, 0xc0ba6cad, 0xedb88320, 0x9abfb3b6, 0x03b6e20c, 0x74b1d29a,
    0xead54739, 0x9dd277af, 0x04db2615, 0x73dc1683, 0xe3630b12, 0x94643b84,
    0x0d6d6a3e, 0x7a6a5aa8, 0xe40ecf0b, 0x9309ff9d, 0x0a00ae27, 0x7d079eb1,
    0xf00f9344, 0x8708a3d2, 0x1e01f268, 0x6906c2fe, 0xf762575d, 0x806567cb,
    0x196c3671, 0x6e6b06e7, 0xfed41b76, 0x89d32be0, 0x10da7a5a, 0x67dd4acc,
    0xf9b9df6f, 0x8ebeeff9, 0x17b7be43, 0x60b08ed5, 0xd6d6a3e8, 0xa1d1937e,
    0x38d8c2c4, 0x4fdff252, 0xd1bb67f1, 0xa6bc5767, 0x3fb506dd, 0x48b2364b,
    0xd80d2bda, 0xaf0a1b4c, 0x36034af6, 0x41047a60, 0xdf60efc3, 0xa867df55,
    0x316e8eef, 0x4669be79, 0xcb61b38c, 0xbc66831a, 0x256fd2a0, 0x5268e236,
    0xcc0c7795, 0xbb0b4703, 0x220216b9, 0x5505262f, 0xc5ba3bbe, 0xb2bd0b28,
    0x2bb45a92, 0x5cb36a04, 0xc2d7ffa7, 0xb5d0cf31, 0x2cd99e8b, 0x5bdeae1d,
    0x9b64c2b0, 0xec63f226, 0x756aa39c, 0x026d930a, 0x9c0906a9, 0xeb0e363f,
    0x72076785, 0x05005713, 0x95bf4a82, 0xe2b87a14, 0x7bb12bae, 0x0cb61b38,
    0x92d28e9b, 0xe5d5be0d, 0x7cdcefb7, 0x0bdbdf21, 0x86d3d2d4, 0xf1d4e242,
    0x68ddb3f8, 0x1fda836e, 0x81be16cd, 0xf6b9265b, 0x6fb077e1, 0x18b74777,
    0x88085ae6, 0xff0f6a70, 0x66063bca, 0x11010b5c, 0x8f659eff, 0xf862ae69,
    0x616bffd3, 0x166ccf45, 0xa00ae278, 0xd70dd2ee, 0x4e048354, 0x3903b3c2,
    0xa7672661, 0xd06016f7, 0x4969474d, 0x3e6e77db, 0xaed16a4a, 0xd9d65adc,
    0x40df0b66, 0x37d83bf0, 0xa9bcae53, 0xdebb9ec5, 0x47b2cf7f, 0x30b5ffe9,
    0xbdbdf21c, 0xcabac28a, 0x53b39330, 0x24b4a3a6, 0xbad03605, 0xcdd70693,
    0x54de5729, 0x23d967bf, 0xb3667a2e, 0xc4614ab8, 0x5d681b02, 0x2a6f2b94,
    0xb40bbe37, 0xc30c8ea1, 0x5a05df1b, 0x2d02ef8d,
};

// COPY FROM [VLD 2.3.0]
DWORD CalculateCRC32(UINT_PTR p, UINT startValue)
{      
    register DWORD hash = startValue;
    hash = (hash >> 8) ^ crctab[(hash & 0xff) ^ ((p >>  0) & 0xff)];
    hash = (hash >> 8) ^ crctab[(hash & 0xff) ^ ((p >>  8) & 0xff)];
    hash = (hash >> 8) ^ crctab[(hash & 0xff) ^ ((p >> 16) & 0xff)];
    hash = (hash >> 8) ^ crctab[(hash & 0xff) ^ ((p >> 24) & 0xff)];
#ifdef WIN64
    hash = (hash >> 8) ^ crctab[(hash & 0xff) ^ ((p >> 32) & 0xff)];
    hash = (hash >> 8) ^ crctab[(hash & 0xff) ^ ((p >> 40) & 0xff)];
    hash = (hash >> 8) ^ crctab[(hash & 0xff) ^ ((p >> 48) & 0xff)];
    hash = (hash >> 8) ^ crctab[(hash & 0xff) ^ ((p >> 56) & 0xff)];
#endif
    return hash;
}

// COPY FROM [VLD 2.3.0]
DWORD hash_stack_framepoint(UINT_PTR fpArray[MAXSTACKDEPTH])
{
    DWORD hashcode = 0xD202EF8D;

    // Iterate through each frame in the call stack.
    for (UINT32 frame = 0; frame < MAXSTACKDEPTH; frame++)
    {
        UINT_PTR programcounter = fpArray[frame];
        hashcode = CalculateCRC32(programcounter, hashcode);
    }
    return hashcode;
}