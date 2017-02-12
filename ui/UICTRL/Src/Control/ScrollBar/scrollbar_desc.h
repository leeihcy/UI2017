#pragma once
#include "..\UISDK\Inc\Interface\iobjectdescription.h"
#include "Inc\Interface\iscrollbar.h"

class SystemVScrollBarDescription : public IObjectDescription
{
public:
	static IObjectDescription*  Get()
	{
		static SystemVScrollBarDescription s;
		return &s;
	}

    virtual void  CreateInstance(UI::ISkinRes* p, void** pp) override
    {
        *pp = ISystemVScrollBar::CreateInstance(p);
    }

	virtual OBJ_TYPE  GetMajorType() override
	{
		return OBJ_CONTROL;
	}
	virtual long  GetMinorType() override
	{
		return CONTROL_VSCROLLBAR;
	}
	virtual const wchar_t*  GetCategory() override
	{
		return CATEGORY_CONTROL; 
	}
	virtual const wchar_t*  GetTagName() override
	{
		return L"VScrollBar";
	}
	virtual const GUID&  GetGUID() override
	{
		return __uuidof(ISystemVScrollBar);
	}

	virtual void  GetDefaultSize(long& w, long& h) override
	{
		w = 10;
		h = 100;
	}
	virtual HBITMAP  LoadIcon() override 
	{
		return NULL;
	}
};


class SystemHScrollBarDescription : public IObjectDescription
{
public:
	static IObjectDescription*  Get()
	{
		static SystemHScrollBarDescription s;
		return &s;
	}

    virtual void  CreateInstance(UI::ISkinRes* p, void** pp) override
    {
        *pp = ISystemHScrollBar::CreateInstance(p);
    }

	virtual OBJ_TYPE  GetMajorType() override
	{
		return OBJ_CONTROL;
	}
	virtual long  GetMinorType() override
	{
		return CONTROL_HSCROLLBAR;
	}
	virtual const wchar_t*  GetCategory() override
	{
		return CATEGORY_CONTROL; 
	}
	virtual const wchar_t*  GetTagName() override
	{
		return L"HScrollBar";
	}
	virtual const GUID&  GetGUID() override
	{
		return __uuidof(ISystemHScrollBar);
	}

	virtual void  GetDefaultSize(long& w, long& h) override
	{
		w = 100;
		h = 10;
	}
	virtual HBITMAP  LoadIcon() override 
	{
		return NULL;
	}
};
