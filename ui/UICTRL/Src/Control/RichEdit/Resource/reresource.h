#pragma once
#include "..\UISDK\Inc\Util\iimage.h"
#include "..\UISDK\Inc\Interface\iskindatasource.h"
#include "..\UISDK\Inc\Util\struct.h"

namespace UI
{

class ResBase
{
public:
    ResBase();
    long  AddRef();
    long  Release();

    bool  LoadImage(LPCTSTR szId, ImageWrap* pImage);
    bool  LoadGdiplusBitmap(LPCTSTR szId, GdiplusBitmapLoadWrap* pBitmap);
    bool  LoadGdiplusBitmapFromRes(long lId, Gdiplus::Bitmap** ppBitmap);


protected:
    virtual void  OnLoad() PURE;
    virtual void  OnUnload() PURE;

private:
    long  m_dwRef;
    
};

class BubbleRes : public ResBase
{
public:
	static BubbleRes*  Get();
	BubbleRes();

	ImageWrap*  GetLeftBubbleImage() { return &m_imageLeftBubble; }
	ImageWrap*  GetRightBubbleImage() { return &m_imageRightBubble; }

	C9Region*  GetLeftBubbleStretch() { return &m_stretchLeftBubble; }
	C9Region*  GetRightBubbleStretch() { return &m_stretchRightBubble; }
	REGION4*   GetLeftBubbleIndent() { return &m_indentLeftBubble; }
	REGION4*   GetRightBubbleIndent() { return &m_indentRightBubble; }

protected:
	virtual void  OnLoad() override;
	virtual void  OnUnload() override;

private:
	// 气泡图片
	ImageWrap  m_imageLeftBubble;
	ImageWrap  m_imageRightBubble;

	// 气泡绘制拉伸系数
	C9Region  m_stretchLeftBubble;
	C9Region  m_stretchRightBubble;

	// 气泡中的内容缩进
	REGION4  m_indentLeftBubble;
	REGION4  m_indentRightBubble;
	
};

// 放大镜按钮图片资源
class MagnifierRes : public ResBase
{
public:
	static MagnifierRes*  Get();

	ImageWrap*  GetNormalImage() { return &m_imageNormal; }
	ImageWrap*  GetHoverImage() { return &m_imageHover; }
	ImageWrap*  GetPressImage() { return &m_imagePress; }

private:
	virtual void  OnLoad() override;
	virtual void  OnUnload() override;

private:
	ImageWrap  m_imageNormal;
	ImageWrap  m_imageHover;
	ImageWrap  m_imagePress;
};


// 图片加载、失败
class PictureLoadingFailureRes : public ResBase
{
public:
    PictureLoadingFailureRes();
    static PictureLoadingFailureRes*  Get();
    ImageWrap*  GetFailureImage();
    Gdiplus::Bitmap*  GetLoadingGif();

private:
    virtual void  OnLoad() override;
    virtual void  OnUnload() override;

private:
    ImageWrap  m_imageFailure;
    GdiplusBitmapLoadWrap  m_imageLoaddingGif;
};


}