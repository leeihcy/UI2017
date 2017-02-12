#pragma once
#include "..\UISDK\Inc\Interface\iobjectdescription.h"
#include "Inc\Interface\iedit.h"
#include "password_edit.h"

class EditDescription : public IObjectDescription
{
public:
	static IObjectDescription*  Get()
	{
		static EditDescription s;
		return &s;
	}

    virtual void  CreateInstance(UI::ISkinRes* p, void** pp) override
    {
        *pp = IEdit::CreateInstance(p);
    }

	virtual OBJ_TYPE  GetMajorType() override
	{
		return OBJ_CONTROL;
	}
	virtual long  GetMinorType() override
	{
		return CONTROL_EDIT;
	}
	virtual const wchar_t*  GetCategory() override
	{
		return CATEGORY_CONTROL; 
	}
	virtual const wchar_t*  GetTagName() override
	{
		return L"Edit";
	}
	virtual const GUID&  GetGUID() override
	{
		return __uuidof(IEdit);
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


class InstantEditDescription : public IObjectDescription
{
public:
	static IObjectDescription*  Get()
	{
		static InstantEditDescription s;
		return &s;
	}

    virtual void  CreateInstance(UI::ISkinRes* p, void** pp) override
    {
        *pp = IInstantEdit::CreateInstance(p);
    }

	virtual OBJ_TYPE  GetMajorType() override
	{
		return OBJ_CONTROL;
	}
	virtual long  GetMinorType() override
	{
		return CONTROL_EDIT;
	}
	virtual const wchar_t*  GetCategory() override
	{
		return CATEGORY_CONTROL; 
	}
	virtual const wchar_t*  GetTagName() override
	{
		return L"InstantEdit";
	}
	virtual const GUID&  GetGUID() override
	{
		return __uuidof(IInstantEdit);
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


class PasswordEditDescription : public IObjectDescription
{
public:
	static IObjectDescription*  Get()
	{
		static PasswordEditDescription s;
		return &s;
	}

	virtual void  CreateInstance(UI::ISkinRes* p, void** pp) override
	{
		*pp = IPasswordEdit::CreateInstance(p);
	}

	virtual OBJ_TYPE  GetMajorType() override
	{
		return OBJ_CONTROL;
	}
	virtual long  GetMinorType() override
	{
		return CONTROL_EDIT;
	}
	virtual const wchar_t*  GetCategory() override
	{
		return CATEGORY_CONTROL;
	}
	virtual const wchar_t*  GetTagName() override
	{
		return L"PasswordEdit";
	}
	virtual const GUID&  GetGUID() override
	{
		return __uuidof(IPasswordEdit);
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

