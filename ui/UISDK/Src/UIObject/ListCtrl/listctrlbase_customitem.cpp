#include "stdafx.h"
#include "listctrlbase.h"
#include "Src\Resource\skinres.h"
#include "CustomItem\custom_listitem.h"
#include "Src\UIObject\Panel\panel.h"
#include "ListItemRootPanel\listitemrootpanel.h"

CustomListItem*  ListCtrlBase::InsertCustomItem(
                    LPCTSTR  szLayoutName,
                    long lId,
                    IListItemBase* pParent, 
                    IListItemBase* pInsertAfter)
{
    if (!szLayoutName)
        return NULL;

    UIElementProxy element = m_pSkinRes->
            GetLayoutManager().FindListItemElement(szLayoutName);
    if (!element)
        return NULL;

    ICustomListItem* pItem = ICustomListItem::
            CreateInstance(m_pSkinRes->GetISkinRes());

    CustomListItem* pImpl = pItem->GetImpl();
    if (!this->InsertItem(pImpl, pParent, pInsertAfter))
    {
        SAFE_RELEASE(pItem);
        return NULL;
    }

    pImpl->SetId(lId);
    pImpl->CreateRootPanel();

	Panel* pRootPanel = pImpl->GetRootPanel();

    // 解析子控件
    pRootPanel->LoadAttributeFromXml(element.get(), false);

    // 解析子控件
    m_pSkinRes->GetLayoutManager().ParseChildElement(
        element.get(), pRootPanel, NOTIFY_TARGET_NULL);
 
	
	// 发送初始化通知
	Object* p = nullptr;
	while ( p = pRootPanel->EnumAllChildObject(p))
	{
		UISendMessage(p, UI_MSG_INITIALIZE);

		UIMSG msg;
		UIMSG msg2;
		msg.message = UI_MSG_INITIALIZE;
		msg2.message = UI_MSG_INITIALIZE2;
		Object::ForwardMessageToChildObject2(p, &msg, &msg2);

		UISendMessage(p, UI_MSG_INITIALIZE2);
	}
    return pImpl;
}

CustomListItem*  ListCtrlBase::GetCustomListItem(Object* pChildControl)
{
	if (!pChildControl)
		return NULL;

	Object* pParent = NULL;
	while (pParent = pChildControl->GetParentObject())
	{
		if (pParent == static_cast<Object*>(this))
		{
			ListItemBase* pItem = 
				static_cast<ListItemRootPanel*>(pChildControl)->GetListItem();

			return static_cast<CustomListItem*>(pItem);
		}
		pChildControl = pParent;
	}

	return NULL;
}