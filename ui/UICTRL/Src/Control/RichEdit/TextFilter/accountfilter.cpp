#include "stdafx.h"
#include "AccountFilter.h"
#include "numberop.h"

using namespace UI;

AccountFilter::AccountFilter()
{
}
AccountFilter::~AccountFilter()
{
}

AccountFilter* AccountFilter::CreateInstance(IRichEdit*  pRE)
{
    if (!pRE)
        return NULL;

    AccountFilter* p = new AccountFilter;
    p->BindRichEdit(pRE);
    return p;
}

void  AccountFilter::KeyFilter(UINT nKey, TextFilterResult& bHandled)
{
    bHandled = TFR_NOT_HANDLED;
}

bool  AccountFilter::IsAccountChar(TCHAR c)
{
    if (c >= 32 && c <= 126)
        return true;

    return false;
}

TCHAR   AccountFilter::CharFilter(const TCHAR szSrc, TextFilterResult& bHandled)
{
    bHandled = TFR_NOT_HANDLED;
    
    if (!IsAccountChar(szSrc))
    {
        OnInvalid();
        bHandled = TFR_HANDLED;
    }
    return szSrc;
}

LPCTSTR  AccountFilter::StringFilter(LPCTSTR szSrc, TextFilterResult& bHandled)
{
    bHandled = TFR_HANDLED;
    if (!szSrc)
        return NULL;

    UINT nLength = _tcslen(szSrc);

    // ¼ì²â°üº¬ÆäËü×Ö·û
    for (UINT i = 0; i < nLength; i++)
    {
        if (!IsAccountChar(szSrc[i]))
        {
            return NULL;
        }
    }

    bHandled = TFR_NOT_HANDLED;
    return NULL;
}


