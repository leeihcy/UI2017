#include "stdafx.h"
#include "vscrollbar_creator.h"
#include "Inc\Interface\iscrollbar.h"

#define VSCROLLBAR_ID  TEXT("__vscrollbar")

VScrollbarCreator::VScrollbarCreator()
{
	m_pVScrollBar = NULL;
}
VScrollbarCreator::~VScrollbarCreator()
{
	
}

void  VScrollbarCreator::CreateByEditor(CREATEBYEDITORDATA* pData, IObject* pParent)
{
	EditorAddObjectResData data = {0};
	data.pUIApp = pData->pUIApp;
	data.pParentXml = pData->pXml;
	data.pParentObj = pParent;
	data.bNcChild = true;
	
	data.objiid = __uuidof(ISystemVScrollBar);
	data.ppCreateObj = (void**)&m_pVScrollBar;
	data.szId = VSCROLLBAR_ID;
	pData->pEditor->AddObjectRes(&data);
}

void  VScrollbarCreator::Initialize(IObject* pParent, IScrollBarManager* pMgr)
{
	IObject* pObj = pParent->FindNcObject(VSCROLLBAR_ID);
	if (pObj)
	{
		m_pVScrollBar = (ISystemVScrollBar*)pObj->
			QueryInterface(__uuidof(ISystemVScrollBar));
	}

	if (!m_pVScrollBar)
	{
		return;
		// 不自动创建了，全由xml控件吧
// 		IUIApplication* pUIApp = pParent->GetUIApplication();
// 		ISystemVScrollBar::CreateInstance(pUIApp, &m_pVScrollBar);
// 		pParent->AddNcChild(m_pVScrollBar);

	}
	if (m_pVScrollBar)
	{
		m_pVScrollBar->SetVisible(false); 
		m_pVScrollBar->SetIScrollBarMgr(pMgr);
	}
}