#include "stdafx.h"
#include "mousekey.h"
#include "..\model\doc.h"
#include "..\model\page.h"
#include "..\model\line.h"
#include "..\model\run.h"
#include "..\model\element\element.h"
#include "..\UISDK\Inc\Util\struct.h"
#include "..\richtext.h"
#include "..\UISDK\Inc\Interface\iscrollbarmanager.h"

using namespace UI;
using namespace RT;

MouseKey::MouseKey(RT::Doc& pDoc) : m_doc(pDoc)
{
	m_pUIApp = NULL;
    
	m_pHoverPage = nullptr;
	m_pHoverLine = nullptr;
	m_pHoverRun = nullptr;
	m_pHoverElement = nullptr;
	m_pPressElement = nullptr;

	m_bLButtonPressDown = false;
}

MouseKey::~MouseKey()
{
   
}

void  MouseKey::Init(RichText* pRichText)
{
    UIASSERT(pRichText);
    IRichText* pIRichText = pRichText->GetIRichText();

    m_pUIApp = pIRichText->GetUIApplication();
}

void  MouseKey::Clear()
{
    m_pHoverPage = nullptr;
    m_pHoverLine = nullptr;
    m_pHoverRun = nullptr;

	if (m_pHoverElement)
	{
		m_pHoverElement->NotifyMouseLeave(&m_doc);
		m_pHoverElement = nullptr;
	}
    
    m_pPressElement = nullptr;
	m_bLButtonPressDown = false;
}

bool  MouseKey::IsPageHover(RT::Page*  pPage)
{
    if (pPage)
        return m_pHoverPage == pPage ? true:false;

    return false;
}
bool  MouseKey::IsLineHover(RT::Line*  pLine)
{
    if (pLine)
        return m_pHoverLine == pLine ? true:false;

    return false;
}
bool  MouseKey::IsRunHover(RT::Run*  pRun)
{
    if (pRun)
        return m_pHoverRun == pRun ? true:false;

    return false;
}
bool  MouseKey::IsElementHover(RT::Element*  pElement)
{
    if (pElement)
        return m_pHoverElement == pElement ? true:false;

    return false;
}

void  MouseKey::OnMouseMove(UINT nFlags, POINT ptDoc)
{
    // 方案1：根据内容对象，从根结点往下一级级的搜索Doc->Para->Unit->Element
    //       Doc/Para这些对象是没有坐标数据的，没法做hittest
    // 文案2：根据布局对象，Page->Line->Run，再由run反推得到当前element
    //

	HitTest(ptDoc, &m_pHoverPage, &m_pHoverLine, &m_pHoverRun);

    Element* pOldHover = m_pHoverElement;
    m_pHoverElement = nullptr;
    if (m_pHoverRun)
    {
        m_pHoverElement = m_pHoverRun->GetElement();
    }

    if (pOldHover != m_pHoverElement)
    {
        Node* pLeaveNotifyTo = nullptr;
        Node* pMoveNotifyTo = nullptr;

        // 检测本次需要通知的祖先结点。因为old和new会有共同的父结点，这些父结点状态没有发生
        // 改变。在这里采用倒过来比较的方式，从根结点往后逐个比较，直接找到不同的祖先结点。
        if (pOldHover && m_pHoverElement)
        {
            vector<Node*> vecOld;
            vector<Node*> vecHover;

            Node* p = pOldHover;
            while (p)
            {
                vecOld.push_back(p);
                p = p->GetParentNode();
            }
            p = m_pHoverElement;
            while (p)
            {
                vecOld.push_back(p);
                p = p->GetParentNode();
            }

            vector<Node*>::reverse_iterator iterOld = vecOld.rbegin();
            vector<Node*>::reverse_iterator iterHover = vecHover.rbegin();
            for (; iterOld != vecOld.rend() && iterHover != vecHover.rend();
                    ++iterOld, ++iterHover)
            {
                pLeaveNotifyTo = *iterOld;
                pMoveNotifyTo = *iterHover;
                if (*iterOld != *iterHover)
                {
                    break;
                }
            }
        }
        if (pOldHover)
        {
            pOldHover->NotifyMouseLeave(pLeaveNotifyTo);
        }
        if (m_pHoverElement)
        {
            m_pHoverElement->NotifyMouseMove(nFlags, ptDoc, pMoveNotifyTo);
        }
    }

	// TODO: 更新鼠标样式
// 	if (!m_pPressUnit && pOldHover != m_pHoverUnit)
// 	{
// 		if (!m_pHoverUnit || !m_pHoverUnit->OnSetCursor())
// 		{
// 			static HCURSOR sArrowHandle = LoadCursor(NULL, IDC_ARROW);
// 			SetCursor(sArrowHandle);
// 		}
// 	}

	
    if (m_doc.GetIControl()->IsPress() && m_doc.GetEditMode() != EDITABLE_Disable)
    {
        // 拖拽选区
        Run* pHitRun = get_nearest_run(ptDoc);
        if (pHitRun)
        {
            CaretPos nOffsetInElement = 
                pHitRun->SelectionHitTestByDocPoint(ptDoc);
            m_doc.GetSelection().SetFocus(
                pHitRun->GetElement(), nOffsetInElement);
        }
    }
    m_doc.Refresh();
}

void  MouseKey::OnMouseLeave()
{
    if (m_pHoverElement)
    {
        m_pHoverElement->NotifyMouseLeave(nullptr);
    }

	m_pHoverPage = NULL;
	m_pHoverLine = NULL;
	m_pHoverRun = NULL;
	m_pHoverElement = NULL;
	m_doc.Refresh();
}

BOOL  MouseKey::OnKeyDown(UINT nChar)
{
    if (nChar == TEXT('C') && IsKeyDown(VK_CONTROL))
    {
        m_doc.CopySelection();
		return TRUE;
    }
    else if (nChar == TEXT('A') && IsKeyDown(VK_CONTROL))
    {
        if (!IsKeyDown(VK_LBUTTON))
		{
            m_doc.SelectAll();
			return TRUE;
		}
    }
	else if (nChar == VK_PRIOR)
	{
		IScrollBarManager* pMgr = m_doc.GetScrollBarManager();
		if (pMgr)
		{
			if (pMgr->DoPageUp())
			{
				m_doc.InvalidRegion(nullptr);
				m_doc.Refresh();
			}
			return TRUE;
		}
	}
	else if (nChar == VK_NEXT)
	{
		IScrollBarManager* pMgr = m_doc.GetScrollBarManager();
		if (pMgr)
		{
			if (pMgr->DoPageDown())
			{
				m_doc.InvalidRegion(nullptr);
				m_doc.Refresh();
			}
			return TRUE;
		}
	}
	else if (nChar == VK_UP)
	{
		if (IsKeyDown(VK_SHIFT))
			return FALSE;
		IScrollBarManager* pMgr = m_doc.GetScrollBarManager();
		if (pMgr)
		{
			if (pMgr->DoLineUp())
			{
				m_doc.InvalidRegion(nullptr);
				m_doc.Refresh();
			}
			return TRUE;
		}
	}
	else if (nChar == VK_DOWN)
	{
		if (IsKeyDown(VK_SHIFT))
			return FALSE;
		IScrollBarManager* pMgr = m_doc.GetScrollBarManager();
		if (pMgr)
		{
			if (pMgr->DoLineDown())
			{
				m_doc.InvalidRegion(nullptr);
				m_doc.Refresh();
			}
			return TRUE;
		}
	}
	else if (nChar == VK_END)
	{
		if (IsKeyDown(VK_SHIFT))
			return FALSE;
		IScrollBarManager* pMgr = m_doc.GetScrollBarManager();
		if (pMgr)
		{
			if (pMgr->DoEnd())
			{
				m_doc.InvalidRegion(nullptr);
				m_doc.Refresh();
			}
			return TRUE;
		}
	}
	else if (nChar == VK_HOME)
	{
		if (IsKeyDown(VK_SHIFT))
			return FALSE;
		IScrollBarManager* pMgr = m_doc.GetScrollBarManager();
		if (pMgr)
		{
			if (pMgr->DoHome())
			{
				m_doc.InvalidRegion(nullptr);
				m_doc.Refresh();
			}
			return TRUE;
		}
	}
    return FALSE;
}

BOOL  MouseKey::OnSetCursor()
{
    BOOL bHandled = FALSE;
//     if (m_pHoverRun)
//     {
//         Element* pElement = m_pHoverRun->GetElement();
//         if (pElement)
//         {
//             return pElement->OnSetCursor();
//         }
//     }

    if (m_pHoverElement)
    {
        bHandled = m_pHoverElement->OnSetCursor();
    }

    if (!bHandled && m_doc.GetEditMode() != EDITABLE_Disable)
    {
        SetCursor(LoadCursor(NULL, IDC_IBEAM));
        return TRUE;
    }
    return bHandled;
}

void  MouseKey::doc_point_2_page_point(const POINT& ptDoc, POINT& ptPage)
{
#if 0
    m_doc.PageHitTest(ptDoc, &ptPage);
#endif
}

// 获取离当前鼠标最近的一个文字段，用于选区操作
RT::Run*  MouseKey::get_nearest_run(const POINT& ptDoc)
{
    if (m_pHoverRun)
    {
        return m_pHoverRun;
    }

    Line* pHoverLine = m_pHoverLine;
    if (!pHoverLine)
    {
        Page* pHoverPage = m_pHoverPage;
        // 鼠标在page外面了，要算出首行或末行
        if (!pHoverPage)
        {
            if (m_doc.GetPageMode() == PageMode_multipage)
                return nullptr;

            pHoverPage = m_doc.GetFirstPage();
            if (!pHoverPage)
                return nullptr;
        }

        pHoverLine = pHoverPage->NearestLineHitTest(ptDoc);
    }

    // 检查鼠标所在行
    if (!pHoverLine)
        return nullptr;
    
    RECT rcLine;
    pHoverLine->GetContentRectInDoc(&rcLine);

    // TBD: 如果是空白回车行，这一行是没有run的，只能返上一行的run
    while (pHoverLine && !pHoverLine->GetFirstRun())
    {
        pHoverLine = pHoverLine->GetPrevLine();
    }
    if (!pHoverLine)
        return nullptr;

    if (ptDoc.x <= rcLine.left)
    {
        return pHoverLine->GetFirstRun();
    }
    else if (ptDoc.x >= rcLine.right)
    {
        return pHoverLine->GetLastRun();
    }

    POINT ptInLine = { ptDoc.x - rcLine.left, ptDoc.y - rcLine.top };
    // 中间检测
    return pHoverLine->RunHitTest(ptInLine, nullptr);
}

void  MouseKey::OnLButtonDBClick(UINT nFlags, POINT ptDoc)
{
	m_bLButtonPressDown = true;

	bool bHandled = false;
	if (m_pHoverElement)
	{
		m_pPressElement = m_pHoverElement;
		m_pPressElement->OnLButtonDBClick(nFlags, ptDoc, bHandled);
		if (bHandled)
			return;
	}
	// selection
	if (m_doc.GetEditMode() != EDITABLE_Disable)
	{
		Run* pHitRun = get_nearest_run(ptDoc);
		if (pHitRun)
		{
			CaretPos nOffsetInElement =
				pHitRun->SelectionHitTestByDocPoint(ptDoc);
			m_doc.GetSelection().SetAnchor(
				pHitRun->GetElement(), nOffsetInElement);
		}
	}

	m_doc.Refresh();
}
void  MouseKey::OnLButtonDown(UINT nFlags, POINT ptDoc)
{
//     if (m_pHoverUnit)
//     {
//         SetPressUnit(m_pHoverUnit);
//     }
    m_bLButtonPressDown = true;

    if (m_pHoverElement)
    {
        m_pPressElement = m_pHoverElement;
        m_pPressElement->OnLButtonDown(nFlags, ptDoc);
    }

    // selection
    if (m_doc.GetEditMode() != EDITABLE_Disable)
    {
        Run* pHitRun = get_nearest_run(ptDoc);
        if (pHitRun)
        {
            CaretPos nOffsetInElement =
                pHitRun->SelectionHitTestByDocPoint(ptDoc);
            m_doc.GetSelection().SetAnchorFocus(
                pHitRun->GetElement(), nOffsetInElement);
        }
	}

	m_doc.Refresh();
}

void  MouseKey::OnLButtonUp(UINT nFlags, POINT ptDoc)
{
    if (m_pPressElement)
    {
        Element* pOldPress = m_pPressElement;

        pOldPress->OnLButtonUp(nFlags, ptDoc);
        if (pOldPress == m_pPressElement && 
            pOldPress == m_pHoverElement)
        {
            pOldPress->OnClick(nFlags, ptDoc);
        }

        m_pPressElement = nullptr;
    }

	if (m_bLButtonPressDown)
	{
		if (m_doc.GetEditMode() != EDITABLE_Disable)
		{
			Run* pHitRun = get_nearest_run(ptDoc);
			if (pHitRun)
			{
				CaretPos nOffsetInElement = 
					pHitRun->SelectionHitTestByDocPoint(ptDoc);
				m_doc.GetSelection().SetFocus(
					pHitRun->GetElement(), nOffsetInElement);
			}
		}
	}

	m_bLButtonPressDown = false;

// 	if (m_pPressUnit && m_pHoverUnit == m_pPressUnit)
// 		m_pPressUnit->OnClick();

	//SetPressUnit(NULL);

    m_doc.Refresh();
}

void  MouseKey::HitTest(POINT ptDoc, Page** ppHoverPage, Line** ppHoverLine, Run** ppHoverRun)
{
	if (ppHoverPage)
		*ppHoverPage = NULL;
	if (ppHoverLine)
		*ppHoverLine = NULL;
	if (ppHoverRun)
		*ppHoverRun = NULL;

	// Page测试
	POINT ptInPage = {0};
	Page* pHoverPage = m_doc.PageHitTest(ptDoc, &ptInPage);
	if (!pHoverPage)
		return;

	if (ppHoverPage)
		*ppHoverPage = pHoverPage;

	// Line测试
	POINT ptInLine = {0};
	Line* pHoverLine = pHoverPage->LineHitTest(ptInPage, &ptInLine);
	if (!pHoverLine)
		return;

	if (ppHoverLine)
		*ppHoverLine = pHoverLine;

	// Run测试
	POINT ptInRun = {0};
	Run* pHoverRun = pHoverLine->RunHitTest(ptInLine, &ptInRun);
	if (!pHoverRun)
		return;

	if (ppHoverRun)
		*ppHoverRun = pHoverRun;
}

void UI::MouseKey::OnNodeRemove(RT::Node* p)
{
	if (!p)
		return;

	if (m_pHoverElement == p || p->IsMyChild(m_pHoverElement, true))
	{
		m_pHoverElement = NULL;
		// m_bMouseMoveReady = FALSE;
	}
	if (m_pPressElement == p || p->IsMyChild(m_pPressElement, true))
	{
		m_pPressElement = NULL;
		// m_bMouseMoveReady = FALSE;
	}
}

// BOOL  MouseKey::OnMouseWheel(UINT nFlags, short zDelta)
// {
//     UIMSG msg;
//     msg.message = WM_MOUSEWHEEL;
//     msg.wParam = nFlags;
//     msg.lParam = zDelta;
//     return m_pMgrScrollBar->ProcessMessage(&msg);
// }


