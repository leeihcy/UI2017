#pragma once
#include "animate_base.h"
#include "ui\UICTRL\Inc\Interface\ilistbox.h"
#include "ui\UICTRL\Inc\Interface\ilabel.h"

// Î»ÒÆ¶¯»­

class ListAnimate : public Animate
{
public:
	virtual int  Type() override {
		return AnimateType_List;
	}

	virtual void Init() override
	{
		m_pListBox = (UI::IListBox*)m_pWindow->FindObject(L"listbox");
		UI::LISTCTRLSTYLE s = { 0 };
		s.dragwnd_if_clickblank = 1;
		m_pListBox->ModifyListCtrlStyle(&s, 0);

		TCHAR szImageId[64] = { 0 };
		for (int i = 0; i < 16; i++)
		{
			UI::ICustomListItem* item = 
				m_pListBox->InsertCustomItem(L"ballitem", i);
			UI::IPictureCtrl* image = (UI::IPictureCtrl*)
				item->FindControl(L"image");
				
			_stprintf_s<64>(szImageId, L"ball%02d", i);
			image->SetImageById(szImageId);
		}
		
		m_pListBox->ClickEvent().connect0(this, &ListAnimate::onclick);
	}

	void onclick()
	{
		UI::IListItemBase* item = m_pListBox->GetHoverItem();
		if (!item)
			return;

		m_pListBox->RemoveItem(item);
	}

	virtual void Action() override
	{
	}

	virtual void Release() override
	{
		m_pListBox->ClickEvent().disconnect();
	}

	UI::IListBox*  m_pListBox = nullptr;
};