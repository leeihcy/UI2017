#include "stdafx.h"
#include "imageelement.h"
#include "..\layout_context.h"
#include "..\line.h"
#include "..\doc.h"
#include "..\..\richtext.h"

using namespace UI;
using namespace UI::RT;

ImageElement::ImageElement()
{
    m_sizeDraw.cx = m_sizeDraw.cy = 0;
}

void ImageElement::OnLButtonDBClick(UINT nFlags, POINT ptDoc, bool& bHandled)
{
	RichText* pControl = GetControl();
	if (!pControl)
		return;

	pControl->image_dbclick.emit(
		pControl->GetIRichText(), 
		static_cast<IImageElement*>(this));
}

bool  ImageElement::Load(LPCTSTR szPath)
{
	m_image.Destroy();
	
	if (!szPath)
		return false;

	m_image.Load(szPath);
	return !m_image.IsNull();
}

uint  ImageElement::GetImageWidth()
{
    if (!m_image.IsNull())
	    return m_image.GetWidth();

	if (m_cacheImage)
		return m_cacheImage->GetWidth();

	return 0;
}

uint  ImageElement::GetImageHeight()
{
    if (!m_image.IsNull())
		return m_image.GetHeight();

	if (m_cacheImage)
		return m_cacheImage->GetHeight();

	return 0;
}

uint  ImageElement::GetWidth()
{
    return m_sizeDraw.cx;
}

uint  ImageElement::GetHeight()
{
    return m_sizeDraw.cy;
}

void  ImageElement::Draw(HDC hDC, Run* run, RECT* prcRun)
{
	if (SelectionState_Selected == GetSelectionState())
	{
		// 获取当前选区范围
		CaretPos runSelStart;
		CaretPos runSelEnd;

		if (GetDocNode()->GetSelection().
			GetSelectionRange(run, runSelStart, runSelEnd))
		{
			__super::DrawSelectionBackground(hDC, prcRun);
		}
	}

    int x = prcRun->left;
    int y = prcRun->bottom - m_sizeDraw.cy;

	if (!m_image.IsNull())
	{
		m_image.Draw(hDC,
			x, y, m_sizeDraw.cx, m_sizeDraw.cy,
			0, 0, 
			GetImageWidth(),
			GetImageHeight());
	}
	else if (m_cacheImage)
	{
		RECT rcDest = {x, y, x+m_sizeDraw.cx, y+m_sizeDraw.cy};
		m_cacheImage->Draw(hDC, &rcDest, ImageCache::DrawNormal);
	}
}

void ImageElement::LayoutMultiLine(MultiLineLayoutContext* context)
{
    SIZE sPage = { context->GetLineContentWidth(), context->nPageContentHeight };
    SIZE s = GetLayoutSize(
        sPage, 
        context->getLineRemainWidth()
        );
    context->BeginLayoutElement(this);
        context->PushChar(0, s.cx);
    context->EndLayoutElement(this);
}



// 将内容限制在200*200以内，不跟随页面大小自适应（体验不好，占据一页篇幅太大）
SIZE  ImageElement::GetLayoutSize(SIZE pageContentSize, int lineRemain)
{
    int imageWidth = GetImageWidth();
    int imageHeight = GetImageHeight();

    m_sizeDraw.cy = imageHeight;
	m_sizeDraw.cx = imageWidth;

	if (imageWidth <= 0)
	{
		UIASSERT(0);
		return m_sizeDraw;
	}

	// 决定本行是否能够通过缩小图片来显示下，先定义一个图片允许的最小显示尺寸
	static const int MIN_IMAGE_DRAW_WIDTH = UI::ScaleByDpi(64);
	static const int MAX_IMAGE_DRAW_SIZE = UI::ScaleByDpi(200);

	if (imageWidth <= MAX_IMAGE_DRAW_SIZE && imageHeight <= MAX_IMAGE_DRAW_SIZE)
	{
		return m_sizeDraw;
	}

	if (imageWidth > imageHeight)
	{
		m_sizeDraw.cx = MAX_IMAGE_DRAW_SIZE;
		m_sizeDraw.cy = 
			(uint)((float)imageHeight / (float)imageWidth * (float)MAX_IMAGE_DRAW_SIZE);
	}
	else
	{
		m_sizeDraw.cy = MAX_IMAGE_DRAW_SIZE;
		m_sizeDraw.cx = 
			(uint)((float)imageWidth / (float)imageHeight * (float)MAX_IMAGE_DRAW_SIZE);
	}

    return m_sizeDraw;
}

void  ImageElement::SetCacheImage(const CCacheImage& image)
{
	m_image.Destroy();
	m_cacheImage = image;
	if (m_cacheImage)
	{
		m_cacheImage.SetUpdateCallback(
			static_cast<ICacheImageUpdateCallback*>(this));
	}
}
void  ImageElement::OnCacheImageUpdate(ICacheImage*)
{
	Doc* doc = GetDocNode();
	if (!doc)
		return;

	RichText* pControl = doc->GetControl();
	UIASSERT(pControl);

	pControl->SetLayoutDirty(true);
	doc->InvalidRegion(nullptr);
	doc->Refresh();
}

LPCTSTR UI::RT::ImageElement::GetPath()
{
	if (m_cacheImage)
	{
		return m_cacheImage->GetLocalPath();
	}

	return nullptr;
}
