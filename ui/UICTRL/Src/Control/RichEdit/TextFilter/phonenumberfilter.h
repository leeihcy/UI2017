#pragma once
#include "numberfilterbase.h"

namespace UI
{

// Êý×Ö
class PhoneNumberFilter : public RETextFilterBase
{
public:
    PhoneNumberFilter();
    ~PhoneNumberFilter();

    static PhoneNumberFilter* CreateInstance(IRichEdit*  pRE);

    virtual TextFilterType  GetType() { return TFT_DIGIT; }
    virtual void   KeyFilter(UINT nKey, TextFilterResult& bHandled);
    virtual TCHAR   CharFilter(const TCHAR szSrc, TextFilterResult& bHandled);
    virtual LPCTSTR   StringFilter(LPCTSTR szSrc, TextFilterResult& bHandled);

protected:
    bool  IsPhoneNumberChar(TCHAR c);

private:
    // Temp
    String  m_strResult;

};
}