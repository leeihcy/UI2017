#include "stdafx.h"
#include "windowlessricheditex.h"
#include "3rd\markup\markup.h"
#include "Ole\Gif\gifoleobject.h"
#include "OLE\Composite\Picture\pictureole.h"
#include "OLE\reoleapi.h"
#include "Bubble\rebubblemgr.h"
LONG DXtoHimetricX(LONG dx, LONG xPerInch=-1);

using namespace UI;

WindowlessRichEditEx::WindowlessRichEditEx()
{
    m_bMsgHandled = FALSE;
    m_pBubbleMgr = NULL;
}
WindowlessRichEditEx::~WindowlessRichEditEx()
{
    EnableBubbleMode(false);
}

void  WindowlessRichEditEx::OnCreate()
{
    // 设置资源读取接口
    IRichEdit* pRE = GetControlPtr();
    if (pRE)
    {
        SetREOleResUIApplication(pRE->GetUIApplication());
    }
}

void  WindowlessRichEditEx::EnableBubbleMode(bool b)
{
    if (b)
    {
        if (!m_pBubbleMgr)
        {
            m_pBubbleMgr = new BubbleManager(this);
            m_pBubbleMgr->Init();
        }
    }
    else
    {
        SAFE_DELETE(m_pBubbleMgr);
    }
}

bool  WindowlessRichEditEx::IsBubbleMode()
{
    return m_pBubbleMgr ? true:false;
}

REBubble*  WindowlessRichEditEx::CreateBubble(
	BubbleType eType, BubbleAlign eAlign, int nStartCP, int nEndCP)
{
    // 需要先调用EnableBubbleMode
    if (!m_pBubbleMgr)
        return NULL;

    REBubble*  pBubble = m_pBubbleMgr->CreateBubble(eType, eAlign, nStartCP, nEndCP);
    return pBubble;
}

// 两个连续气泡之间的间隙
// 原理：插入一个空白行，将该行的左缩进设置为无限大（即无法看到）
// 注：当前位置应该是在最后一个\r\n处
void  WindowlessRichEditEx::InsertBubbleSpace()
{
    CHARFORMAT2 cf;
    cf.cbSize = sizeof(cf);
    CopyDefaultCharFormat(&cf);

    PARAFORMAT2 pf;
    pf.cbSize = sizeof(pf);
	CopyDefaultParaFormat(&pf);
    pf.dxStartIndent = 200000;

	SetSelectionCharFormat(&cf);
	SetSelectionParaFormat(&pf);

	InsertText(TEXT("\r\n"));
	ResetSelectionParaFormat();
	ResetSelectionCharFormat();
}

void  WindowlessRichEditEx::OnOleSizeChanged(REOleBase* pItem)
{
    if (!pItem || !m_pBubbleMgr)
        return;

    int cp = GetOleCharPos(pItem);
    if (-1 == cp)
        return;

    REBubble* pBubble = m_pBubbleMgr->FindBubble(cp);
    if (!pBubble)
        return;

    m_pBubbleMgr->InvalidBubbleSize(pBubble);
}

bool  WindowlessRichEditEx::InsertGif(LPCTSTR szGifPath)
{
    GifOle* pGifOle = GifOle::CreateInstance(
        m_pIRichEdit->GetUIApplication(), 
        static_cast<IMessage*>(this->m_pIRichEdit));

    HRESULT hr = pGifOle->LoadGif(szGifPath);
    if (FAILED(hr))
    {
        SAFE_DELETE(pGifOle);
        return false;
    }

    return this->InsertOleObject(pGifOle);
}

bool WindowlessRichEditEx::InsertSkinGif(LPCTSTR szGifId)
{
    GifOle* pGifOle = GifOle::CreateInstance(
        m_pIRichEdit->GetUIApplication(), 
        static_cast<IMessage*>(m_pIRichEdit));

    HRESULT hr = pGifOle->LoadSkinGif(szGifId);
    if (FAILED(hr))
    {
        SAFE_DELETE(pGifOle);
        return false;
    }

    return this->InsertOleObject(pGifOle);
}

IPictureREOle*  WindowlessRichEditEx::InsertPicture(PictureREOleInfo* pInfo)
{
    if (!pInfo)
        return NULL;

    PictureOle* pOle = PictureOle::CreateInstance(pInfo);
    if (!pOle)
        return 0;

    this->InsertOleObject(pOle);
    return pOle->GetIPictureREOle();
}

IPictureREOle*  WindowlessRichEditEx::InsertPicture(HBITMAP bitmap)
{
	if (!bitmap)
		return nullptr;

	PictureOle* pOle = PictureOle::CreateInstance(bitmap);
	if (!pOle)
		return 0;

	this->InsertOleObject(pOle);
	return pOle->GetIPictureREOle();
}

// IPictureREOle*  WindowlessRichEditEx::FindPicture(LPCTSTR szId)
// {
//     if (!szId)
//         return NULL;
// 
//     REOleBase*  pOle = m_pOleMgr->FindOle(REOLE_PICTURE, szId);
//     if (!pOle)
//         return NULL;
// 
//     PictureOle* pPictureOle = static_cast<PictureOle*>(pOle);
//     return pPictureOle->GetIPictureREOle();
// }

bool  WindowlessRichEditEx::PasteEncodeText(LPWSTR szData)
{
    return ReplaceSelectionEncodeText(szData, wcslen(szData));
}
bool  WindowlessRichEditEx::ReplaceSelectionEncodeText(LPCTSTR szText, int nSize)
{
    if (NULL == szText)
        return false;

    CMarkup markup;
    if (!markup.SetDoc(szText))
        return false;

    if (!markup.NextElem(RE_CF_ROOT))
        return false;

    // 解析字体样式
    String  strColor = markup.GetAttrib(RE_CF_FONT_COLOR);
    String  strFace = markup.GetAttrib(RE_CF_FONT_FACE);
    String  strHeight = markup.GetAttrib(RE_CF_FONT_HEIGHT);
    String  strBold = markup.GetAttrib(RE_CF_FONT_BOLD);
    String  strItalic = markup.GetAttrib(RE_CF_FONT_ITALIC);
    String  strUnderline = markup.GetAttrib(RE_CF_FONT_UNDERLINE);

    CHARFORMAT2 cf;
    memset(&cf, 0, sizeof(cf));
    cf.cbSize = sizeof(cf);
    CopyDefaultCharFormat(&cf);

    cf.dwMask = CFM_ALL;
    cf.yHeight = _ttoi(strHeight.c_str());
    if (cf.yHeight == 0)
        cf.yHeight = m_cf.yHeight;
    cf.crTextColor = _ttoi(strColor.c_str());
    _tcscpy(cf.szFaceName, strFace.c_str());
    if (cf.szFaceName[0] == 0)
        _tcscpy(cf.szFaceName, m_cf.szFaceName);
    if (strBold == L"1")
        cf.dwEffects |= CFE_BOLD;
    if (strItalic == L"1")
        cf.dwEffects |= CFE_ITALIC;
    if (strUnderline == L"1")
        cf.dwEffects |= CFE_UNDERLINE;

    String  strAlign = markup.GetAttrib(RE_CF_PARA_ALIGN);
    String  strLineSpacing = markup.GetAttrib(RE_CF_LINESPACING);
    String  strLineBefore = markup.GetAttrib(RE_CF_LINEBEFORE);
    String  strLineAfter = markup.GetAttrib(RE_CF_LINEAFTER);

    PARAFORMAT2  pf;
    memset(&pf, 0, sizeof(pf));
    pf.cbSize = sizeof(pf);
    CopyDefaultParaFormat(&pf);

    if (!strAlign.empty())
    {
        pf.dwMask = PFM_ALIGNMENT;
        pf.wAlignment = _ttoi(strAlign.c_str());
        if (pf.wAlignment == 0)
            pf.wAlignment = PFA_LEFT;
    }

    if (!strLineSpacing.empty())
    {
        pf.dwMask |= PFM_LINESPACING;
        pf.dyLineSpacing = DXtoHimetricX(_ttoi(strLineSpacing.c_str()));
        pf.bLineSpacingRule = tomLineSpaceExactly;
    }

    if (!strLineAfter.empty())
    {
        pf.dwMask |= PFM_SPACEAFTER;
        pf.dySpaceAfter = DXtoHimetricX(_ttoi(strLineAfter.c_str()));
    }

    if (!strLineBefore.empty())
    {
        pf.dwMask |= PFM_SPACEBEFORE;
        pf.dySpaceBefore = DXtoHimetricX(_ttoi(strLineBefore.c_str()));
    }

    SetSelectionCharFormat(&cf);
    SetSelectionParaFormat(&pf);

    // 解析文本与ole对象
    if (markup.IntoElem())
    {
        while (markup.NextElem())
        {
            String strTag = markup.GetTagName();
            if (strTag == RE_CF_OLE_TEXT)
            {
                ReplaceSelectionText(markup.GetData().c_str(), -1);
            }
            else if (strTag == RE_CF_OLE_COM)
            {
                String strCLSID = markup.GetAttrib(RE_CF_OLE_COM_CLSID);

                CLSID clsid = {0};
                CLSIDFromString((LPOLESTR)strCLSID.c_str(), &clsid);
                InsertComObject(clsid);
            }
            else if (strTag == RE_CF_OLE_EMOTION)
            {
                String strId = markup.GetAttrib(RE_CF_OLE_ID);
                InsertSkinGif(strId.c_str());
            }
            else if (strTag == RE_CF_OLE_GIF)
            {
                String strPath = markup.GetAttrib(RE_CF_OLE_PATH);
                InsertGif(strPath.c_str());
            }
            else if (strTag == RE_CF_OLE_PICTURE)
            {
                String strPath = markup.GetAttrib(RE_CF_OLE_PATH);
               
				PictureREOleInfo info = {0};
				// 拷贝扩展属性，用于业务支持
				IPictureREOle* pOle = InsertPicture(&info);
				pOle->LoadFile(strPath.c_str());

				int i = 1;
				while (1)
				{
					String key = markup.GetAttribName(i);
					if (key.empty())
						break;

					String value = markup.GetAttrib(key);
					pOle->AddBizProp(key.c_str(), value.c_str());
					i++;
				}
            }
        }
    }

    return true; 
}

struct InsertMessageContext
{
    enum {MAGIC = 141371003};
    InsertMessageContext()
    {
        nMagicNumber = MAGIC;
        nStartCP = 0;
        eAlign = BUBBLE_ALIGN_LEFT;
		eType = BUBBLE_NORMAL;
    }

    int  nMagicNumber;
    int  nStartCP;
	BubbleType  eType;
    BubbleAlign  eAlign;
};

// 开始插入一段消息，用于支持分多次插入一条消息的内容
// 返回值，用于传递给InsertMessageFinish
HANDLE  WindowlessRichEditEx::InsertMessageStart(BubbleType eType, BubbleAlign eAlign)
{
    if (!IsBubbleMode())
        return NULL;

    int nStartCP = GetSelectionStart();
    InsertMessageContext*  pContext = new InsertMessageContext;
    pContext->nStartCP = nStartCP;
    pContext->eAlign = eAlign;
	pContext->eType = eType;

    // 判断上一个位置是否是一个气泡。连续两个气泡之间要插入一个分隔行
    if (0 == nStartCP || m_pBubbleMgr->IsInBubble(nStartCP-1, NULL))
    {
        InsertBubbleSpace();
        pContext->nStartCP = GetSelectionStart();
    }

    return (HANDLE)pContext;
}

// 结束一段消息的插入
IREBubble*  WindowlessRichEditEx::InsertMessageFinish(HANDLE hInsertStart)
{
	REBubble* pBubble = NULL;

    InsertMessageContext*  pContext = (InsertMessageContext*)hInsertStart;
    if (pContext && pContext->nMagicNumber != InsertMessageContext::MAGIC)
        pContext = NULL;

    if (pContext)
    {
        int nEndcp = GetSelectionStart();
        pBubble = CreateBubble(pContext->eType, pContext->eAlign, pContext->nStartCP, nEndcp);

        // 全量刷新，也能解决气泡比内容大导致的刷新不完整问题
        InvalidateRect(NULL);

        SAFE_DELETE(pContext);
    }

    InsertText(_T("\r\n")); 
    ResetSelectionCharFormat();
    ResetSelectionParaFormat();

    // 由外部去scrollend
    // ScrollEnd();
// 	if (pBubble)
// 		return pBubble->GetIREBubble();
	return NULL;
}

bool  WindowlessRichEditEx::InsertEncodeText(LPCTSTR szText, int nSize)
{
    return ReplaceSelectionEncodeText(szText, nSize);
}
bool  WindowlessRichEditEx::InsertEncodeText(int nPos, LPCTSTR szText, int nSize)
{
    SetSel(nPos);
    return ReplaceSelectionEncodeText(szText, nSize);
}
bool  WindowlessRichEditEx::AppendEncodeText(LPCTSTR szText, int nSize)
{
    SelectEnd();
    return InsertEncodeText(szText, nSize);
}


LRESULT  WindowlessRichEditEx::OnGifFrameTick(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if (!m_pRichEdit)
        return 0;

    // 	if (false == DoGifOleUpdateRequst()) // 该函数主要用于限制动画刷新帧数
    // 		return 0;

    GifOle*  pGifOleObj = (GifOle*)lParam;
    pGifOleObj->OnTick();

    return 0;
}

void  WindowlessRichEditEx::PreDraw(HDC hDC, RECT* prcDraw)
{
	__super::PreDraw(hDC, prcDraw);

    if (m_pBubbleMgr)
	{
		// 先绘制一遍，将内容布局好，才能正确绘制气泡。
		// 将内容画到另一个临时DC上
		IUIApplication* pUIApp = m_pIRichEdit->GetUIApplication();
		HDC hTempDC = pUIApp->GetCacheDC();
		HRGN hRgn = CreateRectRgn(0, 0, 0, 0);
		SelectClipRgn(hTempDC, hRgn);
		{
			RealDraw(hTempDC, prcDraw);
		}
		SelectClipRgn(hTempDC, NULL);
		::DeleteObject(hRgn);
		pUIApp->ReleaseCacheDC(hTempDC);

        m_pBubbleMgr->OnDraw(hDC);
	}
}
void  WindowlessRichEditEx::PostDraw(HDC hDC, RECT* prcDraw)
{

}
void  WindowlessRichEditEx::OnSize(UINT nType, CSize size)
{
    __super::OnSize(nType, size);

    if (m_pBubbleMgr)
        m_pBubbleMgr->OnSize();
    
}

// 解决气泡模式下面，右对齐的文本鼠标样式与其它不一致
// 直接设置为箭头
void WindowlessRichEditEx::TxSetCursor(HCURSOR hcur, BOOL fText)
{
	if (IsBubbleMode())
	{
		static HCURSOR _cursor = LoadCursor(NULL, IDC_ARROW);
		SetCursor(_cursor);
		return;
	}

	__super::TxSetCursor(hcur, fText);
}