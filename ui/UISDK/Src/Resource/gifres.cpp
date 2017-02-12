#include "stdafx.h"
#include "gifres.h"
#include "Src\Resource\skinres.h"
#include "Src\Util\Gif\GifImage.h"
#include "Inc\Interface\iuires.h"
#include "Inc/Interface/imapattr.h"
#include "Src\SkinParse\datasource\skindatasource.h"

namespace UI
{

GifResItem::GifResItem(UIApplication* pUIApp)
{
	m_pMapAttrib = NULL;
	m_pGifImage = NULL;
	m_pOriginImageData = NULL;
	m_bUseSkinHLS = false;
	m_pUIApp = pUIApp;
	m_pIGifResItem = NULL;
}
GifResItem::~GifResItem()
{
	SAFE_DELETE(m_pGifImage);
	SAFE_RELEASE(m_pMapAttrib);
	SAFE_DELETE(m_pIGifResItem);
}

String& GifResItem::GetIdRef()
{
	return m_strId;
}

void GifResItem::SetId(LPCTSTR text)
{
	if (text)
		m_strId = text;
	else
		m_strId.clear();
}

void GifResItem::SetPath(LPCTSTR path)
{
	if (path)
		m_strPath = path;
	else
		m_strPath.c_str();
}

IGifResItem*  GifResItem::GetIGifResItem()
{
	if (NULL == m_pIGifResItem)
	{
		m_pIGifResItem = new IGifResItem(this);
	}
	return m_pIGifResItem;
}

GifImageBase*  GifResItem::GetGifImage(SkinRes* pSkinRes)
{
	if (NULL == m_pUIApp)
		return NULL;

	if (NULL == m_pGifImage)
	{
		String strExt = m_strPath.substr(m_strPath.length()-4, 4);

		if (0 == _tcsicmp(strExt.c_str(), _T(".gif")))
		{
			m_pGifImage = new GifImage();
		}
		else
		{
			m_pGifImage = new PngListGifImage();
		}

		SkinDataSource* pDataSource = pSkinRes->GetDataSource();
		if (!pDataSource)
			return NULL;

		IStreamBufferReader* pBuffer = NULL;
		if (!pDataSource->Load_StreamBuffer(m_strPath.c_str(), &pBuffer))
			return NULL;

		m_pGifImage->Load(pBuffer, m_pMapAttrib);
		SAFE_RELEASE(pBuffer);
	}

	return m_pGifImage;
}

void GifResItem::SetAttribute(IMapAttribute* pMapAttrib) 
{ 
	m_pMapAttrib = pMapAttrib; 
	m_pMapAttrib->AddRef();
}

GifRes::GifRes(SkinRes* p)
{
	m_pUIApp = NULL; 
	m_pIGifRes = NULL;
	m_pSkinRes = p;
}
GifRes::~GifRes()
{
	SAFE_DELETE(m_pIGifRes);
	this->Clear(); 
}

IGifRes*  GifRes::GetIGifRes()
{
	if (NULL == m_pIGifRes)
		m_pIGifRes = new IGifRes(this);

	return m_pIGifRes;
}

GifResItem* GifRes::LoadItem(IMapAttribute* pMapAttrib, const String& strFullPath)
{
	String strID;

	const TCHAR* szText = pMapAttrib->GetAttr(XML_ID, true);
	if (szText)
		strID = szText;

	GifResItem* pItem = NULL;
	if (this->InsertGif(strID, strFullPath, &pItem))
	{
		pItem->SetAttribute(pMapAttrib);
		return pItem;
	}
	else
	{
		UI_LOG_WARN( _T("insert gif id=%s, path=%s failed."), strID.c_str(), strFullPath.c_str());
		return NULL;
	}
}
int GifRes::GetGifCount() 
{
	return (int)m_vGifs.size();
}

GifResItem* GifRes::GetGifItem( int nIndex )
{
	if (nIndex < 0)
		return NULL;
	if (nIndex >= (int)m_vGifs.size())
		return NULL;

	return m_vGifs[nIndex];
}

GifResItem* GifRes::GetGifItem(const TCHAR*  szId)
{
	if (NULL == szId)
		return NULL;

	vector<GifResItem*>::iterator  iter = m_vGifs.begin();
	for  (; iter != m_vGifs.end(); iter++)
	{
		GifResItem* p = *iter;
		if (0 == _tcscmp(szId, p->GetId()))
			return p;
	}
	return NULL;
}

HRESULT  GifRes::GetGifImage(const TCHAR*  szId, __out IGifImage** ppGifImage)
{
	if (NULL == szId || NULL == ppGifImage)
		return E_FAIL;

	GifResItem* pItem = this->GetGifItem(szId);
	if (NULL == pItem)
		return E_FAIL;

	GifImageBase* pGifImage = pItem->GetGifImage(m_pSkinRes);
	if (NULL == pGifImage)
		return E_FAIL;

	*ppGifImage = pGifImage->GetIGifImage();
	return S_OK;
}

bool GifRes::InsertGif(const String& strID, const String& strPath, GifResItem** pRet)
{
	GifResItem* pItem = this->GetGifItem(strID.c_str());
	if (pItem)
	{
		UI_LOG_WARN(_T("failed, insert item=%s, path=%s"), strID.c_str(), strPath.c_str() );
		return false;
	}

	GifResItem*  pGifItem = new GifResItem(m_pUIApp);
	pGifItem->SetId(strID.c_str());
	pGifItem->SetPath(strPath.c_str());

	this->m_vGifs.push_back(pGifItem);  
	if (pRet)
		*pRet = pGifItem;
	return true;
}
bool GifRes::ModifyGif(const String& strID, const String& strPath)
{
	GifResItem* p = this->GetGifItem(strID.c_str());
	if (p)
	{
		return p->ModifyGif(strPath);
	}

	UI_LOG_WARN(_T("failed. modify gif id=%s, path=%s"), strID.c_str(), strPath.c_str() );
	return false;
}
bool GifRes::RemoveGif(const String& strID)
{
	vector<GifResItem*>::iterator  iter = m_vGifs.begin();
	vector<GifResItem*>::iterator  iterEnd = m_vGifs.end();

	for (; iter != iterEnd; iter++)
	{
		GifResItem* p = *iter;
		if (p->GetIdRef() == strID)
		{
			delete p;
			p = NULL;
			m_vGifs.erase(iter);
			return true;
		}
	}
	return false;
}
bool GifRes::Clear()
{
	vector<GifResItem*>::iterator  iter = m_vGifs.begin();
	vector<GifResItem*>::iterator  iterEnd = m_vGifs.end();

	for (; iter != iterEnd; iter++)
	{
		GifResItem* p = *iter;
		delete p;
		p = NULL;
	}

	m_vGifs.clear();
	return true;
}

}