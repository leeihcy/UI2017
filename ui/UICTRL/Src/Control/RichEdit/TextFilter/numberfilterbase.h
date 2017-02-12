#pragma once
#include "UI\UICTRL\Inc\Interface\irichedit.h"
namespace UI
{

class RETextFilterBase : public ITextFilter
{
public:
    RETextFilterBase();
    virtual ~RETextFilterBase() {}

    virtual void  Release(bool bRestoreState);

    void  BindRichEdit(IRichEdit* pRE);
    void  UnbindRichEdit();
    
protected:
    void  OnInvalid();

protected:
    IRichEdit*  m_pRE;

private:
    // 用于状态还原
    bool   m_bOldEnableIme;
    bool   m_bOldRichText;
    bool   m_bOldEnableDragdrop;
    bool   m_bOldMultiline;
};

}