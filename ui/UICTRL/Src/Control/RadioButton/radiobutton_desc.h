#pragma once
#include "..\UISDK\Inc\Interface\iobjectdescription.h"
#include "Inc\Interface\iradiobutton.h"

class RadioButtonDescription : public IObjectDescription
{
public:
	static IObjectDescription*  Get()
	{
		static RadioButtonDescription s;
		return &s;
	}

    virtual void  CreateInstance(UI::ISkinRes* p, void** pp) override
    {
        *pp = IRadioButton::CreateInstance(p);
    }

	virtual OBJ_TYPE  GetMajorType() override
	{
		return OBJ_CONTROL;
	}
	virtual long  GetMinorType() override
	{
		return CONTROL_RADIOBUTTON;
	}
	virtual const wchar_t*  GetCategory() override
	{
		return CATEGORY_CONTROL; 
	}
	virtual const wchar_t*  GetTagName() override
	{
		return L"RadioButton";
	}
	virtual const GUID&  GetGUID() override
	{
		return __uuidof(IRadioButton);
	}

    virtual void  GetDefaultSize(long& w, long& h) override
    {
        w = 100;
        h = 20;
    }
    virtual HBITMAP  LoadIcon() override 
    {
        return NULL;
    }
};