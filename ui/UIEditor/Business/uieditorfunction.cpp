#include "stdafx.h"
#include "uieditorfunction.h"
#include "Dialog\Framework\MainFrame.h"
#include "UISDK\Inc\Interface\ixmlwrap.h"
#include "Dialog\Framework\statusbar.h"
#include "Dialog\Module\Layout\LayoutEditorWnd.h"
#include "Dialog\Framework\propertydialog.h"
#include "UISDK\Inc\Interface\ilayout.h"

bool  UE_AddSkin(LPCTSTR szDir, LPCTSTR szSkinName)
{
    return false;
}
// 
// bool  UE_AddObjectAttribute(ISkinRes* pSkinRes, IObject* pObject, IUIElement* pUIElement, LPCTSTR szKey, LPCTSTR szValue)
// {
//     if (NULL == pSkinRes || 
//         NULL == pObject  ||
//         NULL == pUIElement ||
//         NULL == szKey    ||
//         NULL == szValue)
//         return false;
// 
//     if (pUIElement->HasAttrib(szKey))
//     {
//         String  str = _T("该属性已存在: ");
//         str.append(szKey);
//         g_pGlobalData->m_pStatusBar->SetStatusText2(str.c_str());
//         return false;
//     }
// 
//     if (false == pUIElement->AddAttrib(szKey, szValue))
//     {
//         String  str = _T("添加属性失败: ");
//         str.append(szKey);
//         str.append(_T(" = \""));
//         str.append(szValue);
//         str.append(_T("\""));
//         g_pGlobalData->m_pStatusBar->SetStatusText2(str.c_str());
//         return false;
//     }
// 
//     // 设置dirty属性
//     IUIDocument* pDoc = NULL;
//     pUIElement->GetDocument(&pDoc);
//     pDoc->SetDirty(true);
//     SAFE_RELEASE(pDoc);
// 
//     g_pGlobalData->m_pMainFrame->SetDirty();
// 
//     // 刷新界面
//     UE_ReloadObjectAttribute(pSkinRes, pObject, pUIElement);
//     UE_RefreshLayoutView(pSkinRes, pObject);
// 
//     return true;
// }

//
//  删除指定对象的某个属性
//
// bool  UE_RemoveObjectAttribute(ISkinRes* pSkinRes, IObject* pObject, IUIElement* pUIElement, LPCTSTR szAttrib)
// {
//     if (NULL == pSkinRes || 
//         NULL == pObject  ||
//         NULL == pUIElement ||
//         NULL == szAttrib)
//         return false;
// 
//     if (!pUIElement->HasAttrib(szAttrib))
//     {
//         String  str = _T("该属性不存在: ");
//         str.append(szAttrib);
//         g_pGlobalData->m_pStatusBar->SetStatusText2(str.c_str());
//         return false;
//     }
// 
//     if (false == pUIElement->RemoveAttrib(szAttrib))
//     {
//         String  str = _T("删除属性字段失败: ");
//         str.append(szAttrib);
//         g_pGlobalData->m_pStatusBar->SetStatusText2(str.c_str());
//         return false;
//     }
// 
//     // 设置dirty属性
//     IUIDocument* pDoc = NULL;
//     pUIElement->GetDocument(&pDoc);
//     pDoc->SetDirty(true);
//     SAFE_RELEASE(pDoc);
// 
//     g_pGlobalData->m_pMainFrame->SetDirty();
// 
//     // 刷新界面
//     UE_ReloadObjectAttribute(pSkinRes, pObject, pUIElement);
//     UE_RefreshLayoutView(pSkinRes, pObject);
//     return true;
// }

//
// 清空对象所有属性
//
// bool  UE_ClearObjectAttribute(ISkinRes* pSkinRes, IObject* pObject, IUIElement* pUIElement)
// {
//     if (NULL == pSkinRes ||
//         NULL == pObject  ||
//         NULL == pUIElement)
//         return false;
// 
//     if (false == pUIElement->ClearAttrib())
//     {
//         String  str = _T("清空对象属性失败");
//         g_pGlobalData->m_pStatusBar->SetStatusText2(str.c_str());
//         return false;
//     }
// 
//     // 设置dirty属性
//     IUIDocument* pDoc = NULL;
//     pUIElement->GetDocument(&pDoc);
//     pDoc->SetDirty(true);
//     SAFE_RELEASE(pDoc);
// 
//     g_pGlobalData->m_pMainFrame->SetDirty();
// 
//     // 刷新界面
//     UE_ReloadObjectAttribute(pSkinRes, pObject, pUIElement);
//     UE_RefreshLayoutView(pSkinRes, pObject);
//     return true;
// }

//
// 修改对象属性
//
// bool  UE_ModifyObjectAttribute(ISkinRes* pSkinRes, IObject* pObject, IUIElement* pUIElement, LPCTSTR szKey, LPCTSTR szValue)
// {
//     if (NULL == pSkinRes || 
//         NULL == pObject  ||
//         NULL == pUIElement ||
//         NULL == szKey    ||
//         NULL == szValue)
//         return false;
// 
//     if (!pUIElement->HasAttrib(szKey))
//     {
//         String  str = _T("该属性不存在: ");
//         str.append(szKey);
//         g_pGlobalData->m_pStatusBar->SetStatusText2(str.c_str());
//         return false;
//     }
// 
//     if (false == pUIElement->ModifyAttrib(szKey, szValue))
//     {
//         String  str = _T("修改属性字段失败: ");
//         str.append(szKey);
//         str.append(_T(" = \""));
//         str.append(szValue);
//         str.append(_T("\""));
//         g_pGlobalData->m_pStatusBar->SetStatusText2(str.c_str());
//         return false;
//     }
// 
//     // 设置dirty属性
//     IUIDocument* pDoc = NULL;
//     pUIElement->GetDocument(&pDoc);
//     pDoc->SetDirty(true);
//     SAFE_RELEASE(pDoc);
// 
//     g_pGlobalData->m_pMainFrame->SetDirty();
// 
//     // 刷新界面
//     UE_ReloadObjectAttribute(pSkinRes, pObject, pUIElement);
//     UE_RefreshLayoutView(pSkinRes, pObject);
//     return true;
// }

// 
//  重新加载对象属性，并刷新
//
bool  UE_ReloadObjectAttribute(IObject* pObject)
{
	IPropertyHandler* pHandler = g_pGlobalData->m_pPropertyDialog->GetHandler();
	if (!pHandler)
		return false;

	if (pHandler->GetHandlerType() != LayoutPropertyHandler::PROPERTY_HANDLER_TYPE)
		return false;

	static_cast<LayoutPropertyHandler*>(pHandler)->ReloadObjectAttribute(pObject);
	return true;
}

// 重新设置属性树中某一些的值为pObject的现有值
void  UE_UpdateAttributeInPropertyCtrl(IObject* pObject, LPCTSTR szKey)
{
	IPropertyHandler* pHandler = g_pGlobalData->m_pPropertyDialog->GetHandler();
	if (!pHandler)
		return;

	if (pHandler->GetHandlerType() != LayoutPropertyHandler::PROPERTY_HANDLER_TYPE)
		return;

	static_cast<LayoutPropertyHandler*>(pHandler)->UpdateObjectAttributeInCtrl(pObject, szKey);

// 	if (g_pGlobalData->m_pPropertyDialog)
// 		g_pGlobalData->m_pPropertyDialog->UpdateObjectAttributeInCtrl(pObject, szKey);
}

// 刷新UI窗口
void  UE_RefreshUIWindow(IObject* pObject)
{
    if (!pObject)
        return;

    IWindowBase*  pWindowBase = pObject->GetWindowObject();
    if (!pWindowBase)
        return;

    pWindowBase->GetLayout()->Arrange();
    pWindowBase->Invalidate();
}

//
//  刷新窗口预览视图，同时更新窗口大小，以解决修改窗口大小时layoutview::m_rcWindowDraw
//
bool  UE_RefreshLayoutView(IObject* pObject)
{
    HWND hWnd = g_pGlobalData->m_pMainFrame->GetLayoutHWND(pObject);
    if (hWnd)
    {
        ::SendMessage(hWnd, CLayoutEditorDlg::WM_USER_REFRESH, 0, 0);
        return true;
    }

    return false;
}


HRESULT  UE_ExecuteCommand(ICommand* p, bool bDelay)
{
	if (bDelay)
	{
		g_pGlobalData->m_pCmdHistroyMgr->PostExecute(p);
		return 0;
	}
	else
	{
		return g_pGlobalData->m_pCmdHistroyMgr->Execute(p);
	}
}