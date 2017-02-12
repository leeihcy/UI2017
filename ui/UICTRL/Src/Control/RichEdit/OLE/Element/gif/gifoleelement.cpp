#include "stdafx.h"
#include "GifOleElement.h"
#include "..\..\Composite\compositereole.h"

using namespace UI;

GifOleElement::GifOleElement()
{
    m_pImage = NULL;
    m_bNeedDeleteImage = false;
    m_nFrameCount = 0;
    m_nCurrentFrame = 0;
	m_nPrevFrameTick = 0;
    m_style.bEnable = false; // 不接收鼠标消息
}
GifOleElement::~GifOleElement()
{
    UnloadImage();
}
void  GifOleElement::UnloadImage()
{
	if (!m_pImage)
		return;

	if (m_nFrameCount > 1)
		UnRegisterREOleTimer(static_cast<IREOleTimerCallback*>(this));

    if (m_bNeedDeleteImage && m_pImage)
    {
        SAFE_DELETE(m_pImage);
        m_bNeedDeleteImage = false;
    }

    m_nFrameCount = 0;
    m_nCurrentFrame = 0;
    m_pImage = NULL;
	m_nPrevFrameTick = 0;
    m_vecFrameDelay.clear();
}

GifOleElement*  GifOleElement::CreateInstance()
{
    return new GifOleElement;
}

long  GifOleElement::GetType() 
{
    return GifOleElement::TYPE;
}

void  GifOleElement::Draw(HDC hDC, int xOffset, int yOffset)
{
    if (!m_pImage)
        return;

    Gdiplus::Graphics g(hDC);
    g.DrawImage(m_pImage, 
        m_rcInOle.left + xOffset,
        m_rcInOle.top + yOffset, 
        0, 0, 
        m_pImage->GetWidth(),
        m_pImage->GetHeight(),
        Gdiplus::UnitPixel);
}

SIZE  GifOleElement::GetGifSize()
{
    SIZE s = {0};

    if (!m_pImage)
        return s;

    s.cx = m_pImage->GetWidth();
    s.cy = m_pImage->GetHeight();

    return s;
}

bool  GifOleElement::LoadGif(LPCTSTR szPath)
{
    UnloadImage();

    if (!szPath)
        return false;

    if (!PathFileExists(szPath))
        return false;

    m_pImage = Gdiplus::Bitmap::FromFile(szPath);
    if (!m_pImage)
        return false;

    m_bNeedDeleteImage = true;
    InitGif();
    return true;
}

void  GifOleElement::LoadOuterGifRes(Gdiplus::Bitmap* p)
{
    UnloadImage();
    m_pImage = p;
    m_bNeedDeleteImage = false;
    if (m_pImage)
    {
        InitGif();
    }
}

void  GifOleElement::InitGif()
{
    if (!m_pImage)
        return;

    UINT  nDimensionCount = m_pImage->GetFrameDimensionsCount();
    if (nDimensionCount < 1)
        return;

    // gif只有一个resolutions
    GUID  dimensionID = GUID_NULL;
    if(Gdiplus::Ok != m_pImage->GetFrameDimensionsList(&dimensionID, 1))
        return;

    // 单帧gif直接忽略
    m_nFrameCount = m_pImage->GetFrameCount(&dimensionID);
    if (m_nFrameCount <= 1)
        return;

    UINT nPropertySize = m_pImage->GetPropertyItemSize(PropertyTagFrameDelay);
    if (!nPropertySize)
        return;

    Gdiplus::PropertyItem* pPropertyItem = (Gdiplus::PropertyItem*)malloc(nPropertySize);
    m_pImage->GetPropertyItem(PropertyTagFrameDelay, nPropertySize, pPropertyItem);
    for (UINT i = 0; i < m_nFrameCount; i++)
    {
        UINT nDelay = ((long*)pPropertyItem->value)[i]*10;
        m_vecFrameDelay.push_back(nDelay);
    }
    free(pPropertyItem);

	m_nPrevFrameTick = GetTickCount();
	RegisterREOleTimer(static_cast<IREOleTimerCallback*>(this));
}

void  GifOleElement::NextFrame()
{
    if (!m_pImage || m_nFrameCount <= 1)
        return;

    m_nCurrentFrame++;
    if (m_nCurrentFrame >= m_nFrameCount)
        m_nCurrentFrame = 0;

    // 注：SelectActiveFrame非常慢
    m_pImage->SelectActiveFrame(&Gdiplus::FrameDimensionTime, m_nCurrentFrame);
	m_nPrevFrameTick = GetTickCount();
}

UINT  GifOleElement::GetCurrentFrameDelay()
{
	UINT nSize = m_vecFrameDelay.size();	
	if (m_nCurrentFrame >= nSize)
		return 0;

	return m_vecFrameDelay[m_nCurrentFrame];
}

void  GifOleElement::OnREOleTimer()
{
	if (!m_pREOle)
		return;
	if (!m_pREOle->IsVisibleInField())
		return;

	UINT lElapse = GetTickCount() - m_nPrevFrameTick;		
	if (lElapse >= GetCurrentFrameDelay())
	{
		NextFrame();
		if (m_pREOle)
		{
			m_pREOle->InvalidateOle();
		}
	}
}