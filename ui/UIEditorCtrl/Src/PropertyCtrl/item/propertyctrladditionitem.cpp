#include "stdafx.h"
#include "PropertyCtrlAdditionItem.h"
#include "UISDK\Inc\Util\util.h"
#include "UISDK\Inc\Interface\ilayout.h"
#include "UISDK\Inc\Interface\ipanel.h"

namespace UI
{
PropertyCtrlAdditionItem::PropertyCtrlAdditionItem(
    IPropertyCtrlAdditionItem* p) : PropertyCtrlItemBase(p)
{
    m_pIPropertyCtrlAdditionItem = p;
    m_pBaseShareData = NULL;
}
PropertyCtrlAdditionItem::~PropertyCtrlAdditionItem()
{
    // 被删除时可能控件是失焦状态，自己的选中状态也被清除了
    if (IsEditActive())
    {
        m_pBaseShareData->pEditCtrl->SetVisible(false);
        m_pBaseShareData->pEditCtrl->SetNotify(NULL, 0);
        m_pBaseShareData->pEditCtrl->KeyDownEvent().disconnect();
        m_pBaseShareData->pEditCtrl->RemoveMeInTheTree();
    }
}

void  PropertyCtrlAdditionItem::OnInitialize()
{
    DO_PARENT_PROCESS(IPropertyCtrlAdditionItem, IPropertyCtrlItemBase);

	m_pIPropertyCtrlAdditionItem->SetItemType(IPropertyCtrlAdditionItem::FLAG);
}


void  PropertyCtrlAdditionItem::OnPaint(IRenderTarget* pRenderTarget)
{
	DO_PARENT_PROCESS(IPropertyCtrlAdditionItem, IListItemBase);
	IListCtrlBase*  pListCtrl = m_pIPropertyCtrlAdditionItem->GetIListCtrlBase();

	CRect rcParent;
	m_pIPropertyCtrlAdditionItem->GetParentRect(&rcParent);

	CRect rcBlank(rcParent);
	rcBlank.right = 0;//pListCtrl->GetChildNodeIndent();

	CRect rcLeft(rcParent);
	rcLeft.left = rcBlank.right;
	rcLeft.right = m_pBaseShareData->GetSplitterLinePos();

	// 左侧背景
	Color c(PROPERTYCTRL_LINE_COLOR);
	pRenderTarget->DrawRect(&rcBlank, &c);

	// 提示文本
	DRAWTEXTPARAM param;
	param.color = 0xFF808080;
	param.nFormatFlag = DT_LEFT|DT_VCENTER|DT_END_ELLIPSIS|DT_SINGLELINE;
	rcLeft.left += TEXT_INDENT;
	param.prc = &rcLeft;
	param.szText = TEXT("Add new item ...");
	pRenderTarget->DrawString(m_pBaseShareData->m_pRenderFont, &param);

	// 中间的分隔线
	IRenderPen* pPen = pRenderTarget->CreateSolidPen(1, &c);
	pRenderTarget->DrawLine(rcLeft.right, rcLeft.top, rcLeft.right, rcLeft.bottom, pPen);

	// 一行的横线
	pRenderTarget->DrawLine(rcParent.left, rcParent.bottom-1, rcParent.right, rcParent.bottom-1, pPen);
	SAFE_RELEASE(pPen);
}

void  PropertyCtrlAdditionItem::OnSize(UINT nType, int cx, int cy)
{
    if (m_pIPropertyCtrlAdditionItem->IsSelected() && m_pBaseShareData->pEditCtrl->IsSelfVisible())
    {
        ICanvasLayoutParam* pParam = (ICanvasLayoutParam*)
            m_pBaseShareData->pEditCtrl->GetSafeLayoutParam();
        pParam->SetConfigLeft(m_pBaseShareData->GetSplitterLinePos() + 1);
    }
    SetMsgHandled(FALSE);
}

void  PropertyCtrlAdditionItem::SetValueText(LPCTSTR szText)
{
    if (szText)
        m_strValue = szText;
    else
        m_strValue.clear();

	if (IsEditActive())
	{
		m_pBaseShareData->pEditCtrl->SetText(m_strValue.c_str());
	}
}

void  PropertyCtrlAdditionItem::SetDefaultValueText(LPCTSTR szText)
{
	if (szText)
		m_strDefautValue = szText;
	else
		m_strDefautValue.clear();
}

void  PropertyCtrlAdditionItem::OnStateChanged(UINT nMask)
{
    if (nMask & OSB_SELECTED)
    {
        bool bSelected = m_pIPropertyCtrlAdditionItem->IsSelected();

        if (bSelected)
        {
            BeginEdit();
        }
        else
        {
            if (0 != _tcscmp(m_pBaseShareData->pEditCtrl->GetText(), m_strValue.c_str()))
            {
                AcceptEdit();
            }
            EndEdit();
        }
    }
}
LRESULT  PropertyCtrlAdditionItem::OnSetValue(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LPCTSTR szText = (LPCTSTR )wParam;
	this->SetValueText(szText);

	return 0;
}
LRESULT  PropertyCtrlAdditionItem::OnSetDefaultValue(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LPCTSTR szText = (LPCTSTR )wParam;
	this->SetDefaultValueText(szText);

	return 0;
}

bool  PropertyCtrlAdditionItem::OnEditKeyDown(IEdit*, UINT w)
{
	if (w == VK_ESCAPE)
	{
		m_pBaseShareData->pEditCtrl->SetText(nullptr);
	}
	else if (w == VK_RETURN)
	{
		AcceptEdit();
	}

	return false;
}

void  PropertyCtrlAdditionItem::EndEdit()
{
    if (NULL == m_pBaseShareData->pEditCtrl)
        return;

	if (IsEditActive())
	{
		m_pBaseShareData->pEditCtrl->SetVisible(false);
		m_pBaseShareData->pEditCtrl->SetNotify(NULL, 0);
        m_pBaseShareData->pEditCtrl->KeyDownEvent().disconnect();
		m_pBaseShareData->pEditCtrl->RemoveMeInTheTree();
		m_pBaseShareData->pEditCtrl->SetText(nullptr);
	}
}

void  PropertyCtrlAdditionItem::AcceptEdit()
{
    UIMSG  msg;
	msg.message = UI_MSG_NOTIFY;
    msg.nCode = UI_PROPERTYCTRL_NM_ADDITIONITEM_ACCEPTCONTENT;
    msg.wParam = (WPARAM)m_pIPropertyCtrlAdditionItem;
    msg.lParam= (LPARAM)m_pBaseShareData->pEditCtrl->GetText();

    ACCEPT_EDIT_RESULT eRes = (ACCEPT_EDIT_RESULT)
			m_pIPropertyCtrlAdditionItem->GetIListCtrlBase()->DoNotify(&msg);
	
	switch (eRes)
	{
	case ACCEPT_EDIT_ACCEPT:
	case ACCEPT_EDIT_REJECT:
		if (IsEditActive())
		{
			EndEdit();
		}
		break;

	case ACCEPT_EDIT_REJECT_AND_CONTINUE_EDIT:
		{
			if (IsEditActive())
			{
				m_pBaseShareData->pEditCtrl->SetFocusInWindow();
			}
		}
		break;
	}

	m_pIPropertyCtrlAdditionItem->GetIListCtrlBase()
		->InvalidateItem(m_pIPropertyCtrlAdditionItem);
}

void  PropertyCtrlAdditionItem::BeginEdit()
{
    if (NULL == m_pBaseShareData->pEditCtrl)
        return;

    CRect rc;
    GetKeyColoumnRectInItem(&rc);
    rc.DeflateRect(TEXT_INDENT,1,1,1);

    IPanel* pPanel = m_pIPropertyCtrlAdditionItem->GetRootPanel();
    pPanel->AddChild(m_pBaseShareData->pEditCtrl);

    ICanvasLayoutParam* pParam = (ICanvasLayoutParam*)
        m_pBaseShareData->pEditCtrl->GetSafeLayoutParam();

    pParam->SetConfigLeft(rc.left);
    pParam->SetConfigRight(NDEF);
    pParam->SetConfigWidth(rc.Width());
    pParam->SetConfigTop(1);
    pParam->SetConfigBottom(1);

	m_pBaseShareData->pEditCtrl->SetText(NULL);
    m_pBaseShareData->pEditCtrl->SetVisible(true);
    m_pBaseShareData->pEditCtrl->SetNotify(m_pIPropertyCtrlAdditionItem, 0);
    m_pBaseShareData->pEditCtrl->KeyDownEvent().connect(
        this, &PropertyCtrlAdditionItem::OnEditKeyDown);
    pPanel->GetLayout()->Arrange(m_pBaseShareData->pEditCtrl);

    // 设置Edit焦点，立即启动编辑模式
    IListCtrlBase* pCtrl = m_pIPropertyCtrlAdditionItem->GetIListCtrlBase();
    pCtrl->SetFocusObject(m_pBaseShareData->pEditCtrl);

    // 更新鼠标样式，有可能鼠标现在位于edit上
    // 然后更新EDIT中的光标位置为鼠标下的位置
    HWND  hWnd = pCtrl->GetHWND(); 
    POINT pt;
    GetCursorPos(&pt);
    MapWindowPoints(NULL, hWnd, &pt, 1);

    // 此时鼠标还没弹起来，最后不发送up消息。否则不能直接拖选文字了
    if (Util::IsKeyDown(VK_LBUTTON))
    {
        SendMessage(hWnd, WM_LBUTTONUP, 0, MAKELPARAM(pt.x, pt.y));
        SendMessage(hWnd, WM_MOUSEMOVE, 0, MAKELPARAM(pt.x, pt.y));
        SendMessage(hWnd, WM_LBUTTONDOWN, 0, MAKELPARAM(pt.x, pt.y));
    }

    if (m_pBaseShareData->pEditCtrl->IsHover())
        SetCursor(LoadCursor(NULL, IDC_IBEAM));
}

}