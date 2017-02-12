#include "stdafx.h"
#include "doc.h"
#include "font.h"
#include "paragraph.h"
#include "page.h"
#include "message.h"
#include "resource.h"
#include "line.h"
#include "link.h"
#include "element/imageelement.h"
#include "element/textelement.h"
#include "../richtext.h"
#include "../UISDK/Inc/Interface/iscrollbarmanager.h"

using namespace UI;
using namespace UI::RT;

Doc::Doc() : m_selection(*this), m_mousekey(*this)
{
	memset(&m_defaultcf, 0, sizeof(m_defaultcf));
	memset(&m_defaultcf_link, 0, sizeof(m_defaultcf_link));

    SetRectEmpty(&m_rcInvalidInDoc);
    m_eInvalidState = InvalidStateEmpty;
    FontCache::Instance().AddRef();

    m_pRichText = NULL;

	m_sizeContent.cx = m_sizeContent.cy = 0;
	m_sizeView.cx = m_sizeView.cy = 0;
	m_pMgrScrollBar = nullptr;

	m_pFirstPage = nullptr;
	m_ePageMode = PageMode_multiline;
	m_eEditMode = EDITABLE_Readonly;
	m_bNeedUpdateElementIndex = false;
    m_imageLeftBubble.LoadFromResource(
        g_hInstance, IDB_PNG_RICHTEXT_BUBBLE_DEFAULT_LEFT, L"PNG");
    m_imageRightBubble.LoadFromResource(
        g_hInstance, IDB_PNG_RICHTEXT_BUBBLE_DEFAULT_RIGHT, L"PNG");
}

Doc::~Doc()
{
	IScrollBarManager::DestroyIScrollBarManager(m_pMgrScrollBar);

	Clear();
    FontCache::Instance().Release();
}

void  Doc::Init(RichText* pRichText)
{
    m_pRichText = pRichText;
    m_mousekey.Init(pRichText);

	IRichText* p = pRichText->GetIRichText();
	m_pMgrScrollBar = IScrollBarManager::CreateIScrollBarManager();
	m_pMgrScrollBar->SetBindObject(p);
	m_pMgrScrollBar->SetScrollBarVisibleType(HSCROLLBAR, SCROLLBAR_VISIBLE_NONE);
	m_pMgrScrollBar->SetScrollBarVisibleType(VSCROLLBAR, SCROLLBAR_VISIBLE_AUTO);

	OBJSTYLE s = { 0 };
	s.vscroll = 1;
	p->ModifyObjectStyle(&s, 0);

	initBlank();
}

// 初始化空白内容, 默认一个para
void  Doc::initBlank()
{
	UIASSERT(!GetChildNode());
// 	Paragraph* p = new Paragraph;
// 	this->AddChildNode(p);
// 	m_selection.SetAnchor(p, 0);
// 	m_selection.SetFocus(p, 0);	

    m_selection.SetAnchorFocus(this, CaretPos(0));
    CreateDefaultFont();
}

void  Doc::Clear()
{
    m_selection.Clear();
    m_mousekey.Clear();

	m_selection.SetAnchorFocus(this, CaretPos(0));
	ReleasePage();

	DestroyDecendant();
}

PageMode  Doc::GetPageMode()
{
    return m_ePageMode;
}   

void  Doc::SetPageMode(PageMode e)
{
    m_ePageMode = e;
}

RichText*  Doc::GetControl()
{
    return m_pRichText;
}
IRichText*  Doc::GetIControl()
{
	if (!m_pRichText)
		return nullptr;

	return m_pRichText->GetIRichText();
}

void  Doc::Render2DC(HDC hDC, LPCRECT rcClip)
{
    // 先对树模型整体绘制一次，例如doc背景，message气泡
    Node::PreDraw(hDC, rcClip);

    // 再具体的画每一行，每一行中的文本、对象等
	Page* p = m_pFirstPage;
	while (p)
	{
		p->Draw(hDC, rcClip);
		p = p->GetNextPage();
	}
}


void  Doc::CopyDefaultCharFormat(CharFormat* pcf)
{
    if (!pcf)
        return;

    memcpy(pcf, GetDefaultCharFormat(), sizeof(m_defaultcf));

	// 这些字段不返出去。外部调用该函数时，仅仅是为了便于创建一个类似字体
	pcf->hFont = NULL;
	pcf->nFontHeight = 0;
	pcf->nFontDescent = 0;
}
void  Doc::CopyDefaultLinkCharFormat(CharFormat* pcf)
{
	if (!pcf)
		return;

	memcpy(pcf, GetDefaultLinkCharFormat(), sizeof(m_defaultcf_link));

	// 这些字段不返出去。外部调用该函数时，仅仅是为了便于创建一个类似字体
	pcf->hFont = NULL;
	pcf->nFontHeight = 0;
	pcf->nFontDescent = 0;
}

const CharFormat*  Doc::GetDefaultCharFormat()
{
	if (!m_defaultcf.hFont)
		CreateDefaultFont();
	
	return &m_defaultcf;
}
const CharFormat*  Doc::GetDefaultLinkCharFormat()
{
	if (!m_defaultcf_link.hFont)
		CreateDefaultLinkFont();

	return &m_defaultcf_link;
}


void  Doc::LayoutAll(uint nControlWidth, uint nControlHeight)
{
#if 0
    if (nControlHeight == m_nControlHeight &&
        nControlWidth == nControlWidth)
        return;

    m_nControlHeight = nControlHeight;

    // 如果宽度没变，不用布局？？（有居中的情况）
    // if (m_nControlWidth == nControlWidth)
    //    return;

    m_nControlWidth = nControlWidth;
#endif

    // TODO: 先这样简单处理
    SetNeedUpdateElementIndex();

 	PreLayoutAll();
 
    if (nControlWidth == 0 || nControlHeight == 0)
         return;
 
 	switch (m_ePageMode)
 	{
    case PageMode_singleline:
 		{
 			layoutAllSingleLine(nControlWidth, nControlHeight);
 		}
 		break;

    case PageMode_multiline:
 		{
 			layoutAllMultiLine(nControlWidth, nControlHeight);
 		}
 		break;
#if 0
    case PageMode::multipage:
 		{
 			layoutAllMultiPage(nControlWidth, nControlHeight);
 		}
 		break;
#endif		
 	}
}

// 布局前，释放布局对象:Page、Line、Run
void  Doc::PreLayoutAll()
{
	ReleasePage();

    if (m_bNeedUpdateElementIndex)
    {
        UpdateElementIndex();
    }
}

void  Doc::ReleasePage()
{
	Page* p = m_pFirstPage;
	Page* pNext = NULL;
	while (p)
	{
		pNext = p->GetNextPage();
		delete p;
		p = pNext;
	}
	m_pFirstPage = NULL;
	m_mousekey.Clear();
}

void  Doc::layoutAllSingleLine(uint nControlWidth, uint nControlHeight)
{
    m_pFirstPage = new Page(this);
    m_pFirstPage->SetSize(nControlWidth, nControlHeight);

    Node* pParagraph = this->GetChildNode();
    UIASSERT(pParagraph && pParagraph->GetNodeType() == NodeTypeParagraph);

    Line* pSingleLine = new Line(
        m_pFirstPage, 
        *static_cast<Paragraph*>(pParagraph),
        m_defaultcf.nFontHeight);

    m_pFirstPage->AppendLine(pSingleLine);

    SingleLineLayoutContext context;
    context.pDoc = this;
    context.pPage = m_pFirstPage;
    context.pLine = pSingleLine; // 唯一行
    context.xLineCursor = 0;  // 该行run横向起始位置

    this->LayoutSingleLine(&context);
}


//
// 第一次布局：
//   1. 限定indent
//   2. 生成所有line对象
//
// 第二次布局：
//   1. 调整line内容的对齐方式，起始位置
//   2. 由派生类在post layout中再次调整，如会话气泡
//
void  Doc::layoutAllMultiLine(uint nControlWidth, uint nControlHeight)
{
    m_pFirstPage = new Page(this);

    MultiLineLayoutContext  context = { 0 };
    context.pDoc = this;
    context.pPage = m_pFirstPage;
    context.nPageContentWidth =
        nControlWidth -
        m_style.page_padding.left -
        m_style.page_padding.right;
    context.nPageContentHeight =
        nControlHeight -
        m_style.page_padding.top -
        m_style.page_padding.bottom;

    context.nLineSpace = m_style.line_space;

    this->LayoutMultiLine(&context);

    if (nControlHeight >= 0x7FFFFFFF)  // TODO: GetDesiredSize使用
        return;

    int cySize = context.yLine;
    // 最后一行的高度还没有加到yLine中
    if (context.pLine)
        cySize += context.pLine->GetHeight();
    
	this->SetContentSize(0, cySize);
    this->SetViewSize(0, context.nPageContentHeight);

    m_pFirstPage->SetSize(nControlWidth, cySize);
}

void  Doc::layoutAllMultiPage(uint nControlWidth, uint nControlHeight)
{
}

void  Doc::InvalidRegion(RECT* prcInDoc)
{
    if (!prcInDoc)
    {
        m_eInvalidState = InvalidStateFull;
        SetRectEmpty(&m_rcInvalidInDoc);
        return;
    }

    if (!IsRectEmpty(prcInDoc))
    {
        UnionRect(&m_rcInvalidInDoc, &m_rcInvalidInDoc, prcInDoc);
        m_eInvalidState = InvalidStatePart;
    }
}

void  Doc::ClearInvlidRegion()
{
    m_eInvalidState = InvalidStateEmpty;
    SetRectEmpty(&m_rcInvalidInDoc);
}

void  Doc::Refresh()
{
    if (m_eInvalidState == InvalidStateEmpty)   
        return;

    if (!m_pRichText)
        return;

	if (m_eInvalidState == InvalidStatePart)
        m_pRichText->GetIRichText()->Invalidate(&m_rcInvalidInDoc);
	else
        m_pRichText->GetIRichText()->Invalidate();

    ClearInvlidRegion();
}

void  Doc::SetDefaultFont(CharFormat* pcf)
{
	if (!pcf)
		return;

	memcpy(&m_defaultcf, pcf, sizeof(m_defaultcf));
	m_defaultcf.hFont = 0;
	m_defaultcf.nFontDescent = 0;
	m_defaultcf.nFontHeight = 0;

	// 生成默认字体
	FontCache::Instance().GetFont(&m_defaultcf);

}
void  Doc::SetDefaultLinkFont(CharFormat* pcf)
{
	if (!pcf)
		return;

	memcpy(&m_defaultcf_link, pcf, sizeof(m_defaultcf_link));
	m_defaultcf_link.hFont = 0;
	m_defaultcf_link.nFontDescent = 0;
	m_defaultcf_link.nFontHeight = 0;

	// 生成默认字体
	FontCache::Instance().GetFont(&m_defaultcf_link);
}

void  Doc::CreateDefaultFont()
{
	if (m_defaultcf.hFont)
		return;

	memset(&m_defaultcf, 0, sizeof(m_defaultcf));
	_tcscpy(m_defaultcf.szFontName, TEXT("微软雅黑"));
	m_defaultcf.lFontSize = 9;
	

	FontCache::Instance().GetFont(&m_defaultcf);
}
void  Doc::CreateDefaultLinkFont()
{
	if (m_defaultcf_link.hFont)
		return;

	memset(&m_defaultcf_link, 0, sizeof(m_defaultcf_link));
	_tcscpy(m_defaultcf_link.szFontName, TEXT("微软雅黑"));
	m_defaultcf_link.lFontSize = 9;
	m_defaultcf_link.bUnderline = false;  // 下划线由自己来画，避免多创建一种字体
	m_defaultcf_link.textColor = RGB(0,0,204);
	m_defaultcf_link.bTextColor = true;

	FontCache::Instance().GetFont(&m_defaultcf_link);
}

Style&  Doc::GetStyle()
{
    return m_style;
}

Selection&  Doc::GetSelection()
{
    return m_selection;
}

MouseKey&  Doc::GetMouseKey()
{
    return m_mousekey;
}

Paragraph*  Doc::GetSelectionAnchorParagraph()
{
    Node* node = m_selection.GetAnchor();
    if (!node)
        return nullptr;

    return node->GetParagraphNode();
}

SIZE  Doc::GetDesiredSize()
{
	
    SIZE s = { 0, 0 };
#if 0	
    if (!m_pFirstPage)
        return s;

    if (m_ePageMode == PAGE_MODE_SINGLELINE)
    {
        Line* pLine = m_pFirstPage->GetFirstLine();
        if (pLine)
        {
            s.cx = pLine->GetRunsWidth();
            s.cy = pLine->GetHeight();
        }
    }
    else if (m_ePageMode == PAGE_MODE_MULTILINE)
    {
        Line* pLine = m_pFirstPage->GetLastLine();
        if (pLine)
        {
            s.cy = pLine->GetHeight() + pLine->GetPositionYInPage();
        }

        pLine = m_pFirstPage->GetFirstLine();
        while (pLine)
        {
            int w = pLine->GetRunsWidth();
            if (w > s.cx)
                s.cx = w;

            pLine = pLine->GetNextLine();
        }
    }
    else if (m_ePageMode == PAGE_MODE_MULTIPAGE)
    {
        // TODO:
        UIASSERT(0);
    }
#endif
    return s;
}

EDITABLE  Doc::GetEditMode()
{
    return m_eEditMode;
}

void Doc::SetEditMode(EDITABLE e)
{
    if (e == EDITABLE_Enable)
    {
        UIASSERT(0 && "Not implement");
    }
    m_eEditMode = e;
}

void Doc::CopySelection()
{
    if (m_eEditMode == EDITABLE_Disable)
        return;

    if (m_selection.IsEmpty())
        return;

    String str;
    m_selection.ToString(str);

    if (OpenClipboard(NULL))
    {
        EmptyClipboard();

        int nSize = sizeof(TCHAR) * (str.length() + 1);
        HGLOBAL hBlock = GlobalAlloc(GMEM_MOVEABLE, nSize);
        if (hBlock)
        {
            void* p = GlobalLock(hBlock);
            memcpy(p, str.c_str(), nSize);
            GlobalUnlock(hBlock);
        }
        SetClipboardData(CF_UNICODETEXT, hBlock);

        CloseClipboard();
        // We must not free the object until CloseClipboard is called.
        if (hBlock)
            GlobalFree(hBlock);
    }
}

void  Doc::SelectEnd()
{
	if (m_eEditMode == EDITABLE_Disable)
		return;

	m_selection.Clear();

// 	Node* pLast = this;
// 	Node* pTemp = nullptr;
// 	while (pLast)
// 	{
// 		pTemp = pLast->GetLastChildNode();
// 		if (!pTemp)
// 			break;;
// 		pLast = pTemp;
// 	}
// 	UIASSERT(pLast);
// 	m_selection.SetAnchorFocus(pLast, -1);

	m_selection.SetAnchorFocus(this, 1);

	InvalidRegion(nullptr);
	Refresh();
}

void  Doc::SelectBegin()
{
	if (m_eEditMode == EDITABLE_Disable)
		return;

	m_selection.Clear();

	m_selection.SetAnchorFocus(this, 0);

	InvalidRegion(nullptr);
	Refresh();
}

void Doc::SelectAll()
{
    if (m_eEditMode == EDITABLE_Disable)
        return;

    m_selection.Clear();

    Node* pLast = this;
    Node* pTemp = nullptr;
    while (pLast)
    {
        pTemp = pLast->GetLastChildNode();
        if (!pTemp)
            break;;
        pLast = pTemp;
    }
    UIASSERT(pLast);

    m_selection.SetAnchor(this, 0);
    m_selection.SetFocus(pLast, -1);

    InvalidRegion(nullptr);
    Refresh();
}


// TODO: 有没有更好的方法来判断两个结点的先后顺序
void Doc::SetNeedUpdateElementIndex()
{
    m_bNeedUpdateElementIndex = true;
}

void  Doc::UpdateElementIndex()
{
    UIASSERT(m_bNeedUpdateElementIndex);
    m_bNeedUpdateElementIndex = false;

    int index = 0;
    Node* p = this;
    while (p)
    {
        p->SetIndex(++index);
        p = p->GetNextTreeNode();
    }
}


Page*  Doc::PageHitTest(POINT ptDoc, __out POINT* ptInPage)
{
    if (!m_pFirstPage)
        return NULL;

    if (m_ePageMode == PageMode_singleline || m_ePageMode == PageMode_multiline)
    {
        RECT rcPage;
        m_pFirstPage->GetContentRectInDoc(&rcPage);

        if (PtInRect(&rcPage, ptDoc))
        {
            if (ptInPage)
            {
                ptInPage->x = ptDoc.x - rcPage.left;
                ptInPage->y = ptDoc.y - rcPage.top;
            }
            return m_pFirstPage;
        }

        return NULL;
    }

    if (m_ePageMode == PageMode_multipage)
    {
        Page* p = m_pFirstPage;
        while (p)
        {
            if (p->IsVisible())
            {
                RECT rcPage;
                p->GetContentRectInDoc(&rcPage);

                if (PtInRect(&rcPage, ptDoc))
                {
                    if (ptInPage)
                    {
                        ptInPage->x = ptDoc.x - rcPage.left;
                        ptInPage->y = ptDoc.y - rcPage.top;
                    }
                    return p;
                }
            }
            p = p->GetNextPage();
        }

        return NULL;
    }

    return NULL;
}


Page*  Doc::GetLastPage()
{
    if (!m_pFirstPage)
        return nullptr;

    Page* p = m_pFirstPage;
    while (p)
    {
        if (!m_pFirstPage->GetNextPage())
            return p;

        p = m_pFirstPage->GetNextPage();
    }

    return nullptr;
}
Page*  Doc::GetFirstPage()
{
    return m_pFirstPage;
}

const CharFormat* Doc::GetCharFormat()
{
    return &m_defaultcf;
}

void Doc::SetStyle_LineSpace(int linespace)
{
	m_style.line_space = linespace;
}

BOOL UI::RT::Doc::ProcessMessage(UIMSG* msg, int mapid, bool hook)
{
	if (msg->message == WM_KEYDOWN)
	{
		if (m_mousekey.OnKeyDown(msg->wParam))
		{
			return TRUE;
		}
	}

	if (m_pMgrScrollBar)
	{
		return m_pMgrScrollBar->ProcessMessage(msg, mapid, hook);
	}


// 	if (m_mousekey.ProcessMessage(msg, mapid, hook))
// 		return TRUE;
// 
	return FALSE;
}

void  Doc::SetContentSize(int cx, int cy)
{
	m_sizeContent.cx = cx;
	m_sizeContent.cy = cy;
	m_pMgrScrollBar->SetVScrollRange(cy);
}

void  Doc::SetViewSize(int cx, int cy)
{
	m_sizeView.cx = cx;
	m_sizeView.cy = cy;

	m_pMgrScrollBar->SetVScrollPage(cy);
}

UI::IScrollBarManager* Doc::GetScrollBarManager()
{
	return m_pMgrScrollBar;
}

void UI::RT::Doc::DeleteParagraph(Paragraph* para)
{
	if (!para)
		return;

	para->DeleteFromTree(para);

	GetControl()->SetLayoutDirty(true);
	InvalidRegion(nullptr);
	Refresh();
}


void Doc::OnNodeRemove(Node* p)
{
	if (p->GetSelectionState() == SelectionState_Selected)
	{
		m_selection.Clear();
	}

	m_mousekey.OnNodeRemove(p);
}
