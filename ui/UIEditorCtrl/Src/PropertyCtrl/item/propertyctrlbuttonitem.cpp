#include "stdafx.h"
#include "propertyctrlbuttonitem.h"
#include "UISDK\Inc\Interface\ipanel.h"
#include "UISDK\Inc\Interface\ilayout.h"

namespace UI
{

PropertyCtrlButtonItem::PropertyCtrlButtonItem(IPropertyCtrlButtonItem* p) :
PropertyCtrlItemBase(p)
{
    m_pIPropertyCtrlButtonItem = p;
    m_pBaseShareData = NULL;
}
PropertyCtrlButtonItem::~PropertyCtrlButtonItem()
{
    // 被删除时可能控件是失焦状态，自己的选中状态也被清除了
    // if (m_pIPropertyCtrlButtonItem->IsSelected())

    IObject* pObj = m_pBaseShareData->pButtonCtrl->GetParentObject();
    if (pObj && pObj == m_pIPropertyCtrlButtonItem->GetRootPanel())
    {
        m_pBaseShareData->pButtonCtrl->SetVisible(false);
        m_pBaseShareData->pButtonCtrl->SetNotify(NULL, 0);
        m_pBaseShareData->pButtonCtrl->RemoveMeInTheTree();
    }

	pObj = m_pBaseShareData->pEditCtrl->GetParentObject();
	if (pObj && pObj == m_pIPropertyCtrlButtonItem->GetRootPanel())
	{
		m_pBaseShareData->pEditCtrl->SetVisible(false);
		m_pBaseShareData->pEditCtrl->SetNotify(NULL, 0);
		m_pBaseShareData->pEditCtrl->RemoveMeInTheTree();
	}
}

void  PropertyCtrlButtonItem::OnInitialize()
{
    DO_PARENT_PROCESS(IPropertyCtrlButtonItem, IPropertyCtrlItemBase);
	m_pIPropertyCtrlButtonItem->SetItemType(IPropertyCtrlButtonItem::FLAG);
}


void  PropertyCtrlButtonItem::OnPaint(IRenderTarget* pRenderTarget)
{
    CRect rcValue;
    GetValueColumnRectInCtrl(&rcValue);

    bool bSelectd = m_pIPropertyCtrlButtonItem->IsSelected();
    if (!bSelectd)
    {
        DRAWTEXTPARAM param;
        param.color = 0;
        param.nFormatFlag = DT_LEFT|DT_VCENTER|DT_END_ELLIPSIS|DT_SINGLELINE;
        rcValue.left += TEXT_INDENT;
        param.prc = &rcValue;
        param.szText = m_strValue.c_str();
        pRenderTarget->DrawString(m_pBaseShareData->m_pRenderFont, &param);
    }
}

void  PropertyCtrlButtonItem::OnSize(UINT nType, int cx, int cy)
{
    if (m_pIPropertyCtrlButtonItem->IsSelected() && m_pBaseShareData->pEditCtrl->IsSelfVisible())
    {
        ICanvasLayoutParam* pParam = (ICanvasLayoutParam*)
            m_pBaseShareData->pEditCtrl->GetSafeLayoutParam();

        pParam->SetConfigLeft(m_pBaseShareData->GetSplitterLinePos() + 1);
    }
    SetMsgHandled(FALSE);
}

void  PropertyCtrlButtonItem::SetValueText(const TCHAR* szText)
{
    if (szText)
        m_strValue = szText;
    else
        m_strValue.clear();

//     if (m_pIPropertyCtrlButtonItem->IsSelected() && m_pBaseShareData->pEditCtrl->IsSelfVisible())
//     {
//         m_pBaseShareData->pEditCtrl->SetText(m_strValue.c_str());
//     }
}

void  PropertyCtrlButtonItem::SetDefaultValueText(const TCHAR* szText)
{
	if (szText)
		m_strDefautValue = szText;
	else
		m_strDefautValue.clear();
}


void  PropertyCtrlButtonItem::OnStateChanged(UINT nMask)
{
    if (nMask & OSB_SELECTED)
    {
        bool bSelected = m_pIPropertyCtrlButtonItem->IsSelected();

        if (bSelected)
        {
            BeginEdit();
        }
        else
        {
            if (0 != _tcscmp(m_pBaseShareData->pEditCtrl->GetText(), m_strValue.c_str()))
            {
            //    OnEditReturn(0,0);
            }
            CancelEdit();
        }
    }
}
LRESULT  PropertyCtrlButtonItem::OnSetValue(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	const TCHAR* szText = (const TCHAR* )wParam;
	this->SetValueText(szText);

	return 0;
}
LRESULT  PropertyCtrlButtonItem::OnSetDefaultValue(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	const TCHAR* szText = (const TCHAR* )wParam;
	this->SetDefaultValueText(szText);

	return 0;
}

void  PropertyCtrlButtonItem::CancelEdit()
{
    if (m_pBaseShareData->pButtonCtrl)
	{
		m_pBaseShareData->pButtonCtrl->SetVisible(false);
		m_pBaseShareData->pButtonCtrl->SetNotify(NULL, 0);
		m_pBaseShareData->pButtonCtrl->RemoveMeInTheTree();
	}
	if (m_pBaseShareData->pEditCtrl)
	{
		m_pBaseShareData->pEditCtrl->SetVisible(false);
		m_pBaseShareData->pEditCtrl->SetNotify(NULL, 0);
		m_pBaseShareData->pEditCtrl->RemoveMeInTheTree();
		m_pBaseShareData->pEditCtrl->SetText(NULL);
	}
}
void  PropertyCtrlButtonItem::BeginEdit()
{
    CRect rc;
    GetValueColumnRectInItem(&rc);
    rc.DeflateRect(2,1,1,1);

	IPanel* pPanel = m_pIPropertyCtrlButtonItem->GetRootPanel();

	if (m_pBaseShareData->pButtonCtrl)
	{
		pPanel->AddChild(m_pBaseShareData->pButtonCtrl);

        ICanvasLayoutParam* pParam = (ICanvasLayoutParam*)
            m_pBaseShareData->pButtonCtrl->GetSafeLayoutParam();

		pParam->SetConfigRight(1);
		pParam->SetConfigTop(1);
		pParam->SetConfigBottom(1);
		pParam->SetConfigWidth(20);

		m_pBaseShareData->pButtonCtrl->SetVisible(true);
		m_pBaseShareData->pButtonCtrl->SetNotify(m_pIPropertyCtrlButtonItem, 0);
	}
	
	if (m_pBaseShareData->pEditCtrl)
	{
		m_pBaseShareData->pEditCtrl->SetText(m_strValue.c_str());
		pPanel->AddChild(m_pBaseShareData->pEditCtrl);

        ICanvasLayoutParam* pParam = (ICanvasLayoutParam*)
            m_pBaseShareData->pEditCtrl->GetSafeLayoutParam();

		pParam->SetConfigLeft(rc.left);
		pParam->SetConfigRight(1);
		pParam->SetConfigTop(1);
		pParam->SetConfigBottom(1);

		m_pBaseShareData->pEditCtrl->SetVisible(true);
		m_pBaseShareData->pEditCtrl->SetNotify(m_pIPropertyCtrlButtonItem, 0);
	}

    pPanel->GetLayout()->Arrange(NULL);

    // 设置Edit焦点，立即启动编辑模式
    IListCtrlBase* pCtrl = m_pIPropertyCtrlButtonItem->GetIListCtrlBase();
    pCtrl->SetFocusObject(m_pBaseShareData->pButtonCtrl);

    // 更新鼠标样式，有可能鼠标现在位于edit上
    // 然后更新EDIT中的光标位置为鼠标下的位置
    HWND  hWnd = pCtrl->GetHWND(); 
    POINT pt;
    GetCursorPos(&pt);
    MapWindowPoints(NULL, hWnd, &pt, 1);
    PostMessage(hWnd, WM_MOUSEMOVE, 0, MAKELPARAM(pt.x, pt.y));
    PostMessage(hWnd, WM_LBUTTONDOWN, 0, MAKELPARAM(pt.x, pt.y));
    PostMessage(hWnd, WM_LBUTTONUP, 0, MAKELPARAM(pt.x, pt.y));
}

}