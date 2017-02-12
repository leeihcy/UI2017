#include "stdafx.h"
#include "phonenumberfilter.h"
#include "numberop.h"

using namespace UI;

PhoneNumberFilter::PhoneNumberFilter()
{
}
PhoneNumberFilter::~PhoneNumberFilter()
{
}

PhoneNumberFilter* PhoneNumberFilter::CreateInstance(IRichEdit*  pRE)
{
    if (!pRE)
        return NULL;

    PhoneNumberFilter* p = new PhoneNumberFilter;
    p->BindRichEdit(pRE);
    return p;
}

void  PhoneNumberFilter::KeyFilter(UINT nKey, TextFilterResult& bHandled)
{
    bHandled = TFR_NOT_HANDLED;
}

bool  PhoneNumberFilter::IsPhoneNumberChar(TCHAR c)
{
    switch (c)
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
    case TEXT('+'):
    case TEXT('-'):
    case TEXT('('):
    case TEXT(')'):
    case TEXT('#'):
    case TEXT('*'):
        return true;
        break;
    }

    return false;
}

TCHAR   PhoneNumberFilter::CharFilter(const TCHAR szSrc, TextFilterResult& bHandled)
{
    bHandled = TFR_NOT_HANDLED;
    
    if (!IsPhoneNumberChar(szSrc))
    {
        OnInvalid();
        bHandled = TFR_HANDLED;
    }
    return szSrc;
}

LPCTSTR  PhoneNumberFilter::StringFilter(LPCTSTR szSrc, TextFilterResult& bHandled)
{
    bHandled = TFR_HANDLED;
    if (!szSrc)
        return NULL;

    UINT nLength = _tcslen(szSrc);

    // ¼ì²â°üº¬ÆäËü×Ö·û
    for (UINT i = 0; i < nLength; i++)
    {
        if (!IsPhoneNumberChar(szSrc[i]))
        {
            return NULL;
        }
    }

    bHandled = TFR_NOT_HANDLED;
    return NULL;
}


