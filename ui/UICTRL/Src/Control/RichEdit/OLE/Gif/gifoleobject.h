#pragma 
#include "UI\UISDK\Inc\Util\igifimage.h"
#include "UI\UICTRL\Inc\Interface\irichedit.h"
#include "..\richeditolemgr.h"
#include "..\reoleimpl.h"


namespace UI
{
class GifImageItem;
class GifImageItemMgr;

class GifOle : public REOleImpl
{
protected:
	GifOle(IUIApplication* pUIApp, IMessage* pNotifyObj);
public:
	~GifOle();
    static GifOle* CreateInstance(IUIApplication* pUIApp, IMessage* pNotifyObj);

	HRESULT  LoadGif(LPCTSTR szPath);
    HRESULT  LoadSkinGif(LPCTSTR szId);
	HRESULT  Refresh();
    void     OnTick();

    virtual REOLETYPE  GetOleType() override { return REOLE_GIF; }
    virtual void  OnDraw(HDC hDC, RECT* prc) override;
	virtual void  GetSize(SIZE* pSize) override;
    virtual HRESULT  GetClipboardData(UINT nClipFormat, __out BSTR* pbstrData);
    virtual HRESULT  GetClipboardDataEx(IDataObject* pDataObject);
    
protected:

    IUIApplication*   m_pUIApp;
    IGifImage*        m_pGifImage;   // 从文件加载的图片, 从skin加载时，m_pGifImage为空不使用，这样也不用管释放 了
    IGifImageRender*  m_pGifRender;  // gif绘制接口

	IMessage*         m_pNotifyMsg;
	String            m_strPath;
};


}