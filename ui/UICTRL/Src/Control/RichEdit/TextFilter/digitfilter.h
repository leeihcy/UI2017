#pragma once
#include "numberfilterbase.h"

namespace UI
{

// Êý×Ö
class DigitTextFilter : public RETextFilterBase
{
public:
    DigitTextFilter();
    ~DigitTextFilter();

    static DigitTextFilter* CreateInstance(IRichEdit*  pRE);

    virtual TextFilterType  GetType() { return TFT_DIGIT; }
    virtual void   KeyFilter(UINT nKey, TextFilterResult& bHandled);
    virtual TCHAR   CharFilter(const TCHAR szSrc, TextFilterResult& bHandled);
    virtual LPCTSTR   StringFilter(LPCTSTR szSrc, TextFilterResult& bHandled);

private:
    // Temp
    String  m_strResult;

};
}