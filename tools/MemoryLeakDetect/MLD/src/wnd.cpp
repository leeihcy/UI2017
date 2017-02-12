#include "stdafx.h"
#include "wnd.h"
#include "thread.h"
#include "hook.h"
#include "utility.h"


#define MLD_WND_CLASSNAME  TEXT("mldwnd")
HWND  g_hWnd = NULL;
HWND  g_hPauseButton = NULL;
HWND  g_hClearButton = NULL;
HWND  g_hReportButton = NULL;
HWND  g_hModuleAllocGraphicsButton = NULL;
HWND  g_hComboReportType = NULL;
HWND  g_hComboModuleFilter = NULL;
HWND  g_hAllocOnceCheckBox = NULL;
HFONT  g_hFont = NULL;

bool  g_bNeedUpdateModuleList;

LEAK_REPORT_TYPE  g_report_type = LEAK_REPORT_TYPE_COUNT;


HWND  CreateButton(LPCTSTR szText, RECT* prc, UINT nId=-1)
{
	HWND hWnd = CreateWindowEx(
		0,TEXT("Button"), szText,
		WS_CHILD|WS_VISIBLE|WS_TABSTOP,
		prc->left, prc->top,
		prc->right-prc->left,
		prc->bottom-prc->top,
		g_hWnd, (HMENU)nId, 
		g_hInstance, 0);
	SendMessage(hWnd, WM_SETFONT, (WPARAM)g_hFont, 1);
	return hWnd;
}

HWND  CreateCombo(RECT* prc, bool bSort, UINT nId=-1)
{
    long lStyle = WS_CHILD|WS_VISIBLE|WS_TABSTOP|CBS_DROPDOWNLIST|WS_VSCROLL;
    if (bSort)
        lStyle |= CBS_SORT;

    HWND hWnd = CreateWindowEx(
        0,TEXT("ComboBox"), NULL,
        lStyle,
        prc->left, prc->top,
        prc->right-prc->left,
        prc->bottom-prc->top,
        g_hWnd, (HMENU)nId, 
        g_hInstance, 0);
    SendMessage(hWnd, WM_SETFONT, (WPARAM)g_hFont, 1);
    return hWnd;
}

HWND  CreateCheckBox(RECT* prc, LPCTSTR szText)
{
    long lStyle = WS_VISIBLE|WS_CHILD|WS_TABSTOP|BS_AUTOCHECKBOX;
    HWND hWnd = CreateWindowEx(
        0,TEXT("BUTTON"), szText,
        lStyle,
        prc->left, prc->top,
        prc->right-prc->left,
        prc->bottom-prc->top,
        g_hWnd, (HMENU)-1, 
        g_hInstance, 0);
    SendMessage(hWnd, WM_SETFONT, (WPARAM)g_hFont, 1);
    SendMessage(hWnd, BM_SETCHECK, 1, 0);
    return hWnd;
}

void  CreateLabel(RECT* prc, LPCTSTR szText)
{
    HWND hWnd = CreateWindowEx(
        0,TEXT("Static"), szText,
        WS_CHILD|WS_VISIBLE,
        prc->left, prc->top,
        prc->right-prc->left,
        prc->bottom-prc->top,
        g_hWnd, (HMENU)-1, 
        g_hInstance, 0);
    SendMessage(hWnd, WM_SETFONT, (WPARAM)g_hFont, 1);
}

void  OnCreate(HWND hWnd)
{
	g_hWnd = hWnd;

	LOGFONT  lf = {0};
	lf.lfHeight = -12;
	_tcscpy_s(lf.lfFaceName, 31, TEXT("宋体"));
	lf.lfWeight = FW_NORMAL;
	g_hFont = CreateFontIndirect(&lf);
	
    RECT rcBtnPause = {10,10,80, 36};
    g_hPauseButton = CreateButton(
        g.bPause?TEXT("开始"):TEXT("暂停"), &rcBtnPause);

	RECT rcBtnClear = {100,10,180, 36};
	g_hClearButton = CreateButton(TEXT("清除记录"), &rcBtnClear);

	RECT rcBtnReport = {200,10,280, 36};
	g_hReportButton = CreateButton(TEXT("泄漏报告"), &rcBtnReport);

    RECT rcBtnModuleAllocInfo = {300,10, 380, 36};
    g_hModuleAllocGraphicsButton = CreateButton(TEXT("模块报告"), &rcBtnModuleAllocInfo);

    RECT rcLabel = { 12, 56, 50, 70 };
    CreateLabel(&rcLabel, TEXT("排序："));
    RECT rcCombo = { 45, 52, 145, 140 };
    g_hComboReportType = CreateCombo(&rcCombo, false);
    SendMessage(g_hComboReportType, CB_ADDSTRING, 0, (LPARAM)TEXT("快速"));
    SendMessage(g_hComboReportType, CB_ADDSTRING, 0, (LPARAM)TEXT("时间(未实现)"));
    SendMessage(g_hComboReportType, CB_ADDSTRING, 0, (LPARAM)TEXT("次数"));
    SendMessage(g_hComboReportType, CB_ADDSTRING, 0, (LPARAM)TEXT("字节数"));
    SendMessage(g_hComboReportType, CB_SETCURSEL, g_report_type, 0);

    RECT rcLabel2 = { 160, 56, 220, 70 };
    CreateLabel(&rcLabel2, TEXT("模块过滤："));
    RECT rcCombo2 = { 220, 52, 380, 140 };
    g_hComboModuleFilter = CreateCombo(&rcCombo2, true);
    ComboBox_AddString(g_hComboModuleFilter, TEXT("  全部"));
    ComboBox_SetCurSel(g_hComboModuleFilter, 0);

    RECT rcCheckBox = {12, 90, 160, 115};
    g_hAllocOnceCheckBox = CreateCheckBox(&rcCheckBox, TEXT("不显示只泄漏1次的堆栈"));
    g.bFilterOneCount = true;
}

void  OnDestroy()
{
	DeleteObject(g_hFont);
	g_hFont = NULL;
	g_hWnd = NULL;
	g_hClearButton = NULL;
}


void  UpdateModuleList()
{
    if (!g_bNeedUpdateModuleList)
        return;
    g_bNeedUpdateModuleList = false;

    HMODULE hModuleOld = get_report_filter_module();

    ComboBox_ResetContent(g_hComboModuleFilter);
    ComboBox_AddString(g_hComboModuleFilter, TEXT("  全部")); // 前面加空格，让排在第一位
    int nOldIndex = 0;

    map<HMODULE, MODULE_TYPE> m;
    g_hookMgr.CopyModuleMap(m);

    TCHAR szName[MAX_PATH];
    map<HMODULE, MODULE_TYPE>::iterator iter = m.begin();
    for (; iter != m.end(); ++iter)
    {
        if (iter->second != MODULE_APPLICATION && 
            iter->second != MODULE_GDIPLUS)
            continue;

        HMODULE hModule = iter->first;
        get_module_name(hModule, szName);

        int nIndex = ComboBox_AddString(g_hComboModuleFilter, szName);
        ComboBox_SetItemData(g_hComboModuleFilter, nIndex, hModule);

        if (hModule == hModuleOld)
        {
            nOldIndex = nIndex;
        }
    }
    ComboBox_SetCurSel(g_hComboModuleFilter, nOldIndex);
}

void  OnCommand(WPARAM wParam, HWND hCtrl)
{
	if (hCtrl == g_hClearButton)
	{
		lock_handle_message(THREAD_MSG_ID_CLEAR, 0, 0);
	}
	else if (hCtrl == g_hReportButton)
	{
		lock_handle_message(THREAD_MSG_ID_REPORT, 0, 0);
	}
    else if (hCtrl == g_hPauseButton)
    {
        if (g.bPause)
        {
            SetWindowText(g_hPauseButton, TEXT("暂停"));
        }
        else
        {
            SetWindowText(g_hPauseButton, TEXT("开始"));
        }
        g.bPause = !g.bPause;
    }
    else if (hCtrl == g_hAllocOnceCheckBox)
    {
        int iCheck = (int)SendMessage(g_hAllocOnceCheckBox, BM_GETCHECK, 0, 0);
        if (iCheck == 1)
        {
            g.bFilterOneCount = true;
        }
        else
        {
            g.bFilterOneCount = false;
        }
    }
    else if (hCtrl == g_hComboReportType)
    {
        if (HIWORD(wParam) == CBN_SELCHANGE)
        {
            g_report_type = (LEAK_REPORT_TYPE)SendMessage(
                g_hComboReportType, CB_GETCURSEL, 0, 0);
        }
    }
    else if (hCtrl == g_hComboModuleFilter)
    {
        if (HIWORD(wParam) == CBN_DROPDOWN)
        {
            UpdateModuleList();
        }
    }
    else if (hCtrl == g_hModuleAllocGraphicsButton)
    {
        lock_handle_message(
            THREAD_MSG_ID_MODULEALLOCINFO_REQ, 0, 0);
    }
}


HMODULE  get_report_filter_module()
{
    HMODULE hModule = NULL;
    int nCurSel = ComboBox_GetCurSel(g_hComboModuleFilter);
    if (nCurSel != 0)
        hModule = (HMODULE)ComboBox_GetItemData(g_hComboModuleFilter, nCurSel);
    return hModule;
}

LRESULT CALLBACK MLDWNDPROC(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_CREATE:
		OnCreate(hWnd);
		break;

	case WM_CLOSE:
		::ShowWindow(hWnd, SW_MINIMIZE);
		return 0;

	case WM_DESTROY:
		OnDestroy();
		break;

	case WM_COMMAND:
		OnCommand(wParam, (HWND)lParam);
		break;

    case THREAD_MSG_ID_MODULELIST_UPDATE_REQ:
        g_bNeedUpdateModuleList = true;
        break;

    case WM_CTLCOLORSTATIC:
        return (HRESULT)(HBRUSH)GetStockObject(WHITE_BRUSH);
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

void set_appmodulelist_dirty()
{
    if (!g_hWnd)
        return;

    PostMessage(g_hWnd, THREAD_MSG_ID_MODULELIST_UPDATE_REQ, 0, 0);
}

namespace MLD
{
extern "C"
void __declspec(dllexport) ShowMldWindow()
{
	if (g_hWnd)
		return;

	WNDCLASS wndclass = {0};
	if (!GetClassInfo(g_hInstance, MLD_WND_CLASSNAME, &wndclass))	
	{
		WNDCLASSEX wcex = {0};
		wcex.cbSize = sizeof(WNDCLASSEX);
		wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
		wcex.lpszClassName = MLD_WND_CLASSNAME;
		wcex.lpfnWndProc = MLDWNDPROC;
		wcex.hInstance = g_hInstance;
		wcex.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);

		if (0 == RegisterClassEx(&wcex))
		{
			assert(0);
		}
	}

    POINT pt = {0};
    MONITORINFO mi = {sizeof(mi)};
    GetMonitorInfo(MonitorFromPoint(pt, MONITOR_DEFAULTTOPRIMARY), &mi);
    int w = 420;
    int h = 155;
    pt.x = mi.rcWork.right-w;
    pt.y = mi.rcWork.bottom-h;

	g_hWnd = CreateWindowEx(
		WS_EX_TOPMOST, MLD_WND_CLASSNAME, TEXT("Memory Leak Detect Console"),
		WS_OVERLAPPEDWINDOW|WS_VISIBLE,
		pt.x, pt.y, w, h,
		0,
		0,
		g_hInstance,0);
	assert(g_hWnd);	
}

}




#if 0
#include "thread.h"
#include "memoryleakdetect.h"
#include "print.h"

void try_flush_mld_wnd_msg()
{
	// 为了解决postmessage exceed limit问题，每次在主线程中主动去flush
	if (IsMainThread())
	{
		flush_mld_wnd_msg();
	}
	else
	{
		// 给主线程机会消化缓存数据
		Sleep(100);
	}
}

// 注：有可能在其它线程分配的内存，然后在主线程释放。
//     结果其它线程postmessage，主线程直接调用，导致先处理了free，
//     再处理malloc。
//  因此这里修改为全部排队。在leak report时调用flush_mld_wnd_msg
//  强制取出所有的消息进行处理
void post_mld_wnd_msg(UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (!g_hWnd)
	{
		create_msg_wnd();
	}

	while (!PostMessage(g_hWnd, msg, wParam, lParam))
	{
		// post的消息总数超出最大限制：10000
		DebugBreak();

		try_flush_mld_wnd_msg();
	}
}


// 每个消息队列默认最多可以缓存10000个消息。超过了的话，就会失败。
// There is a limit of 10,000 posted messages per message queue. 
// This limit should be sufficiently large. If your application 
// exceeds the limit, it should be redesigned to avoid consuming
// so many system resources. To adjust this limit, modify the 
// following registry key. 
void flush_mld_wnd_msg()
{
	if (!g_hWnd)
		return;

	MSG msg;
	while (PeekMessage(&msg, g_hWnd, 0, 0, PM_REMOVE))
	{
		handle_message(msg.message, msg.wParam, msg.lParam);
	}
}
#endif