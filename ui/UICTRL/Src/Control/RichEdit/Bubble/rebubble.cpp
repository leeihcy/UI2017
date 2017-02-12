#include "stdafx.h"
#include "rebubble.h"
#include "rebubblemgr.h"
#include "..\windowlessrichedit.h"

// floating-point points与twips是1:20的关系??

// 气泡距离控件边缘的位置
#define BUBBLE_MARGIN_TWIPS  500
#define BUBBLE_MARGIN_FLOAT  25

REBubble::REBubble()
{
	m_pBubbleMgr = NULL;
    m_ncpStart = m_ncpEnd = -1;
    m_nContentWidth = m_nContentHeight = 0;
	m_eAlignReal = m_eAlignDesired = BUBBLE_ALIGN_LEFT;
    m_pIREBubble = NULL;
    m_bNeedDeleteIREBubble = false;
}

REBubble::~REBubble()
{
    if (m_pIREBubble)
    {
        if (m_bNeedDeleteIREBubble)
            delete m_pIREBubble;
        m_pIREBubble = NULL;
    }
}

IREBubble*  REBubble::GetIREBubble()
{
    return m_pIREBubble;
}
void  REBubble::CreateIxxx(IREBubble* p)
{
    if (p)
    {
        m_pIREBubble = p;
        m_bNeedDeleteIREBubble = false;
    }
    else
    {
        m_pIREBubble = new IREBubble(this);
        m_bNeedDeleteIREBubble = true;
    }
}


void  REBubble::SetBubbleManager(BubbleManager* pMgr)
{
	m_pBubbleMgr = pMgr;
}

void  REBubble::GetDrawRegion(RECT* prc)
{
    if (!prc)
        return;

    // ptbubble返回的是左上角位置，如果是OleObject::Draw，拿到的则是ole实际位置，例如底对齐
    POINT  ptBubble;
    m_pBubbleMgr->GetRE()->PosFromChar(m_ncpStart, &ptBubble); 

    prc->left   = ptBubble.x;
    prc->top    = ptBubble.y;
    prc->right  = ptBubble.x + m_nContentWidth;
    prc->bottom = ptBubble.y + m_nContentHeight;


    CRegion4  region(0,0,0,0);
	GetContentPadding(&region);

	prc->left -= region.left;
	prc->top -= region.top;
	prc->right += region.right;
	prc->bottom += region.bottom;
}

void  REBubble::GetContentPadding(REGION4* pPadding)
{
	C9Region*  pRegion = NULL;
	if (m_eAlignReal == BUBBLE_ALIGN_RIGHT)
		pRegion = m_pBubbleMgr->Get9RegionLeftBubble();
	else
		pRegion = m_pBubbleMgr->Get9RegionLeftBubble();

	if (pRegion)
	{
		::SetRect(pPadding, pRegion->left, pRegion->top, pRegion->right, pRegion->bottom);
	}
	else
	{
		SetRect(pPadding, 12,12,12,12);
	}
}

// prcDraw是在bubblemgr中计算出来的气泡绘制区域，包含了缩进等
void  REBubble::Draw(HDC hDC, RECT* prcDraw)
{
    ImageWrap*  pImage = NULL;
    C9Region* p9Region = NULL;;

    if (m_eAlignReal == BUBBLE_ALIGN_LEFT)
    {
        pImage = m_pBubbleMgr->GetLeftBubbleImage();
        p9Region = m_pBubbleMgr->Get9RegionLeftBubble();
    }
    else
    {
        pImage = m_pBubbleMgr->GetRightBubbleImage();
        p9Region = m_pBubbleMgr->Get9RegionRightBubble();
    }

    if (pImage->IsNull())
        return;

    pImage->Draw(hDC, 
        prcDraw->left, 
        prcDraw->top, 
        prcDraw->right-prcDraw->left, 
        prcDraw->bottom-prcDraw->top,
        0, 0, 
        pImage->GetWidth(),
        pImage->GetHeight(), 
        p9Region);
}

void  REBubble::SetContentCPInfo(int nStart, int nEnd)
{
    m_ncpStart = nStart;
    m_ncpEnd = nEnd;
}

void  REBubble::SetDesiredAlign(BubbleAlign e)
{
	m_eAlignDesired = e;
	m_eAlignReal = e;
}

// 注：在调用该函数前后，必须调用SetReadOnlyLight来修改只读属性，解决SetIndents调用失败的问题
void  REBubble::CalcBubblePos()
{
    if (m_ncpStart == -1 || m_ncpEnd == -1)
    {
        // 存在这么一种情况：插入该段内容时，引发滚动条显示，此时还没有设置
        // cpstart/cpend就先触发了OnSize，导致参数异常
        return;
    }

    WindowlessRichEdit*  pRE = m_pBubbleMgr->GetRE();
    ITextDocument2*      pDoc = pRE->GetDocPtr();

    CComPtr<ITextPara>   spPara;
    CComPtr<ITextRange>  spRange;

    pDoc->Range(m_ncpStart, m_ncpEnd, &spRange);
    spRange->GetPara(&spPara);

    // 重置缩进，重新计算
    spPara->SetIndents(0, BUBBLE_MARGIN_FLOAT, BUBBLE_MARGIN_FLOAT);

    int  nLineStart = pRE->LineFromChar(m_ncpStart);;
    int  nLineEnd = pRE->LineFromChar(m_ncpEnd);
    m_nContentHeight = m_nContentWidth = -1;

    // 获取内容宽度
    for (int nLine = nLineStart; nLine <= nLineEnd; nLine++)
    {
        POINT  ptEnd;
        POINT  ptStart;

        int ncpStart = pRE->LineIndex(nLine);
        int nLength  = pRE->LineLength(ncpStart);
        int ncpEnd = ncpStart + nLength;
        if (nLength > 0)
            ncpEnd--;  // zero base
        
        CComPtr<ITextRange>  spRangeLine;
        pDoc->Range(ncpStart, ncpEnd, &spRangeLine);
        if (!spRangeLine)
            return;

        // http://technet.microsoft.com/zh-cn/hh768766(v=vs.90) 新类型定义
#define _tomClientCoord     256  // 默认获取到的是屏幕坐标， Use client coordinates instead of screen coordinates.
#define _tomAllowOffClient  512  // Allow points outside of the client area.

        long lTypeTopLeft     = _tomAllowOffClient|_tomClientCoord|tomStart|TA_TOP|TA_LEFT;
        long lTypeRightBottom = _tomAllowOffClient|_tomClientCoord|tomEnd|TA_BOTTOM|TA_RIGHT;
        spRangeLine->GetPoint(lTypeTopLeft,     &ptStart.x, &ptStart.y);
        spRangeLine->GetPoint(lTypeRightBottom, &ptEnd.x,   &ptEnd.y);

        int nContentHeight = ptEnd.y - ptStart.y;
        int nContentWidht  = ptEnd.x - ptStart.x;

        if (m_nContentWidth < nContentWidht)
            m_nContentWidth = nContentWidht;
        m_nContentHeight += nContentHeight;
    }

	if (m_eAlignReal == BUBBLE_ALIGN_RIGHT)
	{
		this->UpdateRightBubbleIndents(spPara);
	}
	else if (m_eAlignReal == BUBBLE_ALIGN_CENTER)
	{
		this->UpdateCenterBubbleIndents(spPara);
	}
}


LONG TwipstoDX(long xTwips)
{
	HDC hDC = GetDC(NULL);
	long xPerInch = GetDeviceCaps(hDC, LOGPIXELSX); 
	ReleaseDC(NULL, hDC);

	return xTwips * xPerInch / 1440 ;
}

LONG TwipstoDY(long yTwips)
{
	HDC hDC = GetDC(NULL);
	long yPerInch = GetDeviceCaps(hDC, LOGPIXELSY); 
	ReleaseDC(NULL, hDC);

	return yTwips * yPerInch / 1440;
}

LONG DXtoTwips(long dx)
{
	HDC hDC = GetDC(NULL);
	long xPerInch = GetDeviceCaps(hDC, LOGPIXELSX); 
	ReleaseDC(NULL, hDC);

	return dx * 1440 / xPerInch;
}
LONG DYtoTwips(long dy)
{
	HDC hDC = GetDC(NULL);
	long yPerInch = GetDeviceCaps(hDC, LOGPIXELSY); 
	ReleaseDC(NULL, hDC);

	return dy * 1440 / yPerInch;
}

// 更新右侧气泡缩进值，例如插入气泡时、窗口大小改变等
void  REBubble::UpdateRightBubbleIndents(ITextPara*  pPara)
{
    if (m_eAlignReal != BUBBLE_ALIGN_RIGHT)
        return;
    if (!pPara)
        return;

    CRect rcclient;
    WindowlessRichEdit*  pRE = m_pBubbleMgr->GetRE();
    pRE->GetControlPtr()->GetObjectClientRect(&rcclient);

    long lLeftIndent = DXtoTwips(rcclient.Width()- m_nContentWidth - 1) - BUBBLE_MARGIN_TWIPS;
    if (lLeftIndent < BUBBLE_MARGIN_TWIPS)
        lLeftIndent = BUBBLE_MARGIN_TWIPS;

    // ReadOnly的RE调用SetIndents会返回E_ACCESSDENIED Write access is denied. 
    HRESULT hr = pPara->SetIndents(0, (float)lLeftIndent/20, BUBBLE_MARGIN_FLOAT);
    if (hr == E_ACCESSDENIED)
    {
        UIASSERT(!m_pBubbleMgr->GetRE()->IsReadOnly());
    }
}
void  REBubble::UpdateCenterBubbleIndents(ITextPara*  pPara)
{
	if (m_eAlignReal != BUBBLE_ALIGN_CENTER)
		return;
	if (!pPara)
		return;

	CRect rcclient;
	WindowlessRichEdit*  pRE = m_pBubbleMgr->GetRE();
	pRE->GetControlPtr()->GetObjectClientRect(&rcclient);

	long lLeftIndent = DXtoTwips((rcclient.Width()- m_nContentWidth)/2);
	if (lLeftIndent < BUBBLE_MARGIN_TWIPS)
		lLeftIndent = BUBBLE_MARGIN_TWIPS;

	// ReadOnly的RE调用SetIndents会返回E_ACCESSDENIED Write access is denied. 
	HRESULT hr = pPara->SetIndents(0, (float)lLeftIndent/20, BUBBLE_MARGIN_FLOAT);
	if (hr == E_ACCESSDENIED)
	{
		UIASSERT(!m_pBubbleMgr->GetRE()->IsReadOnly());
	}
}

//////////////////////////////////////////////////////////////////////////

REPortraitBubble::REPortraitBubble()
{
	m_pImagePortrait = NULL;
    m_pIREPortraitBubble = NULL;
    m_bNeedDeleteIREPortraitBubble = false;
}

REPortraitBubble::~REPortraitBubble()
{
    if (m_pIREPortraitBubble)
    {
        if (m_bNeedDeleteIREPortraitBubble)
            delete m_pIREPortraitBubble;
        m_pIREPortraitBubble = NULL;
    }
}

IREPortraitBubble*  REPortraitBubble::GetIREPortraitBubble()
{
    return m_pIREPortraitBubble;
}
void  REPortraitBubble::CreateIxxx(IREPortraitBubble* p)
{
    if (p)
    {
        m_pIREPortraitBubble = p;
        m_bNeedDeleteIREPortraitBubble = false;
    }
    else
    {
        m_pIREPortraitBubble = new IREPortraitBubble(this);
        m_bNeedDeleteIREPortraitBubble = true;
    }

    REBubble::CreateIxxx(static_cast<IREBubble*>(m_pIREPortraitBubble));
}

void  REPortraitBubble::SetPortrait(ImageWrap* pPortrait)
{
	m_pImagePortrait = pPortrait;
}


void  REPortraitBubble::Draw(HDC hDC, RECT* prcDraw)
{
	__super::Draw(hDC, prcDraw);

	if (!m_pImagePortrait)
		return;

	POINT ptPortarit = {0};
	
	switch (m_eAlignReal)
	{
	case BUBBLE_ALIGN_LEFT:
		{
			ptPortarit.x = prcDraw->left - PORTRAIT_OFFSET_LEFT_X;
			ptPortarit.y = prcDraw->top - PORTRAIT_OFFSET_LEFT_T;
		}
		break;

	case BUBBLE_ALIGN_RIGHT:
		{
			ptPortarit.x = prcDraw->right + PORTRAIT_OFFSET_LEFT_X;
			ptPortarit.y = prcDraw->top - PORTRAIT_OFFSET_LEFT_T;
		}
		break;

	default:
		return;
	}

	m_pImagePortrait->Draw(hDC, ptPortarit.x, ptPortarit.y);
}
//////////////////////////////////////////////////////////////////////////
