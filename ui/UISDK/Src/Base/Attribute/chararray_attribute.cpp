#include "stdafx.h"
#include "chararray_attribute.h"
#include "attribute.h"

using namespace UI;

namespace UI
{
AttributeBase*  CreateCharArrayAttribute()
{
    return new CharArrayAttribute();
}
}

CharArrayAttribute::CharArrayAttribute()
{
    m_pICharArrayAttribute = NULL;
    m_pBindValue = NULL;
    _this = NULL;
    _getter = NULL;
    _setter = NULL;
	m_bDefaultIsNULL = true;
    m_lBindValueSize = 16;
}

CharArrayAttribute::~CharArrayAttribute()
{
    SAFE_DELETE(m_pICharArrayAttribute);
}

ICharArrayAttribute*  CharArrayAttribute::GetICharArrayAttribute()
{
    if (!m_pICharArrayAttribute)
        m_pICharArrayAttribute = new ICharArrayAttribute(this);

    return m_pICharArrayAttribute;
}

void  CharArrayAttribute::SetBindValue(void* p)
{
    m_pBindValue = (wchar_t*)p;
	m_bDefaultIsNULL = false;
}

void  CharArrayAttribute::SetBindValueSize(long lSize)
{
    m_lBindValueSize = lSize;
}

// 这种情况下，默认值为NULL
void  CharArrayAttribute::SetBindFuction(void* _this, void* _setter, void* _getter)
{
    this->_this = _this;
    this->_setter = (pfnStringSetter)_setter;
    this->_getter = (pfnStringGetter)_getter;   
	m_bDefaultIsNULL = true;
}

LPCTSTR  CharArrayAttribute::Get()
{
    if (m_pBindValue)
    {
        return m_pBindValue;
    }
    else if (_this && _getter)
    {
        LPCTSTR  szReturn = NULL;
		long  localVarThis = (long)_this;
		long  localVarGetter = (long)_getter;

		__asm
		{
			mov  ecx, localVarThis;  // 设置this
			call localVarGetter;    // 调用
			mov  szReturn, eax;        // 获取返回值
		}

        return szReturn;
    }
    else
    {
        return NULL;
    }
}

void  CharArrayAttribute::Set(LPCTSTR szValue)
{
    if (m_pBindValue)
    {
        if (szValue)
        {
            _tcsncpy(m_pBindValue, szValue, m_lBindValueSize-1);
            m_pBindValue[m_lBindValueSize-1] = 0;
        }
        else
        {
            m_pBindValue[0] = 0;
        }
    }
    else if (_this && _setter)
    {
		long  localVarThis = (long)_this;
		long  localVarSetter = (long)_setter;

		__asm
		{
			mov eax, dword ptr [szValue] // 压入参数
			push eax;

			mov  ecx, localVarThis;   // 设置this
			call localVarSetter;     // 调用
		}
    }
}

void  CharArrayAttribute::Reset()
{
	Set(GetDefault());
}

bool  CharArrayAttribute::IsDefaultValue()
{
    LPCTSTR szValue = Get();
	LPCTSTR szDefault = GetDefault();

    if (szValue == NULL && szDefault == NULL)
        return true;

	if (szValue && szDefault && 0 ==_tcscmp(szValue, szDefault))
		return true;

    return false;
}

CharArrayAttribute*  CharArrayAttribute::SetDefault(LPCTSTR szValue)
{
	if (szValue)
	{
		m_bDefaultIsNULL = false;
		m_strDefault = szValue;
	}
	else
	{
		m_bDefaultIsNULL = true;
		m_strDefault.clear();
	}

	return this;
}

LPCTSTR  CharArrayAttribute::GetDefault()
{
	if (m_bDefaultIsNULL)
		return NULL;

	return m_strDefault.c_str();
}

void  CharArrayAttribute::Editor(SERIALIZEDATA* pData, UI::AttributeEditorProxy* p, UI::EditorAttributeFlag e)
{
    p->CharArray2Editor(this, e);
}