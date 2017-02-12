#pragma once
#include "Inc\Interface\iedit.h"
#include "Src\Control\Caret\caret.h"
#include "Inc\Interface\irichedit.h"

#include <usp10.h>
#pragma comment(lib, "usp10.lib")

#ifndef _UNICODE 
	#error "usp10 requirs a unicode build."
#endif

#pragma comment(lib, "imm32.lib")

namespace UI
{
class Edit;

//
//	封装编辑框的数据
//
class EditData
{
public:
	EditData();
	~EditData();

public:
	void  BindToEdit(Edit* pEdit);

	void  SetText(LPCTSTR, bool bFilter, bool& bUpdate);
	void  ReplaceChar(const TCHAR& c, bool& bUpdate);
	void  ReplaceString(const String& str, bool& bUpdate);
	void  Delete(bool& bUpdate);
	void  BackSpace(bool& bUpdate);
    void  DeleteSelectionText(bool& bUpdate);
	void  SetCaret(int nCaret, bool bSetSelStart, bool& bUpdate);

	void  CutToClipboard();
	void  CopyToClipboard();
	void  PasteFromClipboard();
	void  GetPriorItemPos(int nCP, int* pPrior);
	void  GetNextItemPos(int nCP, int* pNext);

	void  SetMaxChar(long nMax);
    long  GetMaxChar();
	void  SetMaxChar2(int nMax, bool& bNeedUpdate);
	void  SetInsertMode(bool bInsertOrOverride);
	bool  GetInsertMode() { return m_bInsertMode; }
	void  SetByteLimit(bool b) { m_bByteLimit = b; }
	bool  GetByteLimit() { return m_bByteLimit; }

	void  GetText(String& str) { str = m_strText; }
	const String&  GetTextRef() { 
            return m_strText; }
	LPCTSTR  GetText() { return m_strText.c_str(); }
	int   GetTextLength() { return (int)m_strText.length(); }
	int   GetCaretIndex() { return m_nCaret; }
	int   GetSelectionLength() { return abs(m_nCaret - m_nSelStart); }
	int   GetTextWidth()  { return m_nTextWidth; }
	void  GetSelectionInfo(int& nLeft, int& nRight) const;
	void  SetSelectionInfo(int nStart, int nEnd, bool& bUpdate);
	void  GetSelectionText(String& str);
	bool  IsSelectionExist();
    bool  Clear();

	bool  CP2X(int nCP, int* pX);
	bool  X2CP(int nX, int* pnCP, int* pbTrailOrLead);
	void  DestroyStringAnalysis();

	void  SetTextFilterDigit();
	void  ClearTextFilter();

protected:
	void  DeleteSelectionText();   // 该函数仅用于内部调用,不对数据进行处理，仅删除文本

	void  Fire_Text_Changed(bool bInvokeSetText);
	TextFilterResult  FilterString(LPCTSTR szSrc, String& strDest);
	TextFilterResult  FilterChar(const TCHAR& cSrc, TCHAR& cDest);

	bool  StringAnalysis();

private:
	String  m_strText;              // 编辑框的内容
	long  m_nMaxChar;             // 允许输入的最大字符数，-1表示无限制
	bool  m_bByteLimit;           // 按字节进行限制，汉字算两个字节

	int   m_nSelStart;            // 选择的字符起点，当没有选区时	m_nSelStart==m_nCaret	
	int   m_nCaret;               // 当前光标的位置，也标志着选区的End pos

	bool  m_bInsertMode;          // 插入/重写模式

	// UniScribe相关变量
	SCRIPT_CONTROL	m_ScriptControl;		// For uniscribe
	SCRIPT_STATE	m_ScriptState;			// For uniscribe

	SCRIPT_STRING_ANALYSIS	m_Analysis;     // For cp2x, x2cp...
	int   m_nTextWidth;           // 当前字符的总长度

	ITextFilter*  m_pTextFilter; // 文字过滤。例如实现整数、小数输入等

	Edit*  m_pEdit;
};

class Edit : public MessageProxy
{
public:
	Edit(IEdit* p);
	~Edit();

	UI_BEGIN_MSG_MAP()
 		UIMSG_PAINT(OnPaint)
		UIMSG_ERASEBKGND(OnEraseBkgnd)
		MSG_WM_SETCURSOR(OnSetCursor)
		MSG_WM_MOUSEMOVE(OnMouseMove)
		MSG_WM_CHAR(OnChar)
		MSG_WM_KEYDOWN(OnKeyDown)
		MSG_WM_LBUTTONDOWN(OnLButtonDown)
		MSG_WM_LBUTTONUP(OnLButtonUp)
		MSG_WM_LBUTTONDBLCLK(OnLButtonDblClk)
		MSG_WM_RBUTTONDOWN(OnRButtonDown)
		MSG_WM_RBUTTONUP(OnRButtonUp)
		UIMSG_SETFOCUS(OnSetFocus)
		UIMSG_KILLFOCUS(OnKillFocus)
		MSG_WM_WINDOWPOSCHANGED(OnWindowPosChanged)
		UIMSG_VISIBLE_CHANGED(OnVisibleChanged)
		MSG_WM_SHOWWINDOW(OnShowWindow)
        MSG_WM_GETDLGCODE(OnGetDlgCode)
        UIMSG_HANDLER_EX(WM_IME_STARTCOMPOSITION, OnStartComposition)
		UIMSG_STATECHANGED(OnStateChanged)
		UIMSG_CREATE_ACCESSIBLE(OnCreateAccessible)
        UIMSG_INITIALIZE(OnInitialize)
        UIMSG_GETDESIREDSIZE(GetDesiredSize)
        UIMSG_QUERYINTERFACE(Edit)
        UIMSG_SERIALIZE(OnSerialize)
	UI_END_MSG_MAP_CHAIN_PARENT_Ixxx(Edit, IControl)

    IEdit*  GetIEdit() { return m_pIEdit; }

	// 消息处理
protected:
    void  OnInitialize();
	void  OnPaint(IRenderTarget* hDC);
	void  OnEraseBkgnd(IRenderTarget* pRenderTarget);
	BOOL  OnSetCursor(HWND hWnd, UINT nHitTest, UINT message);
	void  OnSetFocus(IObject*);
	void  OnKillFocus(IObject*);
	void  OnWindowPosChanged(LPWINDOWPOS);
	void  OnShowWindow(BOOL bShow, UINT nStatus);
    UINT  OnGetDlgCode(LPMSG lpMsg);
	void  OnVisibleChanged(BOOL bVisible, IObject* pParent);
    LRESULT  OnStartComposition(UINT uMsg, WPARAM wParam, LPARAM lParam);
	void  OnStateChanged(UINT nMask);
	void  OnCreateAccessible(IUIAccessibleCreator*);

	void  OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	void  OnLButtonDown(UINT nFlags, POINT point);
	void  OnLButtonUp(UINT nFlags, POINT point);
	void  OnMouseMove(UINT nFlags, POINT point);
	void  OnLButtonDblClk(UINT nFlags, POINT point);
	void  OnRButtonDown(UINT nFlags, CPoint point);
    void  OnRButtonUp(UINT nFlags, CPoint point);

	void  OnInputChar(UINT nChar);
	void  OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	void  OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	void  OnKeyDown_Ctrl_A();
	void  OnKeyDown_Ctrl_X();
	void  OnKeyDown_Ctrl_C();
	void  OnKeyDown_Ctrl_V();
	void  OnKeyDown_Ctrl_Z();
	void  OnKeyDown_Ctrl_Y();
	void  OnKeyDown_Backspace(bool bCtrlDown);
	void  OnKeyDown_Delete(bool bCtrlDown);
	void  OnKeyDown_Left_Top(bool bCtrlDown);
	void  OnKeyDown_Right_Down(bool bCtrlDown);
	void  OnKeyDown_Home(bool bCtrlDown);
	void  OnKeyDown_End(bool bCtrlDown);
	void  OnKeyDown_Insert(bool bCtrlDown);
    void  OnKeyDown_Return();
    void  OnKeydown_Esc();
    void  OnKeydown_Tab();
   
    void  GetDesiredSize(SIZE* pSize); 
    void  OnSerialize(SERIALIZEDATA* pData);

	virtual void virtualShowContentMenu(CPoint point);

public:
	int   GetTextLength();
    LPCTSTR  GetText();
	void  SetText(LPCTSTR szText);
	void  SetTextNoFilter(LPCTSTR szText);
	int   GetEditType();
	void  SetEditType(int n);
	long  GetLimit();
	void  SetLimit(long);
	void  SetByteLimit(bool b);

    void  DeleteSelectionText(bool bUpdate);
    bool  DoCut();
    bool  DoCopy();
    bool  DoPaste();
	void  SetSel(int nStartChar, int nEndChar);
	void  SetSel2(int nStartChar, int nSelLength);
	void  GetSel(int& nStartChar,int& nEndChar) const;
	void  GetSel2(int& nStartChar, int& nSelLength) const;
    void  Clear();
	void  SelectAll();

    bool  IsReadOnly();
    void  SetReadOnly(bool b);
	bool  IsRealFocus();
    void  SetWantTab(bool b);
	
	void  SetTextFilterDigit();
	void  ClearTextFilter();

protected:
	void  EnterFocus();
	void  ExitFocus();
    long  GetTextHeight();
	
private:
	void  DrawNormal(IRenderTarget* pRenderTarget);
	void  DrawFocus(IRenderTarget* pRenderTarget);

	void  CalcCaretPos(int nCaretIndex, bool& bUpdate);
	void  UpdateCaretByPos();
    long  GetCaretHeight();

	void  setText(LPCTSTR szText, bool bFilter, bool bUpdate);
    void  loadThemeEditBkgRender();

//     void  LoadColor(LPCTSTR szColor) 
//     {
//         m_pIEdit->LoadColor(szColor, m_pColor); 
//     }
//     LPCTSTR  SaveColor() 
//     {
//         return m_pIEdit->SaveColor(m_pColor);
//     }
//     void  LoadSelectColor(LPCTSTR szColor)
//     {
//         m_pIEdit->LoadColor(szColor, m_pColorSelect); 
//     }
//     LPCTSTR  SaveSelectColor() 
//     {
//         return m_pIEdit->SaveColor(m_pColorSelect);
//     }
//     void  LoadSelectBkgColor(LPCTSTR szColor)
//     {
//         m_pIEdit->LoadColor(szColor, m_pColorSelectBk); 
//     }
//     LPCTSTR  SaveSelectBkgColor()
//     {
//         return m_pIEdit->SaveColor(m_pColorSelectBk);
//     }

	// 属性
protected:
	EditData  m_editData;
    IEdit*  m_pIEdit;

	// 数据
private:
    // 控件有焦点不代表edit就能编辑，例如instantedit，
	bool  m_bRealFocus;             

    // 是否正在用鼠标进行拖动选择
	bool  m_bMouseDrag;			

    // 是否需要在下次刷新的时候更新光标位置
	bool  m_bNeedUpdateCaretPos;    

	long  m_nXScroll;	            // 字符滚动的长度
	long  m_nXCaretPos;				// 光标的位置，基于padding.left

    // 光标高度不影响文本绘制位置，但影响选中的背景高度
    // 文字居中绘制，m_nTextHeight影响文本居中位置
    long  m_nCaretHeight;			// 光标的高度
    long  m_nTextHeight;            // 字体对应的文本高度，用'1'来测量

	friend class EditData;

	// 绘制
protected:
	Color*   m_pColor;
	Color*   m_pColorSelect;
	Color*   m_pColorSelectBk;
	CCaret   m_caret;     // 用于实现分层窗口上的光标显示

	// long  m_lEditType;   // edit控件类型，例如嵌套在combo中的edit

	EDITSTYLE  m_editStyle;

    // 对齐方式，目前只为instantEdit实现了一部分
    EDITTEXTALIGN  m_textAlign;

    // event
public:
    // 0表示由用户输入触发，1表示由调用api(settext等)触发 bInvokeBySetText
#if _MSC_VER >= 1800    
	signal<IEdit*, bool>  en_change;

	// wParam: RETURN/ESC/TAB
	// return: 1已处理，0未处理
	signal_r<bool, IEdit*, UINT>  keydown;
#else
	signal2<IEdit*, bool>  en_change;

	// wParam: RETURN/ESC/TAB
	// return: 1已处理，0未处理
	signal_r2<bool, IEdit*, UINT>  keydown;
#endif

    
};
}
