#ifndef _INCLUDE_IMAGECACHE_DEF_H_
#define _INCLUDE_IMAGECACHE_DEF_H_

namespace UI
{

namespace ImageCache
{
	// TODO: 这块写的比较烂，没想好怎么优化
	enum RenderType
	{
		RenderOrigin,
		RenderOriginNoDpi,   // 禁用dpi缩放。如聊天中的图片
		RenderOriginAANoDpi, // 需要抗锯齿
		Render30Round,
		Render32Round,
		Render36Round,
		Render50Round,
	};
	enum ImageType
	{
		ImageCommon,
		ImageUserPortrait, // 用户头像
		ImageDeviceIcon,   // 设备图标
	};

	enum ImageDrawFlag
	{
		DrawStretch,  // 拉伸
		DrawNormal,   // 左上角直接绘制
		DrawCenter,   // 居中绘制不拉伸
		DrawAdapt,    // 自适应
	};
}
interface ICacheImageUpdateCallback;
interface ICacheImage
{
	virtual void  AddRef() = 0;
	virtual void  Release() = 0;

	virtual bool  IsExist() = 0;
	virtual bool  IsDynamic() = 0;
	virtual void  Draw(HDC hDC, int x, int y) = 0;
	virtual void  Draw(HDC hDC, LPCRECT prcDest, ImageCache::ImageDrawFlag flags) = 0;
	virtual int   GetWidth() = 0;
	virtual int   GetHeight() = 0;
	virtual LPCTSTR  GetLocalPath() = 0;

	virtual void  xxxAddCallback(ICacheImageUpdateCallback*) = 0;
	virtual void  xxxRemoveCallback(ICacheImageUpdateCallback*) = 0;
};

interface ICacheImageUpdateCallback
{
	virtual void OnCacheImageUpdate(ICacheImage*) = 0;
};

class CCacheImage
{
public:
	CCacheImage() : m_p(nullptr)
	{
		m_pCallback = nullptr;
	}
	CCacheImage(ICacheImage* p) : m_p(p)
	{
		if (m_p)
			m_p->AddRef();
		
		m_pCallback = nullptr;
	}
	CCacheImage(const CCacheImage& o)
	{
		m_p = o.m_p;
		if (m_p)
			m_p->AddRef();

		m_pCallback = nullptr;
		SetUpdateCallback(o.m_pCallback);
	}
	CCacheImage& operator = (const CCacheImage& o)
	{
		m_p = o.m_p;
		if (m_p)
			m_p->AddRef();

		m_pCallback = nullptr;
		SetUpdateCallback(o.m_pCallback);
		return *this;
	}
	~CCacheImage()
	{
		if (m_p)
		{
			if (m_pCallback)
			{
				m_p->xxxRemoveCallback(m_pCallback);
			}

			m_p->Release();
			m_p = nullptr;
		}
	}

	void  SetUpdateCallback(ICacheImageUpdateCallback* p)
	{
		if (m_pCallback)
		{
			if (m_p)
			{
				m_p->xxxRemoveCallback(m_pCallback);
			}
		}
		m_pCallback = p;
		if (m_p)
		{
			m_p->xxxAddCallback(m_pCallback);
		}
	}

	ICacheImage* operator->()
	{
		return m_p;
	}

	bool operator!() const 
	{
		return !m_p;
	}
	operator bool()
	{
		return m_p?true:false;
	}

	const ICacheImage* get() const
	{
		return m_p;
	}

private:
	ICacheImage*  m_p;
	ICacheImageUpdateCallback*  m_pCallback;
};
}
#endif