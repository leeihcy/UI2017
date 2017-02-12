#pragma once
#include "Inc\Interface\imenu.h"

class MenuDescription : public IObjectDescription
{
public:
	static IObjectDescription*  Get()
	{
		static MenuDescription s;
		return &s;
	}

    virtual void  CreateInstance(UI::ISkinRes* p, void** pp) override
    {
        *pp = IMenu::CreateInstance(p);
    }

	virtual OBJ_TYPE  GetMajorType() override
	{
		return OBJ_CONTROL;
	}
	virtual long  GetMinorType() override
	{
		return CONTROL_MENU;
	}
	virtual const wchar_t*  GetCategory() override
	{
		return CATEGORY_CONTROL; 
	}
	virtual const wchar_t*  GetTagName() override
	{
		return L"Menu";
	}
	virtual const GUID&  GetGUID() override
	{
		return __uuidof(IMenu);
	}

    virtual void  GetDefaultSize(long& w, long& h) override
    {
        w = 100;
        h = 200;
    }
    virtual HBITMAP  LoadIcon() override 
    {
        return NULL;
    }
};

