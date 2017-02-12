#include "stdafx.h"
#include "gifoleobject.h"
#include "..\..\windowlessrichedit.h"

namespace UI
{
extern UINT  g_cfRichEditUnicodeXmlFormat;
extern UINT  CF_HTML;
}

GifOle::GifOle(IUIApplication* pUIApp, IMessage* pNotifyObj)
{
    m_pUIApp = pUIApp;
    m_pGifRender = NULL;
    m_pGifImage = NULL;
	m_pNotifyMsg = pNotifyObj;
}
GifOle::~GifOle()
{
	SAFE_RELEASE(m_pGifRender);
    SAFE_DELETE(m_pGifImage);
}

GifOle* GifOle::CreateInstance(IUIApplication* pUIApp, IMessage* pNotifyObj)
{
    GifOle* p = new GifOle(pUIApp, pNotifyObj);
	p->SetInterface(new IGifOle(p));
	return p;
}

HRESULT GifOle::LoadGif(LPCTSTR szPath)
{
    if (m_pGifImage)
        return E_FAIL;

    m_pGifImage = new IGifImage(m_pUIApp);
    if (!m_pGifImage->Load(szPath))
        return E_FAIL;

    m_pGifImage->SetTransparentColor(RGB(255,255,255));

    Gif_Timer_Notify notify;
    memset(&notify, 0, sizeof(notify));
    notify.eType = Gif_Timer_Notify_Send_Msg;
    notify.notify_ui_msg.pNotifyMsgObj = m_pNotifyMsg;
    notify.notify_ui_msg.lParam = (LPARAM)this;

    m_pGifRender = m_pGifImage->AddRender(&notify, m_pUIApp);
    if (!m_pGifRender)
    {
        SAFE_DELETE(m_pGifImage);
        return E_FAIL;
    }
    m_pGifRender->Start();
    m_strPath = szPath;
	return S_OK;
}

HRESULT GifOle::LoadSkinGif(LPCTSTR szId)
{
	UIASSERT(0);
#if 0
    if (m_pGifImage)
        return E_FAIL;

    if (!m_pUIApp)
        return E_FAIL;

    IGifRes* pGifRes = m_pUIApp->GetActiveSkinGifRes();
    if (!pGifRes)
        return E_FAIL;
    
    IGifImage* pGifImage = NULL;
    pGifRes->GetGifImage(szId, &pGifImage);
    if (!pGifImage)
        return E_FAIL;

    pGifImage->SetTransparentColor(RGB(0,0,0));

    Gif_Timer_Notify notify;
    memset(&notify, 0, sizeof(notify));
    notify.eType = Gif_Timer_Notify_Send_Msg;
    notify.notify_ui_msg.pNotifyMsgObj =m_pNotifyMsg;
    notify.notify_ui_msg.lParam = (LPARAM)this;

    m_pGifRender = pGifImage->AddRender(&notify, m_pUIApp);
    if (!m_pGifRender)
        return E_FAIL;
    
    m_pGifRender->Start();
    m_strPath = szId;
#endif
    return S_OK;
}

void  GifOle::OnTick()
{
    WindowlessRichEdit*  pRE = GetWindowlessRE();
    if (!pRE)
        return;
// 
//     int ncp = pRE->GetOleObjectCharPos(static_cast<IOleObject*>(this));
//     if (-1 == ncp)
//         return;
// 
//     // 如果自己是不可见的，不处理
//     int nFirstVisibleCP = 0;
//     int nLastVisibleCP = 0;
//     pRE->GetVisibleCharRange(&nFirstVisibleCP, &nLastVisibleCP);
//     if (ncp < nFirstVisibleCP || ncp > nLastVisibleCP)
//         return;

	if (!IsVisibleInField())
		return;

    // 局部刷新
	RECT rcWnd;
	pRE->GetControlPtr()->ClientRect2WindowRect(&m_rcDrawSave, &rcWnd);
    pRE->InvalidateRect(&rcWnd);
}

HRESULT GifOle::Refresh()
{
	if (m_pViewAdviseSink)
	{
		m_pViewAdviseSink->OnViewChange(DVASPECT_CONTENT,-1);  
	}
	return S_OK;
}

void GifOle::OnDraw(HDC hDC, RECT* prc)
{
	if (NULL == m_pGifRender)
		return;

	m_pGifRender->OnAlphaPaint(hDC, prc->left, prc->top);  // 使用alpha提交，gif可能位于透明背景中。注：使用RGB(0,0,0)背景画刷
	return;
}

void GifOle::GetSize(SIZE* pSize)
{
	if (NULL == pSize)
		return;

	if (NULL == m_pGifRender)
		return;

	pSize->cx = m_pGifRender->GetWidth();
	pSize->cy = m_pGifRender->GetHeight();
	
	return;
}

HRESULT  GifOle::GetClipboardData(UINT nClipFormat, __out BSTR* pbstrData)
{
    if (nClipFormat == CF_UNICODETEXT ||
        nClipFormat == CF_TEXT)
    {
        return S_FALSE;
    }

    if (nClipFormat == g_cfRichEditUnicodeXmlFormat)
    {
        WCHAR  szText[1024] = {0};
        if (m_pGifImage)
        {
            wsprintf(szText, L"<%s %s=\"%s\" />", RE_CF_OLE_GIF, RE_CF_OLE_PATH, m_strPath.c_str());
        }
        else
        {
            wsprintf(szText, L"<%s %s=\"%s\" />", RE_CF_OLE_EMOTION, RE_CF_OLE_ID, m_strPath.c_str());
        }
        *pbstrData = SysAllocString(szText);

        return S_OK;
    }

    if (nClipFormat == CF_HTML)
    {
        return E_FAIL;
    }

    return E_FAIL;
}

HRESULT  GifOle::GetClipboardDataEx(IDataObject* pDataObject)
{
	if (NULL == pDataObject)
		return E_INVALIDARG;

	if (!m_pGifRender)
		return E_FAIL;

	IGifImage*  pGifImage = m_pGifImage;
	if (!pGifImage)
		pGifImage = m_pGifRender->GetIGifImage();
	
	if (!pGifImage)
		return E_FAIL;

	ImageWrap* pImage = pGifImage->GetFrameIImage(0);
	UIASSERT(pImage);
	if (!pImage)
		return E_FAIL;


#pragma region  // CF_DIB
	{
		FORMATETC  format = {0};
		format.dwAspect = DVASPECT_CONTENT;
		format.cfFormat = CF_DIB;
		format.tymed = TYMED_HGLOBAL;

		// CF_DIB的结构为: BITMAPINFOHEADER + BITS
		
		DIBSECTION  dibsec;
		::GetObject(pImage->GetHBITMAP(), sizeof(dibsec), &dibsec);

		LPBYTE  lpImageBits = (LPBYTE)pImage->GetBits();
		int     nPitch = pImage->GetPitch();
		int     nAbsPitch = abs(nPitch);
		int     nImgHeight = pImage->GetHeight();
		int     nImgWidth = pImage->GetWidth();

		HGLOBAL hGlobal = GlobalAlloc(0, sizeof(BITMAPINFOHEADER) + nImgHeight*nAbsPitch);
		LPBYTE  lpbits = (LPBYTE)::GlobalLock(hGlobal);
		
		LPBYTE  pSrc = lpImageBits;
		LPBYTE  pDst = lpbits;

		memcpy(pDst, &dibsec.dsBmih, sizeof(BITMAPINFOHEADER));
		pDst += sizeof(BITMAPINFOHEADER);

		if (nPitch < 0)
		{
			pDst += nAbsPitch*(nImgHeight-1);
		}

		for (int i = 0; i < nImgHeight; i++)
		{
#if 1       // 由于目前解析出来的gif透明位置的像素值为00 00 00 00
			// 导致其它绘图软件粘贴后显示为黑色。因此在这里做一次
			// 转换，将黑色换成白色
			for (int j = 0; j < nAbsPitch; j+=4)
			{
				int n = *(int*)(pSrc+j);
				if (n == 0)
					n = 0x00FFFFFF;

				*(int*)(pDst+j) = n;
			}
#else
			memcpy(pDst, pSrc, abs(nPitch));
#endif
			pDst += nPitch;
			pSrc += nPitch;
		}
		
		::GlobalUnlock(hGlobal);

		STGMEDIUM medium = {0};
		medium.tymed = TYMED_HGLOBAL;
		medium.hGlobal = hGlobal;
		
		if (FAILED(pDataObject->SetData(&format, &medium, TRUE)))
        {
            GlobalFree(hGlobal);
        }
	}
#pragma endregion

#pragma region // CF_BITMAP
	{
		FORMATETC  format = {0};
		format.dwAspect = DVASPECT_CONTENT;
		format.cfFormat = CF_BITMAP;
		format.tymed = TYMED_GDI;

		STGMEDIUM medium = {0};
		medium.tymed = TYMED_GDI;
		medium.hBitmap = (HBITMAP)OleDuplicateData((HANDLE)pImage->GetHBITMAP(), CF_BITMAP, 0);;
		pDataObject->SetData(&format, &medium, TRUE);
	} 
#pragma endregion

	return S_OK;
}

