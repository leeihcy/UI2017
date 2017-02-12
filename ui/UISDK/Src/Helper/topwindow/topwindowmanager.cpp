#include "stdafx.h"
#include "topwindowmanager.h"

#include "Inc\Interface\iobject.h"
#include "Inc\Interface\iwindow.h"
#include "Src\Helper\layout\layout.h"
#include "Src\Resource\skinres.h"
#include "Src\Base\Object\object.h"
#include "Src\UIObject\Window\windowbase.h"
#include "Src\Base\Application\uiapplication.h"

TopWindowManager::TopWindowManager(UIApplication* p)
{
    m_pITopWindowManager = NULL;
    m_pUIApplication = p;
}
TopWindowManager::~TopWindowManager()
{
    SAFE_DELETE(m_pITopWindowManager);
	UIASSERT(m_lTopWindowObject.empty())
}
ITopWindowManager*  TopWindowManager::GetITopWindowManager()
{
    if (NULL == m_pITopWindowManager)
        m_pITopWindowManager = new ITopWindowManager(this);

    return m_pITopWindowManager;
}

HRESULT TopWindowManager::AddTopWindowObject(WindowBase* pObj)
{
	if (NULL == pObj)
		return E_INVALIDARG;

	_MyIter  iter = m_lTopWindowObject.begin();
	_MyIter  iterEnd = m_lTopWindowObject.end();
	for (; iter!=iterEnd; iter++)
	{
		if (pObj == *iter)
			return S_FALSE;
	}

	this->m_lTopWindowObject.push_back( pObj );

	UI_LOG_DEBUG( _T("TopWindowManager::AddTopWindowObject, ID=%s"), pObj->GetId());
	return S_OK;
}

HRESULT TopWindowManager::RemoveTopWindowObject(WindowBase* pObj)
{
	if (NULL == pObj)
		return E_INVALIDARG;

	_MyIter  iter = m_lTopWindowObject.begin();
	_MyIter  iterEnd = m_lTopWindowObject.end();
	for(; iter!=iterEnd; iter++)
	{
		if (pObj == *iter)
		{
			m_lTopWindowObject.erase( iter );

			UI_LOG_DEBUG( _T("TopWindowManager::RemoveTopWindowObject, ID=%s"), pObj->GetId());
			return S_OK;
		}
	}
	return E_FAIL;
}

void TopWindowManager::ClearTopWindowObject( )
{
	this->m_lTopWindowObject.clear();

	UI_LOG_DEBUG( _T("TopWindowManager::ClearTopWindowObject ") );
}

IWindowBase* TopWindowManager::GetWindowBase(HWND hWnd)
{
	_MyIter  iter = m_lTopWindowObject.begin();
	_MyIter  iterEnd = m_lTopWindowObject.end();
	for( ; iter!=iterEnd; iter++ )
	{
		WindowBase* p = *iter;
		if( p->GetHWND() == hWnd )
			return p->GetIWindowBase();
	}

	return NULL;
}

//
//	�޸�Ƥ�����õ�ǰ���д������¼���
//
#if 0
void TopWindowManager::ChangeSkin(SkinRes* pNewSkinRes)
{
	if( NULL == pNewSkinRes )
		return;

	// ps: ??? �������Ĵ���Ӧ�ò����ڰ�...����ֻҪ����ʹ��Ƥ����Դ�Ļ�����Ҫ���� ��2012.12.17��
	vector<WindowBase*>  vecNotJoinChangeSkinWindows;  // �������뻻���Ĵ��ڷ��ڸ��б��У�����ֻ��Ҫ����һ��UI_WM_SKINCHANGING��Ϣ

	_MyIter  iter = m_lTopWindowObject.begin();
	for( ; iter!=m_lTopWindowObject.end(); iter++ )
	{
		WindowBase* pWindow = *iter;
		if (NULL == pWindow)
			continue;

		BOOL bChangeSkin = TRUE;
		UISendMessage(pWindow, UI_MSG_SKINCHANGING, (WPARAM)&bChangeSkin, 0);
		if (FALSE == bChangeSkin)
		{
			vecNotJoinChangeSkinWindows.push_back(pWindow);
			continue;
		}

		//////////////////////////////////////////////////////////////////////////
		//
		// 1. �����е���Ԫ�����У����ں������²���

		list<Object*>  listAllChildObjects;
		this->GetAllChildIntoList((*iter), listAllChildObjects);

		// ������ж������֯��ϵ
		list<Object*>::iterator iter2 = listAllChildObjects.begin();
		for (; iter2 != listAllChildObjects.end(); iter2++)
		{
			(*iter2)->ClearMyTreeRelationOnly();
		}
		pWindow->ClearMyTreeRelationOnly();
        UISendMessage(pWindow, UI_WM_RESETATTRIBUTE);  // ��������һ�Σ����ڷ�ֹReLoadLayoutʧ�ܣ��Ӷ���ȫ����ɾ���������ڵ�������û�б����á�
		pNewSkinRes->ReLoadLayout(pWindow, listAllChildObjects);

		// ��ʣ��û���ٱ�ʹ�õ�object�ͷŵ�
		iter2 = listAllChildObjects.begin();
		for ( ; iter2 != listAllChildObjects.end(); iter2++ )
		{
            Object* pObj = *iter2;
            pWindow->GetMouseMgr()->OnObjectRemoveInd(pObj);   // <-- ����pObj�Ѿ�û����window�����ˣ���������ﵥ������
            
			pObj->GetIObject()->delete_this();
			pObj = NULL;
		}
		listAllChildObjects.clear();
	}
	
	//
	// ˢ�´��ڣ����ں���ͳһˢ�£���ΪChangeSkin�����Ƚ������ᵼ�¸�������֮����ʱ���
	//
	// ע�����ڷֲ㴰�ڣ����SetWindowPosǰ��SIZEδ��Ļ�����ֲ㴰���ղ���WM_SIZE��Ϣ��
	//	   ��Ҳ�����·ֲ㴰�ڵò���ˢ�£������������Ҫ��Ը��������������
	//	   TODO: ��û��ֱ�����Ӵ�С��ͬҲ����WM_SIZE�ķ�����
	//
	for (iter = m_lTopWindowObject.begin(); iter!=m_lTopWindowObject.end(); iter++)
	{
		WindowBase* pWindow = *iter;
		if (NULL == pWindow)
			continue;

		// �ô��ڲ����뻻��
		int nSize = (int)vecNotJoinChangeSkinWindows.size();
		for (int i = 0; i < nSize; i++)
		{
			if (vecNotJoinChangeSkinWindows[i] == pWindow)
				continue;
		}

// 		int nOldWidth = pWindow->GetWidth();
// 		int nOldHeight = pWindow->GetHeight();

		DesktopLayout dl;
		dl.Arrange(pWindow);

        // ֪ͨ
        UISendMessage(pWindow, UI_WM_SKINCHANGED, 0, 0);  // <-- ����arrange���棬��Ϊ���ܹ�����Ӧ����ȷ�õ��ؼ�λ��
                                                          // <-- ����Updateǰ�棬��Ϊ���ܹ����ػ�֮ǰ���»�ȡ�ؼ�ָ�룬�������

//		::SetWindowPos( pWindow->m_hWnd, NULL, pWindow->GetParentRectL(), pWindow->GetParentRectT(), 
//			pWindow->GetWidth(), pWindow->GetHeight(), SWP_NOZORDER|SWP_NOACTIVATE );  // ������������Դ���Ѿ������ˣ�Ӧ�÷�ֹ������ˢ��

// 		if (nOldWidth == pWindow->GetWidth() && nOldHeight == pWindow->GetHeight())
 		{
// 			if (WS_EX_LAYERED & ::GetWindowLong(pWindow->m_hWnd,GWL_EXSTYLE))
// 			{
				pWindow->Invalidate();
// 			}
 		}
	}
}
#endif

//
//	�ı䵱ǰƤ����ɫ��
//
bool TopWindowManager::UpdateAllWindow( )
{
	_MyIter  iter = m_lTopWindowObject.begin();
	_MyIter  iterEnd = m_lTopWindowObject.end();

	// ˢ�´���
	for( iter = m_lTopWindowObject.begin(); iter!=iterEnd; iter++ )
	{
		WindowBase* pWindow = *iter;
        if (NULL == pWindow)
            continue;

		pWindow->Invalidate();
	}
	return true;
}

//
//	�����еĶ��㴰�ڷ�����Ϣ
//
void TopWindowManager::SendMessage2AllWnd(UIMSG* pMsg)
{
	_MyIter  iter = m_lTopWindowObject.begin();
	_MyIter  iterEnd = m_lTopWindowObject.end();
	for ( ; iter!=iterEnd; iter++)
	{
		 WindowBase* pObj = *iter;
         if (NULL == pObj)
             continue;

		 pMsg->pMsgTo = pObj->GetIMessage();
		 ::UISendMessage(pMsg);
	}
}
// void  TopWindowManager::PostMessage2AllWnd(UIMSG* pMsg)
// {
//     _MyIter  iter = m_lTopWindowObject.begin();
//     for (; iter != m_lTopWindowObject.end(); iter++)
//     {
//         WindowBase* pObj = *iter;
//         if (NULL == pObj)
//             continue;
// 
//         pMsg->pMsgTo = pObj->GetIMessage();
//         ::UIPostMessage(m_pUIApplication->GetIUIApplication(), pMsg);
//     }
// }
void  TopWindowManager::ForwardMessage2AllObj(UIMSG*  pMsg)
{
    _MyIter  iter = m_lTopWindowObject.begin();
    for (; iter != m_lTopWindowObject.end(); iter++)
    {
        WindowBase* pObj = *iter;
        if (NULL == pObj)
            continue;

        pMsg->pMsgTo = pObj->GetIMessage();
        UISendMessage(pMsg);

        pObj->ForwardMessageToChildObject(pObj, pMsg);
    }
}

//
//	��һ��������Ӷ����ɢ��ȫ����һ��list���������²��֣�����һ��OBJ_CONTROL�������ٲ�֣�����child�ͱ�������һ������
//
void TopWindowManager::GetAllChildIntoList(Object* pParent, list<Object*>& listObjects)
{
    if (NULL == pParent)
        return;

    if (pParent->GetDescription() && OBJ_CONTROL == pParent->GetDescription()->GetMajorType())
        return;

	Object*  pChild = NULL;
	while (pChild = pParent->EnumChildObject(pChild))
	{
		listObjects.push_back(pChild);

        if (pParent->GetDescription() && OBJ_CONTROL != pParent->GetDescription()->GetMajorType())
			this->GetAllChildIntoList(pChild, listObjects);
	}
}

