#pragma once
#include "numberfilterbase.h"

namespace UI
{

// Êý×Ö
class AccountFilter : public RETextFilterBase
{
public:
    AccountFilter();
    ~AccountFilter();

    static AccountFilter* CreateInstance(IRichEdit*  pRE);

    virtual TextFilterType  GetType() { return TFT_ACCOUNT; }
    virtual void   KeyFilter(UINT nKey, TextFilterResult& bHandled);
    virtual TCHAR   CharFilter(const TCHAR szSrc, TextFilterResult& bHandled);
    virtual LPCTSTR   StringFilter(LPCTSTR szSrc, TextFilterResult& bHandled);

protected:
    bool  IsAccountChar(TCHAR c);

private:
    // Temp
    String  m_strResult;

};
}