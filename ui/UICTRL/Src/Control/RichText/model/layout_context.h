#pragma once
#include "Inc\Interface\irichtext.h"

namespace UI
{
namespace RT
{
class Doc;
class Page;
class Line;
class Element;
class Run;
class Paragraph;

struct SingleLineLayoutContext
{
public:
    Doc*   pDoc;
    Page*  pPage;
    Line*  pLine;
    uint   xLineCursor;  // 在布局过程中，该Line布局到哪里了

    void  PushElement(Element*);
};

// 多行排版时的上下文
struct MultiLineLayoutContext
{
public: 
    Doc*   pDoc;
    Page*  pPage;             // 当前页
    Line*  pLine;             // 当前行
    Paragraph*  pParagraph;
    Element*  pElement;
    Run*   pRun;

    uint  nPageContentWidth;   // 页面内容大小
    uint  nPageContentHeight;

    int   nLineSpace;          // 行间距
    int   nLeftIndent;         // 布局过程中的左侧缩进，由message/paragraph负责动态调整
    int   nRightIndent;        // 布局过程中的左侧缩进，由message/paragraph负责动态调整

    uint  yLine;               // 该行的纵向超始位置
    uint  xLineOffset;         // 该行run横向起始位置，不包含left indent

public:
    void  BeginLayoutElement(Element*);
    void  EndLayoutElement(Element*);
    void  PushChar(uint Index, int width);

private:
    void  checkNewLine();

public:
    void  AppendNewLine();
    void  SetLineFinish();
    int   GetLineContentWidth();
    int   getLineRemainWidth();
};

// 二次布局时的参数
struct MultiLineLayoutContext2nd
{
    uint nPageWidth;
    uint nPageLeftIndent;
    uint nPageRightIndent;
};

}
}