#include "stdafx.h"
#include "node.h"
#include "doc.h"
#include "paragraph.h"

using namespace UI;
using namespace RT;


Node::Node()
{
	parent = nullptr;
	child = nullptr;
	prev = nullptr;
	next = nullptr;

	m_nIndex = 0;
	m_eSelectionState = SelectionState_None;
	memset(&m_style, 0, sizeof(m_style));
}

Node::~Node()
{
}

// 在销毁之前有机会调用其它虚函数
void  Node::Destroy()
{
	Doc* doc = GetDocNode();
	if (doc)
		doc->OnNodeRemove(this);

	DestroyDecendant();
	delete this;
}

void  Node::DestroyDecendant()
{
	Node* p = child;
	while (p)
	{
		Node* next = p->next;
		p->Destroy();
		p = next;
	}
	parent = nullptr;
	child = nullptr;
	prev = nullptr;
	next = nullptr;
}

Node*  Node::GetLastChildNode()
{
    Node* p = child;
    while (p && p->next)
    {
        p = p->next;
    }

    return p;
}

void  Node::AddChildNode(Node* p)
{
    if (!p)
        return;

    p->parent = this;

    if (!this->child)
    {
        this->child = p;
        return;
    }

    Node* pLast = GetLastChildNode();
    UIASSERT(pLast);

    pLast->next = p;
    p->prev = pLast;
}

// 将p作为第一个子结点插入在this的下面
void  Node::AddFrontChildNode(Node* p)
{
	if (!p)
		return;

	p->parent = this;

	Node* pFirst = this->child;
	this->child = p;

	if (!pFirst)
		return;
	
	p->next = pFirst;
	pFirst->prev = p;
}

void  Node::InsertAfter(Node* pInsertAfter)
{
	if (!pInsertAfter)
		return;
	
	this->parent = pInsertAfter->parent;
	
	if (pInsertAfter->next)
		pInsertAfter->next->prev = this;
	this->next = pInsertAfter->next;
	
	pInsertAfter->next = this;
	this->prev = pInsertAfter;
}

void  Node::RemoveFromTree(Node* p)
{
	Doc* doc = GetDocNode();

	// TODO: 怎么快速判断自己的子孙结点有没有被选中？
	if (1)
	{
		Doc* doc = GetDocNode();
		if (doc)
		{
			doc->GetSelection().Clear(); 
		}
	}

	if (this->prev)
	{
		this->prev->next = this->next;
	}
	else
	{
		if (this->parent) 
		{
			this->parent->child = this->next;
		}
	}

	if (this->next)
	{
		this->next->prev = this->prev;
	}

	parent = next = prev = NULL;

	if (doc)
	{
		doc->OnNodeRemove(this);
	}
}

void  Node::DeleteFromTree(Node* p)
{
	if (!p)
		return;

	RemoveFromTree(p);

	p->Destroy();
}


bool Node::IsMyChild(Node* pChild, bool bFindInGrand)	
{
	if (NULL == pChild)
		return false;

	if (bFindInGrand)
	{
		Node* pParent = pChild->parent;
		while (pParent)
		{
			if (pParent == this)
				return true;

			pParent = pParent->parent;
		}
	}
	else
	{
		if (pChild->parent == this)
			return true;
		else
			return false;
	}

	return false;
}

void Node::PreLayoutSingleLine(SingleLineLayoutContext*)
{

}
void Node::LayoutSingleLine(SingleLineLayoutContext* context)
{
    Node* p = GetChildNode();
    while (p)
    {
        p->PreLayoutSingleLine(context);
        p->LayoutSingleLine(context);
        p->PostLayoutSingleLine(context);

        p = p->GetNextNode();
    }
}
void Node::PostLayoutSingleLine(SingleLineLayoutContext*)
{

}

void Node::PreLayoutMultiLine(MultiLineLayoutContext*)
{

}
void Node::LayoutMultiLine(MultiLineLayoutContext* context)
{
    Node* p = GetChildNode();
    while (p)
    {
        p->PreLayoutMultiLine(context);
        p->LayoutMultiLine(context);
        p->PostLayoutMultiLine(context);

        p = p->GetNextNode();
    }
}
void Node::PostLayoutMultiLine(MultiLineLayoutContext*)
{

}

Node*  Node::GetRootNode()
{
    Node* p = this;
    while (p && p->parent)
    {
        p = p->parent;
    }

    return p;
}

Doc*  Node::GetDocNode()
{
    Node* root = GetRootNode();
    if (!root)
        return nullptr;

    if (root->GetNodeType() != NodeTypeDoc)
        return nullptr;

    return static_cast<Doc*>(root);
}

RichText* Node::GetControl()
{
	Doc* pDoc = GetDocNode();
	if (!pDoc)
		return nullptr;

	return pDoc->GetControl();
}

Paragraph*  Node::GetParagraphNode()
{
    if (GetNodeType() == NodeTypeParagraph)
        return static_cast<Paragraph*>(this);

    Node* p = GetAncestorNodeByType(NodeTypeParagraph);
    if (p)
        return static_cast<Paragraph*>(p);

    return nullptr;
}

Node*  Node::GetAncestorNodeByType(NodeType e)
{
    Node* p = this;
    while (p)
    {
        if (p->GetNodeType() == e)
            return p;

        p = p->parent;
    }

    return nullptr;
}

bool Node::IsVirtualNode()
{
    NodeType type = GetNodeType();
    if (type >= NodeTypeVirtualNodeBegin && type <= NodeTypeVirtualNodeEnd)
        return true;

    return false;
}
bool Node::IsRealNode()
{
    NodeType type = GetNodeType();
    if (type >= NodeTypeRealNodeBegin && type <= NodeTypeRealNodeEnd)
        return true;

    return false;
}

bool Node::CanAddParagraphAsChildNode()
{
	NodeType type = GetNodeType();
	if (type < NodeTypeParagraph)
		return true;

	return false;
}

bool Node::IsPress()
{
	return m_style.press;
}
bool Node::IsHover()
{
	return m_style.hover;
}


Node* Node::GetNextTreeNode()
{
    Node* p = this;

    // 有子结点
    if (p->child)
    {
        return p->child;
    }

    // 没有子结点，获取下一个结点
    if (p->next)
    {
        return p->next;
    }

    // 也没有下一个子结点，获取父对象的下一个结点
    Node* pParent = p->parent;
    while (pParent)
    {
        Node* pNext = pParent->next;
        if (pNext)
            return pNext;

        pParent = pParent->parent;
    }

    return NULL;
}

Node* Node::GetPrevTreeNode()
{
    Node* p = this;

    // 获取左侧结点的最后一个子结点
    if (p->prev)
    {
        Node* pLeft = p->prev;
        Node* pChild = NULL;
        while (pChild = pLeft->GetLastChildNode())
        {
            pLeft = pChild;
        }
        return pLeft;
    }

    // 有父结点
    if (p->parent)
    {
        return p->parent;
    }

    return nullptr;
}



void  Node::SetIndex(int n)
{
    m_nIndex = n;
}
int   Node::GetIndex()
{
    return m_nIndex;
}

void  Node::SetSelectionState(SelectionState e)
{
    if (m_eSelectionState == e)
        return;

    m_eSelectionState = e;
}

SelectionState  Node::GetSelectionState()
{
    return m_eSelectionState;
}

const CharFormat*  Node::GetCharFormat()
{
    if (!parent)
        return nullptr;

    return parent->GetCharFormat();
}

// 用于实现虚拟结点的背景绘制。实结点的绘制由page->line->run->发起
void Node::PreDraw(HDC hDC, LPCRECT rcClip)
{
	Node* p = this->child;
    while (p)
    {
        p->PreDraw(hDC, rcClip);
        p = p->next;
    }
}


BOOL  Node::OnSetCursor()
{
    Node* parent = GetParentNode();
    if (!parent)
        return FALSE;

    return parent->OnSetCursor();
}

void  Node::OnLButtonDown(UINT nFlags, POINT ptDoc)
{
    Node* parent = GetParentNode();
    if (!parent)
        return;

    parent->OnLButtonDown(nFlags, ptDoc);
}
void Node::OnLButtonDBClick(UINT nFlags, POINT ptDoc, bool& bHandled)
{
	Node* parent = GetParentNode();
	if (!parent)
		return;

	parent->OnLButtonDBClick(nFlags, ptDoc, bHandled);
}
void  Node::OnLButtonUp(UINT nFlags, POINT ptDoc)
{
    Node* parent = GetParentNode();
    if (!parent)
        return;

    parent->OnLButtonUp(nFlags, ptDoc);
}
void Node::OnClick(UINT nFlags, POINT ptDoc)
{
    Node* parent = GetParentNode();
    if (!parent)
        return;

    parent->OnClick(nFlags, ptDoc);
}

void  Node::NotifyMouseLeave(Node* pBubbleUntil)
{
    Node* p = this;
    while (p)
    {
        p->OnMouseLeave();
        p->m_style.hover = 0;

        if (p == pBubbleUntil)
            break;

        p = p->parent;
    }
}
void  Node::NotifyMouseMove(UINT nFlags, POINT ptDoc, Node* pBubbleUntil)
{
    Node* p = this;
    while (p)
    {
        if (!p->m_style.hover)
        {
            p->m_style.hover = 1;
            p->OnMouseEnter();
        }
        p->OnMouseMove(nFlags, ptDoc);

        if (p == pBubbleUntil)
            break;

        p = p->parent;
    }
}

// 将newParent做为自己的新父结点插入到树中
void  Node::TakePlaceAsMyParent(Node* newParent)
{
    if (!newParent)
        return;

    newParent->parent = this->parent;
    this->parent = newParent;
    newParent->child = this;

    if (!this->prev && newParent->parent)
        newParent->parent->child = newParent;

    newParent->prev = this->prev;
    if (newParent->prev)
        newParent->prev->next = newParent;
    this->prev = nullptr;

    newParent->next = this->next;
    if (newParent->next)
        newParent->next->prev = newParent;
    this->next = nullptr;
}
