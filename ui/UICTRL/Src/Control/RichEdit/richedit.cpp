#include "stdafx.h"
#include "richedit.h"
#include "UI\UISDK\Inc\Interface\iuires.h"
#include "UI\UISDK\Inc\Interface\icolorrender.h"
#include "UI\UISDK\Inc\Interface\iwindow.h"
#include "UI\UISDK\Inc\Interface\irenderlayer.h"
#include "UI\UISDK\Inc\Interface\iscrollbarmanager.h"
#include "UI\UISDK\Inc\Interface\iattribute.h"
#include "UI\UICTRL\Src\Control\ScrollBar\systemscrollbar.h"
#include "UI\UICTRL\Inc\Interface\imenu.h"
#include "Bubble\rebubblemgr.h"
#include "richedit_desc.h"
#include "..\UISDK\Inc\Util\util.h"


using namespace UI;


RichEdit::RichEdit(IRichEdit* p):MessageProxy(p)
{
    m_pIRichEdit = p;
    m_pMgrScrollBar = NULL;
	m_bRedrawing = false;
    m_bPostRedrawMsg = false;
	m_pPromptFont = nullptr;
    SetRectEmpty(&m_rcInvalidate);

    m_bByteLimit = false;
    m_bAutoSize = false;
    m_lSingleLineMaxWidth = NDEF;
    m_lSingleLineMinWidth = NDEF;
    m_lMultiLineMaxHeight = NDEF;
    m_lMultiLineMinHeight = NDEF;

    m_bBubbleMode = false;

	p->SetWindowlessRichEdit(&m_wrapRichEidt);
	p->SetDescription(RichEditDescription::Get());
}

RichEdit::~RichEdit()
{
	SAFE_RELEASE(m_pPromptFont);
    IScrollBarManager::DestroyIScrollBarManager(m_pMgrScrollBar);
}
IRichEdit*  RichEdit::GetIRichEdit() 
{ 
    return m_pIRichEdit; 
}

HRESULT  RichEdit::FinalConstruct(ISkinRes* p)
{
	DO_PARENT_PROCESS(IRichEdit, IControl);
    if (FAILED(GetCurMsg()->lRet))
        return GetCurMsg()->lRet;

    m_pMgrScrollBar = IScrollBarManager::CreateIScrollBarManager();
	m_wrapRichEidt.BindControl(this);
    m_pMgrScrollBar->SetBindObject(m_pIRichEdit);
    m_pMgrScrollBar->SetScrollBarVisibleType(HSCROLLBAR, SCROLLBAR_VISIBLE_NONE);
    m_pMgrScrollBar->SetScrollBarVisibleType(VSCROLLBAR, SCROLLBAR_VISIBLE_AUTO);

    OBJSTYLE s = {0};
    s.enable_ime = 1;
    m_pIRichEdit->ModifyObjectStyle(&s, 0);

	return S_OK;
}

WindowlessRichEdit& RichEdit::GetRichEdit()
{
    return m_wrapRichEidt;
}
IScrollBarManager*  RichEdit::GetScrollMgr()
{
    return m_pMgrScrollBar;
}
CCaret*  RichEdit::GetCaret()
{
    return &m_caret;
}

void  RichEdit::OnSerialize(SERIALIZEDATA* pData)
{
	DO_PARENT_PROCESS(IRichEdit, IControl);
    IUIApplication*  pUIApplication = m_pIRichEdit->GetUIApplication();

    {
        AttributeSerializerWrap as(pData, TEXT("richedit"));
        as.AddBool(XML_RICHEDIT_MSG_BUBBLE_PREFIX XML_RICHEDIT_MSG_BUBBLE_ENABLE, this,
            memfun_cast<pfnBoolSetter>(&RichEdit::EnableBubbleMode),
            memfun_cast<pfnBoolGetter>(&RichEdit::IsBubbleMode))
            ->ReloadOnChanged();

		as.AddI18nString(XML_RICHEDIT_PROMPT_TEXT,
			[this](LPCTSTR text, int size){ 
				this->m_strPrompt.assign(text, size); },
			[this]()->LPCTSTR{ return this->m_strPrompt.c_str(); });
		as.AddTextRenderBase(
			XML_RICHEDIT_PROMPT_FONT_PREFIX, m_pIRichEdit, m_pPromptFont);

    }

    if (IsBubbleMode())
    {
        AttributeSerializerWrap as(pData, TEXT("richedit"));
        as.AddString(
            XML_RICHEDIT_MSG_BUBBLE_PREFIX 
            XML_RICHEDIT_MSG_BUBBLE_LEFT_PREFIX 
            XML_RICHEDIT_MSG_BUBBLE_IMAGE,
            this,
            memfun_cast<pfnStringSetter>(&RichEdit::set_bubble_left_image),
            memfun_cast<pfnStringGetter>(&RichEdit::get_bubble_left_image));

        as.AddString(
            XML_RICHEDIT_MSG_BUBBLE_PREFIX 
            XML_RICHEDIT_MSG_BUBBLE_RIGHT_PREFIX 
            XML_RICHEDIT_MSG_BUBBLE_IMAGE,
            this,
            memfun_cast<pfnStringSetter>(&RichEdit::set_bubble_right_image),
            memfun_cast<pfnStringGetter>(&RichEdit::get_bubble_right_image));
    }

    m_pMgrScrollBar->Serialize(pData);
    m_pMgrScrollBar->SetVScrollWheel(10);
}


void RichEdit::OnInitialize()
{
	// 初始化字体(注：如果先创建textservice,再直接设置字体，
	// 调用OnTxPropertyBitsChange会崩溃
	ITextRenderBase* pText = m_pIRichEdit->GetTextRender();
	IRenderFont* pFont = m_pIRichEdit->GetRenderFont();
	COLORREF color = 0;

	if (pText)
	{
		if (pText->GetType() == TEXTRENDER_TYPE_SIMPLE)
			static_cast<ISimpleTextRender*>(pText)->GetColor(color);
	}
	if (pFont) 
	{
		LOGFONT lf;
		pFont->GetLogFont(&lf);
		m_wrapRichEidt.SetCharFormatByLogFont(&lf, color);
	}

	m_wrapRichEidt.Create(m_pIRichEdit->GetHWND());
	m_wrapRichEidt.ModifyEventMask(ENM_LINK, 0);

    m_vscrollbarCreator.Initialize(m_pIRichEdit, m_pMgrScrollBar);
}

void  RichEdit::OnCreateByEditor(CREATEBYEDITORDATA* pData)
{
    DO_PARENT_PROCESS_MAPID(IRichEdit, IControl, UIALT_CALLLESS);
    m_vscrollbarCreator.CreateByEditor(pData, m_pIRichEdit);
}


void RichEdit::OnEraseBkgnd(IRenderTarget*  pRenderTarget)
{
	//SetMsgHandled(FALSE);
    //DO_PARENT_PROCESS(IRichEdit, IControl);
	IRenderBase* render = m_pIRichEdit->GetBackRender();
	if (!render)
		return;

	CRect rc(0, 0, m_pIRichEdit->GetWidth(), m_pIRichEdit->GetHeight());
	render->DrawState(pRenderTarget, &rc, m_wrapRichEidt.IsFocus() ? 1: 0);
}


void RichEdit::OnPaint(IRenderTarget*  pRenderTarget)
{
	if (!m_strPrompt.empty() && !m_wrapRichEidt.IsFocus() && m_pPromptFont)
	{
		if (0 == m_wrapRichEidt.GetTextLengthW())
		{
			CRect rc;
			m_pIRichEdit->GetObjectClientRect(&rc);
			m_pPromptFont->DrawState(
				pRenderTarget, &rc, 0, m_strPrompt.c_str());
		}
	}

	HDC hDC = pRenderTarget->GetHDC();
	m_wrapRichEidt.Draw(hDC, false/*m_pIRichEdit->TestStyleEx(RICHEDIT_STYLE_TEXTSHADOW)*/);
  
    // m_caret不能放在OnEraseBkgnd中，因为RE在txDraw过程中会去隐藏、显示、设置位置等光标操作
    // 另外有可能RE还有自己的背景(如气泡模式），会覆盖掉光标，因此将光标还是放在OnPaint中，
    // 但需要处理坐标偏移问题(TODO:)

    m_caret.OnControlPaint(m_pIRichEdit, pRenderTarget);

	if (pRenderTarget->IsRenderAlphaChannel())
	{
		Util::FixRenderTargetClipRegionAlpha(pRenderTarget);
	}
}

void  RichEdit::InvalidateRect(RECT* prcWnd)
{
    if (prcWnd)
    {
        //UnionRect(&m_rcInvalidate, &m_rcInvalidate, prcWnd);
		IWindowBase* pWnd = m_pIRichEdit->GetWindowObject();
		if (pWnd)
		{
			pWnd->Invalidate(prcWnd);
		}
    }
    else
    {
        // 注：这里不能只取当前客户区域，应该包括整个控件，场景：
        // 多行编辑框内容超出显示滚动条时，滚动条会挤占控件区域，
        // 导致客户区变小。但之前的老客户区内还有残留文字需要刷新掉
        // m_pIRichEdit->GetWindowRect(&m_rcInvalidate);
		m_pIRichEdit->Invalidate();
    }

	//UIASSERT(0);
//     if (!m_bPostRedrawMsg)
//     {
//         m_bPostRedrawMsg = true;
//         UIMSG msg;
//         msg.message = UI_WM_REDRAWOBJECT;
//         msg.pMsgTo = m_pIRichEdit;
// 
//         // 由于RECT参数在post消息中不好传递，因此将区域保存为成员变量。同时也能起来合并无效区域的作用
//         UIPostMessage(m_pIRichEdit->GetUIApplication(), &msg);  
//     }
}

void  RichEdit::InvalidateRect2(RECT* prcWndArray, int nCount)
{
	IWindowBase* pWnd = m_pIRichEdit->GetWindowObject();
	if (!pWnd)
		return;

    for (int i = 0; i < nCount; i++)
    {
        // UnionRect(&m_rcInvalidate, &m_rcInvalidate, &prcWndArray[i]);
		pWnd->Invalidate(&prcWndArray[i]);
    }

	//UIASSERT(0);
//     if (!m_bPostRedrawMsg)
//     {
//         m_bPostRedrawMsg = true;
//         
//         UIMSG msg;
//         msg.message = UI_WM_REDRAWOBJECT;
//         msg.pMsgTo = m_pIRichEdit;
//         UIPostMessage(m_pIRichEdit->GetUIApplication(), &msg);  
//     }
}

// 由WindowlessRichEdit::TxInvalidateRect post过来的消息，延迟刷新
LRESULT RichEdit::OnRedrawObject(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    m_bPostRedrawMsg = false;  // 收到redraw消息后，允许再次post redraw消息

    if (m_rcInvalidate.IsRectEmpty())
        return 0;

    if (m_caret.GetCaretType()==CARET_TYPE_API)
	    m_caret.HideCaret(m_pIRichEdit);  // 解决系统光标在输入文字时不显示的问题。加上Hide/Show光标即可跟随输入位置

    // 从window坐标转为object坐标
    int nWidth = m_rcInvalidate.Width();
    int nHeight = m_rcInvalidate.Height();

    RECT rcDamage = {0};
    m_pIRichEdit->WindowPoint2ObjectPoint((POINT*)&m_rcInvalidate, (POINT*)&rcDamage, true);
    rcDamage.right = rcDamage.left + nWidth;
    rcDamage.bottom = rcDamage.top + nHeight;

    m_bRedrawing = true;
    {
	    m_pIRichEdit->Invalidate(&rcDamage);   
    }
    m_bRedrawing = false;

    if (m_caret.GetCaretType()==CARET_TYPE_API)
	    m_caret.ShowCaret(m_pIRichEdit);

    m_rcInvalidate.SetRectEmpty();

	return 0;
}

// LRESULT RichEdit::OnMouseRangeMsg(UINT uMsg, WPARAM wParam, LPARAM lParam)
// {
// 	LRESULT lRet = 0;
// 	BOOL bRet = m_wrapRichEidt.ProcessWindowMessage(NULL, uMsg, wParam, lParam, lRet);
// 	if (!bRet)
// 	{
// 		SetMsgHandled(FALSE);
// 		return 0;
// 	}
// 	return lRet;
// }

void RichEdit::OnSetFocus(IObject* pOldFocusObj)
{
	LRESULT lRet = 0;
	BOOL bRet = m_wrapRichEidt.ProcessWindowMessage(NULL, WM_SETFOCUS, NULL, NULL, lRet);
	if (!bRet)
	{
		SetMsgHandled(FALSE);
		return;
	}
}
void RichEdit::OnKillFocus(IObject* pNewFocusObj)
{
	LRESULT lRet = 0;
	BOOL bRet = m_wrapRichEidt.ProcessWindowMessage(NULL, WM_KILLFOCUS, NULL, NULL, lRet);
	if (!bRet)
	{
		SetMsgHandled(FALSE);
	}
}


UINT  RichEdit::OnGetDlgCode(LPMSG lpMsg)
{
	UINT nRet = DLGC_WANTARROWS;
	nRet |= DLGC_WANTCHARS|DLGC_WANTALLKEYS;

// 	if (m_pIRichEdit->GetStyleEx() & EDIT_STYLE_WANTTAB)
// 	{
// 		if (!Util::IsKeyDown(VK_CONTROL))
// 		{
// 			nRet |= DLGC_WANTTAB;
// 		}
// 	}

	return nRet;
}


LRESULT RichEdit::OnForwardMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LRESULT lRet = 0;
	BOOL bHandled = m_wrapRichEidt.ProcessWindowMessage(NULL, uMsg, wParam, lParam, lRet);
	if (!bHandled)
	{
        SetMsgHandled(FALSE);
		return 0;
	}
	return lRet;
}

void RichEdit::OnSize(UINT nType, int cx, int cy)
{
    OnForwardMessage(GetCurMsg()->message, GetCurMsg()->wParam, GetCurMsg()->lParam);

	// 更新滚动条的属性
	CRect rcClient;
	m_pIRichEdit->GetClientRectInObject(&rcClient);

	SIZE sizeContent;
    SIZE sizePage = { rcClient.Width(), rcClient.Height() };

//	rcClient.right = rcClient.left = 0;  // 解决richedit内部，横向滚动条允许滚动到range，而不是range-page的位置
	this->m_pMgrScrollBar->GetScrollRange((int*)&sizeContent.cx, (int*)&sizeContent.cy);
	this->m_pMgrScrollBar->SetScrollPageAndRange(&sizePage, &sizeContent);
}

void RichEdit::OnRButtonDown(UINT nFlags, CPoint point)
{
	SetMsgHandled(FALSE);  // 继续传递给windowless richedit

	if (!m_pIRichEdit->IsFocus())
	{
		m_pIRichEdit->SetFocusInWindow();
	}
}

// 因为richedit会自己负责刷新richedit的内容
LRESULT  RichEdit::OnScroll(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	int nSBCode = LOWORD(wParam);
	int nOldPos = 0;
	if (uMsg==WM_HSCROLL)
		nOldPos = m_pMgrScrollBar->GetHScrollPos();
	else
		nOldPos = m_pMgrScrollBar->GetVScrollPos();

	OnForwardMessage(uMsg, wParam, lParam);

	// 解决当拖拽滑块响应SB_THUMBPOSITION，richedit不会调用txSetScrollPos的问题（只会在SB_ENDSCROLL调用一次）
	if (nSBCode == SB_THUMBTRACK)
	{
        UIMSG msg = *GetCurMsg();
        msg.pMsgTo = m_pMgrScrollBar;
        UISendMessage(&msg);
	}

	// 刷新滚动条
	int nPos = 0;
	if (uMsg==WM_HSCROLL)
		nPos = m_pMgrScrollBar->GetHScrollPos();
	else
		nPos = m_pMgrScrollBar->GetVScrollPos();

	if (nPos != nOldPos)
	{
		IMessage* pMsg = nullptr;
		if (uMsg==WM_HSCROLL)
			pMsg = m_pMgrScrollBar->GetHScrollBar();
		else
			pMsg = m_pMgrScrollBar->GetVScrollBar();

		if (pMsg)
		{
			IControl* pControl = (IControl*)pMsg->QueryInterface(__uuidof(IControl));
			UIASSERT(pControl);
			if (pControl)
			{
				pControl->Invalidate();
			}
		}
	}

	return 0;
}

// 不使用 mgr scroll提供的ui偏移信息，直接使用richedit内部的偏移，richedit已经实现了该功能
LRESULT RichEdit::OnGetScrollOffset(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return 0;
}


//
//	如果这里不处理的话，windowless richeidt将在收到
//  kill focus时销毁光标，但此时背景已被刷新，再调用DestroyCaret时，反而又会刷上
//  一个光标的形状，导致光标残留。
//
// void RichEdit::OnShowWindow(BOOL bShow, UINT nStatus)
// {
// }
void RichEdit::OnVisibleChanged(BOOL bVisible, IObject* pParent)
{
    SetMsgHandled(FALSE);

    if (m_wrapRichEidt.IsFocus()) 
    {
        // 注：这里不能直接调用m_wrapRichEidt.KillFocus，否则edit派生类将得不到
        // killfocus通知，导致一些逻辑不正确。
		// 有OnObjHideInd来保证，这里不再处理。
    //    UISendMessage(m_pIRichEdit, WM_KILLFOCUS);
    }

	// OnShowWindow(bVisible, 0);
}

int  GetTextBytes(LPCTSTR szText);
int  GetCharBytes(TCHAR c);
void  RichEdit::OnEnChanged(BOOL& bHandled)
{

}

void  RichEdit::OnContextMenu(
        WORD seltype, 
        LPOLEOBJECT lpoleobj, 
        CHARRANGE* lpchrg)
{

    {
        ENCONTEXTMENUDATA  data = {seltype, lpoleobj, lpchrg};
        UIMSG  msg;
        msg.message = UI_MSG_NOTIFY;
        msg.nCode = UI_REN_PRE_CONTEXTMENU;
        msg.wParam = (WPARAM)&data;
        msg.pMsgFrom = m_pIRichEdit;
        if (HANDLED == m_pIRichEdit->DoNotify(&msg))
            return;
    }

    UI::LoadMenuData data = {m_pIRichEdit->GetSkinRes()};
    IMenu* pMenu = UILoadMenu(&data);
    if (!pMenu)
        return;
	IListItemBase* pCutItem   = nullptr; 
	IListItemBase* pCopyItem  = nullptr;
	IListItemBase* pPasteItem = nullptr;
	IListItemBase* pSelAllItem = nullptr;

	if (m_wrapRichEidt.IsReadOnly())
	{
		pCopyItem  = pMenu->AppendString(_T("复制"), RE_MENU_ID_COPY);
		pSelAllItem = pMenu->AppendString(_T("全选"), RE_MENU_ID_SELALL);
	}
	else
	{
		pCutItem   = pMenu->AppendString(_T("剪切"), RE_MENU_ID_CUT);
		pCopyItem  = pMenu->AppendString(_T("复制"), RE_MENU_ID_COPY);
		pPasteItem = pMenu->AppendString(_T("粘贴"), RE_MENU_ID_PASTE);
		pMenu->AppendSeparator(-1);
		pSelAllItem = pMenu->AppendString(_T("全选"), RE_MENU_ID_SELALL);
	}

    int nSel = 0, nSelLength = 0;
    m_wrapRichEidt.GetSel(&nSel, &nSelLength);
    if (0 == nSelLength)
    {
		if (pCutItem)
			pCutItem->SetDisable(true);
		if (pCopyItem)
			pCopyItem->SetDisable(true);
    }
    if (pPasteItem &&
		!::IsClipboardFormatAvailable(CF_TEXT) &&
        !::IsClipboardFormatAvailable(CF_UNICODETEXT))
    {
		pPasteItem->SetDisable(true);
    }
    if (pSelAllItem && 0 == m_wrapRichEidt.GetTextLengthW())
    {
		pSelAllItem->SetDisable(true);
    }

    // 给外部再一个机制处理
    {
        ENCONTEXTMENUDATA  data = {seltype, lpoleobj, lpchrg};
        UIMSG  msg;
        msg.message = UI_MSG_NOTIFY;
        msg.nCode = UI_REN_POST_CONTEXTMENU;
        msg.wParam = (WPARAM)&data;
        msg.lParam = (LPARAM)pMenu;
        msg.pMsgFrom = m_pIRichEdit;
        m_pIRichEdit->DoNotify(&msg);
    }

    CPoint pt;
    ::GetCursorPos(&pt);
    UINT nRetCmd = pMenu->TrackPopupMenu(TPM_RETURNCMD, pt.x, pt.y, m_pIRichEdit);
    UIDestroyMenu(pMenu);

    switch (nRetCmd)
    {
    case RE_MENU_ID_CUT:
        {
            m_wrapRichEidt.RESendMessage(WM_CUT, 0, 0);
        }
        break;

    case RE_MENU_ID_COPY:
        {
            m_wrapRichEidt.RESendMessage(WM_COPY);
        }
        break;

    case RE_MENU_ID_PASTE:
        {
            m_wrapRichEidt.RESendMessage(WM_PASTE);
        }
        break;

    case RE_MENU_ID_SELALL:
        {
            m_wrapRichEidt.SelectAll();
        }
        break;

        // 给外部处理
    default:
        {
            UIMSG  msg;
            msg.message = UI_MSG_NOTIFY;
            msg.nCode = UI_REN_HANDLE_CONTEXTMENU_CMD;
            msg.wParam = (WPARAM)nRetCmd;
            msg.pMsgFrom = m_pIRichEdit;
            m_pIRichEdit->DoNotify(&msg);
        }
        break;
    }
}

HRESULT  RichEdit::OnTxNotify(DWORD iNotify, void* pv, BOOL& bHandled)
{
    switch (iNotify)
    {
    case EN_REQUESTRESIZE:
        {
            REQRESIZE* pData = (REQRESIZE*)pv;
            OnRequestAutoSize(&pData->rc);
        }
        break;

    case EN_CHANGE:
        {
            OnEnChanged(bHandled);
            return 0;
        }
        break;

	case EN_LINK:
		{
			ENLINK *pLink = (ENLINK *)pv;
			OnEnLink(pLink);
		}
		return 0;

    default:
        {
            UIMSG  msg;
            msg.message = UI_WM_RICHEDIT_TXNOTIFY;
            msg.wParam = iNotify;
            msg.lParam = (LPARAM)pv;
            
            long lRet = m_pIRichEdit->DoNotify(&msg);
            bHandled = msg.bHandled;
            if (msg.bHandled)
                return lRet;
        }
        break;
    }

    return S_OK;
}

void  RichEdit::OnEnLink(ENLINK *pLink)
{
	EnLink.emit(m_pIRichEdit, pLink);
}

// TODO: 需要验证是否正确刷新
long  RichEdit::OnRequestAutoSize(RECT* prcRequest)
{
    int nDesiredWidth = prcRequest->right - prcRequest->left;
    int nDesiredHeight = prcRequest->bottom - prcRequest->top;

    CRegion4 rPadding;
    m_pIRichEdit->GetPaddingRegion(&rPadding);
    nDesiredWidth += rPadding.left + rPadding.right;
    nDesiredHeight += rPadding.top + rPadding.bottom;

    CRect rcWnd;
    m_pIRichEdit->GetParentRect(&rcWnd);

    bool bMultiLine = m_wrapRichEidt.IsMultiLine();
    if (bMultiLine)  // 宽度固定，高度自适应
    {
        if (m_lMultiLineMaxHeight > 0 && nDesiredHeight > m_lMultiLineMaxHeight)
            nDesiredHeight = m_lMultiLineMaxHeight;
        if (m_lMultiLineMinHeight > 0 && nDesiredHeight < m_lMultiLineMinHeight)
            nDesiredHeight = m_lMultiLineMinHeight;
    }
    else  // 高度固定，宽度自适应
    {
        if (m_lSingleLineMaxWidth > 0 && nDesiredWidth > m_lSingleLineMaxWidth)
            nDesiredWidth = m_lSingleLineMaxWidth;
        if (m_lSingleLineMinWidth > 0 && nDesiredWidth < m_lSingleLineMinWidth)
            nDesiredWidth = m_lSingleLineMinWidth;
    }

    // 通知外部处理一次
    {
        UIMSG  msg;
        msg.message = UI_MSG_NOTIFY;
        msg.nCode = UI_EN_REQUESTAUTOSIZE;
        msg.pMsgFrom = m_pIRichEdit;
        msg.wParam = (WPARAM)prcRequest;
        msg.lParam = (LPARAM)(bMultiLine ? &nDesiredHeight : &nDesiredWidth);
        if (1 == m_pIRichEdit->DoNotify(&msg))
            return 1;
    }

    HWND hWnd = m_pIRichEdit->GetHWND();
    if (bMultiLine)
    {
        if (nDesiredHeight == rcWnd.Height())
            return 0;

        //InvalidateRect(hWnd, &rcWnd, TRUE);
        rcWnd.bottom = rcWnd.top + nDesiredHeight;
    }
    else
    {
        if (nDesiredWidth == rcWnd.Width())
            return 0;

        //InvalidateRect(hWnd, &rcWnd, TRUE);
        rcWnd.right = rcWnd.left + nDesiredWidth;
    }

    m_pIRichEdit->SetObjectPos(&rcWnd, 0/*SWP_NOREDRAW*/);
    //InvalidateRect(hWnd, &rcWnd, TRUE);

    // 通知外部做进一步的处理
    {
        UIMSG  msg;
        msg.message = UI_MSG_NOTIFY;
        msg.nCode = UI_EN_AUTOSIZE_DONE;
        msg.pMsgFrom = m_pIRichEdit;
        m_pIRichEdit->DoNotify(&msg);
    }
    return 1;
}

// 代码修改自mfc源码：CFontDialog dlgfnt.cpp
void FillInLogFont(LOGFONT& m_lf, const CHARFORMAT& cf)
{
    BOOL bIsFormat2 = (cf.cbSize >= sizeof(CHARFORMAT2));

    if (cf.dwMask & CFM_SIZE)
    {
        HDC dc = CreateDC(_T("DISPLAY"), NULL, NULL, NULL);
        LONG yPerInch = ::GetDeviceCaps(dc, LOGPIXELSY);
        m_lf.lfHeight = UI::_round(-((cf.yHeight * yPerInch) / 1440.f));  // <-- 直接转成(int)会导致大小不一致，如11.6,应该转成12，而不是11
        DeleteDC(dc);
    }
    else
    {
        m_lf.lfHeight = 0;
    }

    m_lf.lfWidth = 0;
    m_lf.lfEscapement = 0;
    m_lf.lfOrientation = 0;

    if ((cf.dwMask & (CFM_ITALIC|CFM_BOLD)) == (CFM_ITALIC|CFM_BOLD))
    {
        m_lf.lfWeight = (cf.dwEffects & CFE_BOLD) ? FW_BOLD : FW_NORMAL;
        m_lf.lfItalic = (BYTE)((cf.dwEffects & CFE_ITALIC) ? TRUE : FALSE);
    }
    else
    {
        m_lf.lfWeight = FW_DONTCARE;
        m_lf.lfItalic = FALSE;
    }

    if ((cf.dwMask & (CFM_UNDERLINE|CFM_STRIKEOUT|CFM_COLOR)) ==
        (CFM_UNDERLINE|CFM_STRIKEOUT|CFM_COLOR))
    {
        m_lf.lfUnderline = (BYTE)((cf.dwEffects & CFE_UNDERLINE) ? TRUE : FALSE);
        m_lf.lfStrikeOut = (BYTE)((cf.dwEffects & CFE_STRIKEOUT) ? TRUE : FALSE);
    }
    else
    {
        m_lf.lfUnderline = (BYTE)FALSE;
        m_lf.lfStrikeOut = (BYTE)FALSE;
    }

    m_lf.lfCharSet = DEFAULT_CHARSET;  // <-- 一些特殊字符使用ANISI_CHARSET绘制会显示不出来
    if (cf.dwMask & CFM_CHARSET)
        m_lf.lfCharSet = cf.bCharSet;

    m_lf.lfOutPrecision = OUT_DEFAULT_PRECIS;
    m_lf.lfClipPrecision = CLIP_DEFAULT_PRECIS;
    m_lf.lfQuality = DEFAULT_QUALITY;

    if (bIsFormat2)
    {
        const CHARFORMAT2* pCF2 = (const CHARFORMAT2*) &cf;
        if (pCF2->dwMask & CFM_WEIGHT)
            m_lf.lfWeight = pCF2->wWeight;

        // CHAFORMAT2 has UNICODE face name, CHARFORMAT doesn't

        if (cf.dwMask & CFM_FACE)
        {
            m_lf.lfPitchAndFamily = pCF2->bPitchAndFamily;
            Checked::tcscpy_s(m_lf.lfFaceName, _countof(m_lf.lfFaceName), (LPTSTR)pCF2->szFaceName);
        }
    }
    else
    {
        m_lf.lfPitchAndFamily = cf.bPitchAndFamily;
        String strFaceName(cf.szFaceName);
        Checked::tcsncpy_s(m_lf.lfFaceName, _countof(m_lf.lfFaceName), strFaceName.c_str(), _TRUNCATE);
    }

    if (!(cf.dwMask & CFM_FACE))
    {
        m_lf.lfPitchAndFamily = DEFAULT_PITCH|FF_DONTCARE;
        m_lf.lfFaceName[0] = (TCHAR)0;
    }
}

void  RichEdit::SetReadOnly(bool fReadOnly)
{
    if (fReadOnly == m_wrapRichEidt.IsReadOnly())
        return;

    m_wrapRichEidt.SetReadOnly(fReadOnly);
    UISendMessage(m_pIRichEdit, UI_MSG_STATECHANGED, OSB_READONLY);
}

void  RichEdit::EnableBubbleMode(bool b)
{
    m_bBubbleMode = b;

    if (m_vscrollbarCreator.m_pVScrollBar)
    {
        m_vscrollbarCreator.m_pVScrollBar->SetKeepHoldNonClientRegion(m_bBubbleMode);
    }
    m_wrapRichEidt.EnableBubbleMode(b);
}
bool  RichEdit::IsBubbleMode()
{
    return m_bBubbleMode;
}

void  RichEdit::set_bubble_left_image(LPCTSTR szText)
{
    if (!m_wrapRichEidt.m_pBubbleMgr)
        return;

    ISkinRes*  pSkinRes = m_pIRichEdit->GetUIApplication()->GetDefaultSkinRes();
    IImageResItem* pItem = pSkinRes->GetImageRes().GetImageResItem(szText);
    
    m_wrapRichEidt.m_pBubbleMgr->SetLeftImageResItem(pItem);

}
void  RichEdit::set_bubble_right_image(LPCTSTR szText)
{
    if (!m_wrapRichEidt.m_pBubbleMgr)
        return;

    ISkinRes*  pSkinRes = m_pIRichEdit->GetUIApplication()->GetDefaultSkinRes();
    IImageResItem* pItem = pSkinRes->GetImageRes().GetImageResItem(szText);

    m_wrapRichEidt.m_pBubbleMgr->SetRightImageResItem(pItem);

}

LPCTSTR  RichEdit::get_bubble_left_image()
{
    if (!m_wrapRichEidt.m_pBubbleMgr)
        return NULL;

    IImageResItem* pItem = m_wrapRichEidt.m_pBubbleMgr->GetLeftImageResItem();
    if (!pItem)
        return NULL;

    return pItem->GetId();
}

LPCTSTR  RichEdit::get_bubble_right_image()
{
    if (!m_wrapRichEidt.m_pBubbleMgr)
        return NULL;

    IImageResItem* pItem = m_wrapRichEidt.m_pBubbleMgr->GetRightImageResItem();
    if (!pItem)
        return NULL;

    return pItem->GetId();
}


// 因为一个窗口中可能共存有多个re，所在不再适合由re自己来注册窗口IDropTarget
// 统一走uisdk中的window dragdrop mgr，然后将消息转发给re
void  RichEdit::RegisterDropEvent()
{
    OBJSTYLE s = {0};
    s.receive_dragdrop_event = 1;
    m_pIRichEdit->ModifyObjectStyle(&s, NULL);

    IWindowBase* pWindow = m_pIRichEdit->GetWindowObject();
    if (pWindow)
        pWindow->SetDroppable(true);
}
void  RichEdit::RevokeDropEvent()
{
    OBJSTYLE s = {0};
    s.receive_dragdrop_event = 1;
    m_pIRichEdit->ModifyObjectStyle(NULL, &s);
}

LRESULT UI::RichEdit::OnKeyDown(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (Keydown.emit(m_pIRichEdit, wParam))
	{
		return 0;
	}

	return OnForwardMessage(uMsg, wParam, lParam);	
}
