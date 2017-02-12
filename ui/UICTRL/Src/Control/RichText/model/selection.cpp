#include "stdafx.h"
#include "selection.h"
#include "..\model\element\element.h"
#include "..\model\element\textelement.h"
#include "..\model\run.h"
#include "..\model\doc.h"

namespace UI {
    namespace RT {


Selection::Selection(Doc& doc) : m_doc(doc)
{
    m_nAnchorOffset = 0;
    m_nFocusOffset = 0;

	m_pAnchor = nullptr;
	m_pFocus = nullptr;
}

CaretPos  Selection::GetAnchorOffset()
{
    return m_nAnchorOffset;
}
CaretPos  Selection::GetFocusOffset()
{
    return m_nFocusOffset;
}

Node* Selection::GetAnchor()
{
    return m_pAnchor;
}

Node* Selection::GetFocus()
{
    return m_pFocus;
}

// INode*  Selection::GetAnchorNode()
// {
//     if (!m_pAnchor)
//         return nullptr;
// 
//     return static_cast<INode*>(m_pAnchor);
// }
// INode*  Selection::GetFocusNode()
// {
//     if (!m_pFocus)
//         return nullptr;
// 
//     return static_cast<INode*>(m_pFocus);
// }

//
// 将 p1 ~ p2 之间的element的状态设置为s
//
void  Selection::set_state_range(Node* p1, Node* p2, SelectionState s)
{
    if (!p1 || !p2)
        return;

    if (p2->GetIndex() > p1->GetIndex())
    {
        Node* pIndex = p1;
        while (pIndex)
        {
            pIndex->SetSelectionState(s);
            if (pIndex == p2)
                break;
            pIndex = pIndex->GetNextTreeNode();
        }
    }
    else
    {
        Node* pIndex = p2;
        while (pIndex)
        {
            pIndex->SetSelectionState(s);

            if (pIndex == p1)
                break;
            pIndex = pIndex->GetNextTreeNode();
        }
    }
}

void  Selection::update_state()
{
    if (!m_pAnchor || !m_pFocus)
        return;

    if (m_pFocus == m_pAnchor)
    {
//         int content_length = m_pAnchor->GetMaxGlyphIndex() + 1;
//         bool bStart = m_nAnchorOffset == 0 ? true : false;
//         bool bEnd = m_nFocusOffset == content_length ? true:false;

        m_pFocus->SetSelectionState(SelectionState_Selected);
        return;
    }

    // 提取前后顺序
    Node* pStart = nullptr;
    Node* pEnd = nullptr;
    CaretPos nStartOffset = 0;
    CaretPos nEndOffset = 0;

    if (m_pFocus->GetIndex() > m_pAnchor->GetIndex())
    {
        pStart = m_pAnchor;
        pEnd = m_pFocus;
        nStartOffset = m_nAnchorOffset;
        nEndOffset = m_nFocusOffset;
    }
    else
    {
        pStart = m_pFocus;
        pEnd = m_pAnchor;
        nStartOffset = m_nFocusOffset;
        nEndOffset = m_nAnchorOffset;
    }

    pStart->SetSelectionState(SelectionState_Selected);
    pEnd->SetSelectionState(SelectionState_Selected);

    Node* p = pStart->GetNextTreeNode();
    while (p && p != pEnd)
    {
        p->SetSelectionState(SelectionState_Selected);
        p = p->GetNextTreeNode();
    }
}

// 求elment在selection中的选中范围
bool  Selection::GetSelectionRange(Node* p, CaretPos& start, CaretPos& end)
{
    if (!p)
        return false;

    if (!m_pAnchor || !m_pFocus)
        return false;

    if (p->GetSelectionState() == SelectionState_None)
        return false;
   
    if (m_pFocus == p && m_pAnchor == p)
    {
        start = m_nAnchorOffset;
        end = m_nFocusOffset;

        SortCaretPos(start, end);
        return true;
    }

    start.pos = 0;
    end.pos = 0;
    if (p->GetNodeType() == NodeTypeText)
    {
        end.pos = static_cast<TextElement*>(p)->GetCharacters();
    }
	else // if (p->GetNodeType() != NodeTypeText)
	{
		end.pos = 1;
	}

    // 包在了中间，完全选中
    if (m_pAnchor != p && m_pFocus != p)
    {
        return true;
    }

    // 反向选择的
    bool bReverse = m_pAnchor->GetIndex() > m_pFocus->GetIndex();
    if (m_pAnchor == p)
    {
        if (bReverse)
        {
            end = m_nAnchorOffset;
        }
        else
        {
            start = m_nAnchorOffset;
        }
        return true;
    }

    if (m_pFocus == p)
    {
        if (bReverse)
        {
            start = m_nFocusOffset;
        }
        else
        {
            end = m_nFocusOffset;
        }
        return true;
    }

    return false;
}

// 求run在element中的选中范围
bool  Selection::GetSelectionRange(Run* p, CaretPos& start, CaretPos& end)
{
    if (!p)
        return false;

    Node* pElement = p->GetElement();
    if (!pElement)
        return false;

    CaretPos elemSelStart, elemSelEnd;
    if (!GetSelectionRange(pElement, elemSelStart, elemSelEnd))
        return false;

    uint __runStartIndex = 0;
    uint __runEndIndex = 0;
    p->GetRange(&__runStartIndex, &__runEndIndex);

    CaretPos runStartPos(__runStartIndex);
    CaretPos runEndPos(__runEndIndex + 1);

    // 求交集
    if (runStartPos.pos < elemSelStart.pos)
    {
        if (runEndPos.pos <= elemSelStart.pos)
            return false;

        start = elemSelStart;

        if (runEndPos.pos <= elemSelEnd.pos)
            end = runEndPos;
        else
            end = elemSelEnd;

        return true;
    }
    else if (runStartPos.pos < elemSelEnd.pos)
    {
        start = runStartPos;

        if (runEndPos.pos > elemSelEnd.pos)
            end = elemSelEnd.pos;
        else
            end = runEndPos.pos;

        return true;
    }
    else
    {
        return false;
    }

    return false;
}


void  Selection::clear_state()
{
    set_state_range(m_pAnchor, m_pFocus, SelectionState_None);
}

void Selection::SetAnchor(Node* p, CaretPos offset)
{
    clear_state();
    m_pFocus = nullptr;
    m_nFocusOffset = 0;

	if (p)
	{
		if (-1 == offset.pos)
			offset.pos = p->GetCharacters();
	}

	UIASSERT(offset.pos >= 0 && offset.pos <= 0x7FFFFFFF);

    // 直接先清空所有的选区
    m_pAnchor = p;
    m_nAnchorOffset = offset;

	
}

void Selection::SetFocus(Node* p, CaretPos offset)
{
    UIASSERT(m_pAnchor);
    if (!m_pAnchor)
        return;

    // 更新每个element的选中状态
    if (m_pFocus)
    {
        // 之前有选区
        clear_state();
        //set_state_range(p, m_pAnchor, SelectionState::Inside);
    }
    else
    {
        // 之前没有选区
        //set_state_range(p, m_pAnchor, SelectionState::Inside);
    }

    m_pFocus = p;
	m_nFocusOffset = 0;

	if (m_pFocus)
	{
		if (-1 == offset.pos)
			m_nFocusOffset = m_pFocus->GetCharacters();
		else
			m_nFocusOffset = offset;
	}

    update_state();

    // TODO:
    m_doc.InvalidRegion(nullptr);
}

void  Selection::SetAnchorFocus(Node* p, CaretPos offset)
{
    SetAnchor(p, offset);
    SetFocus(p, offset);
}

bool  Selection::IsEmpty()
{
    if (!m_pFocus || !m_pAnchor)
        return true;

    if (m_pAnchor == m_pFocus &&
        m_nFocusOffset.pos == m_nAnchorOffset.pos)
        return true;

    return false;
}

void  Selection::Clear()
{
    bool bEmpty = IsEmpty();

    clear_state();
    m_pFocus = nullptr;
    m_pAnchor = nullptr;
    m_nFocusOffset = 0;
    m_nAnchorOffset = 0;

    if (!bEmpty)
    {
        // 刷新
        m_doc.InvalidRegion(nullptr);
        m_doc.Refresh();
    }
}

void Selection::ToString(String& str)
{
    str.clear();
    if (IsEmpty())
        return;

    str.reserve(1024);

    if (m_pAnchor == m_pFocus)
    {
        CaretPos start, end;
        GetSelectionRange(m_pAnchor, start, end);

        String  strElem;
        m_pAnchor->ToString(start, end, 0, strElem);
        str.append(strElem);

        return;
    }

    Node* pBegin = m_pAnchor;
    Node* pEnd = m_pFocus;

    if (m_pAnchor->GetIndex() > m_pFocus->GetIndex())
    {
        pBegin = m_pFocus;
        pEnd = m_pAnchor;
    }

    CaretPos start, end;
    GetSelectionRange(pBegin, start, end);

    String  strElem;
    pBegin->ToString(start, end, 0, strElem);
    str.append(strElem);

    Node* p = pBegin;
    p = p->GetNextTreeNode();
    while (p && p != pEnd)
    {
        start = 0;
        end = p->GetCharacters();
        strElem.clear();
        p->ToString(start, end, 0, strElem);
        str.append(strElem);

        p = p->GetNextTreeNode();
    }

    GetSelectionRange(pEnd, start, end);

    pEnd->ToString(start, end, 0, strElem);
    str.append(strElem);
}

}
}