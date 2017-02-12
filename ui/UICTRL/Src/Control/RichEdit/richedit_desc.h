#pragma once
#include "UI\UISDK\Inc\Interface\iobjectdescription.h"
#include "UI\UICTRL\Inc\Interface\irichedit.h"

class RichEditDescription : public IObjectDescription
{
public:
	static IObjectDescription*  Get()
	{
		static RichEditDescription s;
		return &s;
	}

    virtual void  CreateInstance(UI::ISkinRes* p, void** pp) override
    {
        *pp = IRichEdit::CreateInstance(p);
    }

	virtual OBJ_TYPE  GetMajorType() override
	{
		return OBJ_CONTROL;
	}
	virtual long  GetMinorType() override
	{
		return CONTROL_RICHEDIT;
	}
	virtual const wchar_t*  GetCategory() override
	{
		return CATEGORY_CONTROL; 
	}
	virtual const wchar_t*  GetTagName() override
	{
		return L"RichEdit";
	}
	virtual const GUID&  GetGUID() override
	{
		return __uuidof(IRichEdit);
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

