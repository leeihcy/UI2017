#pragma once


// 重要的数据放在这个类当中，不要放在外面申明为全局变量。
// 程序退出时MemoryLeakDetect最后释放，要保证其它数据不能先被释放。

#define MAXSTACKDEPTH  32

struct AllocInfo
{
    LPVOID  lpMem;            // 内存地址
    SIZE_T  dwBytes;          // 分配字节数
    int     nBlockType;       // 内存类型
    DWORD   dwHash;           // HASH值
    DWORD   dwSameHashCount;  // 分配次数。临时变量，只在每次统计时有用。而且同一HASH只有一个AllocInfo的dwSameHashCount有值
    UINT_PTR  framepointArray[MAXSTACKDEPTH];  // 分配时的堆栈
};

class HeapAllocs
{
public:
    HeapAllocs();
    ~HeapAllocs();

    HANDLE hHeap;
    map<LPVOID, AllocInfo*>  map_allocs;
};

class MemoryLeakDetect
{
public:
    MemoryLeakDetect();
    ~MemoryLeakDetect();

    void clear_map_heaps();

public:
    map<HANDLE, HeapAllocs*>  map_heaps;
	DWORD  dwMainThreadId;
    bool  bPause;  // 暂停记录malloc，但仍记录free/destroy。未加锁
    bool  bFilterOneCount;  // 只分配1次的时候不打印
};

extern __declspec(dllexport) MemoryLeakDetect g;

inline bool IsMainThread()
{
	return GetCurrentThreadId() == g.dwMainThreadId;
}