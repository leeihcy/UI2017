#ifndef UI_IRICHEDITOLE_H_201411071253
#define UI_IRICHEDITOLE_H_201411071253
#include "include\imagecache_def.h"

namespace UI
{
class REOleBase;
class PictureOle;

enum REOLETYPE
{
    REOLE_INVALID,
    REOLE_COM,
    REOLE_PICTURE,
    REOLE_GIF,
};

interface  IREOleExternalData
{
    virtual void  Release() PURE;
};
interface UICTRL_API IREOleBase
{
public:
    IREOleBase(REOleBase* p);

    REOLETYPE  GetOleType();
	void  AddBizProp(LPCTSTR key, LPCTSTR value);
	void  ClearBizProp();
	LPCTSTR  GetBizProp(LPCTSTR key);

private:
    REOleBase*  m_pImplBase;
};


enum REOleLoadStatus
{
    REOleLoadStatus_NOT_INITIALIZE =-1,  // 未初始化，不该出现的状态
    REOleLoadStatus_LOADING = 0,
    REOleLoadStatus_SUCCESS,
    REOleLoadStatus_OVERDUE,
    REOleLoadStatus_FAILURE,
};
struct PictureREOleInfo
{
    long lReverse;

    LPCTSTR  szId;
    LPCTSTR  szName;
    LPCTSTR  szDesc;
    LPCTSTR  szType;

//  REOleLoadStatus  eStatus;
// 	LPCTSTR  szPath;
// 	CCacheImage* pCacheImage;  // 如果有cacheImage，就忽略szPath
// 	LPCTSTR  szOriginPath;     // 原图路径，用于双击浏览
};

// 点击了图片上的放大镜按钮
// wParam: IPictureREOle
#define UI_REOLE_PICTURE_MAGNIFIER_BTN_CLICK  141421315

// 双击了图片
// wParam: IPictureREOle
#define UI_REOLE_PICTURE_DBCLICK  141421316

interface UICTRL_API IPictureREOle : public IREOleBase
{
public:
    IPictureREOle(PictureOle*);
    LPCTSTR  GetPath();
    void  UpdateStatus(REOleLoadStatus e, LPCTSTR szPath=NULL);
	bool  GetImageMD5(char buf[256]);
	bool  SaveImage(LPCTSTR path);
	void  SetPath(LPCTSTR path);

	void  LoadFile(LPCTSTR path);
	void  LoadUrl(LPCSTR url);
	void  SetCacheImage(const UI::CCacheImage* image);

private:
    PictureOle*  m_pImpl;
};

class GifOle;
interface IGifOle : public IREOleBase
{
public:
	IGifOle(GifOle* p);
};

}


#endif // UI_IRICHEDITOLE_H_201411071253