#pragma once
#include "Base\icommand.h"
#include "Business\uieditorfunction.h"

// 删除一个控件
class RemoveObjectCommand : public CommandBase
{
public:
	RemoveObjectCommand()
	{
		m_pRemoveElem = NULL;
		m_pRemoveObject = NULL;
		m_pWindow = NULL;
		m_pParentObj = NULL;
        m_pPrevObj = NULL;

        m_bNeedDeleteObject = false;
	}
	~RemoveObjectCommand()
	{
	    SAFE_RELEASE(m_pRemoveElem);

        if (m_bNeedDeleteObject && m_pRemoveObject)
        {
            SAFE_RELEASE(m_pRemoveObject);
        }
	}

	long*  GetKey(void)
	{
		UIASSERT(m_pWindow);
		return (long*)m_pWindow;
	}

	static ICommand*  CreateInstance(IObject* pObj)
	{
		if (NULL == pObj)
			return NULL;

		// 这个类先不支持窗口的删除
		if (!pObj->GetParentObject())
			return NULL;

		RemoveObjectCommand* p = new RemoveObjectCommand;
		p->AddRef();
		p->m_pRemoveObject = pObj;
		p->m_pRemoveElem = GetUIEditor()->GetObjectXmlElem(pObj);
		p->m_pRemoveElem->AddRef();  // 解决在m_uieditor.OnObjectDeleteInd中会释放elem的问题，多保持一份引用
		p->m_pWindow = pObj->GetWindowObject();
		p->m_pParentObj = pObj->GetParentObject();
        p->m_pPrevObj = pObj->GetPrevObject();

		return p;
	}
	virtual long execute()
	{
		// 1. 从编辑器中删除
		GetProjectData()->m_uieditor.OnObjectDeleteInd(m_pRemoveObject);

		// 2. 从父对象中移除
		m_pParentObj->RemoveChildInTree(m_pRemoveObject);

		// 3. 从xml中移除
        IUIElement*  pParentElem = GetUIEditor()->GetObjectXmlElem(m_pParentObj);
		pParentElem->RemoveChild(m_pRemoveElem);

		// 刷新layoutview
		CChildFrame*  pChildFrame = g_pGlobalData->m_pMainFrame->GetLayoutChildFrameByObject(m_pParentObj);
		if (pChildFrame)
		{
			::InvalidateRect(pChildFrame->m_pClientView->GetHWND(), NULL, TRUE);
		}

        m_bNeedDeleteObject = true;
		on_post_do();
		return 0;
	}
	virtual long undo()
	{
        IUIElement*  pPrevElem = NULL;
        if (m_pPrevObj)
        {
            pPrevElem = GetUIEditor()->GetObjectXmlElem(m_pPrevObj);
        }
        IUIElement*  pParentElem = GetUIEditor()->GetObjectXmlElem(m_pParentObj);


		// 添加回去
		m_pParentObj->InsertChild(m_pRemoveObject, m_pPrevObj);
		pParentElem->AddChildAfter(m_pRemoveElem, pPrevElem);

		// 刷新
        UIASSERT(0);
		// m_pParentObj->UpdateMyLayout(false);
		GetProjectData()->m_uieditor.OnObjectAttributeLoad(m_pRemoveObject, m_pRemoveElem);

        // 加载到编辑器树上
        IListItemBase*  pInsertAfter = NULL;
        if (m_pPrevObj)
            pInsertAfter = GetUIEditor()->GetObjectHTREEITEM(m_pPrevObj);
        if (!pInsertAfter)
            pInsertAfter = UITVI_FIRST;

        IListItemBase* pParentItem = GetProjectData()->m_uieditor.GetObjectHTREEITEM(m_pParentObj);
		g_pGlobalData->m_pProjectTreeDialog->InsertLayoutChildObjectNode(
            m_pRemoveObject, 
            pParentItem, 
            pInsertAfter);
        g_pGlobalData->m_pProjectTreeDialog->LoadObjectChildNode(m_pRemoveObject);

		// 刷新layoutview
		CChildFrame*  pChildFrame = g_pGlobalData->m_pMainFrame->GetLayoutChildFrameByObject(m_pParentObj);
		if (pChildFrame)
		{
			// 选中新创建的对象
			ILayoutMDIClientView* pView = (ILayoutMDIClientView*)pChildFrame->m_pClientView->QueryInterface(IID_ILayoutMDIClientView);
			pView->SelectObject(m_pRemoveObject);
		}

        m_bNeedDeleteObject = false;
		on_post_do();
		return 0;
	}

private:
	void  on_post_do()
	{
        // 设置dirty属性
        SetDirty(m_pRemoveElem);

        // 强制刷新窗口
        UE_RefreshUIWindow(m_pParentObj);
        UE_RefreshLayoutView(m_pParentObj);
	}
public: 
	IObject*        m_pRemoveObject;
	IUIElement*     m_pRemoveElem;
	IObject*        m_pParentObj;
    IObject*        m_pPrevObj;
	IWindowBase*    m_pWindow;

    bool   m_bNeedDeleteObject;   // 在执行删除命令时，不会真正删除对象，只是从父对象中移除而已。因此需要判断下最后是否需要释放该对象
};