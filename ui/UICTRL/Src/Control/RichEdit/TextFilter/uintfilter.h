#pragma once
#include "numberfilterbase.h"

// 要实现最小值输入很难。例如最小9，要从9变成10没办法了
// 因此在这里不对最小值做处理

namespace UI
{
class RichEdit;
class UIntTextFilter : public RETextFilterBase
{
public:
    UIntTextFilter();
    ~UIntTextFilter();

    static UIntTextFilter* CreateInstance(IRichEdit*  pRE);

    virtual TextFilterType  GetType() { return TFT_UINT; }
    virtual void   KeyFilter(UINT nKey, TextFilterResult& bHandled);
    virtual TCHAR   CharFilter(const TCHAR szSrc, TextFilterResult& bHandled);
    virtual LPCTSTR   StringFilter(LPCTSTR szSrc, TextFilterResult& bHandled);

    void  SetMinMax(UINT nMax);

protected:
    void  SetText(UINT nValue);
    UINT  GetText();
    void  UIntChar(TCHAR c, TextFilterResult& bHandled);

private:

    UINT   m_nMax;
    UINT   m_nMaxTextLength;   // 最大值文本长度
    //UINT   m_nCurValue;  // 维护成本太高，还是每次重新get吧


    // Temp
    String  m_strResult;

};

}