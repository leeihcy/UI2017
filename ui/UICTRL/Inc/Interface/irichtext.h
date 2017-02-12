#ifndef _UI_IRICHTEXT_H_
#define _UI_IRICHTEXT_H_
#include "..\..\..\UISDK\Inc\Interface\icontrol.h"

namespace UI
{
	class CCacheImage;

struct CharFormat
{
    // HFONT参数
    TCHAR  szFontName[LF_FACESIZE];
    long  lFontSize;
    bool  bBold;
    bool  bItalic;
    bool  bUnderline;
	bool  bDeleteline;

    // 其它参数
    long  textColor;
    long  backColor;
    bool  bTextColor;  // 是否启用文字色textColor
    bool  bBackColor;  // 是否启用背景色backColor

    // 内部使用
    HFONT  hFont;
    unsigned int  nFontHeight;
    unsigned int  nFontDescent;  // 用于下划线的绘制坐标计算
};


enum /*class*/ PageMode
{
    PageMode_singleline,
    PageMode_multiline,
    PageMode_multipage,
    PageMode_error,
};

namespace RT
{
// 目前只实现了单行模式
enum /*class*/ VALIGN // : int
{
    VALIGN_top,
    VALIGN_bottom,
    VALIGN_center,
};

enum /*class*/ HALIGN // : int
{
    HALIGN_left,
    HALIGN_right,         // 每一行内容都右对齐
    HALIGN_center,        // 每一行内容都居中对齐
    HALIGN_paraRight,     // 整个段落右对齐，但段中的行还是左对齐
    HALIGN_paraCenter,    // 整个段落居中对齐，但段中的行还是左对齐
};

// 编辑模式
enum EDITABLE
{
	EDITABLE_Disable,   // 不支持编辑，不显示光标，不支持复制
	EDITABLE_Readonly,  // 显示光标，支持复制（未实现）
	EDITABLE_Enable,    // 支持编辑（未实现）
};

interface IParagraph
{
    virtual void  SetAlign(RT::HALIGN align) = 0;
    virtual void  SetLeftIndent(int indent) = 0;
    virtual void  SetRightIndent(int indent) = 0;
    virtual void  SetTopSpace(int space) = 0;
    virtual void  SetBottomSpace(int space) = 0;
    virtual void  SetCharformat(CharFormat*) = 0;
    virtual void  ShowBubbleBackgournd(bool b) = 0;
};

interface IChatMessage
{
    virtual void  SetAlign(RT::HALIGN align) = 0;
	virtual void  SetPortrait(const CCacheImage& portrait) = 0;
    virtual HANDLE  EnterBubble() = 0;
    virtual void  LeaveBubble(HANDLE) = 0;
};

// interface INode
// {
//     virtual IParagraph*  GetParagraphNode() = 0;
// };

// interface ISelection
// {
//     virtual INode*  GetAnchorNode() = 0;
//     virtual INode*  GetFocusNode() = 0;
// };


interface ILink
{
	virtual int  GetType() = 0;
	virtual void  SetType(int) = 0;

    virtual LPCTSTR  GetId() = 0;
    virtual void  SetId(LPCTSTR) = 0;

    virtual LPCTSTR  GetToolTip() = 0;
    virtual void  SetToolTip(LPCTSTR) = 0;

    virtual LPCTSTR  GetReference() = 0;
    virtual void  SetReference(LPCTSTR) = 0;

	virtual IParagraph*  GetParagraph() = 0;
};

interface IImageElement
{
	virtual LPCTSTR  GetPath() = 0;
};

}


#ifdef ReplaceText
#undef ReplaceText
#endif

class CCacheImage;
class RichText;
interface UICTRL_API_UUID(F1B67364-BA72-44CD-BA63-53DA6B23CB4A) IRichText : public IControl
{
    void  Clear();
    void  SetText(LPCTSTR szText);
    void  GetDefaultCharFormat(CharFormat* pcf);
	void  SetDefaultCharFormat(CharFormat* pcf);
	void  GetDefaultLinkCharFormat(CharFormat* pcf);
	void  SetDefaultLinkCharFormat(CharFormat* pcf);
    
	void  LoadHtml(LPCTSTR szHtml);

	void  SelectEnd();
	void  SelectBegin();
	void  ScrollEnd();

    void  ReplaceText(LPCTSTR szText);
    void  ReplaceTextEx(LPCTSTR szText, CharFormat* pcf);
    void  ReplaceImage(LPCTSTR szPath);
	void  ReplaceCacheImage(const CCacheImage& image);
    RT::ILink*  ReplaceLink(LPCTSTR);
    RT::ILink*  ReplaceLinkEx(LPCTSTR, CharFormat* pcf);
    RT::IChatMessage*  BeginInsertMessage();
    void  EndInsertMessage(RT::IChatMessage*);
	void  DeleteParagraph(RT::IParagraph*);

    void  SetPageMode(PageMode e);
    PageMode  GetPageMode();
	void  SetEditMode(RT::EDITABLE e);
	RT::EDITABLE  GetEditMode();

    //RT::ISelection*  GetSelection();
    RT::IParagraph*  GetSelectionAnchorParagraph();

    void  SetStyle_PageContentVAlign(RT::VALIGN);
    void  SetStyle_PageContentHAlign(RT::HALIGN);
    void  SetStyle_DrawLinkUnderline(bool b);
	void  SetStyle_LineSpace(int linespace);


	signal2<IRichText*, RT::ILink*>&  LinkClickedEvent();
	signal2<IRichText*, RT::IImageElement*>&  ImageDBClickEvent();

	UI_DECLARE_INTERFACE_ACROSSMODULE(RichText);
};


}

#endif // _UI_IRICHTEXT_H_