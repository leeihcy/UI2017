#include "stdafx.h"
#include "doc.h"
#include "paragraph.h"
#include "element\imageelement.h"
#include "element\textelement.h"
#include "font.h"
#include "message.h"
#include "link.h"

using namespace UI;
using namespace UI::RT;

//  全选逻辑
//	anchor = doc
//  anchoroffset = 0
//  focus = last node
//  focusoffset = -1
// 
// 插入逻辑整理
//  text,image,link,message
//  1. 如果有选区范围，则先删除选区内的内容
//  2. 如果选区为根对象，offset=0则插入在addfront，offset!=0，则addback
//  3. 如果选区为文本，将文本拆分，插入在文本后面
//  4. ...


void  Doc::ReplaceText(LPCTSTR szText)
{
    ReplaceTextEx(szText, nullptr);
}

void  stringReplaceR(TCHAR* start, TCHAR* end)
{
    TCHAR* p = start;
    while (p <= end)
    {
        if (*p == TEXT('\r'))
            *p = TEXT(' ');

        p++;
    }
}
void  stringReplaceN(TCHAR* start, TCHAR* end)
{
    TCHAR* p = start;
    while (p <= end)
    {
        if (*p == TEXT('\n'))
            *p = TEXT(' ');

        p++;
    }
}

void  Doc::ReplaceTextEx(LPCTSTR _szText, const CharFormat* pcf)
{
    if (!_szText)
        return;
  
    // 内部要修改这个字符串，重新拷贝一份
    String strText(_szText);


    // 1. 当前选区有内容，先删除当前选区
    if (!m_selection.IsEmpty())
    {
        // todo:
    }

    //  分拆回车字符
    int  nLength = strText.length();
    if (0 == nLength)
        return;

    LPTSTR pStart = (LPTSTR)strText.c_str();
    LPTSTR pEnd = pStart + nLength;
    LPTSTR p = NULL;

    // 按段插入
    do
    {
        p = _tcschr(pStart, TEXT('\n'));

        int nParaLength = 0;
        if (p)
        {
            nParaLength = p - pStart;

            // \n前面的\r过滤掉
            if (nParaLength > 0 && *(p - 1) == TEXT('\r'))  
                nParaLength--;

            // 再将中间的\r替换为空格，不支持单独的\r来回车，回车只识别\n
            stringReplaceR(pStart, pStart + nParaLength);

            if (nParaLength)
                replaceTextNoCarriageReturn(pStart, nParaLength, pcf);
            
            if (m_ePageMode != PageMode_singleline)
            {
				// 插入一个para
				Paragraph* para = new Paragraph;
                insertParaNodeAtSelection(para);
            }

            pStart = p + 1;
        }
        else
        {
            nParaLength = pEnd - pStart;

            stringReplaceR(pStart, pStart + nParaLength);
            replaceTextNoCarriageReturn(pStart, nParaLength, pcf);
        }
    } while (p);
}

// 插入文本、链接、图片，等简单对象可以直接调用该函数。这些对象必须位于一个para下面
// 插入message不能调用。因为message要求在第二级结点。
void  Doc::insertSimpleNodeAtSelection(Node* node)
{
	UIASSERT(node);
	if (!node)
		return;

	Node* cp = m_selection.GetAnchor();
	UIASSERT(cp);
	if (!cp)
		return;

	uint selPos = m_selection.GetAnchorOffset().pos;
	
	if (cp->IsRealNode())
	{
		// 如果是文本，将当前文本进行拆分。如果是其它对象，直接加到后面去
		if (cp->GetNodeType() == NodeTypeText)
		{
			static_cast<TextElement*>(cp)->SplitTwoElement(selPos);
		}

		node->InsertAfter(cp);
		m_selection.SetAnchorFocus(node, 0);
		return;
	}

	// 当前位置是在根结点 
	if (cp == this)
	{
		Paragraph* para = new Paragraph;
		para->AddChildNode(node);

		if (0 == selPos)
		{
			// 插入在前面 SelectBegin 状态
			this->AddFrontChildNode(para);
		}
		else
		{
			// 插入在后面
			this->AddChildNode(para);
		}
		m_selection.SetAnchorFocus(node, 0);
		return;
	}

	// 如果在一个结点的最后面，例如message
	// 			if (selPos == -1)
	// 			{
	// 				Paragraph* para = new Paragraph;
	// 				para->InsertAfter(cp);
	// 				para->AddChildNode(pText);
	// 			}
	// 			else 
	// 如果当前不在一个段落里，先加一个段落
	if (!cp->GetParagraphNode())
	{
		Paragraph* para = new Paragraph;
		cp->AddChildNode(para);
		para->AddChildNode(node);
	}
	else
	{
		cp->AddChildNode(node);
	}

	m_selection.SetAnchorFocus(node, 0);
}

void  Doc::insertParaNodeAtSelection(Paragraph* para)
{
	Node* selNode = m_selection.GetAnchor();
	if (selNode == this)
	{
		CaretPos pos = m_selection.GetAnchorOffset();
		if (0 == pos.pos)
		{
			// 插入在前面 SelectBegin 状态
			this->AddFrontChildNode(para);
		}
		else
		{
			// 插入在后面
			this->AddChildNode(para);
		}
	}
	else
	{
		Paragraph* pThisPara = nullptr;
		if (selNode)
			pThisPara = selNode->GetParagraphNode();

		if (pThisPara)
		{
			para->InsertAfter(pThisPara);
		}
		// 这种类型下面可以放para子结点
		else if (selNode && selNode->CanAddParagraphAsChildNode())
		{
			selNode->AddChildNode(para);
		}
		else
		{
			this->AddChildNode(para);
		}
	}                              

	m_selection.SetAnchorFocus(para, CaretPos(0));
}

// 插入文本，该文本中不包含回车换行
void  Doc::replaceTextNoCarriageReturn(
	LPCTSTR szText, uint length, const CharFormat* pcf)
{
	// 2. 判断当前位置，是否可以合并到一个textelement内
	Node* cp = m_selection.GetAnchor();
	UIASSERT(cp);

	bool bCanMerge = false;

	// 规定：虚拟结点直接往下添加内容,实结点往后面添加内容.
	if (cp->GetNodeType() == NodeTypeText)
	{
		const CharFormat* pcf_cp =
			static_cast<TextElement*>(cp)->GetOriginCharFormat();

		if (!pcf)
		{
			pcf = pcf_cp;
			bCanMerge = true;
		}
		else if (IsSameFormat(pcf_cp, pcf))
		{
			bCanMerge = true;
		}
	}


	uint selPos = m_selection.GetAnchorOffset().pos;

	if (bCanMerge)
	{
		// 3. 如果能合并，插入
		static_cast<TextElement*>(cp)->InsertTextAt(selPos, szText, length);

		selPos += length;
		m_selection.SetAnchorFocus(cp, CaretPos(selPos));
	}
	else
	{
		// 3. 如果不能合并，创建一个新的
		TextElement* pText = new TextElement();
		insertSimpleNodeAtSelection(pText);

		pText->SetCharFormat(pcf);
		pText->SetText(szText, length);

		uint pos = length;
		m_selection.SetAnchorFocus(pText, CaretPos(pos));
	}
}

void  Doc::ReplaceImage(LPCTSTR szPath)
{
	ImageElement* image = new ImageElement;
	insertSimpleNodeAtSelection(image);

	if (image)
	{
		image->Load(szPath);
	}
}

bool  Doc::ReplaceCacheImage(const CCacheImage& image)
{
	if (!image)
		return false;

	ImageElement* pImage = new ImageElement;
	insertSimpleNodeAtSelection(pImage);
	if (pImage)
	{
		pImage->SetCacheImage(image);
	}

	return true;
}


IChatMessage*  Doc::BeginInsertMessage()
{
	ChatMessage* message = new ChatMessage(*this);

	// 获取当前位置的二级结点，插在后面。
	// 目前约定，message结点只出现在doc结点下来

	// 直接启用新的行，放在当前结点后面
	Node* p = m_selection.GetAnchor();
	if (p == this)
	{
		CaretPos pos = m_selection.GetAnchorOffset();
		if (0 == pos.pos)
		{
			// 插入在前面 SelectBegin 状态
			this->AddFrontChildNode(message);
		}
		else
		{
			// 插入在后面
			this->AddChildNode(message);
		}
	}
	else
	{
		while (p)
		{
			if (p->GetParentNode() == this)
				break;

			p = p->GetParentNode();
		}

		if (p)
		{
			message->InsertAfter(p);
		}
		else
		{
			this->AddChildNode(message);
		}
	}

	m_selection.SetAnchorFocus(message, CaretPos(0));
	return static_cast<IChatMessage*>(message);
}

ILink*  Doc::ReplaceLinkEx(LPCTSTR szText, const CharFormat* pcf)
{
	// 约定：链接不接收\r\n，只能在一行

	Link* link = new Link;
	insertSimpleNodeAtSelection(link);
	link->SetCharFormat(pcf);

	ReplaceTextEx(szText, nullptr);

	return static_cast<ILink*>(link);
}


// 将光标放到message后面去
void  Doc::EndInsertMessage(IChatMessage* pMsg)
{
	UIASSERT(pMsg);
	ChatMessage* msg = static_cast<ChatMessage*>(pMsg);

	// 用-1来表示光标在这个结点后面，不属于这个结点  ???????
	m_selection.SetAnchorFocus(
		static_cast<Node*>(msg), CaretPos(-1));
}

ILink*  Doc::ReplaceLink(LPCTSTR szText)
{
	return ReplaceLinkEx(szText, nullptr);
}