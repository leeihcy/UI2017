#pragma once
#include "..\UISDK\Inc\Interface\iobjectdescription.h"
#include "Inc\Interface\icustom_control.h"

class CustomControlDescription : public IObjectDescription
{
public:
	static IObjectDescription*  Get()
	{
		static CustomControlDescription s;
		return &s;
	}

    virtual void  CreateInstance(UI::ISkinRes* p, void** pp) override
    {
        *pp = ICustomControl::CreateInstance(p);
    }

	virtual OBJ_TYPE  GetMajorType() override
	{
		return OBJ_CONTROL;
	}
	virtual long  GetMinorType() override
	{
		return CONTROL_PICTURE;
	}
	virtual const wchar_t*  GetCategory() override
	{
		return CATEGORY_CONTROL; 
	}
	virtual const wchar_t*  GetTagName() override
	{
		return L"CustomControl";
	}
	virtual const GUID&  GetGUID() override
	{
		return __uuidof(ICustomControl);
	}

    virtual void  GetDefaultSize(long& w, long& h) override
    {
        w = 100;
        h = 100;
    }
    virtual HBITMAP  LoadIcon() override 
    {
        return NULL;
    }
};