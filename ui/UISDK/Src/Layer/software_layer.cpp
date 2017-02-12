#include "stdafx.h"
#include "software_layer.h"
#include "compositor.h"
#include "Inc/Interface/renderlibrary.h"


SoftwareLayer::SoftwareLayer()
{
}

SoftwareLayer::~SoftwareLayer()
{
}

void  SoftwareLayer::UpdateDirty()
{
    if (!m_pLayerContent)
        return;

    if (!m_dirtyRectangles.GetCount())
        return;

    IRenderTarget* pRenderTarget = GetRenderTarget();
    uint nCount = m_dirtyRectangles.GetCount();
    for (uint i = 0; i < nCount; i++)
        pRenderTarget->Clear(m_dirtyRectangles.GetRectPtrAt(i));

    pRenderTarget->BeginDraw();

    pRenderTarget->SetMetaClipRegion(
        m_dirtyRectangles.GetArrayPtr(), 
        m_dirtyRectangles.GetCount());

    m_pLayerContent->Draw(pRenderTarget);
    pRenderTarget->EndDraw();

    m_dirtyRectangles.Destroy();

#ifdef _DEBUGx
	static int i = 0;
	TCHAR szPath[100];
	_stprintf(szPath, L"D:\\test\\%d.png", i++);
	pRenderTarget->Save(szPath);
#endif
}
