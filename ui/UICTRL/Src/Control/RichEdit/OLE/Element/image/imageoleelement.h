#pragma once
#include "..\oleelement.h"
#include "..\UISDK\Inc\Util\iimage.h"
#include <gdiplus.h>

namespace UI
{

class ImageOleElement : public OleElement
{
public:
    enum {TYPE = 140281055};

    ImageOleElement();
    ~ImageOleElement();

    static ImageOleElement* CreateInstance();

public:
    bool  LoadImage(LPCTSTR  szPath);
	bool  LoadBitmap(HBITMAP hBitmap);
    void  LoadOuterImageRes(ImageWrap* p);
	bool  SaveImage(LPCTSTR path);

    ImageWrap*  GetImage() const;
    SIZE  GetImageSize() const;
	bool  GetImageMD5(char buf[256]);

public:
    virtual long  GetType() override;
    virtual void  Draw(HDC hDC, int xOffset, int yOffset) override;

private:
    void  UnloadImage();
    void  DrawImage(ImageWrap* pImage, HDC hDC, int xOffset, int yOffset);
	void  createAABitmap();

private:
    ImageWrap*  m_pImage;  // 引用图片资源中的图片
	Gdiplus::Bitmap*  m_pBitmapForAA;  // 用于缩放时的抗锯齿
	bool  m_bNeedDeleteImage;  // 图片是自己new出来的还是外部设进来的
};

}