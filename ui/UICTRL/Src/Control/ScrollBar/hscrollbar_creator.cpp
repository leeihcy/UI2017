#include "stdafx.h"
#include "hscrollbar_creator.h"
#include "Inc\Interface\iscrollbar.h"

#define HSCROLLBAR_ID  TEXT("__hscrollbar")

HScrollbarCreator::HScrollbarCreator()
{
	m_pHScrollbar = NULL;
}
HScrollbarCreator::~HScrollbarCreator()
{
	
}

void  HScrollbarCreator::CreateByEditor(CREATEBYEDITORDATA* pData, IObject* pParent)
{
	EditorAddObjectResData data = {0};
	data.pUIApp = pData->pUIApp;
	data.pParentXml = pData->pXml;
	data.pParentObj = pParent;
	data.bNcChild = true;
	
	data.objiid = __uuidof(ISystemHScrollBar);
	data.ppCreateObj = (void**)&m_pHScrollbar;
	data.szId = HSCROLLBAR_ID;
	pData->pEditor->AddObjectRes(&data);
}

void  HScrollbarCreator::Initialize(IObject* pParent, IScrollBarManager* pMgr)
{
	IObject* pObj = pParent->FindNcObject(HSCROLLBAR_ID);
	if (pObj)
	{
		m_pHScrollbar = (ISystemHScrollBar*)pObj->
			QueryInterface(__uuidof(ISystemHScrollBar));
	}

	if (!m_pHScrollbar)
	{
		return;
		// 不自动创建了，全由xml控件吧
// 		IUIApplication* pUIApp = pParent->GetUIApplication();
// 		ISystemVScrollBar::CreateInstance(pUIApp, &m_pHScrollbar);
// 		pParent->AddNcChild(m_pHScrollbar);

	}
	if (m_pHScrollbar)
	{
		m_pHScrollbar->SetVisible(false); 
		m_pHScrollbar->SetIScrollBarMgr(pMgr);
	}
}