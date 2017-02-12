#pragma once
#include "..\UISDK\Inc\Interface\iobjectdescription.h"
#include "Inc\iprojecttreeview.h"

class ProjectTreeViewDescription : public IObjectDescription
{
public:
	static IObjectDescription*  Get()
	{
        static ProjectTreeViewDescription s;
		return &s;
	}

    virtual void  CreateInstance(UI::ISkinRes* p, void** pp) override
    {
        *pp = IProjectTreeView::CreateInstance(p);
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
		return L"ProjectTreeView";
	}
	virtual const GUID&  GetGUID() override
	{
        return __uuidof(IProjectTreeView);
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

