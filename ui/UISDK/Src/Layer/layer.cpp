#include "stdafx.h"
#include "layer.h"
#include "compositor.h"
#include "Src\Base\Application\uiapplication.h"
#include "..\Base\Object\object_layer.h"
#include "..\Base\Object\object.h"
#include <memory>
#include "..\common\math\math.h"

using namespace UI;
static LayerAnimateParam  s_defaultLayerAnimateParam;

enum LAYER_ANIMATE_TYPE
{
    STORYBOARD_ID_OPACITY = 1,
	STORYBOARD_ID_XROTATE,
	STORYBOARD_ID_YROTATE,
	STORYBOARD_ID_ZROTATE,
	STORYBOARD_ID_TRANSLATE,
	STORYBOARD_ID_SCALE,
};

Layer::Layer(): m_iLayer(this)
{
    m_pCompositor = NULL;
	m_pRenderTarget = nullptr;

	m_pParent = NULL;
	m_pFirstChild = NULL;
	m_pNext = NULL;
	m_pPrev = NULL;

    m_size.cx = m_size.cy = 0;
    m_pLayerContent = NULL;
    m_bClipLayerInParentObj = true;

	m_nOpacity = m_nOpacity_Render = 255;
	m_fxRotate = 0;
	m_fyRotate = 0;
	m_fzRotate = 0;
	m_fxScale = 1;
	m_fyScale = 1;

    m_xTranslate = 0;
    m_yTranslate = 0;
    m_zTranslate = 0;

// 	m_transfrom3d.perspective(1000);
// 	m_transfrom3d.set_transform_rotate_origin(
// 		TRANSFORM_ROTATE_ORIGIN_LEFT, 0,
// 		TRANSFORM_ROTATE_ORIGIN_CENTER, 0, 0);

#ifdef _DEBUG
	UI_LOG_DEBUG(L"Create Layer");
#endif
}

Layer::~Layer()
{
	if (m_pLayerContent)
	{
		m_pLayerContent->OnLayerDestory();
	}

	UIA::IAnimateManager* pAni = m_pCompositor->
		GetUIApplication()->GetAnimateMgr();
	if (pAni)
	{
		pAni->ClearStoryboardByNotify(
			static_cast<UIA::IAnimateEventCallback*>(this));
	}

	if (m_pParent && m_pParent->m_pFirstChild == this)
	{
		m_pParent->m_pFirstChild = m_pNext;
	}

    // 由每个 object 自己去销毁自己的layer
//     while (m_pFirstChild)
//     {
//         Layer* p = m_pFirstChild;
//         m_pFirstChild = m_pFirstChild->m_pNext;
//         delete p;
//     }

	if (m_pCompositor)
	{
		if (m_pCompositor->GetRootLayer() == this)
		{
			m_pCompositor->SetRootLayer(0);
		}
	}

	SAFE_RELEASE(m_pRenderTarget);

#ifdef _DEBUG
	UI_LOG_DEBUG(L"Layer Destroy");
#endif
}

ILayer*  Layer::GetILayer()
{
    return &m_iLayer;
}

void  Layer::SetCompositorPtr(Compositor* p)
{
    m_pCompositor = p;
}

void  Layer::Invalidate(LPCRECT prcDirtyInLayer, uint nCount)
{
	for (uint i = 0; i < nCount; i++)
		Invalidate(prcDirtyInLayer+i);
}

void  Layer::Invalidate(LPCRECT prcDirtyInLayer)
{
    RECT rcDirty = {0};

    if (!prcDirtyInLayer)
    {
        m_dirtyRectangles.Destroy();
        
        SetRect(&rcDirty, 0, 0, m_size.cx, m_size.cy);
        m_dirtyRectangles.AddRect(&rcDirty);
    }
    else
    {
	    if (IsRectEmpty(prcDirtyInLayer))
		    return;

        CopyRect(&rcDirty, prcDirtyInLayer);
	    m_dirtyRectangles.UnionDirtyRect(prcDirtyInLayer);
    }

    // 如果是软件渲染，向上冒泡
    if (m_pParent && GetType() == Layer_Software)
    {
        // rcDirty 转换成 父layer位置，并由父layer去请求合成
        RECT rcParent = {0};
        m_pParent->m_pLayerContent->GetParentWindowRect(&rcParent);

        RECT rcSelf = {0};
        m_pLayerContent->GetWindowRect(&rcSelf);

        OffsetRect(&rcDirty, 
            rcSelf.left-rcParent.left, 
            rcSelf.top-rcParent.top);

        m_pParent->Invalidate(&rcDirty);
    }
    else
    {
    	if (m_pCompositor)
	    	m_pCompositor->RequestInvalidate();
    }
}

bool Layer::AddSubLayer(Layer* pLayer, Layer* pInsertBeforeChild)
{
	UIASSERT(pLayer);
    UIASSERT (!pLayer->m_pParent);

    if (pLayer->m_pParent)
        return false;

    if (!m_pFirstChild)
    {
        m_pFirstChild = pLayer;
    }
    else
    {
        // 放在最后面
        if (!pInsertBeforeChild)
        {
            Layer* p = m_pFirstChild;
            while (p && p->m_pNext)
            {
                p = p->m_pNext;
            }


            p->m_pNext = pLayer;
            pLayer->m_pPrev = p;
        }
        else
        {
            if (pInsertBeforeChild->m_pPrev)
                pInsertBeforeChild->m_pPrev->m_pNext = pLayer;
            pLayer->m_pPrev = pInsertBeforeChild->m_pPrev;

            pInsertBeforeChild->m_pPrev = pLayer;
            pLayer->m_pNext = pInsertBeforeChild;
        }
    }

    pLayer->m_pParent = this;

    on_layer_tree_changed();
    return true;
}

void  Layer::RemoveMeInTheTree()
{
    if (m_pPrev)
    {
        m_pPrev->m_pNext = m_pNext;
        if (m_pNext)
            m_pNext->m_pPrev = m_pPrev;
    }
    else
    {
        if (m_pParent)
            m_pParent->m_pFirstChild = m_pNext;
    }

    m_pParent = NULL;
    m_pPrev = NULL;
    m_pNext = NULL;
}

void  Layer::MoveLayer2NewParentEnd(Layer* pOldParent, Layer* pNewParent)
{
	UIASSERT(pOldParent && pNewParent);

    RemoveMeInTheTree();
    pNewParent->AddSubLayer(this, NULL);
}

void  Layer::SetContent(ILayerContent* p)
{
    m_pLayerContent = p;
}
ILayerContent*  Layer::GetContent()
{
    return m_pLayerContent;
}

void  Layer::on_layer_tree_changed()
{
	UIASSERT(m_pCompositor);

    Layer* pRootLayer = this;

    while (pRootLayer && pRootLayer->m_pParent)
    {
        pRootLayer = pRootLayer->m_pParent;
    }
    while (pRootLayer && pRootLayer->m_pPrev)
    {
        pRootLayer = pRootLayer->m_pPrev;
    }
    m_pCompositor->SetRootLayer(pRootLayer);
}

Layer* Layer::GetNext()
{
    return m_pNext;
}

Layer*  Layer::GetFirstChild()
{
    return m_pFirstChild;
}

void  Layer::OnSize( uint nWidth, uint nHeight )
{
    m_size.cx = nWidth;
    m_size.cy = nHeight;
    Invalidate(NULL);

    if (!m_pRenderTarget)
    {
        GetRenderTarget();
    }
    m_pRenderTarget->ResizeRenderBuffer(nWidth, nHeight);

    virtualOnSize(nWidth, nHeight);
}

void  Layer::PostCompositorRequest()
{
   m_pCompositor->RequestInvalidate();
} 

void Layer::SetOpacity(byte b, LayerAnimateParam* pParam)
{
	if (m_nOpacity == b)
		return;

	m_nOpacity = b;

	if (pParam == DefaultLayerAnimateParam)
	{
		pParam = &s_defaultLayerAnimateParam;
	}
	// 开启隐式动画
	if (pParam)
	{
		UIA::IAnimateManager* pAni = m_pCompositor->
			GetUIApplication()->GetAnimateMgr();

		pAni->RemoveStoryboardByNotityAndId(
			static_cast<UIA::IAnimateEventCallback*>(this), 
			STORYBOARD_ID_OPACITY);

		UIA::IStoryboard* pStoryboard = pAni->CreateStoryboard(
			static_cast<UIA::IAnimateEventCallback*>(this), 
			STORYBOARD_ID_OPACITY);

		pStoryboard->CreateTimeline(0)->SetParam(
			m_nOpacity_Render, 
			m_nOpacity, 
			pParam->GetDuration())
			->SetEaseType(pParam->GetEaseType());

		LayerAnimateParam* pSaveParam = new LayerAnimateParam;
		*pSaveParam = *pParam;
		pStoryboard->SetWParam((WPARAM)pSaveParam);

		if (pParam->IsBlock())
		{
			pStoryboard->BeginBlock();
		}
		else
		{
			pStoryboard->Begin();
		}
	}
	else
	{
		m_nOpacity_Render = m_nOpacity;
		if (GetType() == Layer_Software)
		{
			Object* obj = GetLayerContentObject();
			if (obj)
				obj->Invalidate();
		}
		else
		{
			m_pCompositor->RequestInvalidate();
		}
	}
}

byte Layer::GetOpacity()
{
	return m_nOpacity_Render;
}

void  Layer::RotateYBy(float f, LayerAnimateParam* param)
{
	RotateYTo(m_fyRotate + f, param);
}

void  Layer::RotateYTo(float f, LayerAnimateParam* pParam)
{
	if (m_fyRotate == f)
		return;

	m_fyRotate = f;

	UIA::IAnimateManager* pAni = m_pCompositor->
		GetUIApplication()->GetAnimateMgr();

	pAni->RemoveStoryboardByNotityAndId(
		static_cast<UIA::IAnimateEventCallback*>(this),
		STORYBOARD_ID_YROTATE);

	if (pParam == DefaultLayerAnimateParam)
	{
		pParam = &s_defaultLayerAnimateParam;
	}
	if (pParam)
	{
		UIA::IStoryboard* pStoryboard = pAni->CreateStoryboard(
			static_cast<UIA::IAnimateEventCallback*>(this), 
			STORYBOARD_ID_YROTATE);

		pStoryboard->CreateTimeline(0)->SetParam(
			m_transfrom3d.get_rotateY(), 
			m_fyRotate, 
			pParam->GetDuration())
			->SetEaseType(pParam->GetEaseType());

		LayerAnimateParam* pSaveParam = new LayerAnimateParam;
		*pSaveParam = *pParam;
		pStoryboard->SetWParam((WPARAM)pSaveParam);

		m_transfrom3d.perspective(2000);

		if (pParam->IsBlock())
		{
			pStoryboard->BeginBlock();
		}
		else
		{
			pStoryboard->Begin();
		}
	}
	else
	{
		m_transfrom3d.rotateY(f);
	}

	if (m_pLayerContent)
	{
		if (GetType() == Layer_Software)
		{
			Object* obj = GetLayerContentObject();
			if (obj)
				obj->Invalidate();
		}
		else
		{
			m_pCompositor->RequestInvalidate();
		}
	}
}


void  Layer::RotateXBy(float f, LayerAnimateParam* param)
{
	RotateXTo(m_fxRotate + f, param);
}

void  Layer::RotateXTo(float f, LayerAnimateParam* pParam)
{
	if (m_fxRotate == f)
		return;

	m_fxRotate = f;

	UIA::IAnimateManager* pAni = m_pCompositor->
		GetUIApplication()->GetAnimateMgr();

	pAni->RemoveStoryboardByNotityAndId(
		static_cast<UIA::IAnimateEventCallback*>(this),
		STORYBOARD_ID_XROTATE);

	if (pParam == DefaultLayerAnimateParam)
	{
		pParam = &s_defaultLayerAnimateParam;
	}
	if (pParam)
	{
		UIA::IStoryboard* pStoryboard = pAni->CreateStoryboard(
			static_cast<UIA::IAnimateEventCallback*>(this),
			STORYBOARD_ID_XROTATE);

		pStoryboard->CreateTimeline(0)->SetParam(
			m_transfrom3d.get_rotateX(),
			m_fxRotate,
			pParam->GetDuration())
			->SetEaseType(pParam->GetEaseType());

		LayerAnimateParam* pSaveParam = new LayerAnimateParam;
		*pSaveParam = *pParam;
		pStoryboard->SetWParam((WPARAM)pSaveParam);

		m_transfrom3d.perspective(2000);

		if (pParam->IsBlock())
		{
			pStoryboard->BeginBlock();
		}
		else
		{
			pStoryboard->Begin();
		}
	}
	else
	{
		m_transfrom3d.rotateX(f);
	}

	if (m_pLayerContent)
	{
		if (GetType() == Layer_Software)
		{
			Object* obj = GetLayerContentObject();
			if (obj)
				obj->Invalidate();
		}
		else
		{
			m_pCompositor->RequestInvalidate();
		}
	}
}


void  Layer::RotateZBy(float f, LayerAnimateParam* param)
{
	RotateZTo(m_fzRotate + f, param);
}

void  Layer::RotateZTo(float f, LayerAnimateParam* pParam)
{
	if (m_fzRotate == f)
		return;

	m_fzRotate = f;

	UIA::IAnimateManager* pAni = m_pCompositor->
		GetUIApplication()->GetAnimateMgr();

	pAni->RemoveStoryboardByNotityAndId(
		static_cast<UIA::IAnimateEventCallback*>(this),
		STORYBOARD_ID_ZROTATE);

	if (pParam == DefaultLayerAnimateParam)
	{
		pParam = &s_defaultLayerAnimateParam;
	}
	if (pParam)
	{
		UIA::IStoryboard* pStoryboard = pAni->CreateStoryboard(
			static_cast<UIA::IAnimateEventCallback*>(this),
			STORYBOARD_ID_ZROTATE);

		pStoryboard->CreateTimeline(0)->SetParam(
			m_transfrom3d.get_rotateZ(),
			m_fzRotate,
			pParam->GetDuration())
			->SetEaseType(pParam->GetEaseType());

		LayerAnimateParam* pSaveParam = new LayerAnimateParam;
		*pSaveParam = *pParam;
		pStoryboard->SetWParam((WPARAM)pSaveParam);

		m_transfrom3d.perspective(2000);

		if (pParam->IsBlock())
		{
			pStoryboard->BeginBlock();
		}
		else
		{
			pStoryboard->Begin();
		}
	}
	else
	{
		m_transfrom3d.rotateZ(f);
	}

	if (m_pLayerContent)
	{
		if (GetType() == Layer_Software)
		{
			Object* obj = GetLayerContentObject();
			if (obj)
				obj->Invalidate();
		}
		else
		{
			m_pCompositor->RequestInvalidate();
		}
	}
}

float  Layer::GetYRotate() 
{
	return m_fyRotate;
}

void  Layer::ScaleTo(float x, float y, LayerAnimateParam* pParam)
{
	if (fequ(x, m_fxScale) && fequ(y, m_fyScale))
		return;

	m_fxScale = x;
	m_fyScale = y;

	UIA::IAnimateManager* pAni = m_pCompositor->
		GetUIApplication()->GetAnimateMgr();

	pAni->RemoveStoryboardByNotityAndId(
		static_cast<UIA::IAnimateEventCallback*>(this),
		STORYBOARD_ID_SCALE);

	if (pParam == DefaultLayerAnimateParam)
	{
		pParam = &s_defaultLayerAnimateParam;
	}
	if (pParam)
	{
		UIA::IStoryboard* pStoryboard = pAni->CreateStoryboard(
			static_cast<UIA::IAnimateEventCallback*>(this),
			STORYBOARD_ID_SCALE);

		pStoryboard->CreateTimeline(0)->SetParam(
			m_transfrom3d.get_scaleX(),
			m_fxScale,
			pParam->GetDuration())
			->SetEaseType(pParam->GetEaseType());
		pStoryboard->CreateTimeline(1)->SetParam(
			m_transfrom3d.get_scaleY(),
			m_fyScale,
			pParam->GetDuration())
			->SetEaseType(pParam->GetEaseType());

		LayerAnimateParam* pSaveParam = new LayerAnimateParam;
		*pSaveParam = *pParam;
		pStoryboard->SetWParam((WPARAM)pSaveParam);

		if (pParam->IsBlock())
		{
			pStoryboard->BeginBlock();
		}
		else
		{
			pStoryboard->Begin();
		}
	}
	else
	{
		m_transfrom3d.scale3d(m_fxScale, m_fyScale, 0);
	}

	if (m_pLayerContent)
	{
		if (GetType() == Layer_Software)
		{
			Object* obj = GetLayerContentObject();
			if (obj)
				obj->Invalidate();
		}
		else
		{
			m_pCompositor->RequestInvalidate();
		}
	}
}


void  Layer::TranslateXTo(float x, LayerAnimateParam* param)
{
	TranslateTo(x, m_yTranslate, m_zTranslate, param);
}
void  Layer::TranslateYTo(float y, LayerAnimateParam* param)
{
	TranslateTo(m_xTranslate, y, m_zTranslate, param);
}

void  Layer::TranslateBy(float x, float y, float z, LayerAnimateParam* param)
{
	TranslateTo(m_xTranslate+x, m_yTranslate+y, m_zTranslate+z, param);
}
void  Layer::TranslateTo(float x, float y, float z, LayerAnimateParam* pParam)
{
    if (m_xTranslate == x && 
        m_yTranslate == y &&
        m_zTranslate == z)
        return;

    m_xTranslate = x;
    m_yTranslate = y;
    m_zTranslate = z;

	UIA::IAnimateManager* pAni = m_pCompositor->
		GetUIApplication()->GetAnimateMgr();

	pAni->RemoveStoryboardByNotityAndId(
		static_cast<UIA::IAnimateEventCallback*>(this),
		STORYBOARD_ID_TRANSLATE);
        
	if (pParam == DefaultLayerAnimateParam)
	{
		pParam = &s_defaultLayerAnimateParam;
	}
    if (pParam)
    {
        UIA::IStoryboard* pStoryboard = pAni->CreateStoryboard(
            static_cast<UIA::IAnimateEventCallback*>(this),
            STORYBOARD_ID_TRANSLATE);

        pStoryboard->CreateTimeline(0)->SetParam(
            m_transfrom3d.get_translateX(),
            m_xTranslate,
			pParam->GetDuration())
			->SetEaseType(pParam->GetEaseType());

        pStoryboard->CreateTimeline(1)->SetParam(
            m_transfrom3d.get_translateY(),
            m_yTranslate,
			pParam->GetDuration())
			->SetEaseType(pParam->GetEaseType());

        pStoryboard->CreateTimeline(2)->SetParam(
            m_transfrom3d.get_translateZ(),
            m_zTranslate,
			pParam->GetDuration())
			->SetEaseType(pParam->GetEaseType());


		LayerAnimateParam* pSaveParam = new LayerAnimateParam;
		*pSaveParam = *pParam;
		pStoryboard->SetWParam((WPARAM)pSaveParam);

		if (pParam->IsBlock())
		{
			pStoryboard->BeginBlock();
		}
		else
		{
			pStoryboard->Begin();
		}
    }
    else
    {
        m_transfrom3d.translate3d(x, y, z);
    }

	if (m_pLayerContent)
	{
		if (GetType() == Layer_Software)
		{
			Object* obj = GetLayerContentObject();
			if (obj)
				obj->Invalidate();
		}
		else
		{
			m_pCompositor->RequestInvalidate();
		}
	}
}

float  Layer::GetXTranslate()
{
    return m_xTranslate;
}
float  Layer::GetYTranslate()
{
    return m_yTranslate;
}
float  Layer::GetZTranslate()
{
    return m_zTranslate;
}

UIA::E_ANIMATE_TICK_RESULT Layer::OnAnimateTick(UIA::IStoryboard* pStoryboard)
{
	LayerAnimateParam* pParam = (LayerAnimateParam*)pStoryboard->GetWParam();
	bool isblock = false;
	if (pParam)
		isblock = pParam->IsBlock();

	switch (pStoryboard->GetId())
	{
	case STORYBOARD_ID_OPACITY:
	{
		m_nOpacity_Render = (byte)pStoryboard->
			GetTimeline(0)->GetCurrentIntValue();

		static_cast<ObjectLayer*>(m_pLayerContent)->GetObjet().Invalidate();
		// 有可能是block动画，需要立即刷新
		if (isblock)
			m_pCompositor->DoInvalidate();
		else
			m_pCompositor->RequestInvalidate();
	}
	break;

	case STORYBOARD_ID_XROTATE:
	{
		m_transfrom3d.rotateX(pStoryboard->
			GetTimeline(0)->GetCurrentValue());

		if (isblock)
			m_pCompositor->DoInvalidate();
		else
			m_pCompositor->RequestInvalidate();
	}
	break;
	case STORYBOARD_ID_YROTATE:
	{
		m_transfrom3d.rotateY(pStoryboard->
			GetTimeline(0)->GetCurrentValue());

		if (isblock)
			m_pCompositor->DoInvalidate();
		else
			m_pCompositor->RequestInvalidate();
	}
	break;
	case STORYBOARD_ID_ZROTATE:
	{
		m_transfrom3d.rotateZ(pStoryboard->
			GetTimeline(0)->GetCurrentValue());

		if (isblock)
			m_pCompositor->DoInvalidate();
		else
			m_pCompositor->RequestInvalidate();
	}
	break;

	case STORYBOARD_ID_TRANSLATE:
	{
		m_transfrom3d.translate3d(
			pStoryboard->GetTimeline(0)->GetCurrentValue(),
			pStoryboard->GetTimeline(1)->GetCurrentValue(),
			pStoryboard->GetTimeline(2)->GetCurrentValue());

		if (isblock)
			m_pCompositor->DoInvalidate();
		else
			m_pCompositor->RequestInvalidate();
	}
	break;

	case STORYBOARD_ID_SCALE:
	{
		m_transfrom3d.scale3d(
			pStoryboard->GetTimeline(0)->GetCurrentValue(),
			pStoryboard->GetTimeline(1)->GetCurrentValue(),0);

		if (isblock)
			m_pCompositor->DoInvalidate();
		else
			m_pCompositor->RequestInvalidate();
	}
	break;
	}

	return UIA::ANIMATE_TICK_RESULT_CONTINUE;
}

void  Layer::OnAnimateEnd(UIA::IStoryboard* pStoryboard, UIA::E_ANIMATE_END_REASON e)
{
	std::shared_ptr<LayerAnimateParam> pParam(
		(LayerAnimateParam*)pStoryboard->GetWParam());

	if (pStoryboard->GetId() == STORYBOARD_ID_TRANSLATE)
	{
		// 偏移类动画结束后，将偏移量转嫁到控件位置坐标上面..z轴仍然保留，如果有的话
		Object* obj = GetLayerContentObject();
		if (obj)
		{
			RECT rcParent = { 0 };
			obj->GetParentRect(&rcParent);

			obj->SetObjectPos( 
				rcParent.left + (int)m_xTranslate, 
				rcParent.top + (int)m_yTranslate,
				0, 0, SWP_NOSIZE|SWP_NOREDRAW);
		}

		m_xTranslate = 0;
		m_yTranslate = 0;
		m_transfrom3d.translate3d(0, 0, m_zTranslate);
	}
	if (pParam->GetFinishCallback())
	{
		LayerAnimateFinishParam info = { 0 };
		info.endreason = e;
		pParam->GetFinishCallback()(info);
	}

	// !=normal时，可能是当前动画正在被新的动画取代，这个时候不去尝试销毁，由新的动画结束后触发
	if (e == UIA::ANIMATE_END_NORMAL)
	{
		TryDestroy();
	}
}

void  Layer::CopyDirtyRect(RectArray& arr)
{   
    arr = m_dirtyRectangles;
}

IRenderTarget*  Layer::GetRenderTarget()
{
    if (!m_pRenderTarget)
    {
        if (!m_pCompositor)
            return NULL;

        m_pCompositor->CreateRenderTarget(&m_pRenderTarget);
        if (!m_pRenderTarget)
            return NULL;

        m_pRenderTarget->CreateRenderBuffer(NULL);
    }

    return m_pRenderTarget;
}

// 强制销毁 
void  Layer::Destroy()
{
	delete this;
}

void  Layer::TryDestroy()
{
	if (CanDestroy())
		Destroy();
}
// 判断一个控件的layer当前是否可以被销毁，如果有动画，则不销毁。在动画结束后判断一次
bool  Layer::CanDestroy()
{
	if (m_nOpacity_Render != 255)
		return false;
	if (m_nOpacity != 255)
		return false;

	if (m_xTranslate != 0)
		return false;
	if (m_yTranslate != 0)
		return false;
	if (m_zTranslate != 0)
		return false;

	if (!m_transfrom3d.is_identity())
		return false;

	Object* obj = GetLayerContentObject();
	if (obj)
	{
		OBJSTYLE s = { 0 };
		s.layer = 1;
		if (obj->TestObjectStyle(s))
			return false;
	}

	if (!IsEmptyDegree(m_fxRotate))
		return false;

	if (!IsEmptyDegree(m_fyRotate))
		return false;

	if (!IsEmptyDegree(m_fzRotate))
		return false;

	return true;
}

Object*  Layer::GetLayerContentObject()
{
	if (!m_pLayerContent)
		return nullptr;

	if (m_pLayerContent->Type() != LayerContentTypeObject)
		return nullptr;

	return &static_cast<IObjectLayerContent*>(m_pLayerContent)->GetObject();
}

