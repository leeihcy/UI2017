#include "stdafx.h"
#include "textrenderbase_attribute.h"
#include "attribute.h"
#include "..\Application\uiapplication.h"
#include "..\Object\object.h"

namespace UI
{
    AttributeBase*  CreateTextRenderBaseAttribute()
    {
        return new TextRenderBaseAttribute();
    }
}

TextRenderBaseAttribute::TextRenderBaseAttribute() 
{
    ReloadOnChanged();

    m_pITextRenderBaseAttribute = NULL;
    m_ppBindValue = NULL;
    m_pObject = NULL;
//     _this = NULL;
//     _setter = NULL;
//     _getter = NULL;
}

TextRenderBaseAttribute::~TextRenderBaseAttribute()
{
	SAFE_DELETE(m_pITextRenderBaseAttribute);
}

void  TextRenderBaseAttribute::SetBindValue(void* p)
{
    m_ppBindValue = (ITextRenderBase**)p;
}

void  TextRenderBaseAttribute::Set(LPCTSTR szType)
{
    if (!m_ppBindValue)
        return;

	if (!m_pUIApplication || !m_pObject)
    {
        UIASSERT(0);
        return;
    }

    SAFE_RELEASE(*m_ppBindValue);
    m_pUIApplication->GetIUIApplication()->CreateTextRenderBaseByName(
		szType, m_pObject->GetIObject(), m_ppBindValue);
}

void  TextRenderBaseAttribute::Reset()
{
	if (m_ppBindValue)
	{
		SAFE_RELEASE(*m_ppBindValue);
	}
	else
	{

	}
}

LPCTSTR  TextRenderBaseAttribute::Get()
{
    if (!m_ppBindValue)
        return NULL;

	if (!*m_ppBindValue)
		return NULL;

	TEXTRENDER_TYPE eType = (*m_ppBindValue)->GetType();
	TextRenderFactory& factory = m_pUIApplication->GetTextRenderFactroy();

	LPCTSTR szType = factory.GetTextRenderBaseName(eType);
	return szType;
}

bool  TextRenderBaseAttribute::IsDefaultValue()
{
	if (m_ppBindValue)
	{
		if (*m_ppBindValue == NULL)
			return true;

//         if ((*m_ppBindValue)->GetType() == TEXTRENDER_TYPE_SIMPLE)
//             return true;
	}
    
	return false;
}


void  TextRenderBaseAttribute::do_child_action(SERIALIZEDATA* pData)
{
	// 子属性序列化
	if (m_ppBindValue && *m_ppBindValue)
	{
		LPCTSTR szOldPrefix = pData->szPrefix;
		LPCTSTR szOldParentKey = pData->szParentKey;

		pData->szPrefix = m_strPrefix.c_str();
		pData->szParentKey = GetKey();

		(*m_ppBindValue)->Serialize(pData);

		pData->szPrefix = szOldPrefix;
		pData->szParentKey = szOldParentKey;
	}
}

void  TextRenderBaseAttribute::Load(SERIALIZEDATA* pData)
{
	// 保存自己的类型
	__super::Load(pData);

	// 子属性序列化
	do_child_action(pData);
}
void  TextRenderBaseAttribute::Save(SERIALIZEDATA* pData)
{
	// 保存自己的类型
	__super::Save(pData);

	// 子属性序列化
	do_child_action(pData);
}

void  TextRenderBaseAttribute::Editor(SERIALIZEDATA* pData, AttributeEditorProxy* p, EditorAttributeFlag e)
{
    p->TextRenderBase2Editor(this, e);

	if (pData && e == EDITOR_ATTRIBUTE_ADD)
	{
		// 子属性序列化
		do_child_action(pData);
	}
}

ITextRenderBaseAttribute*  TextRenderBaseAttribute::GetITextRenderBaseAttribute()
{
    if (!m_pITextRenderBaseAttribute)
        m_pITextRenderBaseAttribute = new ITextRenderBaseAttribute(this);

    return m_pITextRenderBaseAttribute;
}
