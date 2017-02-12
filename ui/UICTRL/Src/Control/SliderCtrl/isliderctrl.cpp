#include "stdafx.h"
#include "sliderctrl.h"
#include "Inc\Interface\isliderctrl.h"

namespace UI
{
UI_IMPLEMENT_INTERFACE_ACROSSMODULE(SliderCtrl, Control)

int   ISliderCtrl::SetPos(int nPos, bool bUpdate) 
{ 
	return __pImpl->SetPos(nPos, bUpdate); 
}
int   ISliderCtrl::SetPage(int nPage, bool bUpdate)
{ 
	return __pImpl->SetPage(nPage, bUpdate);
}
void  ISliderCtrl::SetRange(int nLower, int nUpper, bool bUpdate) 
{
	__pImpl->SetRange(nLower, nUpper, bUpdate); 
}
void  ISliderCtrl::SetScrollInfo(LPCSCROLLINFO lpsi, bool bUpdate)
{ 
	__pImpl->SetScrollInfo(lpsi, bUpdate); 
}

void  ISliderCtrl::SetLine(int nLine)
{
	__pImpl->SetLine(nLine); 
}
int   ISliderCtrl::GetPos() 
{ 
	return __pImpl->GetPos(); 
}
int   ISliderCtrl::GetPage()
{
	return __pImpl->GetPage();
}
int   ISliderCtrl::GetLine()
{ 
	return __pImpl->GetLine();
}
void  ISliderCtrl::GetRange(int& nLower, int& nUpper)
{ 
	__pImpl->GetRange(nLower, nUpper);
}
int   ISliderCtrl::GetRange() 
{ 
	return __pImpl->GetRange();
}
void  ISliderCtrl::SetDirectionType(PROGRESSBAR_SCROLL_DIRECTION_TYPE eType)
{ 
	__pImpl->SetDirectionType(eType); 
}
PROGRESSBAR_SCROLL_DIRECTION_TYPE  ISliderCtrl::GetDirectionType()
{
	return __pImpl->GetDirectionType(); 
}
int   ISliderCtrl::SetTickFreq(int nFreq)
{ 
	return __pImpl->SetTickFreq(nFreq); 
}

UI::signal<SliderPosChangedParam*>& ISliderCtrl::PosChangedEvent()
{
	return __pImpl->PosChanged;
}

}