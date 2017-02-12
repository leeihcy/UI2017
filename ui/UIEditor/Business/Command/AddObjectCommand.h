#pragma once
#include "Base\icommand.h"
#include "UISDK\Inc\Interface\ixmlwrap.h"
#include "Dialog\Framework\statusbar.h"
#include "Dialog\Framework\MainFrame.h"
#include "Business\uieditorfunction.h"
#include "UISDK\Inc\Interface\ilayout.h"


// 添加一个控件
class AddObjectCommand : public CommandBase
{
public:
    AddObjectCommand()
    {
        m_pCreateObj = NULL;
        m_pParent = NULL;
        m_pCreateElem = NULL;
        m_bNeedDeleteObject = false;
        m_nConfigLeft = NDEF;
        m_nConfigTop = NDEF;
		m_pObjDesc = nullptr;
    }
    ~AddObjectCommand()
    {
        SAFE_RELEASE(m_pCreateElem);

        if (m_bNeedDeleteObject && m_pCreateObj)
        {
            SAFE_RELEASE(m_pCreateObj);
        }
    }

    // 如果该控件位于canvas布局中的话，可以指定nLeft,nTop位置，-1表示不设置
    static ICommand*  CreateInstance(
		ISkinRes* pSkinRes, 
		IObject* pParent,
		IObjectDescription* pObjDesc,
		int nLeft=NDEF, int nTop=NDEF)
    {
        if (NULL == pParent  ||
            NULL == pObjDesc)
            return NULL;

        AddObjectCommand* p = new AddObjectCommand;
        p->AddRef();
        p->m_pParent = pParent;
		p->m_pObjDesc = pObjDesc;
        p->m_nConfigLeft = nLeft;
        p->m_nConfigTop = nTop;

        return p;
    }
    virtual long execute()
    {
		IListItemBase*  pItem = GetUIEditor()->GetObjectHTREEITEM(m_pParent);
		IUIElement*     pParentElem = GetUIEditor()->GetObjectXmlElem(m_pParent);
		
		if (!pItem || !pParentElem)
			return -1;

        if (NULL == m_pCreateObj)
		{
			m_pObjDesc->CreateInstance(m_pParent->GetSkinRes(), (void**)&m_pCreateObj);
		}
        m_pParent->AddChild(m_pCreateObj);

		m_pCreateElem = pParentElem->AddChild(m_pObjDesc->GetTagName()).get();
        if (m_pCreateElem)
            m_pCreateElem->AddRef();

		m_pCreateObj->InitDefaultAttrib();

		// 通知对象创建，初始化一些参数。如大小、子对象等
		CREATEBYEDITORDATA  data = {0};
		data.pUIApp = GetEditUIApplication();
		data.pXml = m_pCreateElem;
		data.pEditor = GetUIEditor();

		UISendMessage(m_pCreateObj, UI_MSG_CREATEBYEDITOR, (WPARAM)&data, 0,0,0, UIALT_CALLLESS);

		UIMSG msg1, msg2;
		msg1.message = UI_MSG_INITIALIZE;
		msg2.message = UI_MSG_INITIALIZE2;
		UISendMessage(m_pCreateObj, UI_MSG_INITIALIZE);
		m_pCreateObj->ForwardMessageToChildObject2(&msg1, &msg2);
		UISendMessage(m_pCreateObj, UI_MSG_INITIALIZE2);

		// 坐标初始化(目前只支持canvas布局)
        ILayoutParam*  pLayoutParam = m_pCreateObj->GetSafeLayoutParam();
		if (pLayoutParam && pLayoutParam->GetLayoutType() == LAYOUT_TYPE_CANVAS)
		{
			ICanvasLayoutParam*  pCanvasLayoutParam = static_cast<ICanvasLayoutParam*>(pLayoutParam);
			pCanvasLayoutParam->SetConfigLeft(m_nConfigLeft);
			pCanvasLayoutParam->SetConfigTop(m_nConfigTop);

			long width = 0;
			long height = 0;
			m_pObjDesc->GetDefaultSize(width, height);

			pCanvasLayoutParam->SetConfigWidth(width);
			pCanvasLayoutParam->SetConfigHeight(height);
		}

		GetProjectData()->m_uieditor.OnObjectAttributeLoad(m_pCreateObj, m_pCreateElem);
		GetUIEditor()->SaveAttribute2Xml(m_pCreateObj);

		// TODO: 发送UI_WM_INITIALIZE消息
		// TODO: 加载子控件

		g_pGlobalData->m_pProjectTreeDialog->InsertLayoutChildObjectNode(m_pCreateObj, pItem);
		g_pGlobalData->m_pProjectTreeDialog->LoadObjectChildNode(m_pCreateObj);

		// 强制刷新窗口
		UE_RefreshUIWindow(m_pParent);
		UE_RefreshLayoutView(m_pParent);

		// 选中新创建的对象
		CChildFrame*  pChildFrame = g_pGlobalData->m_pMainFrame->GetLayoutChildFrameByObject(m_pParent);
		if (pChildFrame)
		{
			ILayoutMDIClientView* pView = (ILayoutMDIClientView*)pChildFrame->m_pClientView->QueryInterface(IID_ILayoutMDIClientView);
			pView->SelectObject(m_pCreateObj);
		}

        m_bNeedDeleteObject = false;
        on_post_do();

        return 0;
    }
    virtual long undo()
    {
		GetUIEditor()->OnObjectDeleteInd(m_pCreateObj);

        IUIElement*  pParentElem = GetUIEditor()->GetObjectXmlElem(m_pParent);

        m_pParent->RemoveChildInTree(m_pCreateObj);
        pParentElem->RemoveChild(m_pCreateElem);
        SAFE_RELEASE(m_pCreateElem);

        //SAFE_RELEASE(m_pCreateObj);  // 不能删除！后续的其它命令操作都依赖于iobject指针

        UE_RefreshUIWindow(m_pParent);
        UE_RefreshLayoutView(m_pParent);

        m_bNeedDeleteObject = true;
        on_post_do();
        return 0;
    }

	long*  GetKey(void)
	{
		UIASSERT(m_pParent);
		IWindowBase*  pWindow = m_pParent->GetWindowObject();
		if (pWindow)
			return (long*)pWindow;
		else
			return (long*)m_pParent;
	}

private:
    void  on_post_do()
    {
        // 设置dirty属性
        if (m_pCreateElem)
        {
            SetDirty(m_pCreateElem);
        }
    }

public:
    IObject*      m_pParent;
	IObjectDescription*  m_pObjDesc;
    IObject*      m_pCreateObj;
    IUIElement*   m_pCreateElem;
    int           m_nConfigLeft;
    int           m_nConfigTop;

    bool          m_bNeedDeleteObject;
};

