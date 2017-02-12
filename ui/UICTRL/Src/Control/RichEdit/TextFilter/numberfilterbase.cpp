#include "stdafx.h"
#include "numberfilterbase.h"

using namespace UI;


RETextFilterBase::RETextFilterBase()
{
    m_pRE = NULL;

    m_bOldEnableIme = false;
    m_bOldRichText = false;
    m_bOldEnableDragdrop = false;
    m_bOldMultiline = false;
}

void  RETextFilterBase::BindRichEdit(IRichEdit* pRE)
{
    this->m_pRE = pRE;

	if (!m_pRE)
		return;

    // 禁用输入法
    OBJSTYLE s = {0};
    s.enable_ime = 1;
    m_bOldEnableIme = m_pRE->TestObjectStyle(s);
    if (m_bOldEnableIme)
        m_pRE->ModifyObjectStyle(NULL, &s);

    // 单行
    m_bOldMultiline = m_pRE->IsMultiLine();
    if (m_bOldMultiline)
        m_pRE->SetMultiLine(false);

    // 只处理CF_TEXT/CF_UNICODETEXT
    m_bOldRichText = m_pRE->GetRichTextFlag();
    if (m_bOldRichText)
        m_pRE->SetRichTextFlag(false);  // 这样就已经不能drop了

    // 不处理拖拽
    //     m_bOldEnableDragdrop = m_pRE->CanDragDrop();
    //     if (m_bOldEnableDragdrop)
    //         m_pRE->EnableDragDrop(false);
}
void  RETextFilterBase::UnbindRichEdit()
{
    if (!this->m_pRE)
        return;

    if (m_bOldEnableIme)
    {
        OBJSTYLE s = {0};
        s.enable_ime = 1;
        m_pRE->ModifyObjectStyle(&s, NULL);
    }
    if (m_bOldRichText)
        m_pRE->SetRichTextFlag(true);

    if (m_bOldMultiline)
        m_pRE->SetMultiLine(m_bOldMultiline);

    //     if (m_bOldEnableDragdrop)
    //         m_pRE->EnableDragDrop(true);

}

void  RETextFilterBase::Release(bool bRestoreState)
{
    if (bRestoreState)
    {
        UnbindRichEdit();
    }
    delete this;
}

// 检测一个按键是否按下，如CTRL,SHIFT,ALT
bool IsKeyDown(UINT vk);

void  RETextFilterBase::OnInvalid()
{
    if (IsKeyDown(VK_CONTROL) || IsKeyDown(VK_SHIFT))
    {
        // 如果是快捷键，则不提醒
        return;
    }

    MessageBeep(-1);
}