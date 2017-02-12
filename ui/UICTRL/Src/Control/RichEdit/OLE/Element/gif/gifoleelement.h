#pragma once
#include "..\oleelement.h"
#include "..\..\reoleapi.h"
#include "..\UISDK\Inc\Util\iimage.h"
#include <gdiplus.h>
#pragma comment(lib, "gdiplus.lib")

namespace UI
{

class GifOleElement : 
		public OleElement,
		public IREOleTimerCallback
{
public:
    enum {TYPE = 141381050};

    GifOleElement();
    ~GifOleElement();

    static GifOleElement* CreateInstance();

public:
    bool  LoadGif(LPCTSTR  szPath);
    void  LoadOuterGifRes(Gdiplus::Bitmap* p);
    SIZE  GetGifSize();

public:
    virtual long  GetType() override;
    virtual void  Draw(HDC hDC, int xOffset, int yOffset) override;

	virtual void  OnREOleTimer() override;

private:
    void  UnloadImage();
    void  InitGif();
    void  NextFrame();
	UINT  GetCurrentFrameDelay();

private:
    Gdiplus::Bitmap*  m_pImage;  // 引用图片资源中的图片
    bool  m_bNeedDeleteImage;  // 图片是自己new出来的还是外部设进来的

    // gif参数
    UINT  m_nFrameCount;
    UINT  m_nCurrentFrame;
	UINT  m_nPrevFrameTick;

    vector<UINT>  m_vecFrameDelay;
};

}