#pragma once
class CPropertyDialog;

// 属性窗口可以用来显示各种属性。每种属性需要注册自己的handler

enum PROPERTY_COMMAND_TYPE
{
    PROPERTY_COMMAND_UNKNOWN,
    PROPERTY_COMMAND_INSERTAFTER,
    PROPERTY_COMMAND_INSERTBEFORE,
    PROPERTY_COMMAND_ADD,
    PROPERTY_COMMAND_DELETE,
    PROPERTY_COMMAND_CLEAR,
    PROPERTY_COMMAND_MODIFY
};

interface  IPropertyHandler : public IMessage
{
    IPropertyHandler() : IMessage(CREATE_IMPL_TRUE){};

	virtual long  GetHandlerType() PURE;
    virtual void  OnLoad(CPropertyDialog* pWnd) PURE;
    virtual void  OnUnload() PURE;

};