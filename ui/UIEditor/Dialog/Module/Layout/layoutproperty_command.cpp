#include "stdafx.h"
#include "Dialog\Framework\propertydialog.h"
#include "Business\uieditorfunction.h"
#include "Business\Command\Base\commandhistorymgr.h"
#include "Business\Command\AddObjectAttributeCommand.h"
#include "Business\Command\RemoveObjectAttributeCommand.h"
#include "Business\Command\ModifyObjectAttributeCommand.h"
#include "Business\Command\ClearObjectAttributeCommand.h"
#include "LayoutPropertyHandler.h"

long  LayoutPropertyHandler::DoCommand(LPCTSTR  szCommand)
{
    if (NULL == szCommand)
        return -1;

    // 先拆成两部分: command  args
    String  strCommand;
    String  strArgs;

    ParsePropertyCommand(szCommand, strCommand, strArgs);

    // 解析命令类型
    PROPERTY_COMMAND_TYPE eCommand = GetCommandType(strCommand);
    if (eCommand == PROPERTY_COMMAND_UNKNOWN)
    {
        String str = _T("不能解析的命令: ");
        str.append(strCommand);
        ShowStatusText(str.c_str());
        return -1;
    }

    return DoCommand(eCommand, strArgs.c_str());
}

long  LayoutPropertyHandler::DoCommand(PROPERTY_COMMAND_TYPE eType, LPCTSTR szArg)
{
    if (NULL == m_pSkinRes || NULL == m_pObject)
    {
        ShowStatusText(_T("无目标对象"));
        return -1;
    }

    switch (eType)
    {
    case PROPERTY_COMMAND_ADD:
        return OnCommand_Add(szArg);
        break;

    case PROPERTY_COMMAND_DELETE:
        return OnCommand_Delete(szArg);
        break;

    case PROPERTY_COMMAND_CLEAR:
        return OnCommand_Clear(szArg);
        break;

    case PROPERTY_COMMAND_MODIFY:
        return OnCommand_Modify(szArg);
        break;
    }

    return -1;
}

long  LayoutPropertyHandler::OnCommand_Add(LPCTSTR szArg)
{
    String strKey;
    String strValue;

    ParsePropertyCommandArg(szArg, strKey, strValue);
    if (strKey.empty())
    {
        String  str = _T("无效属性");
        ShowStatusText(str.c_str());
        return -1;
    }

    if (-1 == UE_ExecuteCommand(AddObjectAttributeCommand::CreateInstance(
                m_pSkinRes, m_pObject, m_pXmlElement, strKey.c_str(), strValue.c_str())))
    {
        return -1;
    }

    {
        String  str = _T("Command: Add ");
        str.append(strKey.c_str());
        str.append(_T("=\""));
        str.append(strValue.c_str());
        str.append(_T("\" 成功"));
        ShowStatusText(str.c_str());
    }
    return 0;
}

long  LayoutPropertyHandler::OnCommand_Delete(LPCTSTR szArg)
{
    if (NULL == szArg)
        return -1;
    
    String strKey(szArg);
    TrimString(strKey);

    if (strKey.empty())
    {
        // 获取选中项
        IListItemBase* pItem = m_pPropertyCtrl->GetFirstSelectItem();
		if (NULL == pItem || pItem->GetItemFlag() == IPropertyCtrlGroupItem::FLAG)
        {
            String  str = _T("delete 命令需要一个属性字段参数");
            ShowStatusText(str.c_str());
            return -1;
        }

        strKey = pItem->GetText();
        {
            String  str = _T("再次回车确认删除当前选择项:");
            str.append(strKey);
            ShowStatusText(str.c_str());

            if (!WaitForEnter2Confirm(m_pPropertyDialog->GetHWND()))
            {
                String  str = _T("delete 命令已取消");
                ShowStatusText(str.c_str());
                return -1;
            }
        }
    }

    if (-1 == UE_ExecuteCommand(
				RemoveObjectAttributeCommand::CreateInstance(
				m_pSkinRes, m_pObject, m_pXmlElement, strKey.c_str())))
    {
        return -1;
    }

    {
        String  str = _T("删除属性字段完成: ");
        str.append(strKey);
        ShowStatusText(str.c_str());
    }
    return 0;
}


long  LayoutPropertyHandler::OnCommand_Modify(LPCTSTR szArg)
{
    String strKey;
    String strValue;

    ParsePropertyCommandArg(szArg, strKey, strValue);
    if (strKey.empty())
    {
        String  str = _T("无效属性");
        ShowStatusText(str.c_str());
        return -1;
    }

    if (-1 == UE_ExecuteCommand(
        ModifyObjectAttributeCommand2::CreateInstance(
            m_pObject, strKey.c_str(), strValue.c_str())))
    {
        return -1;
    }

    {
        String  str = _T("Command: Modify ");
        str.append(strKey.c_str());
        str.append(_T("=\""));
        str.append(strValue.c_str());
        str.append(_T("\" 成功"));
        ShowStatusText(str.c_str());
    }

    return 0;
}

long  LayoutPropertyHandler::OnCommand_Clear(LPCTSTR szArg)
{
    String  str = _T("再次回车确认清空对象所有属性");
    ShowStatusText(str.c_str());

    if (!WaitForEnter2Confirm(m_pPropertyDialog->GetHWND()))
    {
        String  str = _T("clear 命令已取消");
        ShowStatusText(str.c_str());
        return -1;
    }

    if (-1 == UE_ExecuteCommand(
				ClearObjectAttributeCommand::CreateInstance(
					m_pSkinRes, m_pObject, m_pXmlElement)))
    {
        return -1;
    }

    {
        String  str = _T("清空对象属性完成");
        ShowStatusText(str.c_str());
    }

    return 0;
}

long  LayoutPropertyHandler::OnCommand_Insert(LPCTSTR szArg)
{
    return -1;
}
long  LayoutPropertyHandler::OnCommand_InsertBefore(LPCTSTR szArg)
{
    return -1;
}


// TODO: 现在命令少，直接if判断。以后命令多了再考虑优化
PROPERTY_COMMAND_TYPE  LayoutPropertyHandler::GetCommandType(String& strCommand)
{
    PROPERTY_COMMAND_TYPE eType = PROPERTY_COMMAND_UNKNOWN;
    if (strCommand.empty())
        return eType;

    std::transform(strCommand.begin(), strCommand.end(), strCommand.begin(), tolower);

    if (strCommand == _T("insert") ||
        strCommand == _T("i")      ||
        strCommand == _T("insertafter"))
    {
        eType = PROPERTY_COMMAND_INSERTAFTER;
    }else
    if (strCommand == _T("insertbefore") ||
        strCommand == _T("ib"))
    {
        eType = PROPERTY_COMMAND_INSERTBEFORE;
    }else
    if (strCommand == _T("add") ||
        strCommand == _T("a"))
    {
        eType = PROPERTY_COMMAND_ADD;
    }else 
    if (strCommand == _T("clear") ||
        strCommand == _T("c"))
    {
        eType = PROPERTY_COMMAND_CLEAR;
    }else
    if (strCommand == _T("delete") ||
        strCommand == _T("d")      ||
        strCommand == _T("remove") ||
        strCommand == _T("r")      ||
        strCommand == _T("erase")  ||
        strCommand == _T("e"))
    {
        eType = PROPERTY_COMMAND_DELETE;
    }else
    if (strCommand == _T("modify") ||
        strCommand == _T("m"))
    {
        eType = PROPERTY_COMMAND_MODIFY;
    }

    return eType;
}
