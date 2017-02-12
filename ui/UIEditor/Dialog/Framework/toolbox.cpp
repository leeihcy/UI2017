#include "stdafx.h"
#include "toolbox.h"
#include "UISDK\Inc\Interface\iimagerender.h"
#include "UIEditorCtrl\Inc\itoolboxtreeview.h"

CToolBox::CToolBox() : IWindow(CREATE_IMPL_TRUE)
{
    m_pTreeView = NULL;
}
CToolBox::~CToolBox()
{

}

void  CToolBox::OnInitialize()
{
    m_pTreeView = (ITreeView*)FindObject(_T("tree"));
    if (m_pTreeView)
    {
        GetEditUIApplication()->LoadUIObjectListToToolBox();
        // m_pTreeView->ModifyStyleEx(TREE_STYLE_HASLINE, 0, 0);
    }
}

void  CToolBox::OnClose()
{
    HideWindow();
}
void  CToolBox::InsertObject(IObjectDescription* pDesc)
{
    if (!m_pTreeView)
        return;

    OBJ_TYPE nMajorType = pDesc->GetMajorType();
    long  nMinorType = pDesc->GetMinorType();
    LPCTSTR szName = pDesc->GetTagName();
    LPCTSTR szCategory = pDesc->GetCategory();

    if (nMajorType == OBJ_WINDOW)
        return;

    if (NULL == szCategory)
        szCategory = _T("Default");

    IListItemBase* pParent = NULL;

    Util::ISplitStringEnum* pEnum = NULL;
    int nSize = Util::SplitString(szCategory, XML_PATH_SEPARATOR, &pEnum);
    for (int i = 0; i < nSize; i++)
    {
        IListItemBase* pFind = m_pTreeView->FindChildItemByText(pEnum->GetText(i), pParent, NULL);
        if (pFind)
            pParent = pFind;
        else
            pParent = m_pTreeView->InsertNormalItem(pEnum->GetText(i), pParent?pParent:UITVI_ROOT);
    }
    SAFE_RELEASE(pEnum);

    IToolBoxTreeItem* pTreeItem = IToolBoxTreeItem::CreateInstance(
        g_pGlobalData->m_pMyUIApp->GetDefaultSkinRes());

    m_pTreeView->InsertItem(pTreeItem, pParent, UITVI_LAST);
    pTreeItem->SetText(szName);
	pTreeItem->BindObject(pDesc);

    // ÉèÖÃÍ¼Æ¬
    IRenderBitmap*  pIcon = NULL;
    GetObjectTypeBitmap(nMajorType, nMinorType, &pIcon);
    if (pIcon)
    {
        IImageRender* pImageRender = IImageRender::CreateInstance(
            g_pGlobalData->m_pMyUIApp->GetDefaultSkinRes());

        pImageRender->SetImageDrawType(DRAW_BITMAP_CENTER);
        pImageRender->SetRenderBitmap(pIcon);
        SAFE_RELEASE(pIcon);
        pTreeItem->SetIconRender(pImageRender);
    }
}
