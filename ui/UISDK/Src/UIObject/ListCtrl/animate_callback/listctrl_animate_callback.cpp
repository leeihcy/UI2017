#include "stdafx.h"
#include "listctrl_animate_callback.h"
#include "..\ListItemBase\listitembase.h"

#if 0
ListCtrlAnimateCallback::ListCtrlAnimateCallback()
{
    m_pfnListItemRectChangedCallback = NULL;
}
ListCtrlAnimateCallback::~ListCtrlAnimateCallback()
{

}

ListCtrlAnimateCallback&  ListCtrlAnimateCallback::Get()
{
    static ListCtrlAnimateCallback s;
    return s;
}

void  ListCtrlAnimateCallback::SetListItemRectChangedCallback(
            pfnListItemRectChangedCallback f)
{
    Get().m_pfnListItemRectChangedCallback = f;
}

bool  ListCtrlAnimateCallback::HandleItemRectChanged(
           ListItemBase& item,
           LPCRECT prcOld, 
           LPCRECT prcNew)
{
    if (!Get().m_pfnListItemRectChangedCallback)
        return false;

    
    return Get().m_pfnListItemRectChangedCallback(
                *item.GetIListItemBase(),
                prcOld, 
                prcNew);
}
#endif