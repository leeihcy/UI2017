#pragma once

extern "C"
{
    UIAPI bool UIEditor_Ctrl_RegisterUIObject(UI::IUIApplication* p);

    UIAPI UINT  GetToolBoxItemDragClipboardFormat();
    UIAPI UINT  GetProjectTreeControlDragClipboardFormat();
}


// SkinItem×ÖÌåÇ°×º
#define XML_PROJECT_TREEVIEW_SKINITEM_TEXT_PREFIX  _T("skinitem.")


// {708EF2DE-BB6F-4669-9F66-E9A5B47E303B}
static const GUID IID_UI_IProjectTreeView = 
{ 0x708ef2de, 0xbb6f, 0x4669, { 0x9f, 0x66, 0xe9, 0xa5, 0xb4, 0x7e, 0x30, 0x3b } };
// {D716833E-4BF4-49ff-8C65-852B6C927172}
static const GUID CLSID_UI_ProjectTreeView = 
{ 0xd716833e, 0x4bf4, 0x49ff, { 0x8c, 0x65, 0x85, 0x2b, 0x6c, 0x92, 0x71, 0x72 } };

// {0E71ED13-5C21-48d4-898E-4C1C288839F8}
static const GUID IID_UI_IPropertyCtrl = 
{ 0xe71ed13, 0x5c21, 0x48d4, { 0x89, 0x8e, 0x4c, 0x1c, 0x28, 0x88, 0x39, 0xf8 } };
