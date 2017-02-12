#pragma once
#include "UISDK\Inc\Interface\iwindow.h"

namespace UI
{
    interface IButton;
    interface IComboBox;
}
class CToolBar : public IWindow
{
public:
    CToolBar();
    ~CToolBar();

    UI_BEGIN_MSG_MAP_Ixxx(CToolBar)
        UIMSG_ERASEBKGND(OnEraseBkgnd)
        UIMSG_INITIALIZE(OnInitialize)
    UI_END_MSG_MAP_CHAIN_PARENT(IWindow)

public:
    bool  TestCommandIsValid(long ld);
    void  OnDirtyChanged(bool bDirty);
    void  OnProjectOpened();
    void  OnProjectClosed();
    void  OnUndoRedoCountChanged(long* pKey);
	void  UpdateBtnState();

    long*  GetCurViewKey();

protected:
    void  OnInitialize();
    void  OnEraseBkgnd(IRenderTarget* pRenderTarget);

    void  OnBtnSave();
    void  OnBtnNewWindow();
    void  OnBtnUndo();
    void  OnBtnRedo();
    void  OnBtnPreview();

    void  update_language();

private:
    IButton*  m_pBtnSave;
    IButton*  m_pBtnNewWindow;
    IButton*  m_pBtnUndo;
    IButton*  m_pBtnRedo;

    IButton*  m_pBtnPreview;
    UI::IComboBox*  m_pLangCombo;
};

