#pragma once
#include <richedit.h>
#include <textserv.h>
#include <RichOle.h>
#include <tom.h>

#include "UI\UICTRL\Src\Control\Caret\caret.h"
#include "UI\UICTRL\Inc\Interface\irichedit.h"


//#pragma comment(lib, "Riched20.lib")  // -- vs2012上编译不过
typedef HRESULT (_stdcall *pfuncCreateTextServices)
(
    IUnknown *punkOuter,
    ITextHost *pITextHost, 
    IUnknown **ppUnk
);

//  RichEdit及其Callback的实现，可以参考MFC CRichEditView
//  RichEidt的ole view的实现，可以参考ATL插入一个控件对象的实现代码

// 
//
//	Q1. 无窗口RE是怎么创建的？
//		a. 调用CreateTextSevices()
//      b. ??
//
//  Q2. 无窗口RE是怎么绘制到窗口上面的？
//		a. 拿到一个内存DC?
//		b. 调用itextservices->txdraw方法，随便提供一个clientrect，这样就能在该窗口区域上显示一个白色背景了
//		c. 
//
//	Q3. 无窗口RE是怎么决定自己在窗口上的范围的？
//		a. Draw的时候给出的clientrect就是显示的范围
//		b. 然后在其它时候判断当前鼠标是否位于窗口内或者返回TxGetClientRect
//	
//	Q4. 怎么输入文字？
//		a. 将WM_CHAR消息传递给service text即可。需要注意的是模态对话框是收不到WM_CHAR消息的。
//
//	Q5. 输入文字后是怎么刷新显示的？
//		a. 内部将调用TxInvalidateRect接口，在该接口中，我们自己去实现为::InvalidateRect
//
//	Q6. 怎么设置它的样式？例如多行编辑功能？
//		a. TxGetPropertyBits 返回的
//
//	Q7. 当有选中区域时，发现光标的位置显示不太准确，怎么才能在有选中区域时不显示光标？
//		a. 其实这个时候TxCreateCaret(HBITMAP hbmp 这个hbmp是不为空的，使用它创建的caret就不会显示。why?
//
//  Q8. 为什么我用CCaretWindow实现光标时，不能做到输入文字时将系统光标隐藏？
//		
//
//	Q9. 光标是怎么销毁的？
//		a. 向text service传递kill focus消息，而且还得传递set focus消息，否则再次得到焦点时光标不显示。
//      b. re内部收到wm_killfoucs时，会调用::destroycaret
//
//  Q10.为什么我向textservice发送SETFOCUS消息后，却不会响应TxCreateCaret消息
//		a. 因为没有调用OnTxInPlaceActivate
//
//	Q11.在控件位置改变后，怎么去同步光标的位置？
//      a. 向txt service发送一个通知：m_spTextServices->OnTxPropertyBitsChange(TXTBIT_CLIENTRECTCHANGE, TRUE);
//
//  Q12.为什么拖拽时，刷新会出现异常？先添加后删除，但是分开刷新的
//      这应该也是刷新区域未处理的原因
//
//  Q13.如何显示滚动条？
//      在TxGetScrollBars中，返回你的m_dwStyle样式。因此需要在m_dwStyle中指定
//      WM_HSCROLL,WM_VSCROLL...
//
//  Q14.OLE的生命周期.
//      因为richedit的undo功能，对象会一直保存在内存中，直到richedit关闭。????
//
//  Q15.Rich Edit 剪贴板操作 
//
//		Rich Edit控件注册两种剪贴板格式："富文本格式(RTF)"和一种叫做"RichEdit 文本与对象"的格式。应用程序
//      可以使用RegisterClipboardFormat函数来注册这些格式，其取值为CF_RTF与CF_RETEXTOBJ
//
//   Q16.IOleRichEditCallBack中的QueryAcceptData和QueryInsertData有什么区别?
//       
//      QueryInsertData针对调用了richedit的InsertObject插入一个OLE时触发。
//      QueryAcceptData是在粘贴和拖拽时将会触发。
//
//   Q17.如果提高richedit的绘制效率
//       1. 注意txinvalidaterect中的第一个参数，在输入文字时，prc会只是一行的区域，而不是整个client rect.
//          在分层窗口上绘制时可以根据这个进行剪裁刷新
//
//   Q18.回车时滚动条闪烁怎么回事？
//       详见WindowlessRichEdit::TxSetScrollPos函数注释
//
//   Q19. 怎么隐藏显示windowless richedit
//      
//   Q20. 处理WM_IME_COMPOSITION这个消息会导致输入法输入一个字按回车后居然显示两个字
//        是因为在对话框窗口中返回了一个0，导致对话框会再去调用默认的dialog proc，导致多发出来一个WM_CHAR消息
//   Q21. 微软拼音输入法的问题
//
//        . 输入时不显示输入窗口
//        . 或者只显示窗口模式，而不是虚线模式
//        这两个问题是WM_IME_COMPOSITION处理不对。第一个是由于没有这个消息传递给默认的窗口过程导致。
//        第二个是由于没有将WM_IME_COMPOSITION消息传递给RE导致，
//        也有可能是没有调用OnTxInPlaceActivate导致
//
//        . 显示虚线模式，但输入列表没有跟随
//          RE没有处理WM_IME_REQUEST导致
//
//        . google拼音的候选列表位置不正确
//        . 极品五笔不显示输入小窗口，只显示候选小窗口
//          原因：有些ime消息，txsendmessage会返回S_FALSE，此时需要将消息继续传递给DefWindowProc
//        . 将WM_IME_COMPOSITION消息继续传递给DefWindowProc之后，发现除了微软拼音，其它的输入法又重复上字了。
//          一个是在WM_IME_COMPOSITION中，一个是在WM_CHAR中
//          原因：未将WM_IME_CHAR消息传递给RE处理
//
//   Q22. RE现在使用什么样的坐标系？光标呢
//        RE的坐标现在是相对于窗口的。光标SetPos在RE中传递的也是窗口，但绘制时要转换为object client
//
//   Q23. RE大小改变时，会出现内容不在最底端的问题，什么原因？
//        RE大小改变时，内容中的图片（不分行，多张图片）会闪烁？
//
//   Q24. 如何禁止richedit文字拖拽
//         ES_NOOLEDRAGDROP <--> TXTBIT_DISABLEDRAG 
//
namespace UI
{

class  RichEdit;
class  REOleBase;
class  REOleManager;
class  REBubble;
class  BubbleManager;

class ITextHostImpl : public ITextHost
{
public:
	ITextHostImpl();
	~ITextHostImpl();
protected:
	void Destroy();
public:

	// ITextHost Interface
	virtual HDC 		TxGetDC();
	virtual INT			TxReleaseDC(HDC hdc);
// 	virtual BOOL 		TxShowScrollBar(INT fnBar, BOOL fShow);
// 	virtual BOOL 		TxEnableScrollBar (INT fuSBFlags, INT fuArrowflags);
// 	virtual BOOL 		TxSetScrollRange(INT fnBar,LONG nMinPos,INT nMaxPos,BOOL fRedraw);
// 	virtual BOOL 		TxSetScrollPos (INT fnBar, INT nPos, BOOL fRedraw);
//	virtual void		TxInvalidateRect(LPCRECT prc, BOOL fMode);
	virtual void 		TxViewChange(BOOL fUpdate);
//	virtual BOOL		TxShowCaret(BOOL fShow);
// 	virtual BOOL 		TxSetTimer(UINT idTimer, UINT uTimeout);
// 	virtual void 		TxKillTimer(UINT idTimer);
	virtual void		TxScrollWindowEx (INT dx,INT dy,LPCRECT lprcScroll,LPCRECT lprcClip,HRGN hrgnUpdate,LPRECT lprcUpdate,UINT fuScroll);
	virtual void		TxSetCapture(BOOL fCapture);
//	virtual void		TxSetFocus();
	virtual void 	    TxSetCursor(HCURSOR hcur, BOOL fText);
	virtual BOOL 		TxScreenToClient (LPPOINT lppt);
	virtual BOOL		TxClientToScreen (LPPOINT lppt);
	virtual HRESULT		TxActivate( LONG * plOldState );
	virtual HRESULT		TxDeactivate( LONG lNewState );
//	virtual HRESULT		TxGetClientRect(LPRECT prc);
//	virtual HRESULT		TxGetViewInset(LPRECT prc);
	virtual HRESULT 	TxGetCharFormat(const CHARFORMATW **ppCF );
	virtual HRESULT		TxGetParaFormat(const PARAFORMAT **ppPF);
	virtual COLORREF	TxGetSysColor(int nIndex);
//	virtual HRESULT		TxGetBackStyle(TXTBACKSTYLE *pstyle);
	virtual HRESULT		TxGetMaxLength(DWORD *plength);
	virtual HRESULT		TxGetScrollBars(DWORD *pdwScrollBar);
	virtual HRESULT		TxGetPasswordChar(TCHAR *pch);
	virtual HRESULT		TxGetAcceleratorPos(LONG *pcp);
	virtual HRESULT		TxGetExtent(LPSIZEL lpExtent);
	virtual HRESULT 	OnTxCharFormatChange (const CHARFORMATW * pcf);
	virtual HRESULT		OnTxParaFormatChange (const PARAFORMAT * ppf);
	virtual HRESULT		TxGetPropertyBits(DWORD dwMask, DWORD *pdwBits);
	// Far East Methods for getting the Input Context
	//#ifdef WIN95_IME
	virtual HIMC		TxImmGetContext();
	virtual void		TxImmReleaseContext( HIMC himc );
	//#endif
	virtual HRESULT		TxGetSelectionBarWidth (LONG *lSelBarWidth);

	// 外部设置方法 （部分参考microsoft windowlessRE工程）
	bool  IsPasswordMode() { return m_fPassword; }
	void  SetPasswordMode(bool b);
	WCHAR SetPasswordChar(WCHAR chPasswordChar);
	LONG  SetAccelPos(LONG l_accelpos);
    bool  SetCharFormatByLogFont(LOGFONT* plf, COLORREF color);
	bool  SetCharFormat(CHARFORMAT2* pcf);
	void  SetCharFormatEx(UINT nPart, CHARFORMAT2* pcf);
    void  SetSelectionCharFormat(CHARFORMAT2* pcf);
    void  SetSelectionParaFormat(PARAFORMAT2* ppf);
    void  GetDefaultCharFormat(CHARFORMAT2* pcf);
    void  CopyDefaultCharFormat(CHARFORMAT2* pcf);
    void  CopyDefaultParaFormat(PARAFORMAT2* ppf);
    void  ResetSelectionCharFormat();
    void  ResetSelectionParaFormat();

	void  InitDefaultParaFormat();
    void  SetDefaultParaFormat(PARAFORMAT2* ppf);
    void  SetDefaultParaAlign(long eAlign/* = PFA_LEFT*/);
	bool  IsWordWrap() { return m_fWordWrap; }
	void  SetWordWrap(bool fWordWrap);
	DWORD GetMaxLength() { return m_dwMaxLength; }
	void  SetMaxLegnth(DWORD dw);
	LONG  GetSelBarWidth();
	LONG  SetSelBarWidth(LONG l_SelBarWidth);
	bool  GetRichTextFlag() { return m_fRich; }
    void  SetMultiLine(bool b);
    bool  IsMultiLine();
    void  EnableUpdate(bool b);
    void  EnableDrag(bool b);

    LRESULT GetLangOptions();
    bool  IsDualFont();
    void  SetDualFont(bool useDualFont);

	IRichEditOle* GetRichEditOle() { return m_spOle; }
    LRESULT  RESendMessage(UINT uMsg, WPARAM wParam = 0, LPARAM lParam = 0, LRESULT* pInvokeResult=NULL);
protected:

	// unknown attribute
// 	SIZE    m_sizeExtent;        // text service 用来实现缩放的参数。Each HIMETRIC unit corresponds to 0.01 millimeter.
	int     m_nxPerInch;
	int     m_nyPerInch;    
	LONG	m_laccelpos;         // Accelerator position

	// 已知属性
	DWORD   m_dwStyle;           // 编辑框样式
	WCHAR	m_chPasswordChar;    // Password character, TODO: 该接口未测试过
	DWORD   m_dwMaxLength;       // 最大输入内容长度
	LONG    m_lSelBarWidth;      // 类似于VS的左侧（显示行数字），专门用于点击选中一行的东东
	
	unsigned  m_fWordWrap:1; // Whether control should word wrap
	unsigned  m_fRich:1;     // Whether control is rich text
	unsigned  m_fRegisteredForDrop:1; // whether host has registered for drop
	unsigned  m_fPassword:1; // 
    unsigned  m_fShowSelectionInactive:1;
    unsigned  m_fDisableDrag:1;

	CHARFORMAT2W  m_cf;          // Default character format
	PARAFORMAT2   m_pf;          // Default paragraph format

	//  其它资源、数据
	CComPtr<ITextServices>   m_spTextServices;
	CComPtr<IRichEditOle>    m_spOle;
    CComPtr<ITextDocument2>  m_spTextDoc;

	HWND   m_hParentWnd; // 所在的窗口句柄
    HDC    m_hDCCache;   // 避免每次都getdc
};

interface ITextEditControl
{
	virtual BOOL ProcessWindowMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lResult, DWORD dwMsgMapID = 0) = 0;
};

 
// 条件判断
#define PRE_HANDLE_MSG() \
	{ \
		SetMsgHandled(TRUE); \
		lResult = OnPreHandleMsg(hWnd,uMsg,wParam,lParam); \
		if(IsMsgHandled()) \
			return FALSE; \
	}

// 默认处理
#define POST_HANDLE_MSG() \
	{ \
		SetMsgHandled(TRUE); \
		lResult = OnPostHandleMsg(hWnd,uMsg,wParam,lParam); \
		if(IsMsgHandled()) \
			return TRUE; \
	}

class  WindowlessRichEdit : public ITextHostImpl
	                           , public ITextEditControl
	                           , public IRichEditOleCallback
                               , public ICaretUpdateCallback
{
public:
	WindowlessRichEdit();
	~WindowlessRichEdit(void);

public:
	//////////////////////////////////////////////////////////////////////////
	// 处理从父窗口中转发过来的消息

	BEGIN_MSG_MAP_EX(WindowlessRichEdit)
		PRE_HANDLE_MSG()
		MSG_WM_SETCURSOR( OnSetCursor )
		MSG_WM_KILLFOCUS( OnKillFocus )
        MSG_WM_SETFOCUS( OnSetFocus )
        MSG_WM_SIZE( OnSize )

		MESSAGE_HANDLER_EX( WM_KEYDOWN,  OnKeyDown )
		MESSAGE_HANDLER_EX( WM_CHAR,     OnChar )
		MESSAGE_RANGE_HANDLER_EX( WM_MOUSEFIRST, WM_MOUSELAST, OnMouseMessage )
        MESSAGE_HANDLER_EX( WM_MOUSELEAVE, OnMouseMessage )
		MESSAGE_HANDLER_EX( WM_VSCROLL,  OnScroll )
		MESSAGE_HANDLER_EX( WM_HSCROLL,  OnScroll )
        MESSAGE_HANDLER_EX( UI_MSG_DROPTARGETEVENT, OnUIDropTargetEvent )
		MESSAGE_HANDLER_EX( WM_TIMER,    OnTimer )
        
        // 必须将WM_IME_STARTCOMPOSITION传递给RE，否则微软拼音输入法不能正常使用（虚线模式不启用）
        MESSAGE_HANDLER_EX( WM_IME_STARTCOMPOSITION, OnDefaultHandle )
        MESSAGE_HANDLER_EX( WM_IME_ENDCOMPOSITION, OnDefaultHandle )
        MESSAGE_HANDLER_EX( WM_IME_COMPOSITION, OnDefaultHandle )
		MESSAGE_HANDLER_EX( WM_IME_REQUEST, OnDefaultHandle )
		MESSAGE_HANDLER_EX( WM_IME_CHAR, OnDefaultHandle )

//		POST_HANDLE_MSG()		
	END_MSG_MAP()

protected:
	LRESULT OnPreHandleMsg( HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam );
	LRESULT OnPostHandleMsg( HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam );
	BOOL    OnSetCursor(HWND wnd, UINT nHitTest, UINT message);
	LRESULT OnMouseMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
    LRESULT OnScroll(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnDefaultHandle(UINT uMsg, WPARAM wParam, LPARAM lParam);
    LRESULT OnUIDropTargetEvent(UINT uMsg, WPARAM wParam, LPARAM lParam);
	void    OnKillFocus(HWND wndFocus);
    void    OnSetFocus(HWND wndOld);
	LRESULT OnChar(UINT uMsg, WPARAM wParam, LPARAM lParam);
    LRESULT OnKeyDown(UINT uMsg, WPARAM wParam, LPARAM lParam);
    void    OnSize(UINT nType, CSize size);
	LRESULT OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam);
	bool    DoGifOleUpdateRequst();

protected:
	// *** IRichEditOleCallback methods ***
 	virtual HRESULT __stdcall GetNewStorage(LPSTORAGE FAR * lplpstg);
 	virtual HRESULT __stdcall GetInPlaceContext(LPOLEINPLACEFRAME FAR * lplpFrame, 
        LPOLEINPLACEUIWINDOW FAR * lplpDoc, LPOLEINPLACEFRAMEINFO lpFrameInfo) ;
 	virtual HRESULT __stdcall ShowContainerUI(BOOL fShow) ;
 	virtual HRESULT __stdcall QueryInsertObject(LPCLSID lpclsid, LPSTORAGE lpstg, LONG cp) ;
 	virtual HRESULT __stdcall DeleteObject(LPOLEOBJECT lpoleobj) ;
 	virtual HRESULT __stdcall QueryAcceptData(LPDATAOBJECT lpdataobj, 
        CLIPFORMAT FAR * lpcfFormat, DWORD reco, BOOL fReally, HGLOBAL hMetaPict) ;
 	virtual HRESULT __stdcall ContextSensitiveHelp(BOOL fEnterMode) ;
 	virtual HRESULT __stdcall GetClipboardData(CHARRANGE FAR * lpchrg,
        DWORD reco, LPDATAOBJECT FAR * lplpdataobj) ;
 	virtual HRESULT __stdcall GetDragDropEffect(BOOL fDrag, 
        DWORD grfKeyState, LPDWORD pdwEffect) ;
 	virtual HRESULT __stdcall GetContextMenu(WORD seltype,
        LPOLEOBJECT lpoleobj, CHARRANGE FAR * lpchrg, HMENU FAR * lphmenu) ;

    // ICaretUpdateCallback 
    virtual void  CaretInvalidateRect(RECT* prc, int nCount);
    virtual bool  NeedFixItalicCaretOffset() {return true; }

public:

	// Function
	void  BindControl(RichEdit* p);
	bool  Create(HWND hWndParent);
	void  Draw(HDC, bool bDrawShadow);
    //void  DrawShadow(HDC hDC, CRect* prcClient);
	void  RealDraw(HDC hDC, RECT* prcClient);
    void  InvalidateRect(RECT* prcWnd);
	bool  HitTest(POINT pt);
    IRichEdit*   GetControlPtr();
    ITextDocument2*  GetDocPtr();
    void  SetTextFilterUint(UINT nMax);
    void  SetTextFilterDigit();
    void  SetTextFilterPhoneNumber();
    void  SetTextFilterAccount();

    // 基本操作
    bool  SetText(LPCTSTR szText);
    int   GetText(TCHAR* szBuf, uint nLen);
    void  GetText(BSTR* pbstr);
    // void  GetTextW(IBuffer** ppBuffer);
    const char*  GetRtfBuffer(int nFlags=0);
    void   ReleaseRtfBuffer(const char*);
	int   GetTextRange(TCHAR*  szBuf, uint nStart, uint nSize);
    int   GetTextRange(BSTR* pbstr, uint nStart, uint nSize);
    int   GetTextLengthW();
    // bool  GetEncodeTextW(IBuffer** ppBuffer);
    // bool  GetEncodeTextW(LPCTSTR bstrContent, int ncpStart, IBuffer** ppBuffer);
    bool  AppendText(LPCTSTR szText, int nLength=-1);
    bool  InsertText(int nPos, LPCTSTR szText, int nLength=-1);
    bool  InsertText(LPCTSTR szText, int nLength=-1);
    bool  ReplaceSelectionText(LPCTSTR szText, int nSize);
    bool  IsReadOnly() { return (m_dwStyle & ES_READONLY) != 0; }
    void  SetReadOnly(bool fReadOnly);
    void  SetReadOnlyLight(bool fReadOnly);
    void  SetRichTextFlag(bool b);

    // message helper
    void  SetSel(int nPos, int nLen);
    void  SetSel(int nPos);
    void  SetSel2(int nStartChar, int nEndChar);
    void  SetSel(CHARRANGE &cr);
    void  SelectAll();
    void  SelectEnd();
    void  ScrollEnd();
    void  GetSel(int* pnPos, int* pnLen);
    int   GetSelectionStart();
    int   LineIndex(int nLineNum = -1);
    int   LineFromChar(int ncp);
    int   LineLength(int nLineCP = -1);
    int   GetLineCount();
    int   GetLine(int nLineIndex, TCHAR* szBuffer, int nBufferSize);
    int   GetFirstVisibleLine();
    void  GetVisibleCharRange(int* pnFirstCp, int* pnLastCp);
    void  GetCharRect(int ncp, RECT* prc);

    void  DisableUndoOp();
    int   CharFromPos(POINT pt);
    void  PosFromChar(UINT ncp, POINT* pPos);
    void  SetUndoLimit(int nUndoSize);
    void  SetLimitText(int nLimit);
	void  EnableWordBreak(bool b);

    void  KillFocus();
    void  SetFocus();
    bool  IsFocus();
    bool  IsScrollBarVisible();

    // event mask
    long  GetEventMask();
    void  SetEventMask(long n);
    void  ModifyEventMask(long nAdd, long nRemove);
    void  SetAutoResize(bool b, int nMaxSize = 0);
	
    // Ole操作
    WORD  GetSelectionType();
    REOleBase*  GetSelectionOle();
    REOleBase*  GetOleByCharPos(uint ncp);
    int   GetOleCharPos(IOleObject* pOleObj);
    int   GetOleCharPos(REOleBase* pOle);
    IOleObject*  CreateOleObjectFromData(LPDATAOBJECT pDataObject, 
            bool bOleCreateFromDataOrOleCreateStaticFromData = true,
            OLERENDER render = OLERENDER_DRAW, 
            CLIPFORMAT cfFormat = 0, 
            LPFORMATETC lpFormatEtc = NULL);
//  int   GetObjectTypeByOleObject(LPOLEOBJECT pOleObject); 

    bool  InsertOleObject(REOleBase* pItem);
    bool  InsertComObject(REFCLSID clsid);

	void  Paste();
    HRESULT DoPaste(LPDATAOBJECT pDataObject, BOOL fReally, CLIPFORMAT* pOutClipFormat);
    void  PasteTextFilter(LPDATAOBJECT pDataObject, bool& bHandled);

    void  RevokeDragDrop(void);
    void  RegisterDragDrop(void);
    bool  CanDragDrop();

    // 给子类WindowlessRichEditEx的虚函数
    virtual void  OnCreate(){};
    virtual void  PreDraw(HDC hDC, RECT* prcDraw);
    virtual void  PostDraw(HDC hDC, RECT* prcDraw) {};
    virtual void  OnOleSizeChanged(REOleBase* pItem) {};

    // TODO: 仅为了解决在本类中无法调用子类的ReplaceSelectionEncodeText
    virtual bool  PasteEncodeText(LPWSTR szData) {return false;};

public:
	// IUnknown  Interface
	virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid,void **ppvObject);
	virtual ULONG   STDMETHODCALLTYPE AddRef(void);
	virtual ULONG   STDMETHODCALLTYPE Release(void);

	// ITextHost Interface
	// 需要根据控件属性进行定制的接口放在这里实现，其它接口接口放在ITextHostImpl中实现
	virtual BOOL    TxShowScrollBar(INT fnBar, BOOL fShow);
	virtual BOOL    TxEnableScrollBar (INT fuSBFlags, INT fuArrowflags);
	virtual BOOL    TxSetScrollRange(INT fnBar,LONG nMinPos,INT nMaxPos,BOOL fRedraw);
	virtual BOOL 	TxSetScrollPos (INT fnBar, INT nPos, BOOL fRedraw);
	virtual HRESULT TxGetClientRect(LPRECT prc);
	virtual void    TxInvalidateRect(LPCRECT prc, BOOL fMode);
	virtual HRESULT TxGetBackStyle(TXTBACKSTYLE *pstyle);
	virtual HRESULT	TxGetViewInset(LPRECT prc);
    virtual BOOL 	TxSetTimer(UINT idTimer, UINT uTimeout);
    virtual void 	TxKillTimer(UINT idTimer);
    virtual void	TxSetFocus();
    virtual BOOL	TxCreateCaret(HBITMAP hbmp, INT xWidth, INT yHeight);
    virtual BOOL	TxSetCaretPos(INT x, INT y);
    virtual BOOL    TxShowCaret(BOOL fShow);
    virtual HRESULT TxNotify(DWORD iNotify, void *pv);

protected:
	void    ClearOleObjects();
    void    OnRequestResize(RECT* prc);
    void    SyncScrollPos();
    HRESULT DefTxNotify(DWORD iNotify, void* pv);

protected:
    RichEdit*    m_pRichEdit;
    IRichEdit*   m_pIRichEdit;
	REOleManager*  m_pOleMgr;

    ITextFilter*    m_pTextFilter; // 文字过滤。例如实现整数、小数输入等

	int    m_nLastTimerKickCount; // 刷新FPS限制。这里还需要考虑下优化：即使只有一个GIF，也需要整个client rect刷新吗
	
    bool   m_bDuringTxDraw;       // 标志当前是否正在调用TxDraw。在这个期间的光标操作（针对自绘类型）都不应该再去刷新RE了，否则死循环
    bool   m_bFocus;
    bool   m_bScrollBarVisible;

    int    m_nAutoResizeMaxSize;  // 自适应大小时的最大值。singleline对应width,multiline对应height。小于0表示无限制

protected:
	// 非windowless richedit要调用的初始化函数
	static LPCTSTR  GetLibraryName();
	void   InitRichEidtDll();
	void   ReleaseRichEidtDll();
	static HMODULE  s_RichEditDll;
	static LONG     s_refDll;
	static pfuncCreateTextServices  s_funcCreateTextServices;
public:
	//RichEdit formats
	static UINT     s_cfRichTextFormat;      // CLIPFORMAT
	static UINT     s_cfRichTextAndObjects;  // CLIPFORMAT
};

}
