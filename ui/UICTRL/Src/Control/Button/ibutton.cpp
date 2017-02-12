#include "stdafx.h"
#include "Inc\Interface\ibutton.h"
#include "button.h"

namespace UI
{

UI_IMPLEMENT_INTERFACE_ACROSSMODULE(ButtonBase, Control)
UI_IMPLEMENT_INTERFACE(Button, ButtonBase)

void  IButtonBase::SetCheck(int nCheckState)
{
    __pImpl->SetCheck(nCheckState);
}
void  IButtonBase::SetCheck(bool bCheck)
{
    __pImpl->SetCheck(bCheck);
}
void  IButtonBase::SetChecked()
{
    __pImpl->SetChecked();
}
void  IButtonBase::SetUnChecked()
{
    __pImpl->SetUnChecked();
}
int   IButtonBase::GetCheck()
{
    return __pImpl->GetCheck();
}
bool  IButtonBase::IsChecked()
{
    return __pImpl->IsChecked();
}
void  IButtonBase::SetButtonType(BUTTON_TYPE n)
{
    __pImpl->SetButtonType(n);
}
void  IButtonBase::ModifyButtonStyle(BUTTONSTYLE* add, BUTTONSTYLE* remove)
{
	__pImpl->ModifyButtonStyle(add, remove);
}
//////////////////////////////////////////////////////////////////////////


void  IButton::SetIconFromFile(LPCTSTR szIconPath)
{ 
    __pImpl->SetIconFromFile(szIconPath);
}
void  IButton::SetIconFromHBITMAP(HBITMAP hBitmap)
{ 
    __pImpl->SetIconFromHBITMAP(hBitmap); 
}
void  IButton::SetText(LPCTSTR  szText) 
{
    __pImpl->SetTextAndUpdate(szText); 
}
LPCTSTR  IButton::GetText() 
{ 
    return __pImpl->GetText(); 
}

void  IButton::Click()
{
	__pImpl->Click();
}

void  IButton::SetDrawFocusType(BUTTON_RENDER_DRAW_FOCUS_TYPE eType)
{ 
    __pImpl->SetDrawFocusType(eType);
}
void  IButton::SetAutoSizeType(BUTTON_AUTO_SIZE_TYPE eType)
{
    __pImpl->SetAutoSizeType(eType);
}

signal<IButton*>&  IButton::ClickedEvent()
{
    return __pImpl->clicked;
}

}