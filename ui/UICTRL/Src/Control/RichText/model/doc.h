#pragma once
#include "Inc\Interface\irichtext.h"
#include "node.h"
#include "style.h"
#include "selection.h"
#include "mousekey.h"
#include "layout_context.h"
#include "..\UISDK\Inc\Util\iimage.h"


//
// richtext对象模型：
// 内容对象  
//  Doc, Element, Paragraph
// 布局对象
//  Page, Line, Run
//
// Doc：代表一个RichText，维护page列表
// Unit: 代表外部插入的一个内容单元，如超链接，作为一个完整的unit对象统一刷新、点击。// 维护Element列表。
//       或者是作为一个整体进行光标操作，例如收件人列表，
// Element: 被\r\n、uniscribe将一个Unit内容拆分为多个Element。Element将被遍历用来布局
// Page: 富文本的一页。维护Line列表
// Line: 一行。维护Run列表
// Run: 一个Element在布局时拆分出来的单元。如果一行装不下一个Element，将被拆分为多个Run
// Param: 用于内容排版，设置段落格式
//
// Element 有两种： TextElement/ObjectElement
// Unit 有两个派生类： TextUnit/ObjectUnit，其它类型的unit直接从这两个unit类中派生
// 
namespace UI
{
namespace RT
{
class Page;
class Element;
class TextElement;
class ImageUnit;
class Line;
class Doc;
class Paragraph;

class Doc : public Node
{
public:
	Doc();
	~Doc();

    virtual NodeType  GetNodeType() override { return NodeTypeDoc; }
    virtual const CharFormat*  GetCharFormat() override;

	void  Init(RichText* pObj);
	void  Clear();
	RichText*  GetControl();
    IRichText*  GetIControl();
    Style&  GetStyle();
    Selection&  GetSelection();
    MouseKey&  GetMouseKey();
    Paragraph*  GetSelectionAnchorParagraph();
	IScrollBarManager*  GetScrollBarManager();

    PageMode  GetPageMode();
    EDITABLE    GetEditMode();
    void  SetPageMode(PageMode e);
    void  SetEditMode(EDITABLE e);

	void  Render2DC(HDC hDC, LPCRECT rcClip);
    void  LayoutAll(uint nControlWidth, uint nControlHeight);
    SIZE  GetDesiredSize();

    void  CopyDefaultCharFormat(CharFormat* pcf);
	void  CopyDefaultLinkCharFormat(CharFormat* pcf);
	const CharFormat*  GetDefaultCharFormat();
	const CharFormat*  GetDefaultLinkCharFormat();

    void  InvalidRegion(RECT* prcInDoc);
    void  ClearInvlidRegion();
    void  Refresh();
	void  ReleasePage();

	void  SetDefaultFont(CharFormat* pcf);
	void  SetDefaultLinkFont(CharFormat* pcf);
	void  CreateDefaultFont();
	void  CreateDefaultLinkFont();
     
    // selection 操作
    void  CopySelection();
    void  SelectAll();
	void  SelectEnd();
	void  SelectBegin();
    void  SetNeedUpdateElementIndex();
    void  UpdateElementIndex();
	void  ScrollEnd();

    void  ReplaceText(LPCTSTR szText);
    void  ReplaceTextEx(LPCTSTR szText, const CharFormat* pcf);
    ILink*  ReplaceLink(LPCTSTR szText);
    ILink*  ReplaceLinkEx(LPCTSTR szText, const CharFormat* pcf);
    void  ReplaceImage(LPCTSTR szPath);
	bool  ReplaceCacheImage(const CCacheImage& image);
	void  DeleteParagraph(Paragraph*);

    IChatMessage*  BeginInsertMessage();
    void  EndInsertMessage(IChatMessage*);

    Page*  GetFirstPage();
    Page*  GetLastPage();
    Page*  PageHitTest(POINT ptDoc, __out POINT* ptInPage);

	// style
	void  SetStyle_LineSpace(int linespace);

	BOOL  ProcessMessage(UIMSG*, int, bool);
	void  OnNodeRemove(Node* p);

private:
    void  replaceTextNoCarriageReturn(
            LPCTSTR szText, uint length, const CharFormat* pcf);
	void  insertSimpleNodeAtSelection(Node*);
	void  insertParaNodeAtSelection(Paragraph*);

	void  SetContentSize(int cx, int cy);
	void  SetViewSize(int cx, int cy);

private:
	void  initBlank();
	
	virtual void  PreLayoutAll();
	void  layoutAllSingleLine(
            uint nControlWidth,
            uint nControlHeight
            );
    void  layoutAllSingleLineEachUnit(
            Unit* pUnit,
            SingleLineLayoutContext* context
            );
	void  layoutAllMultiLine(
            uint nControlWidth,
            uint nControlHeight
            );
	void  layoutAllMultiPage(
            uint nControlWidth, 
            uint nControlHeight
            );
    virtual void  PostLayoutAll() {};
    virtual void  PreRender2DC(HDC) {};
    virtual void  PostRender2DC(HDC) {}

	// bool  layout_all_multiline_imageelement(MultiLineLayoutContext* pContext);
    // void  append_text(LPCTSTR szText, TextUnit* pUnit, RichTextCharFormat* pcf);


protected:
    RichText*  m_pRichText;

    // 字体
	CharFormat  m_defaultcf;  
	CharFormat  m_defaultcf_link;  
    Style  m_style; 

	Page*  m_pFirstPage;
    PageMode  m_ePageMode;

    EDITABLE  m_eEditMode;
    Selection  m_selection;
    MouseKey  m_mousekey;

	// 内容大小
	SIZE  m_sizeContent;
	SIZE  m_sizeView;
	IScrollBarManager*   m_pMgrScrollBar;

    // 无效区域
    enum InvalidState
    {
        InvalidStateEmpty,
        InvalidStateFull,
        InvalidStatePart,
    };
    InvalidState  m_eInvalidState;
    RECT  m_rcInvalidInDoc;  // 全0表示无刷新，全-1表示全部刷新，其它表示刷新刷新

    // 为每个element排序，方便比较两个element的前后顺序
    // 在Element类构造函数中设置该值，目前只考虑每次新增一个element时更新。
    bool  m_bNeedUpdateElementIndex;

public:
    ImageWrap  m_imageLeftBubble;
    ImageWrap  m_imageRightBubble;
    
};

}
}