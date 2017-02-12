#include "stdafx.h"
#include "fontres.h"
#include "Src\Renderlibrary\gdi\gdifont.h"
#include "Src\Resource\skinres.h"
#include "Inc\Interface\iuires.h"

int FontHeight2Size(int nHeight);

FontResItem::FontResItem() 
{ 
    m_pIFontResItem = NULL;
	m_pGdiFont = NULL;
}
FontResItem::~FontResItem()
{ 
	SAFE_DELETE(m_pGdiFont);
    SAFE_DELETE(m_pIFontResItem);
}

IFontResItem*  FontResItem::GetIFontResItem()
{
    if (NULL == m_pIFontResItem)
        m_pIFontResItem = new IFontResItem(this);

    return m_pIFontResItem;
}
//
// 通过结构体来快速赋值
//
void FontResItem::SetLogFont( LOGFONT* pLogFont )
{
	if (!pLogFont)
		return;

	memcpy(&m_lf, pLogFont, sizeof(LOGFONT));
	_tcscpy(m_lf.lfFaceName, pLogFont->lfFaceName);
	m_lf.lfEscapement = m_lf.lfOrientation;    // TODO: 实现字体角度的话，这两个值好像要相等
}

void FontResItem::ModifyFont(LOGFONT* pLogFont)
{
	this->SetLogFont(pLogFont);

	if (m_pGdiFont)
	{
		m_pGdiFont->ModifyFont(pLogFont);
	}
}

int FontResItem::GetFontSize()
{
	return FontHeight2Size(m_lf.lfHeight); 
}

IRenderFont* FontResItem::GetFont(SkinRes* pSkinRes, GRAPHICS_RENDER_LIBRARY_TYPE eRenderType )
{
	switch(eRenderType)
	{
	case GRAPHICS_RENDER_LIBRARY_TYPE_GDI:
		{
			if (NULL == m_pGdiFont)
			{
				GDIRenderFont::CreateInstance((IRenderFont**)&m_pGdiFont);
				if (m_pGdiFont)
				{
					m_pGdiFont->SetOutRef((IRenderResource**)&m_pGdiFont);
					m_pGdiFont->Load(&m_lf);
					UI_LOG_DEBUG(_T("gdi font create: %s, facename=%s, size=%d, Ptr=0x%08X"), m_strId.c_str(), m_lf.lfFaceName, m_lf.lfHeight, m_pGdiFont );
				}
                return (IRenderFont*)m_pGdiFont;
			}
			if (m_pGdiFont )
			{
				m_pGdiFont->AddRef();
			}
			return (IRenderFont*)m_pGdiFont;
		}
		break;

	default:
		return NULL;
	}

	return NULL;
}

bool FontResItem::IsMyRenderFont(IRenderFont* pRenderFont)
{ 
    if (m_pGdiFont == pRenderFont) 
    {
        return true;
    }  
    return false;
}

FontRes::FontRes(SkinRes* pSkinRes)
{
    m_pSkinRes = pSkinRes;
    m_pIFontRes = NULL;
}
FontRes::~FontRes()
{
	this->Clear();
    SAFE_DELETE(m_pIFontRes);
}
IFontRes&  FontRes::GetIFontRes()
{
    if (!m_pIFontRes)
        m_pIFontRes = new IFontRes(this);

    return *m_pIFontRes;
}
long  FontRes::GetFontCount() 
{ 
	return (long) m_vFonts.size(); 
}
bool  FontRes::GetFontResItem(long lIndex, IFontResItem** ppResItem)
{
	if (NULL == ppResItem)
		return false;
	
	FontResItem* pItem = this->GetFontItem(lIndex);
	if (NULL == pItem)
		return false;

    *ppResItem = pItem->GetIFontResItem();
	return true;
}

FontResItem*  FontRes::GetFontItem( int nIndex )
{
	if (nIndex < 0)
		return NULL;
	if (nIndex >= (int)m_vFonts.size() )
		return NULL;

	return m_vFonts[nIndex];
}

FontResItem*  FontRes::GetFontItem(const String& strId)
{
	vector<FontResItem*>::iterator  iter = m_vFonts.begin();
	vector<FontResItem*>::iterator  iterEnd = m_vFonts.end();

	for( ; iter != iterEnd; iter++ )
	{
		FontResItem* p = *iter;
		LPCTSTR szId = p->GetId();
		if (strId == szId)
			return p;
	}
	return NULL;
}

bool FontRes::GetFont(LPCTSTR szFontId, GRAPHICS_RENDER_LIBRARY_TYPE eRenderType, __out IRenderFont** ppOut)
{
	if (!szFontId || !ppOut)
		return false;

	vector<FontResItem*>::iterator iter = m_vFonts.begin();
	for (;iter != m_vFonts.end(); ++iter)
	{
		if (0 == wcscmp((*iter)->GetId(), szFontId))
		{
			// if (p->GetWParam() == 0 && p->GetLParam() == 0 )
			{
				*ppOut = (*iter)->GetFont(m_pSkinRes, eRenderType);
				return true;
			}
		}
	}

	// 获取失败，尝试向上一级资源获取
	if (m_pSkinRes->GetParentSkinRes())
	{
		return m_pSkinRes->GetParentSkinRes()->
			GetFontRes().GetFont(szFontId, eRenderType, ppOut);
	}

	return false;
}

bool  FontRes::GetDefaultFont(GRAPHICS_RENDER_LIBRARY_TYPE eRenderType, __out IRenderFont** ppFont)
{
	if (NULL == ppFont)
		return false;

	if (!m_vFonts.empty())
	{
		if (m_vFonts[0])
		{
			*ppFont = m_vFonts[0]->GetFont(m_pSkinRes, eRenderType);
			return true;
		}
	}

	SkinRes* pParentRes = m_pSkinRes->GetParentSkinRes();
	if (pParentRes)
	{
		return pParentRes->GetFontRes().GetDefaultFont(eRenderType, ppFont);
	}

	return false;
}

LPCTSTR  FontRes::GetDefaultFontId()
{
	if (0 == (int)m_vFonts.size())
		return NULL;

	if (NULL == m_vFonts[0])
		return NULL;

	return m_vFonts[0]->GetId();
}

LPCTSTR  FontRes::GetRenderFontId(IRenderFont* pFont)
{
	vector<FontResItem*>::iterator iter = m_vFonts.begin();
	vector<FontResItem*>::iterator iterEnd = m_vFonts.end();

	for( ; iter != iterEnd; iter++ )
	{
		FontResItem* p = *iter;
		if (p->IsMyRenderFont(pFont) )
		{
			return p->GetId();
		}
	}
	return NULL;
}

bool FontRes::InsertFont(LPCTSTR szId, LOGFONT* pLogFont)
{
	FontResItem* pItem = this->InsertFont(szId, pLogFont, 0,0 );
	if (!pItem)
		return false;
	
	return true;
}

FontResItem* FontRes::InsertFont( 
	LPCTSTR szId,
	LOGFONT* pLogFont,
	WPARAM wParam,
	LPARAM lParam)
{
	if (NULL == pLogFont)
	{
		UI_LOG_ERROR(_T("FontRes::InsertFont failed."));
		return NULL;
	}
// 	vector<FontResItem*>::iterator  iter = m_vFonts.begin();
// 	vector<FontResItem*>::iterator  iterEnd = m_vFonts.end();
// 
// 	for( ; iter != iterEnd; iter++ )
// 	{
// 		FontResItem* p = *iter;
// 
// 		if (strID == p->GetId() /*&& p->GetWParam() == wParam && p->GetLParam() == lParam */)
// 		{
// 			UI_LOG_WARN(_T("FontRes::InsertFont failed, insert item=%s"), strID.c_str() );
// 			return NULL;
// 		}
// 	}

	FontResItem*  pFontItem = new FontResItem;
	pFontItem->SetId(szId);
	pFontItem->SetLogFont( pLogFont );

	this->m_vFonts.push_back(pFontItem);  
	return pFontItem;
}

bool FontRes::ModifyFont( const String& strID, LOGFONT* pLogFont )
{
	if (NULL == pLogFont )
	{
		UI_LOG_ERROR(_T("FontRes::ModifyFont failed."));
		return NULL;
	}
	vector<FontResItem*>::iterator  iter = m_vFonts.begin();
	vector<FontResItem*>::iterator  iterEnd = m_vFonts.end();

	for (; iter != iterEnd; iter++)
	{
		FontResItem* p = *iter;

		if (strID == p->GetId())
		{
			p->ModifyFont(pLogFont);
			return true;
		}
	}
	UI_LOG_WARN(_T("FontRes::InsertFont failed, insert item=%s"), strID.c_str() );
	return false;
}

bool FontRes::RemoveFont( const String& strID )
{
	vector<FontResItem*>::iterator  iter = m_vFonts.begin();
	vector<FontResItem*>::iterator  iterEnd = m_vFonts.end();

	for( ; iter != iterEnd; iter++ )
	{
		FontResItem* p = *iter;

		if (strID == p->GetId())
		{
			delete p;
			p = NULL;
			m_vFonts.erase(iter);
			return true;
		}
	}
	return false;
}

void FontRes::Clear()
{
	vector<FontResItem*>::iterator  iter = m_vFonts.begin();
	vector<FontResItem*>::iterator  iterEnd = m_vFonts.end();

	for (; iter != iterEnd; iter++)
	{
		FontResItem* p = *iter;
		delete p;
		p = NULL;
	}

	m_vFonts.clear();
}

