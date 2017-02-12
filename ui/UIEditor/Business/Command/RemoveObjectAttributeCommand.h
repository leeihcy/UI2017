#pragma once
#include "Base/icommand.h"
#include "UISDK\Inc\Interface\ixmlwrap.h"
#include "Dialog\Framework\statusbar.h"
#include "Dialog\Framework\MainFrame.h"

class RemoveObjectAttributeCommand : public CommandBase
{
public:
    static ICommand*  CreateInstance(ISkinRes* pSkinRes, IObject* pObject, IUIElement* pUIElement, LPCTSTR szKey)
    {
        if (NULL == pSkinRes || 
            NULL == pObject  ||
            NULL == pUIElement ||
            NULL == szKey)
            return NULL;

        if (!pUIElement->HasAttrib(szKey))
        {
            String  str = _T("�����Բ�����: ");
            str.append(szKey);
            g_pGlobalData->m_pStatusBar->SetStatusText2(str.c_str());
            return NULL;
        }

        RemoveObjectAttributeCommand* p = new RemoveObjectAttributeCommand;
        p->AddRef();

        p->pSkinRes = pSkinRes;
        p->pObject = pObject;
        p->pUIElement = pUIElement;
        p->strKey = szKey;

        // ��ȡvalue����redo
        CComBSTR  bstrValue;
        pUIElement->GetAttrib(szKey, &bstrValue);
        p->strValue = (BSTR)bstrValue;

        return p;
    }
    virtual long execute()
    {
        if (false == pUIElement->RemoveAttrib(strKey.c_str()))
        {
            String  str = _T("ɾ�������ֶ�ʧ��: ");
            str.append(strKey.c_str());
            g_pGlobalData->m_pStatusBar->SetStatusText2(str.c_str());
            return -1;
        }
        // ͬ�������Կؼ�
        // TODO:
        UIASSERT(0);
//         if (g_pGlobalData->m_pPropertyDialog->m_pObject == this->pObject)
//         {
//             g_pGlobalData->m_pPropertyDialog->RemoveAttribInCtrl(strKey.c_str());
//         }

        on_post_do();
        return 0;
    }
    virtual long undo()
    {
        if (false == pUIElement->AddAttrib(strKey.c_str(), strValue.c_str()))
        {
            String  str = _T("��������ʧ��: ");
            str.append(strKey.c_str());
            str.append(_T(" = \""));
            str.append(strValue.c_str());
            str.append(_T("\""));
            g_pGlobalData->m_pStatusBar->SetStatusText2(str.c_str());
            return -1;
        }

        // ͬ�������Կؼ�
        // TODO:
        UIASSERT(0);
//         if (g_pGlobalData->m_pPropertyDialog->m_pObject == this->pObject)
//         {
//             g_pGlobalData->m_pPropertyDialog->AddAttributeInCtrl(strKey.c_str(), strValue.c_str());
//         }
        
        on_post_do();
        return 0;
    }

	long*  GetKey(void)
	{
		UIASSERT(pObject);
		IWindowBase*  pWindow = pObject->GetWindowObject();
		if (pWindow)
			return (long*)pWindow;
		else
			return (long*)pObject;
	}

private:
    void  on_post_do()
    {
        // ����dirty����
        SetDirty(pUIElement);

        // ˢ�½���
        UE_ReloadObjectAttribute(pObject);
        UE_RefreshUIWindow(pObject);
        UE_RefreshLayoutView(pObject);
    }

public:
    ISkinRes*     pSkinRes;
    IObject*      pObject;
    IUIElement*   pUIElement;
    String  strKey;
    String  strValue;
};