#pragma once
#include "..\UISDK\Inc\Interface\iobjectdescription.h"
#include "Inc\Interface\ibutton.h"
#include "system_button.h"

class ButtonDescription : public IObjectDescription
{
public:
	static IObjectDescription*  Get()
	{
		static ButtonDescription s;
		return &s;
	}

    virtual void  CreateInstance(UI::ISkinRes* p, void** pp) override
    {
        *pp = IButton::CreateInstance(p);
    }

	virtual OBJ_TYPE  GetMajorType() override
	{
		return OBJ_CONTROL;
	}
	virtual long  GetMinorType() override
	{
		return CONTROL_BUTTON;
	}
	virtual const wchar_t*  GetCategory() override
	{
		return CATEGORY_CONTROL; 
	}
	virtual const wchar_t*  GetTagName() override
	{
		return L"Button";
	}
	virtual const GUID&  GetGUID() override
	{
		return __uuidof(IButton);
	}

    virtual void  GetDefaultSize(long& w, long& h) override
    {
        w = 100;
        h = 24;
    }
    virtual HBITMAP  LoadIcon() override 
    {
        return NULL;
    }
};

class SystemButtonDescription : public ButtonDescription
{
public:
    static IObjectDescription*  Get()
    {
        static SystemButtonDescription s;
        return &s;
    }

    virtual const wchar_t*  GetTagName() override
    {
        return L"SystemButton";
    }
    virtual void  GetDefaultSize(long& w, long& h) override
    {
        w = 20;
        h = 20;
    }
   
    virtual void  CreateInstance(UI::ISkinRes* p, void** pp) override
    {
        *pp = ISystemButton::CreateInstance(p);
    }
};