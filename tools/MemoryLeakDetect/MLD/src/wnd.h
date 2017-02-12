#pragma once

#define THREAD_MSG_ID_BASE    (WM_USER+2256)
#define THREAD_MSG_ID_MALLOC  (THREAD_MSG_ID_BASE+1)
#define THREAD_MSG_ID_FREE    (THREAD_MSG_ID_BASE+2)
#define THREAD_MSG_ID_REPORT  (THREAD_MSG_ID_BASE+3) // �޸�Ϊ���߳�report��mld�̹߳���
#define THREAD_MSG_ID_CLEAR   (THREAD_MSG_ID_BASE+4)
#define THREAD_MSG_ID_SUSPEND (THREAD_MSG_ID_BASE+5)
#define THREAD_MSG_ID_HEAPDESTROY  (THREAD_MSG_ID_BASE+6)
#define THREAD_MSG_ID_PAUSE  (THREAD_MSG_ID_BASE+7)  // wParam: 0��ʼ,1��ͣ
#define THREAD_MSG_ID_MODULEALLOCINFO_REQ (THREAD_MSG_ID_BASE+8)
#define THREAD_MSG_ID_MODULELIST_UPDATE_REQ (THREAD_MSG_ID_BASE+9)

enum LEAK_REPORT_TYPE  
{
    LEAK_REPORT_TYPE_FAST,
    LEAK_REPORT_TYPE_TIME,
    LEAK_REPORT_TYPE_COUNT,
    LEAK_REPORT_TYPE_BYTES,
};
extern LEAK_REPORT_TYPE  g_report_type;

void set_appmodulelist_dirty();
HMODULE  get_report_filter_module();
