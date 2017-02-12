#ifndef MEMORY_LEAK_DETECT_H_LEEIHCY
#define MEMORY_LEAK_DETECT_H_LEEIHCY
#ifdef _DEBUG
#ifdef MLD_EXPORTS
#define MLD_API __declspec(dllexport)
#else
#define MLD_API __declspec(dllimport)
#endif

////////////////////////////////////////////////////////////////////////// 
//
//   Copy From: Visual Leak Detector (Open Source)
//
//////////////////////////////////////////////////////////////////////////

#pragma comment(lib, "mld.lib")

// 外部工程只需要添加#include "mld.h"即可触发MemoryLeakDetect全局函数构造，
// 不用再调用其它导出的API
// 
// Force a symbolic reference to the global VisualLeakDetector class object from
// the DLL. This ensures that the DLL is loaded and linked with the program,
// even if no code otherwise imports any of the DLL's exports.
#pragma comment(linker, "/include:__imp_?g@@3VMemoryLeakDetect@@A")

namespace MLD
{
    extern "C"
    {
        void MLD_API PauseLeakDetect(bool bPause);
		void MLD_API ShowMldWindow();
    }
}

#endif
#endif // MEMORY_LEAK_DETECT_H_LEEIHCY


//////////////////////////////////////////////////////////////////////////
/* 要利用全局变量来实现启动时在所有内存分配之前开始监听、退出时在所有
 * 内存释放结束后报告泄露非常麻烦。
 * 主要问题有:
 *
 *  1. 如何确保自己的全局变量在其它全局变量前初始化？
 *     按照VLD的做法，使用#pragma指令。并导出一个对象：
 *     #pragma comment(linker, "/include:__imp_?g_mld@@3VMemoryLeakDetect@@A")
 *     这样外部模块只要引用了"mld.h"就会触发全局类的构造函数
 *
 *  2. 在全局类MLD中创建线程，并不能立即成功。使用waitforsingleobject(thread)
 *     反而会造成死锁。
 *     估计是因为全局类构造函数中还处于crt初始化阶段，不能立即创建成功。
 *
 *  3. 在全局类的析构函数中，其它全局数据也已被释放，包括自己的线程、自己分
 *     配的全局数据等。
 *
 *  4. 如何解决全局类/静态单例类中分配的内存，只有在程序退出时才会释放的问题？
 *     这样情况还得使用全局变量析构中leak report
 *  
 *  MLD引用动态库msvcr90d.dll，_CrtSetAllocHook只会为所有动态引用msvcr90d.dll
 *  的模块设置该HOOK，静态引用msvcr90d.dll的模块有自己的hook全局变量，不会
 *  被设置为MLD中的HOOK。
 *  因此放弃使用_CrtSetAllocHook，改为HOOK RtlAllocateHeap
 *  
 *  如何解决在report_leak期间，停止调试会导致VS夯住的问题。
 *  1. [参照VLD]每次report之后加上一个 sleep(50)。
 *     sleep(10)也不够，还是会卡住一下。
 *
 *  如何实现双击输出内容，即可定位到相应的代码行？
 *  1. 使用OutputDebugString时，内容的前缀格式为：
 *     filepath(line)
 *  2. 前面只能跟空格，不可以是其它内容
 *  3. 后面可以是任意内容
 *
 *
 *  什么时候去为MODULE加载符号？
 *  1. 没有在hook_loadlibrary中为每个新模块加载一次符号，而是在最后
 *     leak_report的时候去解析
 *  
 *  
 *
 *    原本是想专门为mld创建一个线程来处理malloc/free/report等事情，但
 *  由于要在momoryleakdetect全局类的构造/析构中处理这些事情，线程在构造函数
 *  中还创建不了，中析构函数中又提前被销毁了。
 *    因此直接使用主线程来处理，把所有的malloc/free通过postmessage放到一个
 *  mld_wnd中去排队处理。
 *    使用postmessage的方法有一个很大的缺陷，每个消息队列的最大限制为：10000.
 *  超过该限制时，消息发送将失败。
 *    最后修改为使用critical_section来同步map的多线程共享问题。
 *
 *
 *  在Hook_HeapAlloc中，怎么确定这个是否是CRT调用的内存。如果不是，则不能
 *  调用pHdr函数来获取block type。 因此目前还不支持自定义heap来分配内存。
 *
 *
 * 遗留问题：
 *  1. 如何确保mld模块优先于其它模块先被exe加载，这样memoryleakdetect全局对象
 *     就能够提前创建，最后被释放。
 *     将mld.lib设置到exe工程的：Linker->Input->Additional Dependencies的第一位
 *
 *  2. 目前判断一个block是否是crt分配的方法不够准确test_is_crt_block
 *
 */