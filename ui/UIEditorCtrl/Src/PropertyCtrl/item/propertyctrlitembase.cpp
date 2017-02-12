#include "stdafx.h"
#include "PropertyCtrlItemBase.h"
#include "UICTRL\Inc\Interface\icombobox.h"
#include "UICTRL\Inc\Interface\iedit.h"
#include "UISDK\Inc\interface\ipanel.h"
#include "UISDK\Inc\Util\util.h"

using namespace UI;

PropertyCtrlItemBaseShareData::PropertyCtrlItemBaseShareData(
    IPropertyCtrlItemBaseShareData* p) : MessageProxy(p)
{
    m_pIPropertyCtrlItemBaseShareData = p;
    m_pSlitterLinePos = NULL;
    m_pRenderFont = NULL;
    pEditCtrl = NULL;
    pButtonCtrl = NULL;
    pComboBoxCtrl = NULL;
}

uint  PropertyCtrlItemBaseShareData::GetSplitterLinePos()
{
    if (!m_pSlitterLinePos)
        return 50;

    return *m_pSlitterLinePos;
}
//////////////////////////////////////////////////////////////////////////

PropertyCtrlItemBase::PropertyCtrlItemBase(IPropertyCtrlItemBase* p)
    : MessageProxy(p)
{
    m_pIPropertyCtrlItemBase = p;
    m_pBaseShareData = NULL;
}
PropertyCtrlItemBase::~PropertyCtrlItemBase()
{
}

void  PropertyCtrlItemBase::OnInitialize()
{
    DO_PARENT_PROCESS(IPropertyCtrlItemBase, IListItemBase);

    IListCtrlBase*  pListCtrl = m_pIPropertyCtrlItemBase->GetIListCtrlBase();
    IPropertyCtrlItemBaseShareData*  pData = static_cast<IPropertyCtrlItemBaseShareData*>(
		pListCtrl->GetItemTypeShareData(IPropertyCtrlItemBaseShareData::FLAG));

    if (!pData)
    {
        pData = IPropertyCtrlItemBaseShareData::CreateInstance(
                pListCtrl->GetSkinRes());

        pListCtrl->SetItemTypeShareData(
                IPropertyCtrlItemBaseShareData::FLAG, pData);

        m_pBaseShareData = pData->GetImpl();
        m_pBaseShareData->SetSplitterLinePosPtr(
                (uint*)UISendMessage(
                pListCtrl, UI_PROPERTYCTRL_MSG_GETSPLITTERLINEPOS_PTR));

        ITextRenderBase* pTextRender = pListCtrl->GetTextRender();
        if (!pTextRender)
            pTextRender = pListCtrl->CreateDefaultTextRender();
        if (pTextRender)
        {
            m_pBaseShareData->m_pRenderFont = pTextRender->GetRenderFont();
        }

        m_pBaseShareData->pEditCtrl = (IEdit*)UISendMessage(
            pListCtrl, UI_PROPERTYCTRL_MSG_GETEDITCTRL);
        m_pBaseShareData->pComboBoxCtrl = (IComboBox*)UISendMessage(
            pListCtrl, UI_PROPERTYCTRL_MSG_GETCOMBOBOXCTRL);

    }
    else
    {
        m_pBaseShareData = pData->GetImpl();
    }

    m_pIPropertyCtrlItemBase->CreateRootPanel();
}


void  PropertyCtrlItemBase::OnPaint(IRenderTarget* pRenderTarget)
{
    DO_PARENT_PROCESS(IPropertyCtrlItemBase, IListItemBase);

    IListCtrlBase*  pListCtrl = m_pIPropertyCtrlItemBase->GetIListCtrlBase();

    CRect rcParent;
    m_pIPropertyCtrlItemBase->GetParentRect(&rcParent);

    CRect rcBlank(rcParent);
    int nIndent = pListCtrl->GetChildNodeIndent();
    int nDepth = m_pIPropertyCtrlItemBase->CalcDepth();
	if (nDepth >= 3)
		nDepth = 2;

    rcBlank.right = nIndent * nDepth;

    CRect rcLeft(rcParent);
    rcLeft.left = rcBlank.right;
    rcLeft.right = m_pBaseShareData->GetSplitterLinePos();

    CRect rcRight(rcParent);
    rcRight.left = rcLeft.right;

    // 左侧背景
    Color c(PROPERTYCTRL_LINE_COLOR);
    pRenderTarget->DrawRect(&rcBlank, &c);

    // 中间的分隔线
    IRenderPen* pPen = pRenderTarget->CreateSolidPen(1, &c);
    pRenderTarget->DrawLine(rcLeft.right, rcLeft.top, rcLeft.right, rcLeft.bottom, pPen);

    // 一行的横线
	pRenderTarget->DrawLine(rcParent.left, rcParent.bottom-1, rcParent.right, rcParent.bottom-1, pPen);
    SAFE_RELEASE(pPen);

    // 选中状态
    bool bSelectd = m_pIPropertyCtrlItemBase->IsSelected();
    bool bFocus = pListCtrl->IsFocus();

    if (bSelectd)
    {
        if (bFocus)
        {
            Color c(10,36,106,255);
            pRenderTarget->DrawRect(&rcLeft, &c);
        }
        else
        {
            pRenderTarget->DrawRect(&rcLeft, &c);
        }
    }

    // 绘制文本
    if (m_pBaseShareData->m_pRenderFont)
    {
        DRAWTEXTPARAM param;
        if (bSelectd && bFocus)
            param.color = 0xFFFFFFFF;
        else
            param.color = 0;

        param.nFormatFlag = DT_LEFT|DT_VCENTER|DT_END_ELLIPSIS|DT_SINGLELINE;
        rcLeft.left += TEXT_INDENT;
        param.prc = &rcLeft;
        param.szText = m_pIPropertyCtrlItemBase->GetText();
        pRenderTarget->DrawString(m_pBaseShareData->m_pRenderFont, &param);

        if (!bSelectd)
        {
        }
    }
}

void  PropertyCtrlItemBase::SetKey(LPCTSTR szText)
{
	if (szText)
		m_strKey = szText;
	else
		m_strKey.clear();
}

LPCTSTR  PropertyCtrlItemBase::GetKey()
{
    return m_strKey.c_str();
}

void  PropertyCtrlItemBase::GetKeyColoumnRectInItem(LPRECT prc)
{
	if (NULL == prc)
		return;

	CRect rcParent;
	m_pIPropertyCtrlItemBase->GetParentRect(&rcParent);

	SetRectEmpty(prc);
	prc->right = m_pBaseShareData->GetSplitterLinePos();
	prc->bottom = rcParent.Height();
}

void  PropertyCtrlItemBase::GetValueColumnRectInItem(LPRECT prc)
{
    if (NULL == prc)
        return;

    CRect rcParent;
    m_pIPropertyCtrlItemBase->GetParentRect(&rcParent);

    SetRectEmpty(prc);
    prc->right = rcParent.Width();
    prc->bottom = rcParent.Height();
    prc->left = m_pBaseShareData->GetSplitterLinePos();
    prc->left += SPLITLINE_WIDTH;
}


void  PropertyCtrlItemBase::GetKeyColoumnRectInCtrl(LPRECT prc)
{
    if (NULL == prc)
        return;

    CRect rcParent;
    m_pIPropertyCtrlItemBase->GetParentRect(&rcParent);

    CopyRect(prc, &rcParent);
    prc->right = m_pBaseShareData->GetSplitterLinePos();
}

void  PropertyCtrlItemBase::GetValueColumnRectInCtrl(LPRECT prc)
{
    if (NULL == prc)
        return;

    CRect rcParent;
    m_pIPropertyCtrlItemBase->GetParentRect(&rcParent);

    CopyRect(prc, &rcParent);
    prc->right = rcParent.Width();
    prc->left = m_pBaseShareData->GetSplitterLinePos();
    prc->left += SPLITLINE_WIDTH;
}

void  PropertyCtrlItemBase::OnLButtonDown(UINT nFlags, POINT point)
{
	CRect rcValue;
	GetValueColumnRectInItem(&rcValue);

	POINT ptItem;
	m_pIPropertyCtrlItemBase->GetIListCtrlBase()->WindowPoint2ItemPoint(m_pIPropertyCtrlItemBase, &point, &ptItem);
	if (PtInRect(&rcValue, ptItem))
	{
		this->OnValueColumnClick(point, ptItem);;
	}
	SetMsgHandled(FALSE);
}

LRESULT  PropertyCtrlItemBase::OnGetKey(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    return (LRESULT)m_strKey.c_str();
}

LRESULT  PropertyCtrlItemBase::OnGetValue(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    return (LRESULT)GetValueText();   
}

bool  PropertyCtrlItemBase::IsEditActive()
{
	if (!m_pIPropertyCtrlItemBase || !m_pBaseShareData || !m_pBaseShareData->pEditCtrl)
		return false;

	IObject* pObj = m_pBaseShareData->pEditCtrl->GetParentObject();
	if (pObj && pObj == m_pIPropertyCtrlItemBase->GetRootPanel())
		return true;

	return false;
}

bool  PropertyCtrlItemBase::IsComboBoxActive()
{
    if (!m_pIPropertyCtrlItemBase || !m_pBaseShareData || !m_pBaseShareData->pComboBoxCtrl)
        return false;

    IObject* pObj = m_pBaseShareData->pComboBoxCtrl->GetParentObject();
    if (pObj && pObj == m_pIPropertyCtrlItemBase->GetRootPanel())
        return true;

    return false;
}

void PropertyCtrlItemBase::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
    SetMsgHandled(FALSE);

    if (nChar == TEXT('C') && Util::IsKeyDown(VK_CONTROL))
    {
        if (OpenClipboard(NULL))
        {
            EmptyClipboard();

            int nSize = sizeof(TCHAR) * (m_strKey.length() + 1);
            HGLOBAL hBlock = GlobalAlloc(GMEM_MOVEABLE, nSize);
            if (hBlock)
            {
                void* p = GlobalLock(hBlock);
                memcpy(p, m_strKey.c_str(), nSize);
                GlobalUnlock(hBlock);
            }
            SetClipboardData(CF_UNICODETEXT, hBlock);

            CloseClipboard();
            if (hBlock)
                GlobalFree(hBlock);
        }
    }
}