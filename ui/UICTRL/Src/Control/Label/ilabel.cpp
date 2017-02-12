#include "stdafx.h"
#include "Inc\Interface\ilabel.h"
#include "Src\Control\Label\label.h"

namespace UI
{

UI_IMPLEMENT_INTERFACE_ACROSSMODULE(Label, Control);
UI_IMPLEMENT_INTERFACE_ACROSSMODULE(PictureCtrl, Control);
UI_IMPLEMENT_INTERFACE_ACROSSMODULE(GifCtrl, Control);

LPCTSTR  ILabel::GetText()
{ 
	return __pImpl->GetText(); 
}
void  ILabel::SetText(LPCTSTR pszText) 
{
    __pImpl->SetTextAndInvalidate(pszText); 
}
IBindSourceChangedListener*  ILabel::GetListener()
{
	return __pImpl->GetListener();
}

bool IPictureCtrl::SetImageByPath(LPCTSTR szPath)
{
    return __pImpl->SetImageByPath(szPath);
}
bool IPictureCtrl::SetImageById(LPCTSTR szId)
{
    return __pImpl->SetImageById(szId);
}
bool IPictureCtrl::AttachBitmap(HBITMAP bmp)
{
	return __pImpl->AttachBitmap(bmp);
}


// UI::signal2<IPictureCtrl*, IRenderTarget*>& IPictureCtrl::OwnerDrawEvent()
// {
// 	return __pImpl->OwnerDraw;
// }

bool  IGifCtrl::Start()
{ 
	return __pImpl->Start(); 
}
bool  IGifCtrl::Pause()
{
	return __pImpl->Pause();
}
bool  IGifCtrl::Stop() 
{ 
	return __pImpl->Stop();
}
}