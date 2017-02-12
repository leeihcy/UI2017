#include "stdafx.h"
#include "uintfilter.h"
#include "numberop.h"

using namespace UI;

UIntTextFilter::UIntTextFilter()
{
    SetMinMax(999999999);
}
UIntTextFilter::~UIntTextFilter()
{
}

UIntTextFilter* UIntTextFilter::CreateInstance(IRichEdit*  pRE)
{
    UIntTextFilter* p = new UIntTextFilter;

	if (pRE)
		p->BindRichEdit(pRE);
    return p;
}

void  UIntTextFilter::KeyFilter(UINT nKey, TextFilterResult& bHandled)
{
    bHandled = TFR_NOT_HANDLED;

    // 不接受回车 (修改为单行模式即可)
//     if (nKey == VK_RETURN)
//     {
//         bHandled = TFR_HANDLED;
//     }
}

TCHAR   UIntTextFilter::CharFilter(const TCHAR szSrc, TextFilterResult& bHandled)
{
    bHandled = TFR_NOT_HANDLED;
    switch (szSrc)
    {
    case TEXT('1'):
    case TEXT('2'):
    case TEXT('3'):
    case TEXT('4'):
    case TEXT('5'):
    case TEXT('6'):
    case TEXT('7'):
    case TEXT('8'):
    case TEXT('9'):
    case TEXT('0'):
        UIntChar(szSrc/*szSrc - TEXT('0')*/, bHandled);
        break;

    default:
        OnInvalid();
        bHandled = TFR_HANDLED;
    }
    return szSrc;
}

LPCTSTR  UIntTextFilter::StringFilter(LPCTSTR szSrc, TextFilterResult& bHandled)
{
    bHandled = TFR_HANDLED;
    if (!szSrc)
        return NULL;

    UINT nLength = _tcslen(szSrc);
    if (0 == nLength || nLength > m_nMaxTextLength)
    {
        OnInvalid();
        return NULL;
    }

    // 检测包含其它字符
    for (UINT i = 0; i < nLength; i++)
    {
        switch (szSrc[i])
        {
        case TEXT('1'):
        case TEXT('2'):
        case TEXT('3'):
        case TEXT('4'):
        case TEXT('5'):
        case TEXT('6'):
        case TEXT('7'):
        case TEXT('8'):
        case TEXT('9'):
        case TEXT('0'):
            continue;

        default:
            return NULL;
        }
    }

    // 尝试
    TCHAR szText[32] = {0};
    m_pRE->GetText(szText, 31);

    NumberOP  op(szText);

    int nPos = 0;
    int nLen = 0;
    m_pRE->GetSel(&nPos, &nLen);

    if (nLen)
    {
        op.Remove(nPos, nLen);
    }
    op.Insert(nPos, szSrc);

    // 溢出检测
    UINT nCurValue = 0;
    if (!op.UintValue(&nCurValue) || nCurValue > m_nMax)
    {
        OnInvalid();
        bHandled = TFR_HANDLED;
        return NULL;
    }

    if (op.IsZeroPrefix())
    {
        SetText(nCurValue);
        bHandled = TFR_HANDLED;
        return NULL;
    }
    
    bHandled = TFR_NOT_HANDLED;
    return NULL;
}

void  UIntTextFilter::SetText(UINT nValue)
{
    if (!m_pRE)
        return;

    TCHAR szText[16] = {0};
    _stprintf(szText, TEXT("%u"), nValue);
    m_pRE->SetText(szText);
    m_pRE->SelectEnd();

    m_pRE->Invalidate();
}

UINT   UIntTextFilter::GetText()
{
    if (!m_pRE)
        return 0;

    TCHAR szText[32] = {0};
    m_pRE->GetText(szText, 31);

    return _ttoi(szText);
}

void  UIntTextFilter::UIntChar(TCHAR c, TextFilterResult& bHandled)
{
    bHandled = TFR_NOT_HANDLED;
    if (!m_pRE)
        return;

    TCHAR szText[32] = {0};
    m_pRE->GetText(szText, 31);

    NumberOP  op(szText);

    int nPos = 0;
    int nLen = 0;
    m_pRE->GetSel(&nPos, &nLen);

    if (nLen)
    {
        op.Remove(nPos, nLen);
    }
    op.Insert(nPos, c);

    UINT nCurValue = 0;
    // 溢出检测
    if (!op.UintValue(&nCurValue) || nCurValue > m_nMax)
    {
        OnInvalid();
        bHandled = TFR_HANDLED;
        return;
    }

    if (op.IsZeroPrefix())
    {
        SetText(nCurValue);
        bHandled = TFR_HANDLED;
    }
}

void  UIntTextFilter::SetMinMax(/*UINT nMin, */UINT nMax)
{
//     if (nMin > nMax)
//     {
//         UINT nTemp = nMin;
//         nMin = nMax;
//         nMax = nTemp;
//     }

    m_nMax = nMax;

    TCHAR szText[32] = {0};
    _stprintf(szText, TEXT("%u"), m_nMax);
    m_nMaxTextLength = _tcslen(szText);
}
