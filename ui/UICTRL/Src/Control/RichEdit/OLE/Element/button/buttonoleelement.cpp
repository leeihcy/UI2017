#include "stdafx.h"
#include "ButtonOleElement.h"
#include "..\..\Composite\compositereole.h"

using namespace UI;

ButtonOleElement::ButtonOleElement()
{
    m_bHover = false;
    m_bPress = false;

	m_pImageNormal = NULL;
	m_pImageHover = NULL;
	m_pImagePress = NULL;
	m_pImageDisable = NULL;
}

ButtonOleElement::~ButtonOleElement()
{

}

ButtonOleElement*  ButtonOleElement::CreateInstance()
{
    return new ButtonOleElement;
}

long  ButtonOleElement::GetType() 
{
    return ButtonOleElement::TYPE;
}

void  ButtonOleElement::SetImage(
			   ImageWrap* pNormal,
			   ImageWrap* pHover,
			   ImageWrap* pPress, 
			   ImageWrap* pDisable)
{
	m_pImageNormal = pNormal;
	m_pImageHover = pHover;
	m_pImagePress = pPress;
	m_pImageDisable = pDisable;
}

void  ButtonOleElement::SetTooltip(LPCTSTR szTooltip)
{
    if (szTooltip)
        m_strTooltip = szTooltip;
    else
        m_strTooltip.clear();
}

void  ButtonOleElement::Draw(HDC hDC, int xOffset, int yOffset)
{
    ImageWrap* pImage = m_pImageNormal;
    if (m_bPress && m_bHover)
    {
        pImage = m_pImagePress;
    }
    else if (m_bPress || m_bHover)
    {
        pImage = m_pImageHover;
    }
	DrawImage(pImage, hDC, xOffset, yOffset);
}

void  ButtonOleElement::DrawImage(ImageWrap* pImage, HDC hDC, int xOffset, int yOffset)
{
	if (!pImage)
		return;
	if (pImage->IsNull())
	    return;
	
	pImage->Draw(hDC, 
	    m_rcInOle.left + xOffset,
	    m_rcInOle.top + yOffset, 
	    m_rcInOle.Width(),
	    m_rcInOle.Height(),
	    0, 0, 
	    pImage->GetWidth(),
	    pImage->GetHeight());
}

LRESULT  ButtonOleElement::ProcessMessage(UINT msg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    bHandled = TRUE;
    switch (msg)
    {
    case WM_MOUSEMOVE:
        {
            if (!m_bHover)
            {
                m_bHover = true;

                if (!m_strTooltip.empty())
                {
                    m_pREOle->ShowToolTip(m_strTooltip.c_str());
                }
                Invalidate();
            }
        }
        break;

    case WM_MOUSELEAVE:
        {
            if (m_bHover)
            {
                m_bHover = false;
            }
            if (m_bPress)
            {
                m_bPress = false;
            }

            if (!m_strTooltip.empty())
            {
                m_pREOle->ShowToolTip(NULL);
            }
            Invalidate();

            // 在按下状态离开时，收不到WM_SETCURSOR消息，手动重置一把
            HCURSOR _cursor = LoadCursor(NULL, IDC_ARROW);
            SetCursor(_cursor);
        }
        break;

    case WM_LBUTTONDOWN:
    case WM_LBUTTONDBLCLK:
        {
            if (!m_bPress)
            {
                m_bPress = true;
                Invalidate();
            }
        }
        break;

    case WM_LBUTTONUP:
        {
            if (m_bPress)
            {
                m_bPress = false;
                Invalidate();
            }
            if (m_bHover)
            {
                OnClick();
            }
        }
        break;

    case WM_SETCURSOR:
        {
            HCURSOR _cursor = LoadCursor(NULL, IDC_HAND);
            SetCursor(_cursor);
        }
        return 1;
        break;

    default:
        bHandled = FALSE;
        break;
    }

	
	return 0;
}

void  ButtonOleElement::OnClick()
{
    if (m_pREOle)
    {
        BOOL bHandled = FALSE;
        m_pREOle->ProcessMessage(WM_NOTIFY, BN_CLICKED, (LPARAM)this, bHandled);
    }
}