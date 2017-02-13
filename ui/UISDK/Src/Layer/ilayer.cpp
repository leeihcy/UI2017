#include "stdafx.h"
#include "Inc\Interface\irenderlayer.h"
#include "windowrender.h"
#include "layer.h"

IWindowRender::IWindowRender(WindowRender* p)
{
	m_pWindowRenderImpl = p;
}
IWindowRender::~IWindowRender()
{

}

WindowRender*  IWindowRender::GetImpl()
{
	return m_pWindowRenderImpl;
}
void  IWindowRender::SetCanCommit(bool b)
{
	m_pWindowRenderImpl->SetCanCommit(b);
}
// void  IWindowRender::Commit(HDC hDC, RECT* prc, int nCount)
// {
//     m_pWindowRenderImpl->Commit(hDC, prc, nCount);
// }
void  IWindowRender::InvalidateNow()
{
    m_pWindowRenderImpl->InvalidateNow();
}
GRAPHICS_RENDER_LIBRARY_TYPE  IWindowRender::GetGraphicsRenderType()
{
	return m_pWindowRenderImpl->GetGraphicsRenderType();
}
void  IWindowRender::SetGraphicsRenderType(GRAPHICS_RENDER_LIBRARY_TYPE  eTpye)
{
	return m_pWindowRenderImpl->SetGraphicsRenderType(eTpye);
}
bool  IWindowRender::GetRequireAlphaChannel()
{
	return m_pWindowRenderImpl->GetRequireAlphaChannel();
}
void  IWindowRender::SetCommitListener(IWindowCommitListener* p)
{
    m_pWindowRenderImpl->SetCommitListener(p);
}

IWindowCommitListener*  IWindowRender::GetCommitListener()
{
    return m_pWindowRenderImpl->GetCommitListener();
}

//////////////////////////////////////////////////////////////////////////

ILayer::ILayer(Layer* p)
{
	m_pImpl = p;
}

IRenderTarget*  ILayer::GetRenderTarget()
{
    return m_pImpl->GetRenderTarget();
}

#if 0
bool  ILayer::IsAutoAnimate()
{
	return m_pImpl->IsAutoAnimate();
}
void  ILayer::EnableAutoAnimate(bool b)
{
	m_pImpl->EnableAutoAnimate(b);
}

void  ILayer::SetAnimateFinishCallback(pfnLayerAnimateFinish f, long* userdata)
{
//	m_pImpl->SetAnimateFinishCallback(f, userdata);
}

void ILayer::SetOpacity(byte b, LayerAnimateParam* p)
{
	m_pImpl->SetOpacity(b, p);
}
byte ILayer::GetOpacity()
{
	return m_pImpl->GetOpacity();
}
void  ILayer::SetYRotate(float f)
{
	m_pImpl->SetYRotate(f);
}
float  ILayer::GetYRotate()
{
	return m_pImpl->GetYRotate();
}

void  ILayer::SetTranslate(float x, float y, float z)
{
	m_pImpl->SetTranslate(x, y, z);
}

float  ILayer::GetXTranslate()
{
	return m_pImpl->GetXTranslate();
}
float  ILayer::GetYTranslate()
{
	return m_pImpl->GetYTranslate();
}
float  ILayer::GetZTranslate()
{
	return m_pImpl->GetZTranslate();
}
#endif