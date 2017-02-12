#pragma once
#include "Other\resource.h"

class LayoutTreeItemData;
namespace UI
{
    interface IWindow;
	interface IMenu;
}

interface IMDIClientView
{
public:
    IMDIClientView() { /*m_dwRef = 0;*/ }
    virtual ~IMDIClientView() = 0 {};


    virtual void ** QueryInterface(REFIID riid)
    {
        if (::IsEqualIID(IID_IMDIClietnView, riid))
        {
            return (void **)static_cast<IMDIClientView*>(this);
        }
        return NULL;
    }

    virtual ISkinRes*  GetSkinRes() = 0;
    virtual void   SetSkinRes(ISkinRes* hSkin) = 0;
    virtual long*  GetKey() = 0;
    virtual void   OnMDIActivate(bool bActivate)
    {
    }

    virtual UI_RESOURCE_TYPE GetResourceType() = 0;
    virtual HWND   GetHWND() = 0;
    virtual void   Destroy()
    {
        delete this;
    }
};

interface IExplorerMDIClientView : public IMDIClientView
{
public:
    virtual bool   GetFilePath(__out TCHAR* szPath) = 0;
    virtual bool   SetFilePath(__in  LPCTSTR szPath) = 0;
};

interface ILogMDIClientView : public IMDIClientView
{
public:
    virtual bool AttachLogItem(__in LPCTSTR szLogItem) = 0;
};

interface ILayoutMDIClientView : public IMDIClientView
{
public:
    virtual bool ShowLayoutNode(__inout LayoutTreeItemData* pData) = 0;
    virtual IWindow*  GetWindow() = 0;
    virtual void  SelectObject(__in IObject*) = 0;
    virtual void  OnObjectDeleteInd(IObject* p) = 0;
};

interface IMenuMDIClientView : public IMDIClientView
{
public:
    virtual bool ShowLayoutNode(__inout LayoutTreeItemData* pData) = 0;
    virtual IMenu*  GetMenu() = 0;
};

class CChildFrame : public CMDIChildWindowImpl<CChildFrame>
{
public:
    DECLARE_FRAME_WND_CLASS(NULL, IDR_IMAGEBUILD)
    CChildFrame(CMainFrame* pMainFrame);

    virtual void OnFinalMessage(HWND);

    BEGIN_MSG_MAP_EX(CChildFrame)
        MSG_WM_CREATE(OnCreate)
        MSG_WM_DESTROY(OnDestroy)
        //MSG_WM_ERASEBKGND(OnEraseBkgnd)
        MESSAGE_HANDLER(WM_FORWARDMSG, OnForwardMsg)
        MESSAGE_HANDLER_EX(WM_MDIACTIVATE, OnMDIActivate)
        CHAIN_MSG_MAP(CMDIChildWindowImpl<CChildFrame>)
     END_MSG_MAP()

public:

    int   OnCreate(LPCREATESTRUCT lpCreateStruct);
    void  OnDestroy();
    //BOOL  OnEraseBkgnd(CDCHandle dc) { return TRUE; }

    LRESULT OnForwardMsg(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/);
    LRESULT OnMDIActivate(UINT uMsg, WPARAM wParam, LPARAM lParam);

    void  SetClientView(IMDIClientView* pClientWnd);

private:
    void  UpdateTitle();

public:
    CMainFrame*       m_pMainFrame;
    IMDIClientView*    m_pClientView; // 光一个m_hClientWnd去获取信息太费劲，添加一个接口用于外部获取子窗口的信息
};

