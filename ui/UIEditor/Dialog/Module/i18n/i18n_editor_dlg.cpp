#include "StdAfx.h"
#include "i18n_editor_dlg.h"
#include "UISDK\Inc\Interface\iskindatasource.h"
#include "UICTRL\Inc\Interface\ibutton.h"
#include "UISDK\Inc\Interface\ihwndhost.h"
#include "Business\uieditorfunction.h"
#include "UICTRL\Inc\Interface\ilistbox.h"
#include "UICTRL\Inc\Interface\ilabel.h"


CI18nEditorDlg::CI18nEditorDlg() : IWindow(CREATE_IMPL_TRUE)
{
	m_pSkin = NULL;
	m_pListView = NULL;
}
CI18nEditorDlg::~CI18nEditorDlg()
{
}

void CI18nEditorDlg::OnInitDialog()
{
	m_pListView = (IListBox*)FindObject(_T("listview"));
    if (m_pListView)
    {
//         m_pListView->DBClickEvent().connect(this,
//             &CI18nEditorDlg::OnItemDBClick);
// 
//         m_pListView->SelectChangedEvent().connect(this,
//             &CI18nEditorDlg::OnItemChanged);
    }

}

//
//	设置数据源
//
void CI18nEditorDlg::AttachSkin(ISkinRes* h)
{
	if (m_pSkin == h)
		return;

	// changed notify
	m_pSkin = h;

	Reload();
}

bool CI18nEditorDlg::Reload()
{
	if (!m_pSkin)
		return false;
	if (!m_pListView)
		return false;

	II18nRes& rI18nRes = m_pSkin->GetI18nRes();

	m_pListView->RemoveAllItem();

	// 获取该皮肤中的所有图片
// 	long nCount = pImageRes->GetImageCount();
// 	for (long i = 0; i < nCount; i++)
// 	{
// 		IImageResItem*  pInfo = pImageRes->GetImageResItem(i);
// 		if (pInfo)
// 		{
//             ICustomListItem* pItem = m_pListView->InsertCustomItem(
//                 TEXT("imageview.list"), i);
// 
//             UI::ILabel* pLabelId = (UI::ILabel*)pItem->FindControl(TEXT("id"));
//             UI::ILabel* pLabelPath = (UI::ILabel*)pItem->FindControl(TEXT("path"));
// 
//             pLabelId->SetText(pInfo->GetId());
//             pLabelPath->SetText(pInfo->GetPath());
// 			pItem->SetData((LPARAM)pInfo);
// 		}
// 	}

	// 将新增的图片显示出来
	m_pListView->SelectItem(m_pListView->GetFirstItem(), false);

	return true;
}

//
//	显示当前被选中的图片
//
// void  CI18nEditorDlg::OnItemChanged(IListCtrlBase*)
// {
//     ICustomListItem* pListViewItem = 
//         (ICustomListItem*)m_pListView->GetFirstSelectItem();
//     if (!pListViewItem)
//         return;
// 
//     UI::ILabel* pLabel = (UI::ILabel*)pListViewItem->FindControl(TEXT("path"));
//     m_pImageDisplayView->Attach(m_pSkin, pLabel->GetText());
// }
// 
// void  CI18nEditorDlg::OnItemDBClick(IListCtrlBase*, IListItemBase*)
// {
//     ICustomListItem* pListViewItem = (ICustomListItem*)
//         m_pListView->GetFirstSelectItem();
// 	if (!pListViewItem)
// 		return;
// 
// 	CAddImageDlg dlg(m_pSkin,true);
// 	dlg.SetImageInfo((IImageResItem*)pListViewItem->GetData());
// 	if (IDOK == dlg.DoModal())
// 	{
//         UI::ILabel* pLabel = (UI::ILabel*)
//             pListViewItem->FindControl(TEXT("path"));
// 
//         pLabel->SetText(dlg.m_strPath.c_str());
// 
// // 		m_pImageDisplayView->Invalidate(TRUE);
// // 		m_pImageDisplayView->UpdateWindow();
// 	}
// }

long*  CI18nEditorDlg::GetKey()
{
	return (long*)&m_pSkin->GetI18nRes();
}
