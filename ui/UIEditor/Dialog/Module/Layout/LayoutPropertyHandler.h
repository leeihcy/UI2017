#pragma once
#include "Dialog\Framework\ipropertyhandler.h"
#include "UISDK\Inc\Interface\iwindow.h"
#include "UICTRL\Inc\Interface\iedit.h"
#include "UIEditorCtrl\Inc\ipropertyctrl.h"


namespace UI
{
    interface IPropertyCtrl;
    interface IEdit;
    interface IRadioButton;
    interface IRichEdit;
    interface IButton;
}


// 延迟重新加载对象属性
#define MESSAGE_DELAY_RELOAD_OBJECT_ATTR  (WM_USER + 1)

class LayoutPropertyHandler : public IPropertyHandler
{
public:
	enum 
	{
		PROPERTY_HANDLER_TYPE = 154162157,
	};
    LayoutPropertyHandler();
    ~LayoutPropertyHandler();

    UI_BEGIN_MSG_MAP_Ixxx(LayoutPropertyHandler)
		UIMSG_NOTIFY(UI_PROPERTYCTRL_NM_EDITITEM_ACCEPTCONTENT, OnEditItemAcceptContent)
        UIMSG_NOTIFY(UI_PROPERTYCTRL_NM_COMBOBOXITEM_ACCEPTCONTENT, OnComboBoxItemAcceptContent)
        UIMSG_HANDLER_EX(MESSAGE_DELAY_RELOAD_OBJECT_ATTR, OnDelayReloadObjectAttr)
    UI_END_MSG_MAP()

private:
	virtual long  GetHandlerType() { return PROPERTY_HANDLER_TYPE; }
    virtual void  OnLoad(CPropertyDialog* pWnd) override;
    virtual void  OnUnload() override;

public:
    void  AttachObject(ISkinRes*, IObject* pObj);
    void  AttachHandler();
	void  DetachHandler();

	void  ReloadObjectAttribute(IObject* pObj);
	void  UpdateObjectAttributeInCtrl(IObject* pObj, LPCTSTR szKey);

private:
    bool  OnCommandEditKeyDown(IEdit*, UINT);
    void  OnCommandEditTab();
    void  OnCommandEnChange(IEdit*, bool);
    LRESULT  OnEditItemAcceptContent(WPARAM wParam, LPARAM lParam);
    LRESULT  OnComboBoxItemAcceptContent(WPARAM wParam, LPARAM lParam);
    LRESULT  OnDelayReloadObjectAttr(UINT uMsg, WPARAM wParam, LPARAM lParam);
    void     OnRadioButtonAttr(UI::IButton*);

    void  OnAcceptContent(LPCTSTR szKey, LPCTSTR szValue);

    // 调用UI_GetObjectAttributeList之后的回调函数
    long  OnAddGroupProperty(IObject* pObj, LPCTSTR szGroupName);
    void  OnAddProperty(IObject* pObj, long nGroupID, LPCTSTR szName);

    long  RemoveAttribInCtrl(LPCTSTR szKey);
    long  AddAttributeInCtrl(LPCTSTR szKey, LPCTSTR szValue);
    long  ModifyAttributeInCtrl(LPCTSTR szKey, LPCTSTR szValue);
    long  ClearAttribInCtrl();

    void  FillObjectAttribute();
    void  SaveObjectPropertyEditStatus(IObject* pObj);
    void  LoadObjectPropertyEditStatus(IObject* pObj);

    void  AttachObject_IDAttrib(ISkinRes* hSkin, IObject* pObj, const String& strID);
    void  AttachObject_ClassAttrib(ISkinRes* hSkin, IObject* pObj, const String& strClassStyle);
    void  AttachObject_TagAttrib(ISkinRes* hSkin, IObject* pObj, const String& strTagName);

    bool  IsShowXmlAttr();
    bool  IsShowObjAttr();

    void  ClearData();
    void  ShowStatusText(LPCTSTR  szText);


#pragma region // Command
    long   DoCommand(LPCTSTR  szCommand);
    long   DoCommand(PROPERTY_COMMAND_TYPE, LPCTSTR szArg);

    long   OnCommand_Add(LPCTSTR szArg);  
    long   OnCommand_Delete(LPCTSTR szArg);
    long   OnCommand_Modify(LPCTSTR szArg);
    long   OnCommand_Clear(LPCTSTR szArg);
    long   OnCommand_Insert(LPCTSTR szArg);
    long   OnCommand_InsertBefore(LPCTSTR szArg);

    PROPERTY_COMMAND_TYPE   GetCommandType(String&  strCommand);
#pragma endregion 


private:
    CPropertyDialog*  m_pPropertyDialog;
    IPropertyCtrl*   m_pPropertyCtrl;
    IEdit*           m_pCommandEdit;
    IRadioButton*    m_pRadioBtnXmlAttr;
    IRadioButton*    m_pRadioBtnObjAttr;
    IRadioButton*    m_pRadioBtnStyleAttr;
    enum ATTR_TYPE
    {
        XmlAttr,
        ObjAttr,
        StyleAttr
    };
    ATTR_TYPE  m_eCurAttrType;

    vector<LPCTSTR>  m_vSearchStrings;
    int  m_nSearchStringsIndex;
    String    m_strCurSearchPrefix;  // 在Edit中输入Tab时用于判断的属性前缀

    ISkinRes*  m_pSkinRes;
    IObject*   m_pObject;
    IUIElement*  m_pXmlElement;
};