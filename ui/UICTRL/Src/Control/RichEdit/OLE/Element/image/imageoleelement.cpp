#include "stdafx.h"
#include "ImageOleElement.h"

using namespace UI;

ImageOleElement::ImageOleElement()
{
    m_pImage = NULL;
    m_bNeedDeleteImage = false;
    m_style.bEnable = false; // 不接收鼠标消息
	m_pBitmapForAA = nullptr;
}
ImageOleElement::~ImageOleElement()
{
    UnloadImage();
}
void  ImageOleElement::UnloadImage()
{
	if (m_pBitmapForAA)
	{
		SAFE_DELETE(m_pBitmapForAA);
	}
    if (m_bNeedDeleteImage && m_pImage)
    {
        SAFE_DELETE(m_pImage);
        m_bNeedDeleteImage = false;
        return;
    }

    m_pImage = NULL;
}

ImageOleElement*  ImageOleElement::CreateInstance()
{
    return new ImageOleElement;
}

long  ImageOleElement::GetType() 
{
    return ImageOleElement::TYPE;
}

void  ImageOleElement::Draw(HDC hDC, int xOffset, int yOffset)
{
    DrawImage(m_pImage, hDC, xOffset, yOffset);
}

void  ImageOleElement::DrawImage(
	ImageWrap* pImage, HDC hDC, int xOffset, int yOffset)
{
    if (!pImage || pImage->IsNull())
        return;

	if (m_rcInOle.Width() != pImage->GetWidth() ||
		m_rcInOle.Height() != pImage->GetHeight())
	{
		createAABitmap();
		if (m_pBitmapForAA)
		{
			Gdiplus::Graphics g(hDC);
			g.SetInterpolationMode(
				Gdiplus::InterpolationModeHighQualityBicubic);

			Gdiplus::RectF dest;
			dest.X = (Gdiplus::REAL)(m_rcInOle.left + xOffset);
			dest.Y = (Gdiplus::REAL)(m_rcInOle.top + yOffset);
			dest.Width = (Gdiplus::REAL)(m_rcInOle.Width());
			dest.Height = (Gdiplus::REAL)(m_rcInOle.Height());

			g.DrawImage(m_pBitmapForAA,
				dest,
				0, 0,
				(Gdiplus::REAL)(m_pBitmapForAA->GetWidth()),
				(Gdiplus::REAL)(m_pBitmapForAA->GetHeight()),
				Gdiplus::UnitPixel);

			return;
		}
	}

    pImage->Draw(hDC, 
        m_rcInOle.left + xOffset,
        m_rcInOle.top + yOffset, 
        m_rcInOle.Width(),
        m_rcInOle.Height(),
        0, 0, 
        pImage->GetWidth(),
        pImage->GetHeight());
}

ImageWrap*  ImageOleElement::GetImage() const
{
    return m_pImage;
}

SIZE  ImageOleElement::GetImageSize() const
{
    SIZE s = {0};
    if (!m_pImage || m_pImage->IsNull())
    {
        return s;
    }

    s.cx = m_pImage->GetWidth();
    s.cy = m_pImage->GetHeight();
    return s;
}

bool  ImageOleElement::LoadBitmap(HBITMAP hBitmap)
{
	UnloadImage();

	if (!hBitmap)
		return false;

	m_pImage = new ImageWrap;
	m_bNeedDeleteImage = true;

	if (hBitmap)
	{
		m_pImage->Attach(hBitmap, true);
	}
	if (m_pImage->IsNull())
	{
		UnloadImage();
	}
#ifdef _DEBUGx
	m_pImage->SaveAsPng(L"D:\\aaa.png");
#endif

	return true;
}
bool  ImageOleElement::LoadImage(LPCTSTR  szPath)
{
    UnloadImage();

    if (!szPath)
        return false;

    m_pImage = new ImageWrap;
    m_bNeedDeleteImage = true;

    if (szPath)
    {
        m_pImage->Load(szPath, true);
    }
    if (m_pImage->IsNull())
    {
        UnloadImage();
    }

    return true;
}

void  ImageOleElement::LoadOuterImageRes(ImageWrap* p)
{
    UnloadImage();
    m_pImage = p;
    m_bNeedDeleteImage = false;
}

// #include "include\util\util_inc.h"
// #pragma comment(lib, "Util.lib")
bool  ImageOleElement::GetImageMD5(char buf[256])
{
	UIASSERT(0);
	return false;
// 	if (!m_pImage || m_pImage->IsNull())
// 		return false;
// 
// 	byte* pBit = m_pImage->GetBits();
// 	int nPitch = m_pImage->GetPitch();
// 	if (nPitch < 0)
// 	{
// 		pBit = pBit + (nPitch * (m_pImage->GetHeight()-1));
// 	}
// 	int size = abs(nPitch) * m_pImage->GetHeight();
// 	Lenovo::GetMD5(pBit, size, buf);
// 	return true;
}

bool  ImageOleElement::SaveImage(LPCTSTR path)
{
	if (!m_pImage || m_pImage->IsNull())
		return false;

	m_pImage->SaveAsPng(path);
	return true;
}

void UI::ImageOleElement::createAABitmap()
{
	if (m_pBitmapForAA)
		return;

	if (!m_pImage || m_pImage->IsNull() || m_pImage->GetBPP() != 32)
		return;

	m_pBitmapForAA = new Gdiplus::Bitmap(
		m_pImage->GetWidth(),
		m_pImage->GetHeight(), 
		m_pImage->GetPitch(),
		PixelFormat32bppPARGB,
		m_pImage->GetBits());
}
