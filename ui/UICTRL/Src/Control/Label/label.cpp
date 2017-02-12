#include "stdafx.h"
#include "label.h"
#include "label_desc.h"
#include <atlstr.h>
#include "..\UISDK\Inc\Util\igifimage.h"
#include "..\UISDK\Inc\Interface\iuires.h"
#include "..\UISDK\Inc\Interface\iimagerender.h"
#include "..\UISDK\Inc\Interface\ilayout.h"

Label::Label(ILabel* p):MessageProxy(p)
{
    m_pILabel = p;
	p->SetDescription(LabelDescription::Get());
}

Label::~Label()
{
}

HRESULT  Label::FinalConstruct(ISkinRes* p)
{
    DO_PARENT_PROCESS(ILabel, IControl);

	OBJSTYLE s = {0};
	s.default_transparent = 1;  // 默认透明
	s.default_reject_all_mouse_msg = 1;  // 默认不接收鼠标消息

    OBJSTYLE sRemove = { 0 };
    sRemove.default_tabstop = 1;     // 没有焦点
    m_pILabel->ModifyObjectStyle(&s, &sRemove);

    return S_OK;
}

LPCTSTR  Label::GetText()
{
	return this->m_strText.c_str();
}
void  Label::SetText(LPCTSTR pszText)
{
	if (NULL == pszText)
		m_strText.clear();
	else
		m_strText = pszText;
}

void Label::SetTextAndInvalidate(LPCTSTR pszText)
{
	if (pszText)
	{
		if (m_strText == pszText)
			return;
	}
	else 
	{
		if (m_strText.empty())
			return;
	}

	SetText(pszText);

	ILayoutParam*  param = m_pILabel->GetSafeLayoutParam();
	if (param && param->IsSizedByContent())
	{
		UI::IObject* parent = m_pILabel->GetParentObject();
		if (parent)
		{
			UI::ILayout* pLayout = (UI::ILayout*)
				UISendMessage(parent, UI_MSG_GETLAYOUT);
			if (pLayout)
			{
				pLayout->SetDirty(true);
				parent->Invalidate();
			}
		}
	}
	m_pILabel->Invalidate();
}

void Label::OnPaint(IRenderTarget* pRenderTarget)
{
	CRect  rcText;
	m_pILabel->GetObjectClientRect(&rcText);

    if (m_strText.length() > 0)
    {
        ITextRenderBase* p = m_pILabel->GetTextRenderDefault();
        if (p)
	        p->DrawState(pRenderTarget, &rcText, 0, m_strText.c_str());
    }
}

// void Label::OnLButtonUp(UINT nFlags, POINT point)
// {
//     IObject* pParent = m_pILabel->GetParentObject();
//     if (NULL == pParent)
//         return;
// 
//     POINT ptObj = {0,0};
//     pParent->WindowPoint2ObjectClientPoint(&point, &ptObj, true);
// 
//     // UI_WM_HITTEST是判断与自己的m_rcParent的交集，因此要将窗口坐标转换成parent的client坐标
//     UINT nHitTest = UISendMessage(m_pILabel, UI_WM_HITTEST, (WPARAM)&ptObj);
//     if (HTNOWHERE != nHitTest)
//     {
//         this->OnClicked(&point);   // 备注：由于DoNotify可能导致当前press hover对象发生改变，使得本控件丢失刷新
//     }
// }


// void Label::OnClicked(POINT* pt)
// {
//     // 通知消息
//     UIMSG   msg;
//     msg.message = UI_WM_NOTIFY;
//     msg.nCode   = UI_BN_CLICKED;
//     msg.wParam  = NULL; //(WPARAM)pt;
//     msg.lParam  = NULL;
//     msg.pMsgFrom = this->m_pILabel;
// 
//     msg.pMsgTo = NULL;
//     m_pILabel->DoNotify(&msg);
// }

void  Label::OnSerialize(SERIALIZEDATA* pData)
{
	DO_PARENT_PROCESS(ILabel, IControl);

    AttributeSerializerWrap s(pData, TEXT("Label"));

    s.AddI18nString(XML_TEXT,
        [this](LPCTSTR t, int s){ m_strText.assign(t, s); },
        [this](){ return m_strText.c_str(); })
        ->AsData();
}


void  Label::GetDesiredSize(SIZE* pSize)
{
	pSize->cx = pSize->cy = 0;

    ITextRenderBase* pTextRender = m_pILabel->GetTextRenderDefault();
	if (pTextRender)
	{
		LPCTSTR szText = m_strText.c_str();
		if (0 == szText[0])
		{
			// TBD: 内容为空时，至少保证高度 
			szText = TEXT(" ");
			*pSize = pTextRender->GetDesiredSize(
				szText, 
				m_pILabel->GetMaxWidth());
			pSize->cx = 0;
		}
		else
		{
			*pSize = pTextRender->GetDesiredSize(
				szText,
				m_pILabel->GetMaxWidth());
		}
	}

    REGION4 rcNonClient;
    m_pILabel->GetNonClientRegion(&rcNonClient);

    pSize->cx += rcNonClient.left+rcNonClient.right;
    pSize->cy += rcNonClient.top+rcNonClient.bottom;
}

void Label::OnBindSourceChanged(long key, bindable* src, void* context)
{
	if (key == ILabel::PROPERTY_TEXT)
	{
		if (src->get_type() == bindable::prop_value_int)
		{
			ATL::CString strBuffer;
			strBuffer.Format(TEXT("%d"), static_cast<bindable_int*>(src)->get());

			SetTextAndInvalidate(strBuffer);
		}
		else if(src->get_type() == bindable::prop_value_string)
		{
			SetTextAndInvalidate((LPCWSTR)(*static_cast<bindable_string*>(src)));
		}
	}
}

 
PictureCtrl::PictureCtrl(IPictureCtrl* p):MessageProxy(p)
{
    m_pIPictureCtrl = p;
	p->SetDescription(PictureCtrlDescription::Get());
}
PictureCtrl::~PictureCtrl()
{
}

HRESULT  PictureCtrl::FinalConstruct(ISkinRes* p)
{
	HRESULT lRet = DO_PARENT_PROCESS3(IPictureCtrl, IControl);

	OBJSTYLE sAdd = {0};
    sAdd.default_reject_all_mouse_msg = 1;

    OBJSTYLE sRemove = { 0 };
    sRemove.tabstop = 1;
    sRemove.default_tabstop = 1;
    m_pIPictureCtrl->ModifyObjectStyle(&sAdd, &sRemove);  // 默认不接收鼠标消息

	return lRet;
}

void  PictureCtrl::GetDesiredSize(SIZE* pSize)
{
	pSize->cx = pSize->cy = 0;
    IRenderBase* p = NULL;

    p = m_pIPictureCtrl->GetBackRender();
	if (p)
		*pSize = p->GetDesiredSize();
	
    p = m_pIPictureCtrl->GetForeRender();
    if (p)
        *pSize = p->GetDesiredSize();
}

void PictureCtrl::OnPaint(IRenderTarget* pRenderTarget)
{
// 	ControlStyle s = {0};
// 	s.ownerdraw = 1;
// 	if (m_pIPictureCtrl->TestControlStyle(&s))
// 	{
// 		OwnerDraw.emit(m_pIPictureCtrl, pRenderTarget);
// 		return;
// 	}

    IRenderBase* p = m_pIPictureCtrl->GetForeRender();;
	if (p)
	{
		CRect rc;
		m_pIPictureCtrl->GetObjectClientRect(&rc);
		p->DrawState(pRenderTarget, &rc, 0);   // TODO: 这里可以扩展为支持hover/press状态
	}
}

bool  PictureCtrl::SetImageByPath(LPCTSTR szPath)
{
    IRenderBase* pRender = m_pIPictureCtrl->GetBackRender();
    if (!pRender)
    {
        m_pIPictureCtrl->GetUIApplication()->CreateRenderBase(
            RENDER_TYPE_IMAGE, m_pIPictureCtrl, &pRender);

        m_pIPictureCtrl->SetBackRender(pRender);
    }

    if (pRender->GetType() != RENDER_TYPE_IMAGE)
        return false;

    IRenderBitmap* pRB = NULL;
    UICreateRenderBitmap(
        m_pIPictureCtrl->GetUIApplication(),
        GRAPHICS_RENDER_LIBRARY_TYPE_GDI,
        IMAGE_ITEM_TYPE_IMAGE,
        &pRB);

    if (!pRB)
        return false;

    if (pRB->LoadFromFile(szPath, RENDER_BITMAP_LOAD_CREATE_ALPHA_CHANNEL))
    {
        static_cast<IImageRender*>(pRender)->SetRenderBitmap(pRB);
    }
    SAFE_RELEASE(pRB);
    
    m_pIPictureCtrl->Invalidate();
    return true;
}
bool  PictureCtrl::SetImageById(LPCTSTR szId)
{
    IRenderBase* pRender = m_pIPictureCtrl->GetBackRender();
    if (!pRender)
    {
        m_pIPictureCtrl->GetUIApplication()->CreateRenderBase(
            RENDER_TYPE_IMAGE, m_pIPictureCtrl, &pRender);

        m_pIPictureCtrl->SetBackRender(pRender);
    }

    if (pRender->GetType() != RENDER_TYPE_IMAGE)
        return false;

    IRenderBitmap* pRB = NULL;
    ISkinRes* pSkinRes = m_pIPictureCtrl->GetSkinRes();
    pSkinRes->GetImageRes().GetBitmap(
        szId, GRAPHICS_RENDER_LIBRARY_TYPE_GDI, &pRB);
    
    if (!pRB)
        return false;

    static_cast<IImageRender*>(pRender)->SetRenderBitmap(pRB);
    SAFE_RELEASE(pRB);

    m_pIPictureCtrl->Invalidate();
    return true;
}

bool UI::PictureCtrl::AttachBitmap(HBITMAP bmp)
{
	IRenderBase* pRender = m_pIPictureCtrl->GetBackRender();
	if (!pRender)
	{
		m_pIPictureCtrl->GetUIApplication()->CreateRenderBase(
			RENDER_TYPE_IMAGE, m_pIPictureCtrl, &pRender);

		m_pIPictureCtrl->SetBackRender(pRender);
	}

	if (pRender->GetType() != RENDER_TYPE_IMAGE)
		return false;

	IRenderBitmap* pRB = NULL;
	UICreateRenderBitmap(
		m_pIPictureCtrl->GetUIApplication(),
		GRAPHICS_RENDER_LIBRARY_TYPE_GDI,
		IMAGE_ITEM_TYPE_IMAGE,
		&pRB);

	if (!pRB)
		return false;

	pRB->Attach(bmp, true);
	{
		static_cast<IImageRender*>(pRender)->SetRenderBitmap(pRB);
	}
	SAFE_RELEASE(pRB);

	m_pIPictureCtrl->Invalidate();
	return true;
}

//////////////////////////////////////////////////////////////////////////

GifCtrl::GifCtrl(IGifCtrl* p):MessageProxy(p)
{
	m_pIGifCtrl = p;
	m_pGifRender = NULL;
	p->SetDescription(GifCtrlDescription::Get());
}
GifCtrl::~GifCtrl()
{
	SAFE_RELEASE(m_pGifRender);
}

void  GifCtrl::OnGetDesiredSize(SIZE* pSize)
{
    pSize->cx = pSize->cy = 0;
	if (m_pGifRender)
	{
		pSize->cx = m_pGifRender->GetWidth();
		pSize->cy = m_pGifRender->GetHeight();
	}
}

HRESULT  GifCtrl::FinalConstruct(ISkinRes* p)
{
	HRESULT hRet = DO_PARENT_PROCESS3(IGifCtrl, IControl);

	OBJSTYLE sAdd = {0};
    sAdd.default_reject_all_mouse_msg = 1;

	OBJSTYLE sRemove = {0};
	sRemove.transparent = 1;
    sRemove.tabstop = 1; 
    sRemove.default_tabstop = 1;

	m_pIGifCtrl->ModifyObjectStyle(&sAdd, &sRemove);  // 默认不接收鼠标消息// 默认不透明
	return hRet;
}

void  GifCtrl::OnSerialize(SERIALIZEDATA* pData)
{
    DO_PARENT_PROCESS(IGifCtrl, IControl);

    if (pData->IsLoad())
    {
        IMapAttribute* pMapAttrib = pData->pMapAttrib;
        LPCTSTR  szText = pMapAttrib->GetAttr(XML_GIFCTRL_GIF, true);
        if (szText)
        {
            IGifImage*  pGifImage = NULL;

            IGifRes* pGifRes = m_pIGifCtrl->GetSkinRes()->GetImageManager().GetGifRes();
            if (pGifRes)
                pGifRes->GetGifImage(szText, &pGifImage);

            if (pGifImage)
            {
                SAFE_RELEASE(m_pGifRender);

                Gif_Timer_Notify notify;
                memset(&notify, 0, sizeof(notify));
                notify.eType = Gif_Timer_Notify_Send_Msg;
                notify.notify_ui_msg.pNotifyMsgObj = static_cast<IMessage*>(m_pIGifCtrl);
                m_pGifRender = pGifImage->AddRender(&notify, m_pIGifCtrl->GetUIApplication());
            }
        }

        if (m_pGifRender)
        {
            bool  bAutoPlay = false;
            pMapAttrib->GetAttr_bool(XML_GIFCTRL_AUTOPLAY, true, &bAutoPlay);
            if (bAutoPlay)
                m_pGifRender->Start();
        }
    }
}

void  GifCtrl::OnPaint(IRenderTarget* pRenderTarget)
{
	if (m_pGifRender)
	{
		// GIF都是直接带alpha通道的，可以用原始HDC绘制。否则使用Gdipulus::Graphics.GetHDC
		// 会大大降低绘制效率
		HDC hDC = pRenderTarget->GetHDC(); 
		                     
        if (pRenderTarget->IsRenderAlphaChannel())
        {
            m_pGifRender->OnAlphaPaint(hDC, 0,0);
        }
        else
        {
            m_pGifRender->OnPaint(hDC, 0,0);  // 因为HDC是已经带偏移量的，因此直接绘制在0,0即可
        }
	}
}

void  GifCtrl::OnGifFrameTick(WPARAM wParam, LPARAM lParam)
{
	m_pIGifCtrl->Invalidate();
}

bool  GifCtrl::Start()
{
    if (NULL == m_pGifRender)
        return false;

    m_pGifRender->Start();
    return true;
}
bool  GifCtrl::Pause()
{
    if (NULL == m_pGifRender)
        return false;

    m_pGifRender->Pause();
    return true;
}
bool  GifCtrl::Stop()
{
    if (NULL == m_pGifRender)
        return false;

    m_pGifRender->Stop();
    return true;
}

void GifCtrl::OnVisibleChanged(BOOL bVisible, IObject* pObjChanged)
{
    if (!m_pGifRender)
        return;

    if (!bVisible)
    {
        if (m_pGifRender->GetStatus() == GIF_DRAW_STATUS_START)
        {
            m_pGifRender->Pause();
        }
    }
    else
    {
        if (m_pGifRender->GetStatus() == GIF_DRAW_STATUS_PAUSE)
        {
            m_pGifRender->Start();
        }
    }

}