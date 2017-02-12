#pragma once
#include "..\UISDK\Src\Base\Accessible\iuiaccessible.h"

namespace UI
{
class Edit;

class EditAccessible : public IUIAccessible
{
public:
	EditAccessible(Edit&  m_refEdit);
	~EditAccessible();

	virtual HRESULT get_accValue(VARIANT varChild,
		BSTR* pszValue, BOOL& bHandled) override;

private:
	Edit&  m_refEdit;
};

}