#include "stdafx.h"
#include "cursorres.h"
#include "Src\Resource\skinres.h"
#include "Src\Resource\uicursor.h"
#include "Inc\Interface\iuires.h"
#include "Inc\Interface\imapattr.h"
//#include "other/resource.h"

namespace UI
{

//////////////////////////////////////////////////////////////////////////
//
//	CursorResItem
//
// 
// UICursor::UICursor()
// {
// 	m_dwRef = 0;
// 	m_hCursor = NULL;
// 
// 	m_pOutRef = NULL;
// }
// UICursor::~UICursor()
// {
// 	*m_pOutRef = NULL;
// 	m_pOutRef = NULL;
// 
// 	UIASSERT(NULL == m_hCursor);
// }
// 
// void UICursor::CreateInstance(const String strCurFilePath, UICursor** ppOutRef)
// {
// 	if (NULL == ppOutRef)
// 		return;
// 
// 	UICursor* pCursor = new UICursor();
// 	
// 	bool bRet = pCursor->Load(strCurFilePath.c_str());
// 	if (false == bRet)
// 	{
// 		UI_LOG_WARN(_T("%s LoadCursor Failed. filepath=%s"), FUNC_NAME, strCurFilePath.c_str());
// 		SAFE_DELETE(pCursor);
// 		*ppOutRef = NULL;
// 	}
// 	else 
// 	{
// 		*ppOutRef = pCursor;
// 	}
// }
// void UICursor::CreateInstance(LPTSTR nCursorID, HINSTANCE hInstance, UICursor** ppOutRef)
// {
// 	if (NULL == ppOutRef)
// 		return;
// 
// 	UICursor* pCursor = new UICursor();
// 	bool bRet = pCursor->Load(nCursorID, hInstance);
// 	if (false == bRet)
// 	{
// 		UI_LOG_WARN(_T("%s LoadCursor Failed. cursor id=%s, hInstance=%x"), FUNC_NAME, nCursorID, hInstance);
// 		SAFE_DELETE(pCursor);
// 		*ppOutRef = NULL;
// 	}
// 	else
// 	{
// 		*ppOutRef = pCursor;
// 	}
// }
// HCURSOR UICursor::GetCursor()
// { 
// 	return m_hCursor; 
// }
// 
// bool UICursor::Load(LPCTSTR szCursorFile)
// {
// 	if (NULL == szCursorFile)
// 		return false;
// 
// 	if (m_hCursor)
// 	{
// 		DestroyCursor(m_hCursor);
// 		m_hCursor = NULL;
// 	}
// 
// 	m_hCursor = (HCURSOR)::LoadImage(NULL, szCursorFile, IMAGE_CURSOR, 0,0, LR_DEFAULTSIZE|LR_LOADFROMFILE);
// 	return m_hCursor!=NULL ? true:false;
// }
// 
// bool  UICursor::Load(LPTSTR nCursorID, HINSTANCE hInstance)
// {
// 	if (m_hCursor)
// 	{
// 		DestroyCursor(m_hCursor);
// 		m_hCursor = NULL;
// 	}
// 
// 	m_hCursor = ::LoadCursor(hInstance, nCursorID );
// 	return m_hCursor!=NULL ? true:false;
// }
// void  UICursor::Attach(HCURSOR hCursor)
// {
// 	UIASSERT( NULL != hCursor );
// 	m_hCursor = hCursor;
// }
// HCURSOR  UICursor::Detach()
// {
// 	UIASSERT( NULL!= m_hCursor );
// 	HCURSOR hOldCursor = m_hCursor;
// 	m_hCursor = NULL;
// 
// 	return hOldCursor;
// }
// long  UICursor::AddRef()
// {
// 	m_dwRef++;
// 	return m_dwRef;
// }
// long  UICursor::Release()
// {
// 	m_dwRef--;
// 	if (0 >= m_dwRef )
// 	{
// 		if (m_hCursor)
// 		{
// 			DestroyCursor(m_hCursor);
// 			m_hCursor = NULL;
// 		}
// 		m_dwRef = 0;
// 
// 		delete this;
// 		return 0;
// 	}
// 	return m_dwRef;
// }

//////////////////////////////////////////////////////////////////////////
CursorResItem::CursorResItem()
{
	m_pCursor = NULL;
	m_pICursorResItem = NULL;
};
CursorResItem::~CursorResItem()
{
	SAFE_DELETE(m_pICursorResItem);
}

IUICursor*  CursorResItem::GetCursor()
{
	if (NULL == m_pCursor)
	{
		if (m_strCurFilePath.empty())
		{
			if (m_strID == XML_CURSOR_IDC_ARROW)
				UICursor::CreateInstance2(IDC_ARROW, NULL, &m_pCursor);
			else if (m_strID == XML_CURSOR_IDC_IBEAM)
				UICursor::CreateInstance2(IDC_IBEAM, NULL, &m_pCursor);
			else if (m_strID == XML_CURSOR_IDC_WAIT)
				UICursor::CreateInstance2(IDC_WAIT, NULL, &m_pCursor);
			else if (m_strID == XML_CURSOR_IDC_CROSS)
				UICursor::CreateInstance2(IDC_CROSS, NULL, &m_pCursor);
			else if (m_strID == XML_CURSOR_IDC_UPARROW)
				UICursor::CreateInstance2(IDC_UPARROW, NULL, &m_pCursor);
			else if (m_strID == XML_CURSOR_IDC_SIZE)
				UICursor::CreateInstance2(IDC_SIZE, NULL, &m_pCursor);
			else if (m_strID == XML_CURSOR_IDC_ICON)
				UICursor::CreateInstance2(IDC_ICON, NULL, &m_pCursor);
			else if (m_strID == XML_CURSOR_IDC_SIZENWSE)
				UICursor::CreateInstance2(IDC_SIZENWSE, NULL, &m_pCursor);
			else if (m_strID == XML_CURSOR_IDC_SIZENESW)
				UICursor::CreateInstance2(IDC_SIZENESW, NULL, &m_pCursor);
			else if (m_strID == XML_CURSOR_IDC_SIZEWE)
				UICursor::CreateInstance2(IDC_SIZEWE, NULL, &m_pCursor);
			else if (m_strID == XML_CURSOR_IDC_SIZENS)
				UICursor::CreateInstance2(IDC_SIZENS, NULL, &m_pCursor);
			else if (m_strID == XML_CURSOR_IDC_SIZEALL)
				UICursor::CreateInstance2(IDC_SIZEALL, NULL, &m_pCursor);
			else if (m_strID == XML_CURSOR_IDC_NO)
				UICursor::CreateInstance2(IDC_NO, NULL, &m_pCursor);
			else if (m_strID == XML_CURSOR_IDC_HAND)
				UICursor::CreateInstance2(IDC_HAND, NULL, &m_pCursor);
			else if (m_strID == XML_CURSOR_IDC_APPSTARTING)
				UICursor::CreateInstance2(IDC_APPSTARTING, NULL, &m_pCursor);
			else if (m_strID == XML_CURSOR_IDC_HELP)
				UICursor::CreateInstance2(IDC_HELP, NULL, &m_pCursor);
// 			else if (m_strID == XML_CURSOR_IDC_DRAG_SEPERATOR)
// 				UICursor::CreateInstance2(MAKEINTRESOURCE(IDC_CURSOR_DRAG_SEPERATOR), /*UIApplication::GetModuleInstance()*/g_hInstance, &m_pCursor);
// 			else if (m_strID == XML_CURSOR_IDC_SPLITBAR_H)
// 				UICursor::CreateInstance2(MAKEINTRESOURCE(IDC_CURSOR_SPLITBAR_H), /*UIApplication::GetModuleInstance()*/g_hInstance, &m_pCursor);
// 			else if (m_strID == XML_CURSOR_IDC_SPLITBAR_V)
// 				UICursor::CreateInstance2(MAKEINTRESOURCE(IDC_CURSOR_SPLITBAR_V), /*UIApplication::GetModuleInstance()*/g_hInstance, &m_pCursor);
			else
				return false;
		}
		else
		{
			UICursor::CreateInstance2(m_strCurFilePath, &m_pCursor);
		}
		m_pCursor->SetOutRef(&m_pCursor);
	}
	else
	{
		m_pCursor->AddRef();
	}

	return static_cast<IUICursor*>(m_pCursor);
}
bool  CursorResItem::ModifyCursor( const String& strCurFilePath )
{
	UIASSERT( false && _T("TODO: CursorResItem::ModifyCursor not implement"));
	return false;
}
ICursorResItem*  CursorResItem::GetICursorResItem()
{
	if (NULL == m_pICursorResItem)
	{
		m_pICursorResItem = new ICursorResItem(this);
	}
	return m_pICursorResItem;
}

CursorRes::CursorRes()
{
	m_pICursorRes = NULL;

	// 添加默认的系统图标
	this->InsertCursor( XML_CURSOR_IDC_ARROW, _T(""), NULL);
	this->InsertCursor( XML_CURSOR_IDC_IBEAM, _T(""), NULL);
	this->InsertCursor( XML_CURSOR_IDC_WAIT, _T(""), NULL);
	this->InsertCursor( XML_CURSOR_IDC_CROSS, _T(""), NULL);
	this->InsertCursor( XML_CURSOR_IDC_UPARROW, _T(""), NULL);
	this->InsertCursor( XML_CURSOR_IDC_SIZE, _T(""), NULL);
	this->InsertCursor( XML_CURSOR_IDC_ICON, _T(""), NULL);
	this->InsertCursor( XML_CURSOR_IDC_SIZENWSE, _T(""), NULL);
	this->InsertCursor( XML_CURSOR_IDC_SIZENESW, _T(""), NULL);
	this->InsertCursor( XML_CURSOR_IDC_SIZEWE, _T(""), NULL);
	this->InsertCursor( XML_CURSOR_IDC_SIZENS, _T(""), NULL);
	this->InsertCursor( XML_CURSOR_IDC_SIZEALL, _T(""), NULL);
	this->InsertCursor( XML_CURSOR_IDC_NO, _T(""), NULL);
	this->InsertCursor( XML_CURSOR_IDC_HAND, _T(""), NULL);
	this->InsertCursor( XML_CURSOR_IDC_APPSTARTING, _T(""), NULL);
	this->InsertCursor( XML_CURSOR_IDC_HELP, _T(""), NULL);

	// UIDLL 资源
	this->InsertCursor( XML_CURSOR_IDC_DRAG_SEPERATOR, _T(""), NULL);
	this->InsertCursor( XML_CURSOR_IDC_SPLITBAR_H, _T(""), NULL);
	this->InsertCursor( XML_CURSOR_IDC_SPLITBAR_V, _T(""), NULL);
}
CursorRes::~CursorRes()
{
	this->Clear();
	SAFE_DELETE (m_pICursorRes);
}

ICursorRes*  CursorRes::GetICursorRes()
{
	if (NULL == m_pICursorRes)
		m_pICursorRes = new ICursorRes(this);

	return m_pICursorRes;
}

CursorResItem* CursorRes::GetCursorItem( int nIndex )
{
	if (nIndex < 0)
		return NULL;
	if (nIndex >= (int)m_vCursors.size())
		return NULL;

	return m_vCursors[nIndex];
}	

CursorResItem*  CursorRes::GetCursorItem(UICursor* p)
{
	if (!p)
		return NULL;

	vector<CursorResItem*>::iterator  iter = m_vCursors.begin();
	for (; iter != m_vCursors.end(); iter++)
	{
		if ((*iter)->m_pCursor == p)
			return (*iter);
	}

	return NULL;
}

CursorResItem* CursorRes::GetCursorItem(LPCTSTR szId)
{
	vector<CursorResItem*>::iterator  iter = m_vCursors.begin();
	for (; iter != m_vCursors.end(); ++iter)
	{
		CursorResItem* p = *iter;
		if (0 == _tcscmp(p->GetId(), szId))
		{
			return p;
		}
	}
	return NULL;
}
int   CursorRes::GetCursorCount() 
{
	return (int)m_vCursors.size();
}

void CursorRes::GetCursor(LPCTSTR szCursorId, IUICursor** pp)
{
	if (NULL == szCursorId  || NULL == pp)
		return;

	CursorResItem* p = this->GetCursorItem(szCursorId);
	if (NULL == p)
	{
		UI_LOG_ERROR( _T("failed, id=%s"), szCursorId);
		return;
	}

	*pp = p->GetCursor();
}

LPCTSTR  CursorRes::GetCursorId(IUICursor* p)
{
	if (!p)
		return NULL;

	CursorResItem*  pItem = GetCursorItem(static_cast<UICursor*>(p));
	if (pItem)
		return pItem->GetId();

	return NULL;
}

CursorResItem*  CursorRes::LoadItem(IMapAttribute* pMapAttrib, LPCTSTR szFullPath)
{
	if (NULL == szFullPath)
		return NULL;

	LPCTSTR szId = pMapAttrib->GetAttr(XML_ID, true);

	CursorResItem* pItem = NULL;
	if (this->InsertCursor(szId, szFullPath, &pItem))
	{
		pItem->SetAttribute(pMapAttrib);
		return pItem;
	}
	else
	{
		UI_LOG_WARN( _T("insert cursor m_strID=%s, path=%s failed."), szId, szFullPath);
		return NULL;
	}
}

bool  CursorRes::InsertCursor(LPCTSTR szId, LPCTSTR szCurFilePath, CursorResItem** ppItem)
{
	CursorResItem* p = this->GetCursorItem(szId);
	if (p)
	{
		UI_LOG_WARN(_T("CursorRes::InsertCursor failed, insert item=%s, cursor=%s"), szId, szCurFilePath);
		return false;
	}

	CursorResItem*  pCursorItem = new CursorResItem;
	pCursorItem->SetId( szId );
	pCursorItem->SetCurFilePath(szCurFilePath);

	this->m_vCursors.push_back(pCursorItem);  
	if (ppItem)
	{
		*ppItem = pCursorItem;
	}
	return true;
}
bool  CursorRes::ModifyCursor( LPCTSTR szId, LPCTSTR szCursor )
{
	CursorResItem* p = this->GetCursorItem(szId);
	if (p)
	{
		p->ModifyCursor(szCursor);
		return true;
	}

	return false;
}

bool  CursorRes::RemoveCursor( LPCTSTR szId )
{
	if (NULL == szId)
		return false;

	vector<CursorResItem*>::iterator  iter = m_vCursors.begin();
	vector<CursorResItem*>::iterator  iterEnd = m_vCursors.end();

	for (; iter != iterEnd; iter++)
	{
		CursorResItem* p = *iter;
		if (0 == _tcscmp(p->GetId(), szId))
		{
			delete p;
			p = NULL;
			m_vCursors.erase(iter);
			return true;
		}
	}

	return false;
}
void  CursorRes::Clear()
{
	vector<CursorResItem*>::iterator  iter = m_vCursors.begin();
	vector<CursorResItem*>::iterator  iterEnd = m_vCursors.end();

	for (; iter != iterEnd; iter++)
	{
		CursorResItem* p = *iter;
		delete p;
		p = NULL;
	}

	m_vCursors.clear();
}


}