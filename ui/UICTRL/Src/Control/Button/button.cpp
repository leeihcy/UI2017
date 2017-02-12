#include "stdafx.h"
#include "button.h"

#include "..\UISDK\Inc\Interface\iimagerender.h"
#include "..\UISDK\Inc\Interface\iwindow.h"
#include "..\UISDK\Inc\Interface\iattribute.h"
#include "button_desc.h"
#include "..\UISDK\Inc\Interface\irenderbase.h"
#include "..\UISDK\Inc\Interface\itextrenderbase.h"
#include "..\UISDK\Inc\Interface\renderlibrary.h"
#include "..\UISDK\Inc\Interface\ilayout.h"

using namespace UI;


ButtonBase::ButtonBase(IButtonBase* p):MessageProxy(p)
{
    m_pIButtonBase = p;
	m_nCheckState = BST_UNCHECKED;
	m_bSpaceKeyDown = false;
	m_eButtonType = BUTTON_TYPE_PUSHBUTTON;

	memset(&m_buttonStyle, 0, sizeof(BUTTONSTYLE));
}


//	按钮的类型
BUTTON_TYPE  ButtonBase::GetButtonType()
{
	return m_eButtonType;
}
void ButtonBase::SetButtonType(BUTTON_TYPE n)
{
	m_eButtonType = n;
}

void  ButtonBase::ModifyButtonStyle(BUTTONSTYLE* add, BUTTONSTYLE* remove)
{
#define __REMOVE(x) \
    if (remove->x) m_buttonStyle.x = 0;
#define __ADD(x) \
	if (add->x) m_buttonStyle.x = 1;
    
	if (add)
	{
		__ADD(click_on_pushdown);
		__ADD(click_on_pushdown);
		__ADD(notify_hoverleave);
		__ADD(hand_cursor);
		__ADD(enable_radio_toggle);
	}

	if (remove)
	{
		__REMOVE(click_on_pushdown);
		__REMOVE(click_on_pushdown);
		__REMOVE(notify_hoverleave);
		__REMOVE(hand_cursor);
		__REMOVE(enable_radio_toggle); 
	}
}

bool ButtonBase::TestButtonStyle(const BUTTONSTYLE& test)
{

#define __TEST(x) \
    if (test.x && !m_buttonStyle.x) return false; 

	__TEST(click_on_pushdown);
	__TEST(click_on_pushdown);
	__TEST(notify_hoverleave);
	__TEST(hand_cursor);
	__TEST(enable_radio_toggle);

	return true;
}



const UINT  BUTTON_BKGND_RENDER_STATE_NORMAL = RENDER_STATE_NORMAL | 0;
const UINT  BUTTON_BKGND_RENDER_STATE_HOVER = RENDER_STATE_HOVER | 1;
const UINT  BUTTON_BKGND_RENDER_STATE_PRESS = RENDER_STATE_PRESS | 2;
const UINT  BUTTON_BKGND_RENDER_STATE_DISABLE = RENDER_STATE_DISABLE | 3;
const UINT  BUTTON_BKGND_RENDER_STATE_DEFAULT = RENDER_STATE_DEFAULT | 4;
const UINT  BUTTON_BKGND_RENDER_STATE_SELECTED_NORMAL = RENDER_STATE_NORMAL | RENDER_STATE_SELECTED | 4;
const UINT  BUTTON_BKGND_RENDER_STATE_SELECTED_HOVER = RENDER_STATE_HOVER | RENDER_STATE_SELECTED | 5;
const UINT  BUTTON_BKGND_RENDER_STATE_SELECTED_PRESS = RENDER_STATE_PRESS | RENDER_STATE_SELECTED | 6;
const UINT  BUTTON_BKGND_RENDER_STATE_SELECTED_DISABLE = RENDER_STATE_DISABLE | RENDER_STATE_SELECTED | 7;

const UINT  BUTTON_ICON_RENDER_STATE_NORMAL = RENDER_STATE_NORMAL | 0;
const UINT  BUTTON_ICON_RENDER_STATE_HOVER = RENDER_STATE_HOVER | 1;
const UINT  BUTTON_ICON_RENDER_STATE_PRESS = RENDER_STATE_PRESS | 2;
const UINT  BUTTON_ICON_RENDER_STATE_DISABLE = RENDER_STATE_DISABLE | 3;
const UINT  BUTTON_ICON_RENDER_STATE_SELECTED_NORMAL = RENDER_STATE_NORMAL | RENDER_STATE_SELECTED | 4;
const UINT  BUTTON_ICON_RENDER_STATE_SELECTED_HOVER = RENDER_STATE_HOVER | RENDER_STATE_SELECTED | 5;
const UINT  BUTTON_ICON_RENDER_STATE_SELECTED_PRESS = RENDER_STATE_PRESS | RENDER_STATE_SELECTED | 6;
const UINT  BUTTON_ICON_RENDER_STATE_SELECTED_DISABLE = RENDER_STATE_DISABLE | RENDER_STATE_SELECTED | 7;

void ButtonBase::OnEraseBkgnd(IRenderTarget* pRenderTarget)
{
    IRenderBase* pBkgndRender = m_pIButtonBase->GetBackRender();
	if (pBkgndRender)
	{
		bool  bDisable = !m_pIButtonBase->IsEnable();
		bool  bHover   = m_pIButtonBase->IsHover();
		bool  bPress   = m_pIButtonBase->IsPress();
		bool  bForePress = m_pIButtonBase->IsForcePress();
		bool  bChecked = IsChecked();
        bool  bDefault = m_pIButtonBase->IsDefault();

		CRect rc(0,0, m_pIButtonBase->GetWidth(), m_pIButtonBase->GetHeight());

		if (bDisable)
		{
			pBkgndRender->DrawState(pRenderTarget, &rc, bChecked?BUTTON_BKGND_RENDER_STATE_SELECTED_DISABLE:BUTTON_BKGND_RENDER_STATE_DISABLE);
		}
		else if (bForePress || (bPress && bHover))
		{
			pBkgndRender->DrawState(pRenderTarget, &rc, bChecked?BUTTON_BKGND_RENDER_STATE_SELECTED_PRESS:BUTTON_BKGND_RENDER_STATE_PRESS);
		}
		else if (bHover || bPress)
		{
			pBkgndRender->DrawState(pRenderTarget, &rc, bChecked?BUTTON_BKGND_RENDER_STATE_SELECTED_HOVER:BUTTON_BKGND_RENDER_STATE_HOVER);
		}
        else if (bDefault)
        {
            pBkgndRender->DrawState(pRenderTarget, &rc, BUTTON_BKGND_RENDER_STATE_DEFAULT);
        }
		else 
		{
			pBkgndRender->DrawState(pRenderTarget, &rc, bChecked?BUTTON_BKGND_RENDER_STATE_SELECTED_NORMAL:BUTTON_BKGND_RENDER_STATE_NORMAL);
		}
	}

	if (m_pIButtonBase->IsFocus())  // 将focus放在这里是为了避免配置了padding时,在onpaint中绘制focus rect也会受到影响
	{
        LRESULT lRet = ::SendMessage(m_pIButtonBase->GetHWND(), WM_QUERYUISTATE, 0, 0);
        if (!(lRet & UISF_HIDEFOCUS))
        {
    		this->DrawFocus(pRenderTarget);
        }
	}	
}


void ButtonBase::OnStateChanged2(UINT nMask)
{
    if (nMask & OSB_HOVER)
    {
        if (m_buttonStyle.notify_hoverleave)
        {
            UIMSG  msg;
            msg.pMsgFrom = m_pIButtonBase;
            msg.message = UI_MSG_NOTIFY;
            msg.wParam = (WPARAM)m_pIButtonBase;
            if (m_pIButtonBase->IsHover())
            {
                msg.nCode = UI_BN_HOVER;
            }
            else
            {
                msg.nCode = UI_BN_LEAVE;
            }
            m_pIButtonBase->DoNotify(&msg);
        }
    }

	m_pIButtonBase->Invalidate();
}

// void ButtonBase::OnMouseMove(UINT nFlags, POINT point)
// {
// 	// 判断这个时候的特殊情况：对象处于按下状态，但鼠标在对象外面，这个时候要显示hover状态
// 	bool bOut = false;
// 	bOut = ( HTNOWHERE == this->OnHitTest( &point ) );
// 	if( bOut )
// 	{
// 		if( IsHover() )
// 		{
// 			this->SetHover( false );
// 			this->Invalidate( );
// 		}
// 	}
// 	else
// 	{
// 		if( !IsHover() )
// 		{
// 			this->SetHover( true );
// 			this->Invalidate( );
// 		}
// 	}
// 	
// }
// void ButtonBase::OnMouseLeave()
// {
// 	this->SetHover( false );
// 	this->SetPress( false );
// 	this->Invalidate( );
// }
// void ButtonBase::OnLButtonDown(UINT nFlags, POINT point)
// {
// 	this->SetPress( true );
// 	this->Invalidate( );
// }
void ButtonBase::OnLButtonDblClk(UINT nFlags, POINT point)
{
	// 为了让Hook拦截到这一次的点击事件，如双击滚动条上的按钮
	UIMSG  msg;
	msg.pMsgTo = m_pIButtonBase;
	msg.message = WM_LBUTTONDOWN;
	msg.wParam  = (WPARAM)nFlags;
	msg.lParam  = MAKELPARAM(point.x,point.y);
	::UISendMessage( &msg );  
}

void  ButtonBase::OnLButtonDown(UINT nFlags, POINT point)
{
    if (m_buttonStyle.click_on_pushdown)
    {
        this->OnClicked(&point);  
    }   
}
void ButtonBase::OnLButtonUp(UINT nFlags, POINT point)
{
    if (m_buttonStyle.click_on_pushdown)
        return;

    if (m_pIButtonBase->IsHover())
    {
        this->OnClicked(&point);
    }

//  IObject* pParent = m_pIButtonBase->GetParentObject();
//  if (NULL == pParent)
//      return;
// 
// 	POINT ptObj = {0,0};
// 	pParent->WindowPoint2ObjectClientPoint(&point, &ptObj, true);
// 
//     // UI_WM_HITTEST是判断与自己的m_rcParent的交集，因此要将窗口坐标转换成parent的client坐标
//     UINT nHitTest = UISendMessage(m_pIButtonBase, UI_WM_HITTEST, (WPARAM)&ptObj);
//     if (HTNOWHERE != nHitTest)
// 	{
// 		this->OnClicked(&point);   // 备注：由于DoNotify可能导致当前press hover对象发生改变，使得本控件丢失刷新
// 	}
}
void  ButtonBase::OnRButtonUp(UINT nFlags, POINT point)
{
// 	UIMSG   msg;
// 	msg.message = UI_MSG_NOTIFY;
// 	msg.nCode   = UI_BN_RCLICK;
// 	msg.wParam  = NULL; //(WPARAM)pt;
// 	msg.lParam  = NULL;
// 	msg.pMsgFrom = this->m_pIButtonBase;
// 
// 	m_pIButtonBase->DoNotify(&msg);
}

void ButtonBase::OnClicked(POINT* pt)
{
	virtualOnClicked();
}

void  ButtonBase::DrawFocus(IRenderTarget* p)
{
//     UIMSG msg;
//     msg.pMsgTo = m_pIButtonBase;
//     msg.wParam = (WPARAM)p;
//     msg.nCode = UI_BN_DRAWFOCUS;
//     msg.message = UI_WM_NOTIFY;
// 
//     UISendMessage(&msg);
}

void ButtonBase::OnSetFocus(IObject*)
{
}
void ButtonBase::OnKillFocus(IObject*)
{
    if (IsKeyDown(VK_SPACE))
    {
        bool bIsForePress = m_pIButtonBase->IsForcePress();
        m_pIButtonBase->SetForcePress(false, false);
        if (bIsForePress)
        {
            m_pIButtonBase->Invalidate();
        }
    }
}
void ButtonBase::OnKeyDown( UINT nChar, UINT nRepCnt, UINT nFlags )
{
	if (VK_SPACE == nChar && m_pIButtonBase->IsEnable())
	{
        // alt+space是弹出窗口的sysmenu
        if (IsKeyDown(VK_SHIFT))
            return;
        if (IsKeyDown(VK_CONTROL))
            return;
        if (IsKeyDown(VK_MENU))
            return;

        // windows控件在SPACE按下时，会去为Button窗口xxxBNSetCapture，
        // 这样所有的鼠标消息都只跑到它里面来
		m_pIButtonBase->SetForcePress(true, false);
		m_pIButtonBase->Invalidate();

        m_pIButtonBase->SetMouseCapture(BUTTON_VK_SPACE_MOUSECAPTURE_NOTIFY_ID);
	}
}
void ButtonBase::OnKeyUp( UINT nChar, UINT nRepCnt, UINT nFlags )
{
	if (VK_SPACE == nChar && m_pIButtonBase->IsEnable() && m_pIButtonBase->IsForcePress())
	{
		m_pIButtonBase->SetForcePress(false, false);
        m_pIButtonBase->ReleaseMouseCapture();

		if (!IsKeyDown(VK_LBUTTON))
		{
			// 用于子类扩展
            POINT pt = {0,0};
			GetCursorPos(&pt);

			this->OnClicked(&pt);

		}
		m_pIButtonBase->Invalidate();
	}
}

// 在按下空格后，如果进入拖动窗口状态，将会收不到keyup消息。
LRESULT  ButtonBase::OnEnterSizeMove(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	SetMsgHandled(FALSE);

	if (m_pIButtonBase->IsEnable())
	{
		m_pIButtonBase->SetForcePress(false, false);
		m_pIButtonBase->ReleaseMouseCapture();

		m_pIButtonBase->Invalidate();
	}
	return 0;
}

void ButtonBase::SetCheck(int nCheckState)
{
	if(nCheckState == BST_CHECKED)
		this->SetChecked();
	else
		this->SetUnChecked();
}

void  ButtonBase::SetCheck(bool bCheck)
{
    if (IsChecked() == bCheck)
        return;

    if (bCheck)
        this->SetChecked();
    else
        this->SetUnChecked();

    m_pIButtonBase->Invalidate();
}

void ButtonBase::SetChecked()
{
	if (m_nCheckState & BST_CHECKED)
	{
	}
	else
	{
		m_nCheckState |= BST_CHECKED;
		m_pIButtonBase->Invalidate();
	}
}

void ButtonBase::SetUnChecked()
{
	if ( m_nCheckState & BST_CHECKED )
	{
		m_nCheckState &= ~BST_CHECKED;
		m_pIButtonBase->Invalidate();
	}
	else
	{

	}
}
int  ButtonBase::GetCheck()
{
	return m_nCheckState;
}
bool ButtonBase::IsChecked()
{
	return (m_nCheckState & BST_CHECKED) ? true:false;
}


//////////////////////////////////////////////////////////////////////////
Button::Button(IButton* p):ButtonBase(p)
{
    m_pIButton = p;
    m_eAutoSizeType = BUTTON_AUTOSIZE_TYPE_NDEF;
    m_eIconAlignFlag = ALIGN_LEFT;
    m_lIconMarginText = 0;
    m_eDrawFocusType = BUTTON_RENDER_DRAW_FOCUS_TYPE_NONE;
	p->SetDescription(ButtonDescription::Get());
}
Button::~Button()
{
}

void  Button::OnInitialize()
{
    SetMsgHandled(FALSE);

    // 检测自己是否是DefaultPushButton
	if (m_buttonStyle.default_push_button)
	{
        set_as_window_default_button();
	}
}

void  Button::SetIconFromFile(LPCTSTR szIconPath)
{
	UIASSERT(0);
#if 0
    m_pIButton->SetForegndRender(NULL);
    if (NULL == szIconPath)
        return;

    IUIApplication*  pUIApplication = m_pIButton->GetUIApplication();
    if (NULL == pUIApplication)
    {
        UIASSERT(0);
        return;
    }

    IRenderBase*   pForeRender = NULL;
    pUIApplication->CreateRenderBase(
        RENDER_TYPE_IMAGE, m_pIButton, &pForeRender);
    IImageRender*  pImageForeRender = (IImageRender*)pForeRender->QueryInterface(__uuidof(IImageRender));

    UI::IRenderBitmap*  pRenderBitmap = NULL;
    UI::UICreateRenderBitmap(pUIApplication, m_pIButton->GetGraphicsRenderLibraryType(), IMAGE_ITEM_TYPE_IMAGE, &pRenderBitmap);
    pRenderBitmap->LoadFromFile(szIconPath, RENDER_BITMAP_LOAD_CREATE_ALPHA_CHANNEL);
    pImageForeRender->SetRenderBitmap(pRenderBitmap);
    pImageForeRender->SetImageDrawType(UI::DRAW_BITMAP_CENTER);
    SAFE_RELEASE(pRenderBitmap);

    m_pIButton->SetForegndRender(pForeRender);
    SAFE_RELEASE(pForeRender);
#endif
}

void  Button::SetIconFromImageId(LPCTSTR szImageId)
{
	UIASSERT(0);
#if 0
    m_pIButton->SetForegndRender(NULL);
    if (NULL == szImageId)
        return;

    IUIApplication*  pUIApplication = m_pIButton->GetUIApplication();
    if (NULL == pUIApplication)
    {
        UIASSERT(0);
        return;
    }

    UI::IRenderBitmap*  pRenderBitmap = NULL;
    pUIApplication->GetActiveSkinImageRes()->GetBitmap(
        szImageId, 
        m_pIButton->GetGraphicsRenderLibraryType(), 
        &pRenderBitmap);
    if (!pRenderBitmap)
        return;

    IRenderBase*   pForeRender = NULL;
    pUIApplication->CreateRenderBase(
        RENDER_TYPE_IMAGE, m_pIButton, &pForeRender);
    IImageRender*  pImageForeRender = (IImageRender*)pForeRender->QueryInterface(uiiidof(IImageRender));

    pImageForeRender->SetRenderBitmap(pRenderBitmap);
    pImageForeRender->SetImageDrawType(UI::DRAW_BITMAP_CENTER);
    SAFE_RELEASE(pRenderBitmap);

    m_pIButton->SetForegndRender(pForeRender);
    SAFE_RELEASE(pForeRender);
#endif
}

// 该HBITMAP最后会被RenderBitmap所销毁
void  Button::SetIconFromHBITMAP(HBITMAP hBitmap)
{
	UIASSERT(0);
#if 0
    m_pIButton->SetForegndRender(NULL);
    if (NULL == hBitmap)
        return;

    IUIApplication*  pUIApplication = m_pIButton->GetUIApplication();
    if (NULL == pUIApplication)
    {
        UIASSERT(0);
        return;
    }

    IRenderBase*   pForeRender = NULL;
    pUIApplication->CreateRenderBase(RENDER_TYPE_IMAGE, m_pIButton, &pForeRender);
    IImageRender*  pImageForeRender = (IImageRender*)pForeRender->QueryInterface(uiiidof(IImageRender));

    UI::IRenderBitmap*  pRenderBitmap = NULL;
    UI::UICreateRenderBitmap(pUIApplication, UI::GRAPHICS_RENDER_LIBRARY_TYPE_GDI, IMAGE_ITEM_TYPE_IMAGE, &pRenderBitmap);
    pRenderBitmap->Attach(hBitmap, true);
    pImageForeRender->SetRenderBitmap(pRenderBitmap);
    pImageForeRender->SetImageDrawType(UI::DRAW_BITMAP_CENTER);
    SAFE_RELEASE(pRenderBitmap);

    m_pIButton->SetForegndRender(pForeRender);
    SAFE_RELEASE(pForeRender);
#endif
}

LPCTSTR  Button::GetText() 
{ 
    return m_strText.c_str(); 
}

void  Button::SetText(LPCTSTR szText) 
{
	if (szText)
		m_strText = szText;
	else
		m_strText.clear();
}

void  Button::SetTextAndUpdate(LPCTSTR  szText)
{
    if (szText && m_strText == szText)
        return;

    if (!szText && m_strText.empty())
        return;

    SetText(szText);

	// 内容自适应时，更新所在布局
	if (m_eAutoSizeType == BUTTON_AUTOSIZE_TYPE_CONTENT ||
		m_eAutoSizeType == BUTTON_AUTOSIZE_TYPE_BKIMAGEHEIGHT_CONTENTWIDTH)
	{
		// TODO: 做成一个方法
		ILayoutParam*  param = m_pIButton->GetSafeLayoutParam();
		if (param && param->IsSizedByContent())
		{
			UI::IObject* parent = m_pIButton->GetParentObject();
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
	}

    m_pIButton->Invalidate();
}

void  Button::Click()
{
	this->virtualOnClicked();
}

void  Button::GetDesiredSize(SIZE* pSize)
{
    pSize->cx = pSize->cy = 0;

    BUTTON_AUTO_SIZE_TYPE eAutoSizeType = m_eAutoSizeType;
    if (eAutoSizeType == BUTTON_AUTOSIZE_TYPE_NDEF)
    {
        // hack 多语言支持编辑时，m_text包含两段数据
        if (m_strText.length() && m_strText[0] != 0)
            eAutoSizeType = BUTTON_AUTOSIZE_TYPE_CONTENT;
        else
            eAutoSizeType = BUTTON_AUTOSIZE_TYPE_BKIMAGE;
    }

    // 按钮的auto size可以按背景图标大小来算，也可以按内容的大小来算
    switch (eAutoSizeType)
    {
    case BUTTON_AUTOSIZE_TYPE_BKIMAGE:
        {
            if (m_pIButton->GetBackRender())
            {
                *pSize = m_pIButton->GetBackRender()->GetDesiredSize( );
            }
        }
        break;

    case BUTTON_AUTOSIZE_TYPE_CONTENT:
        {
            REGION4 rcNonClient;
            m_pIButton->GetNonClientRegion(&rcNonClient);

            pSize->cx = rcNonClient.left+rcNonClient.right;
            pSize->cy = rcNonClient.top+rcNonClient.bottom;

            SIZE sIconText = {0,0};
            {
                SIZE sizeIcon = {0,0};
                SIZE sizeText = {0,0};

                if (m_pIButton->GetForeRender())
                {
                    sizeIcon = m_pIButton->GetForeRender()->GetDesiredSize();
                }
                if (m_pIButton->GetTextRenderDefault())
                {
                    sizeText = m_pIButton->GetTextRenderDefault()
                        ->GetDesiredSize(m_strText.c_str());
                }

                switch (m_eIconAlignFlag)
                {
                case ALIGN_LEFT:
                case ALIGN_RIGHT:
                    {
                        sIconText.cx = 
                            sizeIcon.cx + 
                            sizeText.cx + 
                            m_lIconMarginText;
                        sIconText.cy = max( sizeIcon.cy, sizeText.cy );
                    }
                    break;
                case ALIGN_TOP:
                case ALIGN_BOTTOM:
                    {
                        sIconText.cx = max( sizeIcon.cx, sizeText.cx );
                        sIconText.cy = 
                            sizeIcon.cy + 
                            sizeText.cy +
                            m_lIconMarginText;
                    }
                    break;
                }
            }
            pSize->cx += sIconText.cx;
            pSize->cy += sIconText.cy;
        }
        break;

    case BUTTON_AUTOSIZE_TYPE_BKIMAGEHEIGHT_CONTENTWIDTH:
        {
            SIZE sHeight = {0,0};
            if (m_pIButton->GetBackRender())
                sHeight = m_pIButton->GetBackRender()->GetDesiredSize( );

            SIZE sWidth;
            REGION4 rcNonClient;
            m_pIButton->GetNonClientRegion(&rcNonClient);

            sWidth.cx = rcNonClient.left+rcNonClient.right;
            sWidth.cy = rcNonClient.top+rcNonClient.bottom;

            SIZE sIconText = {0,0};
            {
                SIZE sizeIcon = {0,0};
                SIZE sizeText = {0,0};

                if (m_pIButton->GetForeRender())
                {
                    sizeIcon = m_pIButton->GetForeRender()->GetDesiredSize(  );
                }
                if (m_pIButton->GetTextRenderDefault())
                {
                    sizeText = m_pIButton->GetTextRenderDefault()
                        ->GetDesiredSize(m_strText.c_str());
                }

                switch (m_eIconAlignFlag)
                {
                case ALIGN_LEFT:
                case ALIGN_RIGHT:
                    {
                        sIconText.cx =
                            sizeIcon.cx +
                            sizeText.cx + 
                            m_lIconMarginText;
                        sIconText.cy = max( sizeIcon.cy, sizeText.cy );
                    }
                    break;
                case ALIGN_TOP:
                case ALIGN_BOTTOM:
                    {
                        sIconText.cx = max( sizeIcon.cx, sizeText.cx );
                        sIconText.cy = 
                            sizeIcon.cy + 
                            sizeText.cy + 
                            m_lIconMarginText;
                    }
                    break;
                }

                sWidth.cx += sIconText.cx;
                sWidth.cy += sIconText.cy;
            }

            pSize->cx = sWidth.cx;
            pSize->cy = sHeight.cy;
        }

    default:
        break;
    }
}

UINT  Button::OnGetDlgCode(LPMSG lpMsg)
{   
    UINT nRet = DLGC_BUTTON;
    if (m_pIButton->IsDefault())
    {
        nRet |= DLGC_DEFPUSHBUTTON;
    }
    else
    {
        if (m_pIButton->CanTabstop() && HasDefaultPushButtonStyle())
        {
            // 表示现在不是默认按钮，但自己可以成为默认按钮
            nRet |= DLGC_UNDEFPUSHBUTTON;  
        }
    }

    return nRet;
}

void Button::OnSerialize(SERIALIZEDATA* pData)
{
    DO_PARENT_PROCESS(IButton, IButtonBase);

    AttributeSerializerWrap s(pData, TEXT("Button"));

    s.AddI18nString(XML_TEXT,
        [this](LPCTSTR t, int s){ m_strText.assign(t, s); },
        [this](){ return m_strText.c_str(); })
        ->AsData();

    s.AddLong(XML_BUTTON_ICON_TEXT_SPACE, m_lIconMarginText);

    s.AddBool(XML_BUTTON_DEFPUSHBUTTON, this, 
        memfun_cast<pfnBoolSetter>(&Button::SetDefaultPushButtonStyle),
        memfun_cast<pfnBoolGetter>(&Button::HasDefaultPushButtonStyle))
        ->SetDefault(false);

	s.AddBool(XML_BUTTON_HAND_CUROSR, this,
		memfun_cast<pfnBoolSetter>(&Button::SetHandCursorStyle),
		memfun_cast<pfnBoolGetter>(&Button::HasHandCursorStyle))
		->SetDefault(false);

    s.AddEnum(XML_BUTTON_AUTOSIZE_TYPE, *(long*)&m_eAutoSizeType)
        ->AddOption(BUTTON_AUTOSIZE_TYPE_NDEF, TEXT(""))
        ->AddOption(BUTTON_AUTOSIZE_TYPE_CONTENT,  XML_BUTTON_AUTOSIZE_TYPE_CONTENT)
        ->AddOption(BUTTON_AUTOSIZE_TYPE_BKIMAGE,  XML_BUTTON_AUTOSIZE_TYPE_BKIMAGE)
        ->AddOption(BUTTON_AUTOSIZE_TYPE_BKIMAGEHEIGHT_CONTENTWIDTH, 
                    XML_BUTTON_AUTOSIZE_TYPE_BKIMAGEHEIGHT_CONTENTWIDTH)
        ->SetDefault(BUTTON_AUTOSIZE_TYPE_NDEF);

    s.AddEnum(XML_BUTTON_ICON_ALIGN, *(long*)&m_eIconAlignFlag)
        ->AddOption(ALIGN_LEFT,   XML_ALIGN_LEFT)
        ->AddOption(ALIGN_RIGHT,  XML_ALIGN_RIGHT)
        ->AddOption(ALIGN_TOP,    XML_ALIGN_TOP)
        ->AddOption(ALIGN_BOTTOM, XML_ALIGN_BOTTOM)
        ->AddOption(ALIGN_CENTER, XML_ALIGN_CENTER)
        ->SetDefault(ALIGN_LEFT);

    s.AddEnum(XML_BUTTON_DRAW_FOCUS_TYPE, *(long*)&m_eDrawFocusType)
        ->AddOption(BUTTON_RENDER_DRAW_FOCUS_TYPE_DOT,  XML_BUTTON_DRAW_FOCUS_TYPE_DOT)
        ->AddOption(BUTTON_RENDER_DRAW_FOCUS_TYPE_NONE, XML_BUTTON_DRAW_FOCUS_TYPE_NONE)
        ->SetDefault(BUTTON_RENDER_DRAW_FOCUS_TYPE_NONE);
}

// bool  Button::useWinTheme()
// {
// 	m_pIButton->SetBkgndRender(NULL);
// 
// 	if (this->GetButtonType() != BUTTON_STYLE_PUSHBUTTON)
// 		return false;
// 
// 	IRenderBase* p = NULL;
// 	m_pIButton->GetUIApplication()->CreateRenderBase(
// 		RENDER_TYPE_THEME_BUTTON_BKGND, 
// 		this->m_pIButton, &p);
// 
// 	if (!p)
// 		return false;
// 	
// 	IMapAttribute* pMapAttrib = NULL;
// 	m_pIButton->GetMapAttribute(&pMapAttrib);
// 	p->SetAttribute(pMapAttrib, XML_BACKGND_RENDER_PREFIX, true);
// 	pMapAttrib->Release();
// 
// 	m_pIButton->SetBkgndRender(p);
// 	p->Release();
// 
// 	ObjStyle s = {0};
// 	s.transparent = 1;
// 	m_pIButton->ModifyObjectStyle(&s, 0);
// 
// 	return true;
// }

void  Button::SetDrawFocusType(BUTTON_RENDER_DRAW_FOCUS_TYPE eType)
{
    m_eDrawFocusType = eType;
}
void  Button::SetAutoSizeType(BUTTON_AUTO_SIZE_TYPE eType)
{
    m_eAutoSizeType = eType;
}

void  Button::SetDefaultPushButtonStyle(bool b)
{
	m_buttonStyle.default_push_button = b;
}
bool  Button::HasDefaultPushButtonStyle()
{
	return m_buttonStyle.default_push_button;
}

void  Button::SetHandCursorStyle(bool b)
{
	m_buttonStyle.hand_cursor = b;
}
bool  Button::HasHandCursorStyle()
{
	return m_buttonStyle.hand_cursor;
}

//
// 根据参数的设置，计算出图标和文本的绘制区域
//
//	Remark:
//	   求出的位置没有去计算居中位置
//
void Button::calc_icontext_rect(IRenderTarget* pRenderTarget, CRect& rcIcon, CRect& rcText)
{
    m_pIButton->GetObjectClientRect(&rcIcon);
    rcText.CopyRect(rcIcon);

    SIZE s = {0,0};
    if (m_pIButton->GetForeRender())
    {
        s = m_pIButton->GetForeRender()->GetDesiredSize();
    }

    switch (m_eIconAlignFlag)
    {
    case ALIGN_LEFT:
        {
            rcIcon.right = rcIcon.left + s.cx;
            rcText.left  = rcIcon.right + m_lIconMarginText;
        }
        break;
    case ALIGN_TOP:
        {
            rcIcon.bottom = rcIcon.top + s.cy;
            rcText.top = rcIcon.bottom + m_lIconMarginText;
        }
        break;
    case ALIGN_RIGHT:
        {
            rcIcon.left = rcIcon.right - s.cx;
            rcText.right = rcIcon.left - m_lIconMarginText;
        }
        break;
    case ALIGN_BOTTOM:
        {
            rcIcon.top = rcIcon.bottom - s.cy;
            rcText.bottom = rcIcon.top - m_lIconMarginText;
        }
        break;

    case ALIGN_CENTER:
        {
            rcIcon.left = rcIcon.left + (rcIcon.Width() - s.cx)/2;
            rcIcon.right = rcIcon.left + s.cx;
            rcIcon.top = rcIcon.top + (rcIcon.Height() - s.cy)/2;
            rcIcon.bottom = rcIcon.top + s.cy;
        }
        break;
    }
}

void Button::OnPaint(IRenderTarget* pRenderTarget)
{
    CRect rcIcon, rcText;

    this->calc_icontext_rect(pRenderTarget, rcIcon, rcText);

    bool  bDisable = !m_pIButton->IsEnable();
    bool  bHover   = m_pIButton->IsHover();
    bool  bPress   = m_pIButton->IsPress();
    bool  bChecked = IsChecked() /*|| m_pIButton->IsSelected()*/;
    bool  bForePress = m_pIButton->IsForcePress();

    int  nState = 0;
    if (bDisable)
    {
        nState = bChecked?BUTTON_ICON_RENDER_STATE_SELECTED_DISABLE:BUTTON_ICON_RENDER_STATE_DISABLE;
    }
    else if (bForePress || (bPress && bHover))
    {
        nState = bChecked?BUTTON_ICON_RENDER_STATE_SELECTED_PRESS:BUTTON_ICON_RENDER_STATE_PRESS;
    }
    else if (bHover || bPress)
    {
        nState = bChecked?BUTTON_ICON_RENDER_STATE_SELECTED_HOVER:BUTTON_ICON_RENDER_STATE_HOVER;
    }
    else 
    {
        nState = bChecked?BUTTON_ICON_RENDER_STATE_SELECTED_NORMAL:BUTTON_ICON_RENDER_STATE_NORMAL;
    }

    if (m_pIButton->GetForeRender())
        m_pIButton->GetForeRender()->DrawState(pRenderTarget, &rcIcon, nState);

    if (m_strText.length() > 0)
    {
        ITextRenderBase* pTextRender = m_pIButton->GetTextRenderDefault();
        if (pTextRender)
            pTextRender->DrawState(pRenderTarget, &rcText, nState, m_strText.c_str());
    }
}

LRESULT  Button::DrawFocus(WPARAM w, LPARAM)
{
    IRenderTarget* pRenderTarget = (IRenderTarget*)w;
    switch(m_eDrawFocusType)
    {
    case BUTTON_RENDER_DRAW_FOCUS_TYPE_NONE:
        break;
    case BUTTON_RENDER_DRAW_FOCUS_TYPE_DOT:
        {
            CRect  rc;
            switch (this->GetButtonType())
            {
            case BUTTON_TYPE_RADIOBUTTON:
			case BUTTON_TYPE_CHECKBUTTON:
                {
                    CRect rcIcon, rcText;
					this->calc_icontext_rect(pRenderTarget, rcIcon, rcText);  // 返回的坐标是基于client的

                    this->m_pIButton->ClientRect2ObjectRect(&rcText, &rc);   
                }
                break;

            case BUTTON_TYPE_HYPERLINK:
                {
                    this->m_pIButton->GetClientRectInObject(&rc);
                }
                break;

                //case BUTTON_STYLE_PUSHBUTTON:
            default:
                {
                    rc.SetRect(0,0, m_pIButton->GetWidth(), m_pIButton->GetHeight());
                    ::InflateRect( &rc, -3, -3 );
                }
                break;
            }

            pRenderTarget->DrawFocusRect(&rc);
        }
        break;
    }

    return 0;
}

void  Button::virtualOnClicked()
{
//     UIMSG   msg;
//     msg.message = UI_WM_NOTIFY;
//     msg.nCode   = UI_BN_CLICKED;
//     msg.wParam  = NULL; //(WPARAM)&pt;
//     msg.lParam  = NULL;
//     msg.pMsgFrom = m_pIButton;
//     LRESULT lRet = m_pIButton->DoNotify(&msg);

    clicked.emit(m_pIButton);
}

LRESULT Button::OnSetCursor(UINT, WPARAM, LPARAM)
{
	if (HasHandCursorStyle())
		SetCursor(LoadCursor(NULL, IDC_HAND));
	else
		SetCursor(LoadCursor(NULL, IDC_ARROW));
	return 0;
}

LRESULT  Button::OnDefaultEvent(UINT, WPARAM, LPARAM)
{
    this->Click();
    return 0;
}

void  Button::OnVisibleChanged(BOOL bVisible, IObject* pObjChanged)
{
    // 检测自己是否是DefaultPushButton
    if (m_buttonStyle.default_push_button && bVisible)
    {
        set_as_window_default_button();
    }
}

void  Button::set_as_window_default_button()
{
    IWindowBase* pWindow = m_pIButton->GetWindowObject();
    if (pWindow)
    {
        UISendMessage(pWindow,
            UI_DM_SETDEFID, (WPARAM)m_pIButton);
    }
}