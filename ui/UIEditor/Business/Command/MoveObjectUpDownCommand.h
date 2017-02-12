#pragma once
#include "Base\icommand.h"
#include "Dialog\Framework\propertydialog.h"
#include "Dialog\Module\Layout\LayoutPropertyHandler.h"

// 将控件上移、下移、移至顶、移至底
class MoveObjectUpDownCommandBase : public CommandBase
{
public:
    MoveObjectUpDownCommandBase()
    {
        m_pObject = NULL;
    }

    virtual long*  GetKey()
    {
        if (m_pObject)
        {
            return (long*)m_pObject->GetWindowObject();
        }
        return NULL;
    }
    
    // 废弃，使用MoveObjectAfter
    // 先删除，再添加
//     void  MoveObjectUp(IObject*  pObject)
//     {
//         if (!pObject)
//             return;
// 
//         IObject*  pParentObj = pObject->GetParentObject();
//         if (!pParentObj)
//             return;
// 
//         IObject*  pPrevObj = pObject->GetPrevObject();
//         if (!pPrevObj)
//             return;
// 
//         IObject* pPrevPrevObj = pPrevObj->GetPrevObject();
//         IObject* pNextObject = pObject->GetNextObject();
// 
//         // 1. 调整xml位置
//         IUIElement*  pElement = GetUIEditor()->GetObjectXmlElem(pObject);
//         IUIElement*  pPrevElement = GetUIEditor()->GetObjectXmlElem(pPrevObj);
//         IUIElement*  pParentElement = GetUIEditor()->GetObjectXmlElem(pParentObj);
// 
//         pParentElement->RemoveChild(pElement);
//         pParentElement->AddChildBefore(pElement, pPrevElement);
// 
//         // 2. 调整project tree位置
//         IListItemBase*  pListItem = GetUIEditor()->GetObjectHTREEITEM(pObject);
//         IListItemBase*  pPrevListItem = GetUIEditor()->GetObjectHTREEITEM(pPrevObj);
//         IListItemBase*  pParentListItem = GetUIEditor()->GetObjectHTREEITEM(pParentObj);
// 
//         if (pPrevPrevObj)
//         {
//             IListItemBase*  pPrevPrevListItem = GetUIEditor()->GetObjectHTREEITEM(pPrevPrevObj);
// 
//             pPrevPrevListItem->SetNextItem(pListItem);
//             pListItem->SetPrevItem(pPrevPrevListItem);
//         }
//         else
//         {
//             pParentListItem->SetChildItem(pListItem);
//             pListItem->SetPrevItem(NULL);
//         }
// 
//         if (pNextObject)
//         {
//             IListItemBase*  pNextListItem = GetUIEditor()->GetObjectHTREEITEM(pNextObject);
// 
//             pNextListItem->SetPrevItem(pPrevListItem);
//             pPrevListItem->SetNextItem(pNextListItem);
//         }
//         else
//         {
//             pPrevListItem->SetNextItem(NULL);
//         }
// 
//         pListItem->SetNextItem(pPrevListItem);
//         pPrevListItem->SetPrevItem(pListItem);
// 
//         
//         // 3. 调整object位置       
//         if (pPrevPrevObj)
//         {
//             pPrevPrevObj->SetNextObjectDirect(pObject);
//             pObject->SetPrevObjectDirect(pPrevPrevObj);
//         }
//         else
//         {
//             pParentObj->SetChildObjectDirect(pObject);
//             pObject->SetPrevObjectDirect(NULL);
//         } 
// 
//         if (pNextObject)
//         {
//             pNextObject->SetPrevObjectDirect(pPrevObj);
//             pPrevObj->SetNextObjectDirect(pNextObject);
//         }
//         else
//         {
//             pPrevObj->SetNextObjectDirect(NULL);
//         }
// 
//         pObject->SetNextObjectDirect(pPrevObj);
//         pPrevObj->SetPrevObjectDirect(pObject);
//     }

    void  MoveObjectAfter(
            IObject*  pObject, 
            IObject* pNewParent, 
            IObject* pObjAfter)
    {
        if (!pObject)
            return;

        IObject*  pParentObj = pObject->GetParentObject();
        if (!pParentObj)
            return;

        // 不能移动到自己的子结点中
        if (pObject->IsMyChild(pNewParent, true))
            return;
        
        // 1. 调整xml位置
        IUIElement*  pElement = GetUIEditor()->GetObjectXmlElem(pObject);
        IUIElement*  pParentElement = GetUIEditor()->GetObjectXmlElem(pParentObj);
        IUIElement*  pNewParentElement = GetUIEditor()->GetObjectXmlElem(pNewParent);
        IUIElement*  pInsertAfterElement = NULL;
        if (pObjAfter)
        {
            pInsertAfterElement = GetUIEditor()->GetObjectXmlElem(pObjAfter);
        }

        pElement->MoveTo(pNewParentElement, pInsertAfterElement);

        // 2. 调整project tree位置
        IListItemBase*  pListItem = GetUIEditor()->GetObjectHTREEITEM(pObject);
        IListCtrlBase*  pListCtrl = pListItem->GetIListCtrlBase();
        IListItemBase*  pNewParentItem = GetUIEditor()->GetObjectHTREEITEM(pNewParent);
        IListItemBase*  pInsertAfter = UITVI_FIRST;
        if (pObjAfter)
        {
            pInsertAfter = GetUIEditor()->GetObjectHTREEITEM(pObjAfter);
        }

        pListCtrl->MoveItem(pListItem, pNewParentItem, pInsertAfter);
        
        //////////////////////////////////////////////////////////////////////////
        // 3. 调整object位置       
        
        pObject->RemoveMeInTheTree();
        pNewParent->InsertChild(pObject, pObjAfter);

        // 修改了父结点后，重新加载属性。父结点的布局类型可能不一样，需要修改加载属性
        UE_ReloadObjectAttribute(pObject);
    }

    void  MoveUp()
    {
        IObject*  pInsertAfter = m_pObject->GetPrevObject();
        if (!pInsertAfter)
            return;
        pInsertAfter = pInsertAfter->GetPrevObject();

        MoveObjectAfter(
            m_pObject, 
            m_pObject->GetParentObject(), 
            pInsertAfter);
    }
    void  MoveDown()
    {
        IObject*  pNextObj = m_pObject->GetNextObject();
        if (!pNextObj)
            return;

        MoveObjectAfter(
            m_pObject, 
            m_pObject->GetParentObject(), 
            pNextObj);
    }

    void  on_post_do()
    {
        IUIElement*  pElement = GetUIEditor()->GetObjectXmlElem(m_pObject);
        SetDirty(pElement);

        // 刷新界面
        UE_RefreshUIWindow(m_pObject);
        UE_RefreshLayoutView(m_pObject);
    }
protected:
    IObject*  m_pObject;
};

class MoveObjectUpCommand : public MoveObjectUpDownCommandBase
{
public:
    static ICommand*  CreateInstance(IObject* pObject)
    {
        if (!pObject)
            return NULL;

        if (!pObject->GetPrevObject())
            return NULL;

        MoveObjectUpCommand* p = new MoveObjectUpCommand;
        p->AddRef();
        p->m_pObject = pObject;

        return p;
    }

    virtual long execute()
    {
        MoveUp();
        on_post_do();
        return 0;
    }
    virtual long undo()
    {
        MoveDown();
        on_post_do();
        return 0;
    }
};

class MoveObjectDownCommand : public MoveObjectUpDownCommandBase
{
public:
    static ICommand*  CreateInstance(IObject* pObject)
    {
        if (!pObject)
            return NULL;

        if (!pObject->GetNextObject())
            return NULL;

        MoveObjectDownCommand* p = new MoveObjectDownCommand;
        p->AddRef();
        p->m_pObject = pObject;

        return p;
    }

    virtual long execute()
    {
        MoveDown();
        on_post_do();
        return 0;
    }
    virtual long undo()
    {
        MoveUp();
        on_post_do();
        return 0;
    }
};


class MoveObjectTopCommand : public MoveObjectUpDownCommandBase
{
public:
    MoveObjectTopCommand()
    {
        m_pPrevObject = NULL;
    }
    static ICommand*  CreateInstance(IObject* pObject)
    {
        if (!pObject)
            return NULL;

        if (!pObject->GetPrevObject())
            return NULL;

        MoveObjectTopCommand* p = new MoveObjectTopCommand;
        p->AddRef();
        p->m_pObject = pObject;
        p->m_pPrevObject = pObject->GetPrevObject();

        return p;
    }

    virtual long execute()
    {
        MoveObjectAfter(
            m_pObject, 
            m_pObject->GetParentObject(), 
            NULL);

        on_post_do();
        return 0;
    }
    virtual long undo()
    {
        MoveObjectAfter(
            m_pObject, 
            m_pObject->GetParentObject(), 
            m_pPrevObject);

        on_post_do();
        return 0;
    }

protected:
    IObject*  m_pPrevObject; 
};

class MoveObjectBottomCommand : public MoveObjectUpDownCommandBase
{
public:
    MoveObjectBottomCommand()
    {
        m_pPrevObject = NULL;
    }
    static ICommand*  CreateInstance(IObject* pObject)
    {
        if (!pObject)
            return NULL;

        if (!pObject->GetNextObject())
            return NULL;

        MoveObjectBottomCommand* p = new MoveObjectBottomCommand;
        p->AddRef();
        p->m_pObject = pObject;
        p->m_pPrevObject = pObject->GetPrevObject();

        return p;
    }

    virtual long execute()
    {
        MoveObjectAfter(
            m_pObject, 
            m_pObject->GetParentObject(), 
            m_pObject->GetParentObject()->GetLastChildObject());

        on_post_do();
        return 0;
    }
    virtual long undo()
    {
        MoveObjectAfter(
            m_pObject, 
            m_pObject->GetParentObject(), 
            m_pPrevObject);

        on_post_do();
        return 0;
    }

protected:
    IObject*  m_pPrevObject; 
};


// 拖拽控件位置
class DragDropObjectCommand : public MoveObjectUpDownCommandBase
{
public:
    DragDropObjectCommand()
    {
        m_pPrevObject = NULL;
        m_pNewParentObject = NULL;
        m_pInsertAfterObject = NULL;
    }
    static ICommand*  CreateInstance(IObject* pObject, IObject*  pObjNewParent, IObject* pObjInsertAfter)
    {
        if (!pObject || !pObjNewParent)
            return NULL;

        DragDropObjectCommand* p = new DragDropObjectCommand;
        p->AddRef();
        p->m_pObject = pObject;
        p->m_pPrevObject = pObject->GetPrevObject();
        p->m_pParentObject = pObject->GetParentObject();
        p->m_pNewParentObject = pObjNewParent;
        p->m_pInsertAfterObject = pObjInsertAfter;

        return p;
    }

    virtual long execute()
    {
        MoveObjectAfter(
            m_pObject, 
            m_pNewParentObject, 
            m_pInsertAfterObject);

        on_post_do();
        return 0;
    }
    virtual long undo()
    {
        MoveObjectAfter(
            m_pObject, 
            m_pParentObject, 
            m_pPrevObject);

        on_post_do();
        return 0;
    }

protected:
    IObject*  m_pParentObject;
    IObject*  m_pPrevObject; 

    IObject*  m_pNewParentObject;
    IObject*  m_pInsertAfterObject;

};