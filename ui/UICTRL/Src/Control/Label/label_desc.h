#pragma once
#include "Inc\Interface\ilabel.h"

class LabelDescription : public IObjectDescription
{
public:
	static IObjectDescription*  Get()
	{
		static LabelDescription s;
		return &s;
	}

    virtual void  CreateInstance(UI::ISkinRes* p, void** pp) override
    {
        *pp = ILabel::CreateInstance(p);
    }

	virtual OBJ_TYPE  GetMajorType() override
	{
		return OBJ_CONTROL;
	}
	virtual long  GetMinorType() override
	{
		return CONTROL_LABEL;
	}
	virtual const wchar_t*  GetCategory() override
	{
		return CATEGORY_CONTROL; 
	}
	virtual const wchar_t*  GetTagName() override
	{
		return L"Label";
	}
	virtual const GUID&  GetGUID() override
	{
		return __uuidof(ILabel);
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

class PictureCtrlDescription : public IObjectDescription
{
public:
	static IObjectDescription*  Get()
	{
		static PictureCtrlDescription s;
		return &s;
	}

    virtual void  CreateInstance(UI::ISkinRes* p, void** pp) override
    {
        *pp = IPictureCtrl::CreateInstance(p);
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
		return L"Picture";
	}
	virtual const GUID&  GetGUID() override
	{
        return __uuidof(IPictureCtrl);
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

class GifCtrlDescription : public IObjectDescription
{
public:
	static IObjectDescription*  Get()
	{
		static GifCtrlDescription s;
		return &s;
	}

    virtual void  CreateInstance(UI::ISkinRes* p, void** pp) override
    {
        *pp = IGifCtrl::CreateInstance(p);
    }

	virtual OBJ_TYPE  GetMajorType() override
	{
		return OBJ_CONTROL;
	}
	virtual long  GetMinorType() override
	{
		return CONTROL_GIFCTRL;
	}
	virtual const wchar_t*  GetCategory() override
	{
		return CATEGORY_CONTROL; 
	}
	virtual const wchar_t*  GetTagName() override
	{
		return L"Gif";
	}
	virtual const GUID&  GetGUID() override
	{
		return __uuidof(IGifCtrl);
	}

	virtual void  GetDefaultSize(long& w, long& h) override
	{
		w = 40;
		h = 40;
	}
	virtual HBITMAP  LoadIcon() override 
	{
		return NULL;
	}
};