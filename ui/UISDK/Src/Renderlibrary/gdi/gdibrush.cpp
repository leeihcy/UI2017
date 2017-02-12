#include "stdafx.h"
#include "gdibrush.h"

GdiBrush::GdiBrush()
{
	m_hBrush = NULL;
}
GdiBrush::~GdiBrush()
{
	DeleteObject();
}

void  GdiBrush::CreateInstance(IRenderBrush** ppOut)
{
	if (NULL == ppOut)
		return;

	GdiBrush* p = new GdiBrush();
    p->AddRef();
	*ppOut = static_cast<IRenderBrush*>(p);
}

void GdiBrush::DeleteObject()
{
	SAFE_DELETE_GDIOBJECT(m_hBrush);
}

bool GdiBrush::CreateSolidBrush(Color* pColor)
{
	if (NULL == pColor)
		return false;
	
	DeleteObject();

	m_hBrush = ::CreateSolidBrush(RGB(pColor->r, pColor->g, pColor->b));
	return m_hBrush!=NULL;
}