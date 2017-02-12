#include "stdafx.h"
#include "gdipen.h"

GdiPen::GdiPen()
{
	m_hPen = NULL;
}
GdiPen::~GdiPen()
{
	DeleteObject();
}

void  GdiPen::CreateInstance(IRenderPen** ppOutRef)
{
	UIASSERT(NULL != ppOutRef);
	if (NULL == ppOutRef)
		return;

	GdiPen* p = new GdiPen();
    p->AddRef();
	*ppOutRef = static_cast<IRenderPen*>(p);
}

void GdiPen::DeleteObject()
{
	SAFE_DELETE_GDIOBJECT(m_hPen);
}

bool GdiPen::CreateSolidPen(int nWidth, Color* pColor)
{
	if (nWidth < 0 || NULL == pColor)
		return false;
	
	DeleteObject();

	m_hPen = ::CreatePen(PS_SOLID, nWidth, RGB(pColor->r, pColor->g, pColor->b));
	return m_hPen != NULL;
}
bool GdiPen::CreateDotPen(int nWidth, Color* pColor)
{
	if (nWidth < 0 || NULL == pColor)
		return false;

	DeleteObject();

	LOGBRUSH logBrush = {BS_SOLID, RGB(pColor->r, pColor->g, pColor->b), 0};
	m_hPen = ExtCreatePen(PS_GEOMETRIC|PS_DOT, 1, &logBrush, 0, NULL);	
	return m_hPen != NULL;
}