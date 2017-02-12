#include "stdafx.h"
#include "UISDK\Control\Inc\Interface\istringlistitem.h"
#include "UISDK\Control\Src\Control\ListBase\Item\String\stringlistitem.h"

namespace UI
{

UI_IMPLEMENT_INTERFACE_ACROSSMODULE(StringListItem, ListItemBase);
UI_IMPLEMENT_INTERFACE_ACROSSMODULE(StringListItemShareData, ListItemTypeShareData);

void  IStringListItemShareData::GetItemPadding(CRegion4* prc)
{
    __pImpl->GetItemPadding(prc);
}
void  IStringListItemShareData::SetItemPadding(CRegion4* prc)
{
    __pImpl->SetItemPadding(prc);
}
}