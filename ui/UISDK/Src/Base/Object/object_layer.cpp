#include "stdafx.h"
#include "object_layer.h"
#include "object.h"
#include "Src/Layer/windowrender.h"
#include "Src/UIObject/Window/windowbase.h"

using namespace UI;

ObjectLayer::ObjectLayer(Object& o) : m_obj(o)
{
	m_pLayer = nullptr;
}

ObjectLayer::~ObjectLayer()
{
	DestroyLayer();
}

void UI::ObjectLayer::CreateLayer()
{
	if (m_pLayer)
		return;

	WindowRender* pWndRender = NULL;

	WindowBase* pWindow = m_obj.GetWindowObject();
	if (pWindow)
		pWndRender = pWindow->GetWindowRender();

	if (pWndRender)
	{
		m_pLayer = pWndRender->CreateLayer(&m_obj);
		m_pLayer->SetContent(this);

		CRect rcParent;
		m_obj.GetParentRect(&rcParent);
		if (!IsRectEmpty(&rcParent))
			OnSize(rcParent.Width(), rcParent.Height());

		m_obj.OnLayerCreate();
	}
	else
	{
		// 在resize的时候创建
		UIASSERT(0);
	}
}

void  ObjectLayer::TryDestroyLayer()
{
	if (!m_pLayer)
		return;

	m_pLayer->TryDestroy();
}

void  ObjectLayer::DestroyLayer()
{
	if (!m_pLayer)
		return;

	Layer*  p = m_pLayer;
	m_pLayer = nullptr;

	if (p)
		p->Destroy();
}

void  ObjectLayer::Draw(UI::IRenderTarget* pRenderTarget) 
{
    m_obj.DrawToLayer__(pRenderTarget);
}

void  ObjectLayer::GetWindowRect(RECT* prcOut) 
{
    m_obj.GetWindowRect(prcOut);
}

void  ObjectLayer::GetParentWindowRect(RECT* prcOut)
{
    if (m_obj.GetParentObject())
        m_obj.GetParentObject()->GetWindowRect(prcOut);  // TODO: -->> visible part only
}

void  ObjectLayer::OnSize(uint nWidth, uint nHeight)
{
    if (m_pLayer)
    {
        m_pLayer->OnSize(nWidth, nHeight);
    }
}

bool  ObjectLayer::IsChildOf(Object* pParent)
{
    if (!pParent)
        return false;

    if (pParent->IsMyChild(&m_obj, true))
        return true;

    return false;
}

bool  ObjectLayer::IsSelfVisible()
{
    return m_obj.IsSelfVisible();
}

// object在对象树中的位置改变了，同步到分层树上面
void  ObjectLayer::OnObjPosInTreeChanged()
{
    UIASSERT (m_pLayer);

    Layer* pParentLayer = m_pLayer->GetParent();
	UIASSERT(pParentLayer);

    m_pLayer->RemoveMeInTheTree();
    pParentLayer->AddSubLayer(m_pLayer, m_obj.FindNextLayer(pParentLayer));
}

void UI::ObjectLayer::OnLayerDestory()
{
	if (m_pLayer)  // 由ObjectLayer::~ObjectLayer()触发的，不通知
	{
		m_pLayer = nullptr;
		m_obj.OnLayerDestory();
	}
}

void UI::ObjectLayer::Invalidate()
{
	m_obj.Invalidate();
}

// 用于layer在动画结束时，判断自己是否需要被销毁。如果是控件层强制启用的layer，则不释放
bool UI::ObjectLayer::TestLayerStyle()
{
	OBJSTYLE s = { 0 };
	s.layer = 1;
	return m_obj.TestObjectStyle(s);
}
