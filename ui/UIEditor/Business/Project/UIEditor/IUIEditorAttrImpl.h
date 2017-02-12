#pragma once
#include "UISDK\Inc\Interface\iuieditor.h"

#if 0
template<class T>
class IAttributeImpl : public T
{
public:
	IAttributeImpl()
	{
	}
	virtual ~IAttributeImpl()
	{

	}
	virtual void  Release()
	{
		delete this;
	}
	
	virtual LPCTSTR GetName()
	{
		return m_strName.c_str();
	}
	virtual LPCTSTR GetDesc()
	{
		return m_strDesc.c_str();
	}

	virtual void  SetName(LPCTSTR szText)
	{
		if (szText)
			m_strName = szText;
		else
			m_strName.clear();
	}
	virtual void  SetDesc(LPCTSTR szText)
	{
		if (szText)
			m_strDesc = szText;
		else
			m_strDesc.clear();
	}

	virtual void Insert2PropertyCtrl(IPropertyCtrl* p)
	{

	}

protected:
	String   m_strName;
	String   m_strDesc;
	String   m_strDefValue;
};

class CGroupAttribute : public IAttributeImpl<UI::IUIEditorGroupAttribute>
{
public:
	CGroupAttribute();
	~CGroupAttribute();

public:
	virtual void  AddAttribute(IUIEditorAttrBase* p);
	virtual void  Insert2PropertyCtrl(IPropertyCtrl* pCtrl, IListItemBase* pParent);

public:
	list<IUIEditorAttrBase*>  m_AttrList;
	bool  m_bRoot;
};

template <class T>
class IUIEditorAttributeImpl : public IAttributeImpl<T>
{
public:
	virtual void  SetDefaultValue(LPCTSTR szText)
	{
		if (szText)
			m_strDefValue = szText;
		else
			m_strDefValue.clear();
	}

    virtual void  SetKey(LPCTSTR szText)
    {
        if (szText)
            m_strKey = szText;
        else
            m_strKey.clear();
    }

protected:
    String  m_strKey;
};

class CTextAttribute : public IUIEditorAttributeImpl<IUIEditorTextAttribute>
{
public:
    virtual void  Insert2PropertyCtrl(IPropertyCtrl* pCtrl, IListItemBase* pParent);
};

class CBoolAttribute : public IUIEditorAttributeImpl<IUIEditorBoolAttribute>
{
public:
	virtual void  Insert2PropertyCtrl(IPropertyCtrl* pCtrl, IListItemBase* pParent);
};


struct ComboBoxOptionItem
{
    String  strText;
    String  strValue;
};
class CComboBoxAttribute : public IUIEditorAttributeImpl<IUIEditorComboBoxAttribute>
{
public:
    CComboBoxAttribute();
    ~CComboBoxAttribute();

    virtual void  Insert2PropertyCtrl(IPropertyCtrl* pCtrl, IListItemBase* pParent);
    virtual IUIEditorComboBoxAttribute*  AddOption(LPCTSTR szText, LPCTSTR szValue = NULL);
    virtual void  SetReloadOnChanged(bool b);
    virtual bool  GetReloadOnChanged();

protected:
    vector<ComboBoxOptionItem*>  m_vecItems;

    // 当选项改变时，重新加载对象的属性列表。例如修改bkgnd.render.type之后，对象的属性选项需要重新加载
    // 当前为了简单起见，直接将该对象的整个属性列表先清空，再重新加载一次。
    bool   m_bReloadOnChanged;  
};


class CButtonAttribute : public IUIEditorAttributeImpl<IUIEditorButtonAttribute>
{
public:
	virtual void  Insert2PropertyCtrl(IPropertyCtrl* pCtrl, IListItemBase* pParent);
};
#endif