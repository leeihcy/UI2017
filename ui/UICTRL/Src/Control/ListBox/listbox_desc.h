#pragma once
#include "Inc\Interface\ilistbox.h"

class ListBoxDescription : public IObjectDescription
{
public:
	static IObjectDescription*  Get()
	{
		static ListBoxDescription s;
		return &s;
	}

    virtual void  CreateInstance(UI::ISkinRes* p, void** pp) override
    {
        *pp = IListBox::CreateInstance(p);
    }

	virtual OBJ_TYPE  GetMajorType() override
	{
		return OBJ_CONTROL;
	}
	virtual long  GetMinorType() override
	{
		return CONTROL_LISTBOX;
	}
	virtual const wchar_t*  GetCategory() override
	{
		return TEXT("ListControl"); 
	}
	virtual const wchar_t*  GetTagName() override
	{
		return L"ListBox";
	}
	virtual const GUID&  GetGUID() override
	{
		return __uuidof(IListBox);
	}

    virtual void  GetDefaultSize(long& w, long& h) override
    {
        w = 200;
        h = 200;
    }
    virtual HBITMAP  LoadIcon() override 
    {
        return NULL;
    }
};

