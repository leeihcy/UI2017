#pragma once
#include "Inc\Interface\irectracker.h"

class RectTrackerDescription : public IObjectDescription
{
public:
	static IObjectDescription*  Get()
	{
		static RectTrackerDescription s;
		return &s;
	}

    virtual void  CreateInstance(UI::ISkinRes* p, void** pp) override
    {
        *pp = IRectTracker::CreateInstance(p);
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
		return L"RectTracker";
	}
	virtual const GUID&  GetGUID() override
	{
		return __uuidof(IRectTracker);
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
