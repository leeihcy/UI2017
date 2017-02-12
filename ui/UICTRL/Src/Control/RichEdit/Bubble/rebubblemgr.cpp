#include "stdafx.h"
#include "rebubblemgr.h"
#include "rebubble.h"
#include "..\windowlessrichedit.h"
#include "..\UISDK\Inc\Interface\iuires.h"
#include "..\UISDK\Inc\Interface\iskindatasource.h"

using namespace UI;


BubbleManager::BubbleManager(WindowlessRichEdit*  p)
{
	m_pWindowlessRE = p;
    m_pLeftImageResItem = NULL;
    m_pRightImageResItem = NULL;
}
BubbleManager::~BubbleManager()
{
    Clear();
}
void BubbleManager::Clear()
{
    _MyIter iter = m_bubbleList.begin();
    for (; iter != m_bubbleList.end(); iter++)
    {
        REBubble*  p = *iter;
        delete p;
    }
    m_bubbleList.clear();
    m_bubbleSizeInvalidList.clear();
}

void  BubbleManager::Init()
{
   
}

REBubble*  BubbleManager::CreateBubble(BubbleType eType, BubbleAlign eAlign, int nStartCP, int nEndCP)
{
	REBubble* pBubble = NULL;
	switch (eType)
	{
	case BUBBLE_NORMAL:
		pBubble = new REBubble();
        ((REBubble*)pBubble)->CreateIxxx(NULL);
		break;

	case BUBBLE_PORTRAIT:
		pBubble = new REPortraitBubble();
        ((REPortraitBubble*)pBubble)->CreateIxxx(NULL);
		break;

	case BUBBLE_INFO:
		pBubble = new REInfoBubble();
        ((REInfoBubble*)pBubble)->CreateIxxx(NULL);
		break;

	default:
		return NULL;
	}
	
	pBubble->SetBubbleManager(this);
	pBubble->SetDesiredAlign(eAlign);
    pBubble->SetContentCPInfo(nStartCP, nEndCP);

    m_bubbleList.push_back(pBubble);
    m_bubbleSizeInvalidList.push_back(pBubble);
	return pBubble;
}

WindowlessRichEdit*  BubbleManager::GetRE()
{
	return m_pWindowlessRE;
}
ImageWrap*  BubbleManager::GetLeftBubbleImage()
{
	return &m_imageLeftBubble;
}
ImageWrap*  BubbleManager::GetRightBubbleImage()
{
	return &m_imageRightBubble;
}
C9Region*  BubbleManager::Get9RegionLeftBubble()
{
    return &m_9regionLeftBubble;
}
C9Region*  BubbleManager::Get9RegionRightBubble()
{
    return &m_9regionRightBubble;
}
void  BubbleManager::SetLeftImageResItem(IImageResItem* p)
{
    m_pLeftImageResItem = p;

    IRichEdit*  pIRichEdit = m_pWindowlessRE->GetControlPtr();
    ISkinRes*   pSkinRes = pIRichEdit->GetUIApplication()->GetDefaultSkinRes();
    ISkinDataSource*   pDataSource = pSkinRes->GetDataSource();

    if (m_pLeftImageResItem)
    {
        String strLeftBubblePath(m_pLeftImageResItem->GetPath());

        // TODO: 修改为直接引用ImageResItem中的gdi image
        if (!strLeftBubblePath.empty())
            pDataSource->Load_Image(strLeftBubblePath.c_str(), &m_imageLeftBubble);

        m_9regionLeftBubble.Set(12,14,12,12);
    }
    else
    {
        m_imageLeftBubble.Destroy();
    }
}
void  BubbleManager::SetRightImageResItem(IImageResItem* p)
{
    m_pRightImageResItem = p;

    IRichEdit*  pIRichEdit = m_pWindowlessRE->GetControlPtr();
    ISkinRes*   pSkinRes = pIRichEdit->GetUIApplication()->GetDefaultSkinRes();
    ISkinDataSource*   pDataSource = pSkinRes->GetDataSource();

    if (m_pRightImageResItem)
    {
        String strRightBubblePath(m_pRightImageResItem->GetPath());

        // TODO: 修改为直接引用ImageResItem中的gdi image
        if (!strRightBubblePath.empty())
            pDataSource->Load_Image(strRightBubblePath.c_str(), &m_imageRightBubble);

        m_9regionRightBubble.Set(12,14,12,12);
    }
    else
    {
        m_imageRightBubble.Destroy();
    }
}
IImageResItem*  BubbleManager::GetLeftImageResItem()
{
    return m_pLeftImageResItem;
}
IImageResItem*  BubbleManager::GetRightImageResItem()
{
    return m_pRightImageResItem;
}

bool  BubbleManager::PreCalcBubblePos()
{
    bool bReadOnly = m_pWindowlessRE->IsReadOnly();
    if (!bReadOnly)
        return false;

    m_pWindowlessRE->SetReadOnlyLight(false);
    return true;
}
void  BubbleManager::PostCalcBubblePos(bool bPostRet)
{
    if (!bPostRet)
        return;

    IRichEdit*  pRE = m_pWindowlessRE->GetControlPtr();

    m_pWindowlessRE->SetReadOnlyLight(true);
}

// 新插入一个气泡时，调用该函数 --> 改调用InvalidBubbleSize
void  BubbleManager::UpdateSingleBubblePos(REBubble* pBubble)
{
    if (!pBubble)
        return;

    bool bPreRet = PreCalcBubblePos();
    {
        pBubble->CalcBubblePos();
    }
    PostCalcBubblePos(bPreRet);
}

void  BubbleManager::OnSize()
{
    bool bPreRet = PreCalcBubblePos();
    {
        _MyIter iter = m_bubbleList.begin();
        for (; iter != m_bubbleList.end(); iter++)
        {
            (*iter)->CalcBubblePos();
        }
    }
    PostCalcBubblePos(bPreRet);

    // 清空无效列表
    m_bubbleSizeInvalidList.clear();
}

// 采用延迟更新的办法。用于解决在更新ole图片时，可能一个气泡内的多个图片一起更新
// 这样只需要更新一次气泡大小即可
void  BubbleManager::InvalidBubbleSize(REBubble* pBubble)
{
    if (pBubble)
        m_bubbleSizeInvalidList.push_back(pBubble);
}
void  BubbleManager::RefreshInvalidBubbleSize()
{
    if (m_bubbleSizeInvalidList.empty())
        return;

    bool bPreRet = PreCalcBubblePos();
    _MyIter iter = m_bubbleSizeInvalidList.begin();
    for (; iter != m_bubbleSizeInvalidList.end(); ++iter)
    {
        (*iter)->CalcBubblePos();
    }
    PostCalcBubblePos(bPreRet);

    m_bubbleSizeInvalidList.clear();
}

void  BubbleManager::OnDraw(HDC hDC)
{
    if (m_bubbleList.size() == 0)
        return;

    RefreshInvalidBubbleSize();

    int nFirstVisibleCp = 0;
    int nLastVisibleCp = 0;
    m_pWindowlessRE->GetVisibleCharRange(&nFirstVisibleCp, &nLastVisibleCp);

    // PosFromChar返回的坐标与RE坐标一致，是基于窗口的
    CRect  rcWnd;
    m_pWindowlessRE->GetControlPtr()->GetClientRectInWindow(&rcWnd);

    RECT rcClip = {0};
    RECT rcIntersect = {0};
    GetClipBox(hDC, &rcClip);  // 逻辑坐标，带偏移的??

    _MyIter iter = m_bubbleList.begin();
    for (; iter != m_bubbleList.end(); iter++)
    {
        REBubble*  pBubble = *iter;
        if (pBubble->m_ncpEnd < nFirstVisibleCp)
            continue;

        if (pBubble->m_ncpStart > nLastVisibleCp)
            break;

        CRect  rcDraw;
        pBubble->GetDrawRegion(&rcDraw);
        rcDraw.OffsetRect(-rcWnd.left, -rcWnd.top);

        if (!IntersectRect(&rcIntersect, &rcDraw, &rcClip))
            continue;

        pBubble->Draw(hDC, &rcDraw);
    }
}

bool  BubbleManager::IsInBubble(int ncp, __out REBubble** ppBubble)
{
    if (m_bubbleList.empty())
        return false;
	if (ncp < m_bubbleList.front()->m_ncpStart)
		return false;
	if (ncp > m_bubbleList.back()->m_ncpEnd)
		return false;

	_MyReverseIter iter = m_bubbleList.rbegin();
	for (; iter != m_bubbleList.rend(); iter++)
	{
		REBubble*  pBubble = *iter;
		if (pBubble->m_ncpEnd < ncp)
			return false;

		if (pBubble->m_ncpStart > ncp)
			continue;

		if (ppBubble)
			*ppBubble = pBubble;

		return true;
	}
	return false;
}

REBubble*  BubbleManager::FindBubble(int ncp)
{
    REBubble*  pBubble = NULL;
    bool b = IsInBubble(ncp, &pBubble);

    if (!b)
        return NULL;

    return pBubble;
}