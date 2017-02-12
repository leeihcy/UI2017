#pragma once
#include "Inc\Interface\ihyperlink.h"

class HyperLinkDescription : public IObjectDescription
{
public:
	static IObjectDescription*  Get()
	{
		static HyperLinkDescription s;
		return &s;
	}

    virtual void  CreateInstance(UI::ISkinRes* p, void** pp) override
    {
        *pp = IHyperLink::CreateInstance(p);
    }

	virtual OBJ_TYPE  GetMajorType() override
	{
		return OBJ_CONTROL;
	}
	virtual long  GetMinorType() override
	{
		return CONTROL_HYPERLINK;
	}
	virtual const wchar_t*  GetCategory() override
	{
		return CATEGORY_CONTROL; 
	}
	virtual const wchar_t*  GetTagName() override
	{
		return L"HyperLink";
	}
	virtual const GUID&  GetGUID() override
	{
		return __uuidof(IHyperLink);
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

