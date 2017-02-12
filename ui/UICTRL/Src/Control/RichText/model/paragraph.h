#pragma once
#include "element\element.h"
#include "layout_context.h"


//
// 如何实现message的内容结构？
//  方案1. message对象维护两个unit指针，begin/end，在这中间的都属于该message
//  文案2. 将message作为一个特殊的unit，也插入到内容当中，这这两个msg unit中间的，都
//        属于同一个message
//
//  方案1需要维护的东西更多，在内容更新上面比较麻烦，例如要在内容最后或者最前插入新内容就不好实现
//  。这里采用方案2来实现，一个message分为两个unit，将内容包起来。
//

namespace UI
{
namespace RT{

class ChatDoc;
class Paragraph;
class ParagraphTail;


class Paragraph : public RT::Node, public RT::IParagraph
{
public:
    Paragraph();

    ~Paragraph();

    virtual NodeType  GetNodeType() override { return NodeTypeParagraph; }

    virtual void  SetAlign(RT::HALIGN align) override;
    virtual void  SetLeftIndent(int indent) override;
    virtual void  SetRightIndent(int indent) override;
    virtual void  SetTopSpace(int space) override;
    virtual void  SetBottomSpace(int space) override;
    virtual void  SetCharformat(CharFormat*) override;
    virtual void  ShowBubbleBackgournd(bool b) override{};

    virtual const CharFormat*  GetCharFormat() override;
    virtual void  ToString(
            CaretPos start, CaretPos end, int format, String& out) override;
//     virtual void  LayoutMultiLine(
//         RT::MultiLineLayoutContext* pContext) override;

    //void  CreateDefaultTail();

protected:
    virtual void PreLayoutMultiLine(MultiLineLayoutContext*) override;
    virtual void PostLayoutMultiLine(MultiLineLayoutContext*) override;

private:
    // 段落对齐方式
    RT::HALIGN  m_hAlign;
    // 段间距
    int  m_nTopSpace;
    int  m_nBottomSpace;
    int  m_nLeftIndent;
    int  m_nRightIndent;
    // 行间距
    int  m_nLineSpace;
    // 段落的默认字体
    CharFormat*  m_pcf;  
};
#if 0
class Paragraph : 
{
public:
    Paragraph(ChatDoc& doc);

    virtual void  SetAlign(RT::HALIGN align) override;
    virtual void  ShowBubbleBackgournd(bool b) override;

    RT::Unit*  CreateHeadUnit();
    RT::Unit*  CreateTailUnit();

    void  PostLayout();
    void  PreDraw(HDC hDC);

    CRegion4  GetMessageIndent();

private:
    RT::Line*  GetHeadLine();
    RT::Line*  GetTailLine();
    
private:
    ChatDoc&  m_doc;

    bool  m_bDrawBubbleBackground = true;
    CRect  m_rcBubble;

    // 气泡内容的缩进，用于为气泡让点位置
    CRegion4  m_rcMsgIndent;

    // 生命周期，由chatmessage创建, doc负责销毁，
    ChatMessageHead*  m_pHead = nullptr;
    ChatMessageTail*  m_pTail = nullptr;

    RT::HALIGN  m_hAlign;
};
#endif
}
}