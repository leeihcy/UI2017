#include "stdafx.h"
#include "OLE\reoleapi.h"
#include "Inc\Interface\irichedit.h"
#include "richedit.h"
#include "OLE\richeditolemgr.h"

using namespace UI;
UI_IMPLEMENT_INTERFACE_ACROSSMODULE(RichEdit, Control)

void  IRichEdit::SetWindowlessRichEdit(WindowlessRichEditEx* pRE)
{
    m_pWindowlessRE = pRE;
}
bool  IRichEdit::SetText(LPCTSTR szText) 
{ 
    return m_pWindowlessRE->SetText(szText);
}

int   IRichEdit::GetText(TCHAR* szBuf, int nLen)
{
    return m_pWindowlessRE->GetText(szBuf, nLen);
}
void  IRichEdit::GetText(BSTR* bstr)
{
	m_pWindowlessRE->GetText(bstr);
}
void  IRichEdit::GetRangeText(BSTR* pbstr, int nStart, int nSize)
{
	if (nSize < 0 || nSize < 0)
	{
		UIASSERT(0);
		return;
	}
	m_pWindowlessRE->GetTextRange(pbstr, (uint)nStart, (uint)nSize);
}
const char*  IRichEdit::GetRtfBuffer(int nFlags)
{
    return m_pWindowlessRE->GetRtfBuffer(nFlags);
}
void   IRichEdit::ReleaseRtfBuffer(const char* p)
{
    m_pWindowlessRE->ReleaseRtfBuffer(p);
}
bool  IRichEdit::InsertText(LPCTSTR szText, int nLength)
{
	return m_pWindowlessRE->InsertText(szText, nLength);
}
bool  IRichEdit::AppendText(LPCTSTR szText, int nLength)
{
    return m_pWindowlessRE->AppendText(szText, nLength);
}

// bool  IRichEdit::GetEncodeTextW(IBuffer** ppBuffer)
// {
//     return m_pWindowlessRE->GetEncodeTextW(ppBuffer);
// }

bool  IRichEdit::AppendEncodeText(LPCTSTR szText, int nSize)
{
    return m_pWindowlessRE->AppendEncodeText(szText, nSize);
}
bool  IRichEdit::InsertEncodeText(int nPos, LPCTSTR szText, int nSize)
{
    return m_pWindowlessRE->InsertEncodeText(nPos, szText, nSize);
}
bool  IRichEdit::InsertEncodeText(LPCTSTR szText, int nSize)
{
    return m_pWindowlessRE->InsertEncodeText(szText, nSize);
}

HANDLE  IRichEdit::InsertMessageStart(BubbleType eType, BubbleAlign eAlign)
{
    return m_pWindowlessRE->InsertMessageStart(eType, eAlign);
}
void  IRichEdit::InsertMessageFinish(HANDLE hInsertStart)
{
    m_pWindowlessRE->InsertMessageFinish(hInsertStart);
}

LONG  IRichEdit::GetSelBarWidth() 
{ 
    return m_pWindowlessRE->GetSelBarWidth(); 
}
LONG  IRichEdit::SetSelBarWidth(LONG l_SelBarWidth) 
{ 
    return m_pWindowlessRE->SetSelBarWidth(l_SelBarWidth); 
}
bool  IRichEdit::GetRichTextFlag()
{
    return m_pWindowlessRE->GetRichTextFlag();
}
void  IRichEdit::SetRichTextFlag(bool b)
{
    m_pWindowlessRE->SetRichTextFlag(b); 
}
void  IRichEdit::SetMultiLine(bool b)
{
    m_pWindowlessRE->SetMultiLine(b);
}
bool  IRichEdit::IsMultiLine()
{
    return m_pWindowlessRE->IsMultiLine();
}
bool  IRichEdit::IsReadOnly()
{
    return m_pWindowlessRE->IsReadOnly();
}
void  IRichEdit::SetReadOnly(bool fReadOnly)
{
    __pImpl->SetReadOnly(fReadOnly);
}
void  IRichEdit::SetAutoResize(bool b, int nMaxSize)
{
    m_pWindowlessRE->SetAutoResize(b, nMaxSize);
}
bool  IRichEdit::SetCharFormatByLogFont(LOGFONT* plf, COLORREF color)
{
    return m_pWindowlessRE->SetCharFormatByLogFont(plf, color);
}
bool  IRichEdit::SetCharFormat(CHARFORMAT2* pcf)
{
    return m_pWindowlessRE->SetCharFormat(pcf);
}
void  IRichEdit::SetSelectionCharFormat(CHARFORMAT2* pcf)
{
	m_pWindowlessRE->SetSelectionCharFormat(pcf);
}
void  IRichEdit::SetSelectionParaFormat(PARAFORMAT2* ppf)
{
	m_pWindowlessRE->SetSelectionParaFormat(ppf);
}

void  IRichEdit::GetDefaultCharFormat(CHARFORMAT2* pcf)
{
    return m_pWindowlessRE->GetDefaultCharFormat(pcf);
}
void  IRichEdit::GetDefaultParaFormat(PARAFORMAT2* ppf)
{
    return m_pWindowlessRE->CopyDefaultParaFormat(ppf);
}
void  IRichEdit::SetDefaultParaFormat(PARAFORMAT2* ppf)
{
    return m_pWindowlessRE->SetDefaultParaFormat(ppf);
}
void  IRichEdit::SetCharFormatEx(UINT nPart, CHARFORMAT2* pcf)
{
	m_pWindowlessRE->SetCharFormatEx(nPart, pcf);
}
void  IRichEdit::DisableUndoOp()
{
    return m_pWindowlessRE->DisableUndoOp();
}
void  IRichEdit::EnableDragDrop(bool b)
{
    if (b)
        m_pWindowlessRE->RegisterDragDrop();
    else
        m_pWindowlessRE->RevokeDragDrop();
}
bool  IRichEdit::CanDragDrop()
{
    return m_pWindowlessRE->CanDragDrop();
}
bool  IRichEdit::InsertGif(LPCTSTR szGifFile)
{
    return m_pWindowlessRE->InsertGif(szGifFile);
}
bool  IRichEdit::InsertSkinGif(LPCTSTR szGifId)
{
    return m_pWindowlessRE->InsertSkinGif(szGifId);
}
IPictureREOle* IRichEdit::InsertPicture(PictureREOleInfo* pInfo)
{
    return m_pWindowlessRE->InsertPicture(pInfo);
}
void  IRichEdit::SetSel(int nPos, int nLen)
{
	return m_pWindowlessRE->SetSel(nPos, nLen);
}
void  IRichEdit::SelectEnd()
{
    return m_pWindowlessRE->SelectEnd();
}

void  IRichEdit::GetSel(int* pnPos, int* pnLen)
{
    return m_pWindowlessRE->GetSel(pnPos, pnLen);
}

void  IRichEdit::EnableBubbleMode(bool b)
{
    __pImpl->EnableBubbleMode(b);
}
bool  IRichEdit::IsBubbleMode()
{
    return __pImpl->IsBubbleMode();
}
void  IRichEdit::AppendBubbleSpace()
{
	m_pWindowlessRE->InsertBubbleSpace();
}
void  IRichEdit::ScrollEnd()
{
    m_pWindowlessRE->ScrollEnd();
}
void  IRichEdit::EnableWordBreak(bool b)
{
	m_pWindowlessRE->EnableWordBreak(b);
}
void  IRichEdit::Paste()
{
	m_pWindowlessRE->Paste();
}
void  IRichEdit::SetLimitText(int nLimit)
{
	m_pWindowlessRE->SetLimitText(nLimit);
}

signal2<IRichEdit*, ENLINK*>&  IRichEdit::EnLinkEvent()
{
	return __pImpl->EnLink;
}
signal_r2<bool, IRichEdit*, UINT>&  IRichEdit::KeydownEvent()
{
	return __pImpl->Keydown;
}

IREOleBase*  IRichEdit::GetOleByCharPos(int cp)
{
	REOleBase* p = m_pWindowlessRE->GetOleByCharPos(cp);
	if (!p)
		return nullptr;

	return p->GetIREOleBase();
}