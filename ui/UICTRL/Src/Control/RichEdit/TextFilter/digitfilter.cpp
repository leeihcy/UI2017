#include "stdafx.h"
#include "digitfilter.h"
#include "numberop.h"

using namespace UI;

DigitTextFilter::DigitTextFilter()
{
}
DigitTextFilter::~DigitTextFilter()
{
}

DigitTextFilter* DigitTextFilter::CreateInstance(IRichEdit*  pRE)
{
    DigitTextFilter* p = new DigitTextFilter;
	if (pRE)
		p->BindRichEdit(pRE);
    return p;
}

void  DigitTextFilter::KeyFilter(UINT nKey, TextFilterResult& bHandled)
{
    bHandled = TFR_NOT_HANDLED;

    // 不接受回车 (修改为单行模式即可)
//     if (nKey == VK_RETURN)
//     {
//         bHandled = TFR_HANDLED;
//     }
}

TCHAR   DigitTextFilter::CharFilter(const TCHAR szSrc, TextFilterResult& bHandled)
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
        break;

    default:
        OnInvalid();
        bHandled = TFR_HANDLED;
    }
    return szSrc;
}

LPCTSTR  DigitTextFilter::StringFilter(LPCTSTR szSrc, TextFilterResult& bHandled)
{
    bHandled = TFR_HANDLED;
    if (!szSrc)
        return NULL;

    UINT nLength = _tcslen(szSrc);

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

    bHandled = TFR_NOT_HANDLED;
    return NULL;
}


