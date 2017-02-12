#pragma once

bool  UE_AddSkin(LPCTSTR szDir, LPCTSTR szSkinName);

// bool  UE_AddObjectAttribute(   ISkinRes* pSkinRes, IObject* pObject, IUIElement* pUIElement, LPCTSTR szKey, LPCTSTR szValue);
// bool  UE_RemoveObjectAttribute(ISkinRes* pSkinRes, IObject* pObject, IUIElement* pUIElement, LPCTSTR szAttrib);
// bool  UE_ClearObjectAttribute( ISkinRes* pSkinRes, IObject* pObject, IUIElement* pUIElement);
// bool  UE_ModifyObjectAttribute(ISkinRes* pSkinRes, IObject* pObject, IUIElement* pUIElement, LPCTSTR szKey, LPCTSTR szValue);



HRESULT  UE_ExecuteCommand(ICommand* p, bool bDelay = false);
void  UE_RefreshUIWindow(IObject* pObject);
bool  UE_RefreshLayoutView(IObject* pObject);
void  UE_UpdateAttributeInPropertyCtrl(IObject* pObject, LPCTSTR szKey);
bool  UE_ReloadObjectAttribute(IObject* pObject);
