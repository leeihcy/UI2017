#include "stdafx.h"
#include "instantedit.h"
#include "edit_desc.h"
#include "..\UISDK\Inc\Util\util.h"

InstantEdit::InstantEdit(IInstantEdit* p) : Edit(p)
{
    m_pIInstantEdit = p;
    m_bEditing = false;
	m_pColorQuiet = NULL;
	p->SetDescription(InstantEditDescription::Get());
}
InstantEdit::~InstantEdit()
{
	SAFE_RELEASE(m_pColorQuiet);
}

void  InstantEdit::FinalRelease()
{
	DO_PARENT_PROCESS(IInstantEdit, IEdit);
	if (m_bEditing)
	{
		IMessageFilterMgr* pMgr = m_pIEdit->GetUIApplication()->GetMessageFilterMgr();
		if (pMgr)
			pMgr->RemoveMessageFilter(static_cast<IPreTranslateMessage*>(this));
	}
}

void  InstantEdit::OnSerialize(SERIALIZEDATA* pData)
{
	__super::OnSerialize(pData);

	AttributeSerializerWrap as(pData, TEXT("InstantEdit"));
	as.AddColor(XML_INSTANTEDIT_COLOR, m_pColorQuiet);
}

LRESULT  InstantEdit::OnLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	SetMsgHandled(FALSE);
	if (!IsRealFocus())
	{
		if (StartEdit())
			m_pIInstantEdit->Invalidate();
	}
	return 0;
}

LRESULT  InstantEdit::OnRButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	SetMsgHandled(FALSE);
	if (!IsRealFocus())
	{
		if (StartEdit())
			m_pIInstantEdit->Invalidate();
	}
    return 0;
}
LRESULT  InstantEdit::OnSetCursor(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if (m_bEditing)
    {
        SetMsgHandled(FALSE);
        return 0;
    }

    SetCursor(LoadCursor(NULL, IDC_ARROW));
    return 0;
}

void  InstantEdit::OnSetFocus(HWND wndFocus)
{
	// 覆盖基类的实现
	;
}

void  InstantEdit::OnKillFocus(HWND wndFocus)
{
	// 覆盖基类的实现
	if (m_bEditing)
	{
		acceptEdit();
		m_pIInstantEdit->Invalidate();
	}
}

void  InstantEdit::OnPaint(IRenderTarget* pRenderTarget)
{
    if (m_bEditing)
    {
        SetMsgHandled(FALSE);
        return;
    }

	CRect rcClient;
	m_pIEdit->GetObjectClientRect(&rcClient);

	// 文字
	IRenderFont* pFont = m_pIInstantEdit->GetRenderFont();
	if (!pFont)
		return;

	HFONT  hFont = pFont->GetHFONT();
	HDC    hDC = pRenderTarget->GetHDC();
	HFONT  hOldFont = (HFONT)SelectObject(hDC, hFont);

	Color* pColor = m_pColorQuiet;
	if (!m_pIInstantEdit->IsEnable())
	{
		// TODO:
	}
	else if (IsReadOnly())
	{
		// TODO:
	}
	if (!pColor)
		pColor = m_pColor;

	COLORREF colorText = 0x00000000;
	if (pColor)
		colorText = pColor->GetGDIValue();
	
	COLORREF oldTextCol = ::SetTextColor(hDC, colorText);

	long  lTextHeight = GetTextHeight();
	CRect rcText = rcClient;
	rcText.top = rcClient.top
		+ (rcClient.Height() - lTextHeight) / 2;
	rcText.bottom = rcText.top + lTextHeight;

    long drawType = DT_LEFT | DT_END_ELLIPSIS | DT_NOPREFIX | DT_SINGLELINE;
    if (m_textAlign == EDIT_TEXT_ALIGN_CENTER)
        drawType |= DT_CENTER;

	DrawText(hDC, GetText(), -1, rcText, drawType);

	::SetTextColor(hDC, oldTextCol);
	::SelectObject(hDC, hOldFont);

	Util::FixRenderTargetClipRegionAlpha(pRenderTarget);

#if 0
    if (!m_pIEdit->GetTextRender())
        return;

    // edit内部是调用ExtTextOut绘制文本的，只能采用top align绘制
    CRect rcDraw;
    m_pIEdit->GetObjectClientRect(&rcDraw);

    int nTextHeight = __super::GetTextHeight();
    rcDraw.top = rcDraw.top + (rcDraw.Height()-nTextHeight)/2;    
    rcDraw.bottom = rcDraw.top + nTextHeight;

    LPCTSTR szText = GetText();
    m_pIEdit->GetTextRender()->DrawState(pRenderTarget, &rcDraw, 
        EDIT_TEXTREDNER_STATE_NORMAL, szText, 
        DT_LEFT|DT_END_ELLIPSIS|DT_NOPREFIX|DT_SINGLELINE);
#endif
}

void  InstantEdit::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (nChar == VK_ESCAPE)
	{
		if (m_bEditing)
		{
			CancelEdit();
			m_pIInstantEdit->Invalidate();
			return;
		}
	}
	else if (nChar == VK_RETURN)
	{
		if (m_bEditing)
		{
			acceptEdit();
			m_pIInstantEdit->Invalidate();
			return;
		}
	}

	SetMsgHandled(FALSE);
	return;
}

bool  InstantEdit::StartEdit()
{
	UIMSG msg;
	msg.message = UI_MSG_NOTIFY;
	msg.nCode = UI_EN_STARTEDIT;
	msg.pMsgFrom = m_pIInstantEdit;

	START_EDIT_RESULT  eResult = (START_EDIT_RESULT)m_pIInstantEdit->DoNotify(&msg);
	if (eResult == START_EDIT_CANCEL)
		return false;

	IMessageFilterMgr* pMgr = m_pIEdit->GetUIApplication()->GetMessageFilterMgr();
	if (pMgr)
		pMgr->AddMessageFilter(static_cast<IPreTranslateMessage*>(this));

	m_strTextBeforeEdit = GetText();
	::SetCursor(::LoadCursor(NULL, IDC_IBEAM));

	EnterFocus();
	m_bEditing = true;
	return true;
}
void  InstantEdit::CancelEdit()
{
	SetText(m_strTextBeforeEdit.c_str());

	UIMSG msg;
	msg.message = UI_MSG_NOTIFY;
	msg.nCode = UI_EN_CANCELEDIT;
	msg.pMsgFrom = m_pIInstantEdit;
	m_pIInstantEdit->DoNotify(&msg);

	endEdit();
}
void  InstantEdit::acceptEdit()
{
	// 内容没有改变时，直接结束，不通知外部
	if (m_strTextBeforeEdit == GetText())
	{
		endEdit();
		return;
	}

	UIMSG msg;
	msg.message = UI_MSG_NOTIFY;
	msg.nCode = UI_EN_ACCEPTEDIT;
	msg.pMsgFrom = m_pIInstantEdit;

	ACCEPT_EDIT_RESULT  eResult = (ACCEPT_EDIT_RESULT)m_pIInstantEdit->DoNotify(&msg);
	if (eResult == ACCEPT_EDIT_REJECT)
	{
		SetText(m_strTextBeforeEdit.c_str());
		endEdit();
	}
	else if (eResult == ACCEPT_EDIT_REJECT_AND_CONTINUE_EDIT)
	{
		m_pIInstantEdit->SetFocusInWindow();
	}
	else
	{
		endEdit();
	}
}	
void  InstantEdit::endEdit()
{
	IMessageFilterMgr* pMgr = m_pIEdit->GetUIApplication()->GetMessageFilterMgr();
	if (pMgr)
		pMgr->RemoveMessageFilter(static_cast<IPreTranslateMessage*>(this));

	m_strTextBeforeEdit.clear();

	UIMSG msg;
	msg.message = UI_MSG_NOTIFY;
	msg.nCode = UI_EN_ENDEDIT;
	msg.pMsgFrom = m_pIInstantEdit;
	m_pIInstantEdit->DoNotify(&msg);
    
    // 与绘制的文本对齐，避免下次点击时，
    // 又显示了上一次的选择状态及位置，造成闪烁
    SetSel(0, 0);

	m_bEditing = false;
	ExitFocus();
	return;
}


BOOL  InstantEdit::PreTranslateMessage(MSG* pMsg)
{
	switch (pMsg->message)
	{
		// 点击其它位置，停止编辑
	case WM_LBUTTONDOWN:
	case WM_LBUTTONDBLCLK:
	case WM_RBUTTONDOWN:
	case WM_RBUTTONDBLCLK:
    case WM_NCLBUTTONDOWN:
    case WM_NCRBUTTONDOWN:
    case WM_NCLBUTTONDBLCLK:
    case WM_NCRBUTTONDBLCLK:
		{
			POINT  ptWnd = {GET_X_LPARAM(pMsg->lParam), GET_Y_LPARAM(pMsg->lParam)};

			if (m_bEditing)
			{
				RECT rcWnd;
				m_pIEdit->GetWindowRect(&rcWnd);
				if (!PtInRect(&rcWnd, ptWnd))
				{
					acceptEdit();
					m_pIInstantEdit->Invalidate();
				}
			}
		}
		break;

	case WM_KILLFOCUS:
		{
			if (pMsg->hwnd == m_pIEdit->GetHWND())
			{
				if (m_bEditing)
				{
					acceptEdit();
					m_pIInstantEdit->Invalidate();
				}
			}
		}
		break;
	}
	return FALSE;
}