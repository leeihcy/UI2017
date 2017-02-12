#pragma once
#include "..\UISDK\Inc\Interface\iobjectdescription.h"
#include "Inc\Interface\icombobox.h"

class ComboBoxDescription : public IObjectDescription
{
public:
	static IObjectDescription*  Get()
	{
		static ComboBoxDescription s;
		return &s;
	}

    virtual void  CreateInstance(UI::ISkinRes* p, void** pp) override
    {
        *pp = IComboBox::CreateInstance(p);
    }

	virtual OBJ_TYPE  GetMajorType() override
	{
		return OBJ_CONTROL;
	}
	virtual long  GetMinorType() override
	{
		return CONTROL_COMBOBOX;
	}
	virtual const wchar_t*  GetCategory() override
	{
		return _T("ListControl"); 
	}
	virtual const wchar_t*  GetTagName() override
	{
		return L"ComboBox";
	}
	virtual const GUID&  GetGUID() override
	{
		return __uuidof(IComboBox);
	}

    virtual void  GetDefaultSize(long& w, long& h) override
    {
        w = 100;
        h = 25;
    }
    virtual HBITMAP  LoadIcon() override 
    {
        return NULL;
    }
};

