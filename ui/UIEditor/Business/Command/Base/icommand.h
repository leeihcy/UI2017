#pragma once
#include "UISDK\Inc\Interface\ixmlwrap.h"
#include "Dialog\Framework\statusbar.h"
#include "Dialog\Framework\MainFrame.h"
#include "Dialog\Framework\childframe.h"
#include "Business\Project\UIEditor\IUIEditorImpl.h"

interface ICommand : public IRootInterface
{
    virtual long   execute() = 0;
    virtual long   undo() = 0;

	virtual long*  GetKey() = 0;
    virtual ULONG  AddRef(void) = 0;
    virtual ULONG  Release(void) = 0;
};

//
//  备注：
//    在命令当中，不要保存IUIElement* IListItemBase*指针，这些元素随时可能
//    被释放掉，然后创建一个新的。可以相信的指针只有IObject*，因此也要注意
//    在执行删除对象操作时，不要delete pObj
//
class CommandBase : public ICommand
{
protected:
    CommandBase()
    {
        m_lRef = 0;
    }

public:
    virtual long execute()
    {
        return 0;
    }
    virtual long undo()
    {
        return 0;
    }

    virtual ULONG  AddRef(void)
    {
        return ++m_lRef;
    }
    virtual ULONG  Release(void)
    {
        --m_lRef; 
        if (0 == m_lRef)
        {
            delete this; 
            return 0;
        } 
        return m_lRef;
    }

    void  SetDirty(IUIElement* pXmlElem)
    {
        // 设置dirty属性
        if (pXmlElem)
        {
            IUIDocument* pDoc = pXmlElem->GetDocument();
            if (pDoc)
                pDoc->SetDirty(true);
        }

        g_pGlobalData->m_pMainFrame->SetDirty();
    }

    void  SetDirty(IObject* pObject)
    {
        // 设置dirty属性
        if (!pObject)
            return;

        ObjectEditorData* pData = GetUIEditor()->GetObjectEditorData(pObject);
        if (!pData)
            return;

        SetDirty(pData->pXmlElement);
    }

protected:
    ULONG  m_lRef;
};