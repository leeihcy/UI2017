#pragma once
#include "..\UISDK\Inc\Interface\iobjectdescription.h"
#include "Inc\Interface\itreeview.h"

class TreeViewDescription : public IObjectDescription
{
public:
	static IObjectDescription*  Get()
	{
		static TreeViewDescription s;
		return &s;
	}

    virtual void  CreateInstance(UI::ISkinRes* p, void** pp) override
    {
        *pp = ITreeView::CreateInstance(p);
    }

	virtual OBJ_TYPE  GetMajorType() override
	{
		return OBJ_CONTROL;
	}
	virtual long  GetMinorType() override
	{
		return CONTROL_TREEVIEWCTRL;
	}
	virtual const wchar_t*  GetCategory() override
	{
		return _T("ListControl"); 
	}
	virtual const wchar_t*  GetTagName() override
	{
		return L"TreeView";
	}
	virtual const GUID&  GetGUID() override
	{
		return __uuidof(ITreeView);
	}

    virtual void  GetDefaultSize(long& w, long& h) override
    {
        w = 200;
        h = 200;
    }
    virtual HBITMAP  LoadIcon() override 
    {
        return NULL;
    }
};

