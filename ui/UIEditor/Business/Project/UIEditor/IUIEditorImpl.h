#pragma once
#include "UISDK\Inc\Interface\iuieditor.h"
#include "UISDK\Inc\Interface\iattribute.h"
#include "UISDK\Inc\Interface\ilistitembase.h"

class CRootAttribute;

namespace UI
{
	interface IPropertyCtrl;
    interface IPropertyCtrlGroupItem;
}

// 对象属性在PropertyCtrl中的编辑状态。
// 用于切换到其它对象，再切回来时继续展现当前位置，当前折叠
class ObjectPropertyEditStatus
{
public:
    ObjectPropertyEditStatus();

    void  Clear();
    void  AddCollapsedGroup(LPCTSTR szParentKey, LPCTSTR szGroupName);
    bool  IsGroupCollapsed(LPCTSTR szParentKey, LPCTSTR szGroupName);
    void  SetVScrollPos(long);
    void  SetSelectedItem(LPCTSTR szKey);
    LPCTSTR  GetSelectedItem();
    long  GetVScrollPos();

    struct GroupNodeId
    {
        String  strParentKey;
        String  strName;

		bool operator==(const GroupNodeId& right)
		{
			if (strName == right.strName && strParentKey == right.strParentKey)
				return true;

			return false;
		}
    };

private:
    long  m_lVScrollPos;
    String  m_strSelectedItem;

    list<GroupNodeId>  m_listGroupCollapsed;
};

// 每一个对象关联的UIEDITOR数据
class ObjectEditorData
{
public:
	ObjectEditorData();
	~ObjectEditorData();

    void  SetXmlElem(IUIElement* p);

public:
	IListItemBase*   pTreeItem;    // 在树控件中位置
	IUIElement*      pXmlElement;  // 在文档中的位置
    IAttributeEditorProxy*  pAttributeProxy;
    
    // 该对象在PropertyCtrl中的一些状态数据
    // 查看位置、折叠数据;
   ObjectPropertyEditStatus  m_editStatus;
};

class ImageResItemEditorData
{
public:
    ImageResItemEditorData();
    ~ImageResItemEditorData();

    void  SetXmlElem(IUIElement* p);
public:
    IUIElement*  pXmlElement;
};

interface StyleChangedCallback
{
	virtual ISkinRes*  GetSkinRes() PURE;
	virtual void  OnStyleAdd(IStyleResItem* p) PURE;
	virtual void  OnStyleRemove(IStyleResItem* p) PURE;
	virtual void  OnStyleModify(IStyleResItem* p) PURE;
	virtual void  OnStyleAttributeAdd(IStyleResItem* p, LPCTSTR szKey) PURE;
	virtual void  OnStyleAttributeRemove(IStyleResItem* p, LPCTSTR szKey) PURE;
	virtual void  OnStyleAttributeModify(IStyleResItem* p, LPCTSTR szKey) PURE;
};


class CUIEditor : public IUIEditor
{
public:
	CUIEditor(void);
	~CUIEditor(void);

    void  Destroy();  // 在App释放前销毁数据

	typedef map<IObject*, ObjectEditorData*> _MyObjectMap;
	typedef map<IObject*, ObjectEditorData*>::iterator _MyObjectMapIter;
    typedef map<IImageResItem*, ImageResItemEditorData*> _MyImageResItemMap;
    typedef map<IImageResItem*, ImageResItemEditorData*>::iterator _MyImageResItemMapIter;
            
#pragma region

    virtual bool  AddObjectRes(EditorAddObjectResData* pData);

	virtual void  OnObjectAttributeLoad(__in IObject*  pObj, __in IUIElement* pXmlElem) override;
    virtual void  OnObjectDeleteInd(__in IObject* pObj) override;

    virtual void  OnImageItemLoad(__in IImageResItem*  pItem, __in IUIElement* pXmlElem);
    virtual void  OnGifItemLoad(__in IGifResItem*  pItem, __in IUIElement* pXmlElem);
    virtual void  OnCursorItemLoad(__in ICursorResItem*  pItem, __in IUIElement* pXmlElem);
    virtual void  OnImageItemDeleteInd(__in IImageResItem*  pItem);
    virtual void  OnGifItemDeleteInd(__in IGifResItem*  pItem);
    virtual void  OnCursorItemDeleteInd(__in ICursorResItem*  pItem);

	virtual void  OnStyleChanged(ISkinRes* pSkinRes, IStyleResItem* p, EditorOPType e) override;
	virtual void  OnStyleAttributeChanged(ISkinRes* pSkinRes, IStyleResItem* p, LPCTSTR szKey, EditorOPType e) override;

    virtual void  EditorStringAttribute(IStringAttribute*, EditorAttributeFlag e) override;
    virtual void  EditorBoolAttribute(IBoolAttribute*, EditorAttributeFlag e) override;
    virtual void  EditorLongAttribute(ILongAttribute*, EditorAttributeFlag e) override;
	virtual void  EditorEnumAttribute(IEnumAttribute*, EditorAttributeFlag e) override;
    virtual void  EditorFlagsAttribute(IFlagsAttribute*, EditorAttributeFlag e) override;
    virtual void  EditorRectAttribute(IRectAttribute*, EditorAttributeFlag e) override;
	virtual void  EditorSizeAttribute(ISizeAttribute*, EditorAttributeFlag e) override;
	virtual void  EditorRegion9Attribute(IRegion9Attribute*, EditorAttributeFlag e) override;
    virtual void  EditorStringEnumAttribute(IStringEnumAttribute*, EditorAttributeFlag e) override;
	virtual void  EditorColorAttribute(IColorAttribute*, EditorAttributeFlag e) override;
    virtual void  EditorRenderBase(IRenderBaseAttribute* pAttribute, EditorAttributeFlag e) override;
    virtual void  EditorTextRenderBase(ITextRenderBaseAttribute* pAttribute, EditorAttributeFlag e) override;

    virtual void  OnToolBox_AddObject(IObjectDescription*);

#pragma endregion
	void  LoadAttribute2PropertyCtrl(IObject*);
    void  SaveAttribute2Xml(IObject*);
    void  DestroyObjectAttributeProxy(IObject*);

public:
	ObjectEditorData*  GetObjectEditorData(IObject* pObj);
    ImageResItemEditorData*  GetImageResItemEditorData(IImageResItem* pImageResItem);
	
	void  SetObjectHTREEITEM(IObject* pObject, IListItemBase* pTreeItem);
	IListItemBase*  GetObjectHTREEITEM(IObject*);
	IUIElement*  GetObjectXmlElem(IObject*);
    IUIElement*  GetImageResItemXmlElem(IImageResItem*);

	void  RegisterStyleChangedCallback(StyleChangedCallback*);
	void  UnRegisterStyleChangedCallback(StyleChangedCallback*);

protected:
	map<IObject*, ObjectEditorData*>  m_mapObjectData;
    map<IImageResItem*, ImageResItemEditorData*>  m_mapImageResItemUserData;
	list<StyleChangedCallback*>  m_listStyleItemEditorCallback;
};
