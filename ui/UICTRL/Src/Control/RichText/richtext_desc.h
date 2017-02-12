#pragma once
#include "..\UISDK\Inc\Interface\iobject.h"
#include "Inc\Interface\irichtext.h"

class RichTextDescription : public IObjectDescription
{
public:
	static IObjectDescription*  Get()
	{
		static RichTextDescription s;
		return &s;
	}

    virtual void  CreateInstance(UI::ISkinRes* p, void** pp) override
    {
        *pp = IRichText::CreateInstance(p);
    }

	virtual OBJ_TYPE  GetMajorType() override
	{
		return OBJ_CONTROL;
	}
	virtual long  GetMinorType() override
	{
		return CONTROL_RICHTEXT;
	}
	virtual const wchar_t*  GetCategory() override
	{
		return CATEGORY_CONTROL; 
	}
	virtual const wchar_t*  GetTagName() override
	{
		return L"RichText";
	}
	virtual const GUID&  GetGUID() override
	{
		return __uuidof(IRichText);
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

