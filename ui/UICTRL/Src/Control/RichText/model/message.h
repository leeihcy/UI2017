#pragma once
#include "..\model\node.h"
#include "Inc\Interface\irichtext.h"
#include "include\imagecache_def.h"

namespace UI
{
    namespace RT{

// class ChatMessageHead : public Node
// {
// public:
//     virtual NodeType GetNodeType() override {
//         return NodeTypeChatMessageHead;
//     }
// };
// 
// class ChatMessagePortrait : public Node
// {
// public:
//     virtual NodeType GetNodeType() override {
//         return NodeTypeChatMessagePortrait;
//     }
// };

class ChatMessage;
class ChatMessageBubble : public Node
{
public:
    ChatMessageBubble(ChatMessage& message);
    virtual NodeType GetNodeType() override {
        return NodeTypeChatMessageBubble;
    }

    virtual void PreDraw(HDC hDC, LPCRECT rcClip) override;
    virtual void PostLayoutMultiLine(MultiLineLayoutContext*) override;
    virtual void PreLayoutMultiLine(MultiLineLayoutContext* context) override;

	virtual void OnLButtonDBClick(UINT nFlags, POINT ptDoc, bool& bHandled) override;

private:
    ChatMessage& m_message;

    CRect  m_rcContent;

	// 用于隔开消息头与气泡内容。
    int  m_nBubbleTopSpace;
    // int  m_nBubbleBottomSpace;
};

class ChatMessage : public Node, public IChatMessage
{
public:
    ChatMessage(Doc&  doc);

    virtual NodeType GetNodeType() override { return NodeTypeChatMessage; }
    virtual void PreDraw(HDC hDC, LPCRECT rcClip) override;
    virtual void PostLayoutMultiLine(MultiLineLayoutContext*) override;
    virtual void PreLayoutMultiLine(MultiLineLayoutContext* context) override;

    virtual void  SetAlign(RT::HALIGN align) override;
	virtual void  SetPortrait(const CCacheImage& portrait) override;
    virtual HANDLE  EnterBubble() override;
    virtual void  LeaveBubble(HANDLE) override;

    CRegion4  GetMessageIndent();
    
private:
    bool  m_bDrawBubbleBackground;

	CRect  m_rcPortrait;

    // 内容的缩进，用于为气泡让点位置
    int  m_nLeftIndent;
    int  m_nRightIndent;
    int  m_nTopIndent;
    int  m_nBottomIndent;

public:
    Doc&  m_doc;
    RT::HALIGN  m_hAlign;

	CCacheImage  m_portrait;
};



    }
}