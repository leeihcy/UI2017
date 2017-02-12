#include "stdafx.h"
#include "edit_accessible.h"
#include "edit.h"

using namespace UI;

EditAccessible::EditAccessible(Edit& o) : m_refEdit(o)
{

}
EditAccessible::~EditAccessible()
{
 
}

HRESULT  EditAccessible::get_accValue(VARIANT varChild,
		BSTR* pszValue, BOOL& bHandled)
{
	bHandled = TRUE;

	if (!pszValue || varChild.vt != VT_I4)
		return E_INVALIDARG;

	IObject* pObj = m_refEdit.GetIEdit();
	if (varChild.lVal != CHILDID_SELF)
	{
		// 不去支持获取子对象
		bHandled = FALSE;
		return E_NOTIMPL;
	}	

	*pszValue = SysAllocString(m_refEdit.GetText());
	return S_OK;
}