#pragma once
#include "../Base/icommand.h"
#include "Business\Project\ProjectData\projectdata.h"
#include "UISDK\Inc\Interface\iuires.h"

// 添加图片项
// wParam:  IImageResItem*
#define WM_IMAGE_EDITOR_ADD     (WM_USER+100)   
// 删除图片项
// wParam:  IImageResItem*
#define WM_IMAGE_EDITOR_DELETE  (WM_USER+101)

class ImageItemInfo
{
public:
    ImageItemInfo()
    {
        bImageList = false;
        bLayoutH = true;
        nImageListItemCount = 1;
        bHls = false;
        bAntialiasing = false;
    }

	String  strId;
	String  strPath;
	bool    bImageList;
	bool    bLayoutH;
	int     nImageListItemCount;
	bool    bHls;
	bool    bAntialiasing;
};

class ImageItemOpCommand : public CommandBase
{
public:
    ImageItemOpCommand()
    {
        m_pSkin = NULL;
    }
    ~ImageItemOpCommand()
    {
    }

    // 获取IImageResItem数据信息
    void  GetItemInfo(__in IImageResItem* pImageResItem, __out ImageItemInfo* pInfo)
    {
        if (!pImageResItem || !pInfo)
            return;

        pInfo->strId = pImageResItem->GetId();
        pInfo->strPath = pImageResItem->GetPath();
        pInfo->bAntialiasing = pImageResItem->GetNeedAntiAliasing();
        pInfo->bHls = pImageResItem->GetUseSkinHLS();
    }

    bool  Add()
    {
        IImageManager&  rImageManager = m_pSkin->GetImageManager();
        IUIElement* pImageXmlElem = rImageManager.GetImageXmlElem(NULL);

        if (!pImageXmlElem)
            return false;

        // 添加到xml
        IUIElementProxy pUIElement;
        if (m_imageiteminfo.bImageList)
        {
            pUIElement = pImageXmlElem->AddChild(XML_IMAGE_ITEM_TYPE_IMAGELIST);
        }
        else
        {
            pUIElement = pImageXmlElem->AddChild(XML_IMAGE_ITEM_TYPE_IMAGE);
        }

        // 添加到内存
        IImageResItem*  pImageResItem = NULL;
        if (m_imageiteminfo.bImageList)
        {
            pImageResItem = rImageManager.InsertImageItem(
                IMAGE_ITEM_TYPE_IMAGE_LIST,
                m_imageiteminfo.strId.c_str(), 
                m_imageiteminfo.strPath.c_str());
        }
        else
        {
            pImageResItem = rImageManager.InsertImageItem(
                IMAGE_ITEM_TYPE_IMAGE,
                m_imageiteminfo.strId.c_str(), 
                m_imageiteminfo.strPath.c_str());
        }
        if (!pImageResItem)
        {
            String  str = _T("添加图片失败，Id=");
            str.append(m_imageiteminfo.strId);
            str.append(_T("  path="));
            str.append(m_imageiteminfo.strPath);
            g_pGlobalData->m_pStatusBar->SetStatusText2(str.c_str());
            return false;
        }

        UpdateAttribute(pUIElement.get(), pImageResItem, &m_imageiteminfo);

        // 同步到属性控件
		g_pGlobalData->m_pProjectData->m_uieditor.OnImageItemLoad(pImageResItem, pUIElement.get());

		HWND hWnd = g_pGlobalData->m_pMainFrame->GetImageWnd(m_pSkin);
		if (hWnd)
		{
			SendMessage(hWnd, WM_IMAGE_EDITOR_ADD, (WPARAM)pImageResItem, 0);
		}
        SetDirty(pUIElement.get());
        return true;
    }
  
    void UpdateAttribute(IUIElement* pUIElement, IImageResItem* pImageResItem, ImageItemInfo* pItemInfo)
    {
        if (!pUIElement || !pItemInfo || !pImageResItem)
            return;

        // 内存数据
        pImageResItem->SetId(pItemInfo->strId.c_str());
        pImageResItem->SetPath(pItemInfo->strPath.c_str());
        pImageResItem->SetNeedAntiAliasing(pItemInfo->bAntialiasing);
        pImageResItem->SetUseSkinHLS(pItemInfo->bHls);
        if (pItemInfo->bImageList)
        {
            IImageListResItem* pImageListResItem = static_cast<IImageListResItem*>(pImageResItem);
            pImageListResItem->SetItemCount(pItemInfo->nImageListItemCount);
            if (pItemInfo->bLayoutH)
                pImageListResItem->SetLayoutType(IMAGELIST_LAYOUT_TYPE_H);
            else
                pImageListResItem->SetLayoutType(IMAGELIST_LAYOUT_TYPE_V);
        }

        // xml数据
        pUIElement->AddAttrib(XML_ID, pItemInfo->strId.c_str());
        pUIElement->AddAttrib(XML_PATH, pItemInfo->strPath.c_str());
        
        if (pItemInfo->bAntialiasing)
            pUIElement->AddAttrib(XML_IMAGE_NEED_ANTIALIASING, XML_BOOL_VALUE_1);
        else
            pUIElement->RemoveAttrib(XML_IMAGE_NEED_ANTIALIASING);

        if (pItemInfo->bHls)
            pUIElement->AddAttrib(XML_IMAGE_THEMEHLS, XML_BOOL_VALUE_1);
        else
            pUIElement->RemoveAttrib(XML_IMAGE_THEMEHLS);

        if (pItemInfo->bImageList)
        {
            if (!pItemInfo->bLayoutH)
                pUIElement->AddAttrib(XML_IMAGE_IMAGELIST_LAYOUT, XML_IMAGE_IMAGELIST_LAYOUT_V);
            else
                pUIElement->RemoveAttrib(XML_IMAGE_IMAGELIST_LAYOUT);

            TCHAR szCount[16] = {0};
            _stprintf(szCount, _T("%d"), pItemInfo->nImageListItemCount);
            pUIElement->AddAttrib(XML_IMAGE_IMAGELIST_COUNT, szCount);
        }
        else
        {
            pUIElement->RemoveAttrib(XML_IMAGE_IMAGELIST_LAYOUT);
            pUIElement->RemoveAttrib(XML_IMAGE_IMAGELIST_COUNT);
        }
    }

    bool Delete()
    {
        IUIElement* pUIElement = NULL;
        bool bOk = false;
        do 
        {
		    if (!m_pSkin || m_imageiteminfo.strId.empty())
			    break;

		    IImageManager&  rImageManager = m_pSkin->GetImageManager();
            IUIElement* pImageXmlElem = rImageManager.GetImageXmlElem(NULL);
		    if (!pImageXmlElem)
			    break;

            IImageResItem* pImageResItem = rImageManager.GetImageRes().
                GetImageResItem(m_imageiteminfo.strId.c_str());
            if (!pImageResItem)
                break;
    	
            pUIElement = GetUIEditor()->GetImageResItemXmlElem(pImageResItem);
            if (!pUIElement)
                break;
            pUIElement->AddRef();

		    // 同步到控件
		    HWND hWnd = g_pGlobalData->m_pMainFrame->GetImageWnd(m_pSkin);
		    if (hWnd)
		    {
			    SendMessage(hWnd, WM_IMAGE_EDITOR_DELETE, (WPARAM)pImageResItem, 0);
		    }
		    g_pGlobalData->m_pProjectData->m_uieditor.OnImageItemDeleteInd(pImageResItem);

		    // 从内存中删除
		    if (!rImageManager.RemoveImageItem(m_imageiteminfo.strId.c_str()))
                break;

		    // 从xml中删除
            SetDirty(pUIElement);
		    pImageXmlElem->RemoveChild(pUIElement);
            
            bOk = true;
        }
        while (0);

        SAFE_RELEASE(pUIElement);
        if (!bOk)
        {
            String  str = _T("删除图片失败: ");
            str.append(m_imageiteminfo.strId.c_str());
            str.append(_T(" = \""));
            str.append(m_imageiteminfo.strPath.c_str());
            str.append(_T("\""));
            g_pGlobalData->m_pStatusBar->SetStatusText2(str.c_str());
            return false;
        }

		return true;
    }

	long*  GetKey(void)
	{
		UIASSERT(m_pSkin);
		IImageRes&  rImgRes = m_pSkin->GetImageRes();
        return (long*)&rImgRes;
	}

public:
    ISkinRes*     m_pSkin;

    // 不能用指针。因为这一步的操作，在下一步可能被删除，导致这些指针直接失效了。
    // 还得用Id去查找
//     IImageResItem*  m_pImageResItem;
//     IUIElement*   m_pUIElement;
  
    ImageItemInfo  m_imageiteminfo;
};

// 添加图片命令
class AddImageItemCommand : public ImageItemOpCommand
{
public:
    static ICommand*  CreateInstance(ISkinRes* pSkinRes, ImageItemInfo* pInfo)
    {
        if (NULL == pSkinRes || NULL == pInfo)
            return NULL;

        if (pSkinRes->GetImageRes().GetImageResItem(pInfo->strId.c_str()))
        {
            String  str = _T("Id重复: ");
            str.append(pInfo->strId.c_str());
            g_pGlobalData->m_pStatusBar->SetStatusText2(str.c_str());
            return NULL;
        }

        AddImageItemCommand* p = new AddImageItemCommand;
        p->AddRef();

        p->m_pSkin = pSkinRes;
        p->m_imageiteminfo = *pInfo;
        return p;
    }
    virtual long execute()
    {
        Add();
		return 0;
    }
    virtual long undo()
    {
        Delete();
		return 0;
    }
};


// 删除图片命令
class DeleteImageItemCommand : public ImageItemOpCommand
{
public:
    static ICommand*  CreateInstance(ISkinRes* pSkinRes, IImageResItem* pItem)
    {
        if (NULL == pSkinRes || NULL == pItem)
            return NULL;

        DeleteImageItemCommand* p = new DeleteImageItemCommand;
        p->AddRef();

        p->m_pSkin = pSkinRes;
        p->GetItemInfo(pItem, &p->m_imageiteminfo);
        return p;
    }
    virtual long execute()
    {
        Delete();
        return 0;
    }
    virtual long undo()
    {
        Add();
        return 0;
    }
};

// 修改图片命令
class ModifyImageItemCommand : public ImageItemOpCommand
{
public:
    static ICommand*  CreateInstance(ISkinRes* pSkinRes, IImageResItem* pItem, ImageItemInfo* pModifyInfo)
    {
        if (NULL == pSkinRes || NULL == pItem || NULL == pModifyInfo)
            return NULL;

        ModifyImageItemCommand* p = new ModifyImageItemCommand;
        p->AddRef();

        p->m_pSkin = pSkinRes;

        p->m_imageiteminfo = *pModifyInfo;
        p->GetItemInfo(pItem, &p->m_OriginImageItemInfo);
        return p;
    }
    virtual long execute()
    {
        bool bOk = false;
        do 
        {
            IImageManager&  rImageManager = m_pSkin->GetImageManager();
            IUIElement* pImageXmlElem = rImageManager.GetImageXmlElem(NULL);
            if (!pImageXmlElem)
                break;

            IImageResItem* pImageResItem = rImageManager.GetImageRes().
                GetImageResItem(m_imageiteminfo.strId.c_str());
            if (!pImageResItem)
                break;

            IUIElement* pUIElement = GetUIEditor()->GetImageResItemXmlElem(pImageResItem);
            if (!pUIElement)
                break;

            UpdateAttribute(pUIElement, pImageResItem, &m_imageiteminfo);
            SetDirty(pUIElement);

            bOk = true;
        } 
        while (0);

        if (!bOk)
        {
            return -1;
        }
        return 0;
    }
    virtual long undo()
    {
        bool bOk = false;
        do 
        {
            IImageManager&  rImageManager = m_pSkin->GetImageManager();
            IUIElement* pImageXmlElem = rImageManager.GetImageXmlElem(NULL);
            if (!pImageXmlElem)
                break;

            IImageResItem* pImageResItem = rImageManager.GetImageRes().
                GetImageResItem(m_imageiteminfo.strId.c_str());
            if (!pImageResItem)
                break;

            IUIElement* pUIElement = GetUIEditor()->GetImageResItemXmlElem(pImageResItem);
            if (!pUIElement)
                break;

            UpdateAttribute(pUIElement, pImageResItem, &m_OriginImageItemInfo);
            SetDirty(pUIElement);

            bOk = true;
        } 
        while (0);

        if (!bOk)
        {
            return -1;
        }
        return 0;
    }

public:
    ImageItemInfo   m_OriginImageItemInfo;
};
