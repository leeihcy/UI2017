#include "stdafx.h"
#include "checkbutton.h"
#include "checkbutton_desc.h"
#include "..\UISDK\Inc\Interface\iuires.h"

using namespace UI;

CheckButton::CheckButton(ICheckButton* p):Button(p)
{
    m_pICheckButton = p;
	SetButtonType(BUTTON_TYPE_CHECKBUTTON);
	p->SetDescription(CheckButtonDescription::Get());
}

CheckButton::~CheckButton()
{
}

void CheckButton::virtualOnClicked()
{
    // 在外部响应时能够正确调用IsChecked进行判断
    if (m_nCheckState & BST_CHECKED)
        m_nCheckState &= ~BST_CHECKED;
    else
        m_nCheckState |= BST_CHECKED;

	__super::virtualOnClicked();
}

void UI::CheckButton::OnSerialize(SERIALIZEDATA* pData)
{
	__super::OnSerialize(pData);
	AttributeSerializerWrap as(pData, L"CheckButton");
	as.AddString(XML_BUTTON_CHECKED_TOOLTIP, this,
		memfun_cast<pfnStringSetter>(&CheckButton::SetCheckedToolTipText),
		memfun_cast<pfnStringGetter>(&CheckButton::GetCheckedToolTipText)
		);
}

BOOL UI::CheckButton::OnGetToolTipInfo(
	TOOLTIPITEM* pToolTipItem, IToolTipUI* pToolTip)
{
	if (NULL == pToolTipItem || NULL == pToolTip)
		return 0;
	
	LPCTSTR szTooltip = nullptr;
	if (IsChecked())
	{
		szTooltip = GetCheckedToolTipText();
		if (!szTooltip)
			szTooltip = m_pICheckButton->GetToolTipText();
	}
	else
	{
		szTooltip = m_pICheckButton->GetToolTipText();
	}

	if (!szTooltip || !szTooltip[0])
		return 0;

	ISkinRes* pSkinRes = m_pICheckButton->GetSkinRes();
	if (!pSkinRes)
		return 0;

	LPCTSTR szText = pSkinRes->GetI18nRes().MapConfigValue(szTooltip);
	pToolTip->SetText(szText);
	return 1;
}

LPCTSTR UI::CheckButton::GetCheckedToolTipText()
{
	return m_pICheckButton->GetAttribute(XML_BUTTON_CHECKED_TOOLTIP, false);
}

void UI::CheckButton::SetCheckedToolTipText(LPCTSTR szText)
{
	if (!szText || 0 == szText[0])
	{
		m_pICheckButton->GetAttribute(XML_BUTTON_CHECKED_TOOLTIP, true);
	}
	else
	{
		m_pICheckButton->AddAttribute(XML_BUTTON_CHECKED_TOOLTIP, szText);
	}
}

