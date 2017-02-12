#include "stdafx.h"
#include "richeditolemgr.h"
#include "Gif\gifoleobject.h"
#include <algorithm>
#include "3rd\markup\markup.h"
#include "..\windowlessrichedit.h"

REOleBase::REOleBase() 
{ 
    m_dwRef = 0;
    m_pOleObjectMgr = nullptr;
//	m_pExternalData = nullptr;
	m_bVisibleInField = false;
	m_pIREOleBase = nullptr;
}

REOleBase::~REOleBase()
{
//     if (m_pExternalData)
//     {
//         m_pExternalData->Release();
//         m_pExternalData = NULL;
//     }
    if (m_pOleObjectMgr)
    {
        m_pOleObjectMgr->OnOleDelete(this);
    }
	if (m_pIREOleBase)
	{
		delete m_pIREOleBase;
		m_pIREOleBase = nullptr;
	}
}

void  REOleBase::SetInterface(IREOleBase* p)
{
	m_pIREOleBase = p;
}
IREOleBase* REOleBase::GetIREOleBase()
{
	return m_pIREOleBase;
}

long    REOleBase::AddRefImpl()
{
    return ++m_dwRef;
}
long    REOleBase::ReleaseImpl()
{
    if (0 == --m_dwRef)
    {
        delete this;
        return 0;
    }
    return m_dwRef;
}

// void  REOleBase::SetExternalData(IREOleExternalData* p)
// {
//     if (m_pExternalData)
//         m_pExternalData->Release();
// 
//     m_pExternalData = p;
// }
// IREOleExternalData*  REOleBase::GetExternalData() 
// {
//     return m_pExternalData;
// }

void  REOleBase::SetOleObjectManager(REOleManager* pMgr) 
{
    m_pOleObjectMgr = pMgr; 
}

WindowlessRichEdit*  REOleBase::GetWindowlessRE()
{
    if (!m_pOleObjectMgr)
        return NULL;

    return m_pOleObjectMgr->GetRE();
}

bool  REOleBase::NeedWindowMessage() 
{
    return true/*false*/; 
}
LRESULT  REOleBase::ProcessMessage(UINT msg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) 
{
    bHandled = FALSE;
    return 0;
}

bool  REOleBase::IsVisibleInField()
{
	return m_bVisibleInField;
}

// 调用了该函数，即表示该Ole在屏幕中是可见的
void  REOleBase::OnInternalDraw(HDC hDC, RECT* prc)
{
    int nWidth     = prc->right-prc->left;
    int nHieght    = prc->bottom-prc->top;
    int nWidthOld  = m_rcDrawSave.Width();
    int nHeightOld = m_rcDrawSave.Height();

    CopyRect(&m_rcDrawSave, prc);
    m_bVisibleInField = true;

    if (nWidthOld != nWidth || nHeightOld != nHieght)
    {
        OnSizeChanged(nWidth, nHieght);
    }
}

void  REOleBase::ClearVisibleInFieldFlag()
{
	m_bVisibleInField = false;
}

void  REOleBase::GetDrawRect(RECT* prc)
{
    CopyRect(prc, &m_rcDrawSave);
}

void  REOleBase::AddBizProp(LPCTSTR key, LPCTSTR value)
{
	if (!key)
		return;

	if (value)
	{
		m_mapBizProps[String(key)] = String(value);
	}
	else
	{
		m_mapBizProps.erase(String(key));
	}
}
void  REOleBase::ClearBizProp()
{
	m_mapBizProps.clear();
}
LPCTSTR  REOleBase::GetBizProp(LPCTSTR key)
{
	if (!key)
		return nullptr;

	auto iter = m_mapBizProps.find(String(key));
	if (iter == m_mapBizProps.end())
		return nullptr;

	return iter->second.c_str();
}

UI::IRichEdit* UI::REOleBase::GetRichEdit()
{

	if (!m_pOleObjectMgr)
		return nullptr;

	WindowlessRichEdit* p = m_pOleObjectMgr->GetRE();
	if (!p)
		return nullptr;

	return p->GetControlPtr();
}

//////////////////////////////////////////////////////////////////////////

REOleManager::REOleManager(WindowlessRichEdit* pRichEdit)
{
	m_pRichEdit = pRichEdit;
	m_pUIApp = NULL;
    m_bDestroying = false;
    m_pHoverOle = NULL;
    m_pPressOle = NULL;
}
REOleManager::~REOleManager()
{
    m_bDestroying = true;
    m_pPressOle = m_pHoverOle = NULL;

	{
		OLELISTITER iter = m_listOleObj.begin();
		for (; iter != m_listOleObj.end(); iter++)
		{
			REOleBase* pItem = *iter;
			SAFE_RELEASE(pItem);  // 如果ole obj析构，会触发OnOleObjDelete，删除iter
		}
        m_listOleObj.clear();
	}

	// 释放/提交 剪贴板对象
	{
		DATAOBJECTSET::iterator iter = m_setDataObject.begin();
		for (; iter != m_setDataObject.end(); iter = m_setDataObject.begin()) 
		{
			IDataObject* pData = *iter;
			if (S_OK == OleIsCurrentClipboard(pData))
			{
				OleFlushClipboard();
			}
			SAFE_RELEASE(pData);    // 会触发OnDataObjectRelease，删除iter
		}
		m_setDataObject.clear();
	}
}

bool REOleManager::AddOle(REOleBase* pItem)
{
	if (NULL == pItem)
		return false;

	pItem->SetOleObjectManager(this);
	m_listOleObj.push_back(pItem);  // 不需要引用计数。

	return true;
}

// ~REOleBase()会调用OnOleObjDelete
void  REOleManager::OnOleDelete(REOleBase* pItem)
{
    if (!pItem)
        return;

    // 有可能是在销毁过程中，释放的ole object，此时还在遍历m_listOleObj当中，不要再去erase
    if (m_bDestroying)
        return;

    OLELISTITER iter = std::find(m_listOleObj.begin(), m_listOleObj.end(), pItem);
    if (iter != m_listOleObj.end())
    {
        m_listOleObj.erase(iter);
    }

    if (m_pHoverOle == pItem)
        m_pHoverOle = NULL;
    if (m_pPressOle = pItem)
        m_pPressOle = NULL;
}

// REOleBase*  REOleManager::FindOle(long lType, LPCTSTR  szId)
// {
//     if (!szId)
//         return NULL;
// 
//     OLELISTITER iter = m_listOleObj.begin();
//     for (; iter != m_listOleObj.end(); iter++)
//     {
//         REOleBase* pItem = *iter;
// 
//         if (pItem->GetOleType() != lType)
//             continue;
// 
//         if (0 == _tcscmp(pItem->GetId(), szId))
//             return pItem;
//     }    
// 
//     return NULL;
// }

void  REOleManager::CreateDataObject(IDataObject** pp)
{
	if (!pp)
		return;

// 	OleDataObject* p = new OleDataObject(this);
// 	p->AddRef();

	IDataObject* pDataObj = NULL;
	UI::CreateDataObjectInstance(&pDataObj);
	if (!pDataObj)
		return;

    IDataObjectEx* pDataObjectEx = NULL;
    pDataObj->QueryInterface(IID_IDataObjectEx, (void**)&pDataObjectEx);
    if (pDataObjectEx)
    {
        pDataObjectEx->SetSource(static_cast<IDataObjectSource*>(this));
        pDataObjectEx->Release();
    }

	m_setDataObject.insert(pDataObj);
	*pp = pDataObj;
}

void REOleManager::SetUIApplication(IUIApplication* p)
{
	m_pUIApp = p;
}

WindowlessRichEdit*  REOleManager::GetRE()
{
	return m_pRichEdit; 
}

IRichEditOle*  REOleManager::GetREOle()
{
    if (!m_pRichEdit)
        return NULL;

    return m_pRichEdit->GetRichEditOle();
}

BOOL REOleManager::OnRenderData(IDataObject* p, LPFORMATETC lpFormatEtc, LPSTGMEDIUM lpStgMedium) 
{
	return FALSE;  // not implement
}

void  REOleManager::OnDataRelease(IDataObject* p) 
{
	DATAOBJECTSET::iterator iter = m_setDataObject.find(p);
	if (iter != m_setDataObject.end())
	{
		m_setDataObject.erase(iter);
	}
}

REOleBase*  REOleManager::HitTest(POINT ptInControl)
{
    RECT  rcOle = {0};

    // 将pt从窗口坐标转换成控件坐标

    OLELISTRITER iter = m_listOleObj.rbegin();
    for (; iter != m_listOleObj.rend(); iter++)
    {
        REOleBase*  pOle = *iter;

        if (!pOle->IsVisibleInField())
            continue;

        pOle->GetDrawRect(&rcOle);
        if (PtInRect(&rcOle, ptInControl))
            return pOle;
    }

    return NULL;

#if 0
    int nStart = 0, nEnd = 0;
    m_pRichEdit->GetVisibleCharRange(&nStart, &nEnd);

    IRichEditOle*  pREOle = GetREOle();
    if (!pREOle)
        return NULL;

    int nOleCount = pREOle->GetObjectCount();

    REOBJECT  reObj = {0};
    reObj.cbStruct = sizeof(reObj);

    for (int i = 0; i < nOleCount; i++)
    {
        pREOle->GetObject(i, &reObj, 0);
        if (reObj.cp < nStart)
            continue;
        if (reObj.cp > nEnd)
            break;

        RECT  rcOle;
        m_pRichEdit->GetCharRect(reObj.cp, &rcOle);
        if (PtInRect(&rcOle, pt))
            return (REOleBase*)reObj.dwUser;
    }

    return NULL;
#endif
}

LRESULT  REOleManager::OnWindowMessage(UINT msg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    bHandled = FALSE;

    switch (msg)
    {
    case WM_MOUSEMOVE:
        return OnMouseMove(wParam, lParam, bHandled);
        break;

    case WM_LBUTTONDOWN:
        return OnLButtonDown(wParam, lParam, bHandled);
        break;

    case WM_LBUTTONDBLCLK:
        return OnLButtonDBClick(wParam, lParam, bHandled);
        break;

    case WM_LBUTTONUP:
        return OnLButtonUp(wParam, lParam, bHandled);
        break;
        
    case WM_SETCURSOR:
        return OnSetCursor(wParam, lParam, bHandled);
        break;

    case WM_MOUSELEAVE:
        return OnMouseLeave(wParam, lParam, bHandled);
        break;

    case WM_SIZE:
        return OnSize(wParam, lParam, bHandled);
        break;
    }
    return 0;
}

void  REOleManager::WindowPoint2ObjectClientPoint(POINT* ptWnd, POINT* ptClient)
{
    m_pRichEdit->GetControlPtr()->WindowPoint2ObjectClientPoint(ptWnd, ptClient, true);
}

LRESULT  REOleManager::OnMouseMove(WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    POINT pt = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
    WindowPoint2ObjectClientPoint(&pt, &pt);
    lParam = MAKELPARAM(pt.x, pt.y);

    REOleBase*  pNowHover = this->HitTest(pt);

    if (pNowHover && !pNowHover->NeedWindowMessage())
        pNowHover = NULL;

    if (m_pHoverOle == pNowHover)
    {
        if (m_pHoverOle)
        {
            BOOL bHandled2 = FALSE;
            m_pHoverOle->ProcessMessage(WM_MOUSEMOVE, wParam, lParam, bHandled2);
            if (bHandled2)
                bHandled = TRUE;
        }
    }
    else
    {
        if (m_pHoverOle)
        {
            if (m_pPressOle && m_pPressOle == m_pHoverOle)
            {
                m_pPressOle = NULL;
            }

            BOOL bHandled2 = FALSE;
            m_pHoverOle->ProcessMessage(WM_MOUSELEAVE, wParam, lParam, bHandled2);
            if (bHandled2)
                bHandled = TRUE;
        }
        if (pNowHover)
        {
            BOOL bHandled2 = FALSE;
            pNowHover->ProcessMessage(WM_MOUSEMOVE, wParam, lParam, bHandled2);
            if (bHandled2)
                bHandled = TRUE;
        }
        m_pHoverOle = pNowHover;
    }

    return 0;
}

LRESULT  REOleManager::OnMouseLeave(WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    if (m_pPressOle)
    {
        m_pPressOle = NULL;
    }
    if (m_pHoverOle)
    {
        m_pHoverOle->ProcessMessage(WM_MOUSELEAVE, wParam, lParam, bHandled);
        m_pHoverOle = NULL;
    }
    bHandled = FALSE;
    return 0;
}

LRESULT  REOleManager::OnLButtonDown(WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    bHandled = FALSE;

    if (m_pHoverOle)
    {
        POINT pt = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
        WindowPoint2ObjectClientPoint(&pt, &pt);
        lParam = MAKELPARAM(pt.x, pt.y);

        m_pPressOle = m_pHoverOle;
        return m_pHoverOle->ProcessMessage(WM_LBUTTONDOWN, wParam, lParam, bHandled);
    }
    return 0;
}
LRESULT  REOleManager::OnLButtonDBClick(WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    bHandled = FALSE;

    if (m_pHoverOle)
    {
        POINT pt = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
        WindowPoint2ObjectClientPoint(&pt, &pt);
        lParam = MAKELPARAM(pt.x, pt.y);

        m_pPressOle = m_pHoverOle;
        return m_pHoverOle->ProcessMessage(WM_LBUTTONDBLCLK, wParam, lParam, bHandled);
    }
    return 0;
}
LRESULT  REOleManager::OnLButtonUp(WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    bHandled = FALSE;
    if (m_pPressOle)
    {
        POINT pt = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
        WindowPoint2ObjectClientPoint(&pt, &pt);
        lParam = MAKELPARAM(pt.x, pt.y);

        REOleBase* pSave = m_pPressOle;
        m_pPressOle = NULL;

        return pSave->ProcessMessage(WM_LBUTTONUP, wParam, lParam, bHandled);
    }
    return 0;
}

LRESULT REOleManager::OnSetCursor(WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    bHandled = FALSE;
    if (m_pHoverOle)
    {
        return m_pHoverOle->ProcessMessage(WM_SETCURSOR, wParam, lParam, bHandled);
    }
    return 0;
}

LRESULT  REOleManager::OnSize(WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    bHandled = FALSE;
    OLELISTITER iter = m_listOleObj.begin();
    for (; iter != m_listOleObj.end(); iter++)
    {
        REOleBase* pItem = *iter;

        BOOL  b= FALSE;
        pItem->ProcessMessage(WM_SIZE, wParam, lParam, b);
        if (b)
            bHandled = TRUE;
    }

    return 0;
}

void  REOleManager::OnPreDraw(HDC hDC, RECT* prcDraw)
{
	OLELISTITER iter = m_listOleObj.begin();
	for (; iter != m_listOleObj.end(); iter++)
	{
		(*iter)->ClearVisibleInFieldFlag();
	}
}