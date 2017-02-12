#pragma once
#include "Business/Project/UIEditor/IUIEditorImpl.h"
#include "Business/Project/UIProjParse/uiprojparse.h"


// 封装一个工程。这样就能方便的实现加载/卸载工程
class CProjectData
{
public:
    CProjectData();
    ~CProjectData();

    bool  OpenUIProj(LPCTSTR szPath);
    bool  ReRegistUIObject(bool bResotre);  

    LPCTSTR  GetProjName();
	void  AddSkinResPath(LPCTSTR szPath);
// 	uint  GetSkinResPathCount();
// 	LPCTSTR  GetSkinResPathByIndex(uint);

protected:
    bool  InitEditUIApplication();

public:
    String  m_strName;   // 从m_strPath中获取文件名，得到工程名
//	vector<String>    m_vSkinResPath;  // 皮肤资源包

    CUIEditor         m_uieditor;
    IUIApplication*   m_pEditUIApp;  // 正在编辑的皮肤
    ISkinManager*     m_pEditSkinMgr;
    CUIProjParse      m_uiprojParse;
    CDependsCtrlDllMgr  m_mgrDependsCtrlDll;
};