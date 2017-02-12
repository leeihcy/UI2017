#pragma once
#include <Richedit.h>
#include <set>
#include "..\UISDK\Inc\Util\dragdrop.h"
#include "Inc\Interface\iricheditole.h"

//////////////////////////////////////////////////////////////////////////
//
//                      用于richedit中插入的ole对象
//
//////////////////////////////////////////////////////////////////////////


//  Q1. 调用InsertObject时，一定要传递一个Storage指针吗？
//  A1. 如果传递一个NULL，则在复制该对象后，将无法粘贴。内部的原因也还没有搞清楚
//      ILockBytes，它是一个Storage后存储介质之间的桥梁，StgCreateDocfileOnILockBytes
//      代表在内存中创建一个存储对象。相应的StgCreateDocFile表示使用文件创建一个
//      存储对象
//
//   Q2. IDataObject怎么被其它进程访问的？
//   A2. 从目前查到的资料中显示，应该是Marshal的结果
//
//   Q3. 进程退出后，为什么还能进行拷贝粘贴?
//   A3. 其实这全靠OleFlushClipboard。它会将当前剪贴板中的IDataObject进行复制，创建一个
//       临时的IDataObject，并释放之前的IDataObject对象。因此在程序退出前应该调用一下
//       这个函数
//
//   Q4. 如果判断一个OLE是否可见（是否需要进制绘制）
//   A4. 土办法：获取ole位置，获取re的可视范围位置，判断ole是否在这个范围内
//       技巧：每次re绘制前，将ole设置为不可见。如果ole响应的iviewobject::draw即表明这个
//       ole是可见的
//
//
//
// 更多的实现细节可以参考atl的源代码：CComControlBase  IOleObjectImpl
//
//
interface IRichEditOle;
namespace UI
{
class WindowlessRichEdit;
class REOleManager;
interface IREOleExternalData;
interface IRichEdit;


class REOleBase : public IUnknown
{
public:
	REOleBase();
	virtual ~REOleBase();

	void  SetInterface(IREOleBase*);
	IREOleBase* GetIREOleBase();

    // 获取OLE对象的IOleObject接口
	virtual HRESULT  GetOleObject(IOleObject** ppOleObject, bool bAddRef=true) = 0;	

    // 获取ole对象的剪贴板数据
	virtual HRESULT  GetClipboardData(UINT nClipFormat, __out BSTR* pbstrData) = 0;
    // 单选这一个OLE时，获取更丰富的剪贴板数据
    virtual HRESULT  GetClipboardDataEx(IDataObject* pDataObject) = 0;

    virtual REOLETYPE  GetOleType() = 0;                  // 获取自己的类型
	virtual void  OnDraw(HDC hDC, RECT* prc){};      // 绘制ole
	virtual void  GetSize(SIZE* pSize){};            // 获取ole大小
    virtual void  OnSizeChanged(int cx, int cy) {};  // ole绘制大小发生改变

    // 被插入RE
    virtual void  OnPreInsert2RE(){};
    virtual void  OnInsert2RE(){};

    // ole是否需要鼠标/键盘消息
    virtual bool  NeedWindowMessage();
    virtual LRESULT  ProcessMessage(UINT msg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

public:
	// 对外接口
//     void  SetExternalData(IREOleExternalData* p);
//     IREOleExternalData*  GetExternalData();

	void  AddBizProp(LPCTSTR key, LPCTSTR value);
	void  ClearBizProp();
	LPCTSTR  GetBizProp(LPCTSTR key);

public:
    WindowlessRichEdit*  GetWindowlessRE();
	void  SetOleObjectManager(REOleManager* pMgr);

    long  AddRefImpl();
    long  ReleaseImpl();
	bool  IsVisibleInField();
    void  ClearVisibleInFieldFlag();
    void  GetDrawRect(RECT* prc);
	void  OnInternalDraw(HDC, RECT*);

	IRichEdit*  GetRichEdit();

protected:
    long   m_dwRef;
	REOleManager*  m_pOleObjectMgr;
	CRect  m_rcDrawSave;       // 保存上一次的绘制区域，控件左上角为(0,0)
	bool   m_bVisibleInField;  // 是否在可视范围内（是否需要绘制）
	
//   IREOleExternalData*  m_pExternalData;
	IREOleBase*  m_pIREOleBase;

	// 扩展数据，用于实现复制粘贴。使用IREOleExternalData*无法支持复制。
	// 在复制的时候，该map里的数据也会复制到新的ole对象当中。
	map<String, String>  m_mapBizProps;
};

typedef list<REOleBase*>   OLELIST;
typedef OLELIST::iterator  OLELISTITER;
typedef OLELIST::reverse_iterator  OLELISTRITER;
typedef set<IDataObject*>  DATAOBJECTSET;

// 管理richedit中的 ole对象对应的结构体列表
class REOleManager : public IDataObjectSource
{
public:
	REOleManager(WindowlessRichEdit* pRichEdit);
	~REOleManager();

	void   SetUIApplication(IUIApplication* p);
	bool   AddOle(REOleBase* pItem);
    void   OnOleDelete(REOleBase* pItem);
//  REOleBase*  FindOle(long lType, LPCTSTR  szId);

	void   CreateDataObject(IDataObject** p);
    WindowlessRichEdit*  GetRE();
    IRichEditOle*  GetREOle();
    REOleBase*  HitTest(POINT ptInControl);
	void  OnPreDraw(HDC hDC, RECT* prcDraw);

    LRESULT  OnWindowMessage(UINT msg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

protected:
    LRESULT  OnMouseMove(WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    LRESULT  OnMouseLeave(WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    LRESULT  OnLButtonDown(WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    LRESULT  OnLButtonDBClick(WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    LRESULT  OnLButtonUp(WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    LRESULT  OnSetCursor(WPARAM wParam, LPARAM lParam, BOOL& bHandled); 
    LRESULT  OnSize(WPARAM wParam, LPARAM lParam, BOOL& bHandled); 
    void  WindowPoint2ObjectClientPoint(POINT* ptWnd, POINT* ptClient);

protected:
	virtual BOOL OnRenderData(
		IDataObject* pDataObject,
		LPFORMATETC lpFormatEtc,
		LPSTGMEDIUM lpStgMedium 
		) override;

	virtual void  OnDataRelease(
		IDataObject* p
		) override;

protected:
	WindowlessRichEdit*   m_pRichEdit;
    IUIApplication*       m_pUIApp;
    bool                  m_bDestroying;     // 标记自己正在销毁

	OLELIST               m_listOleObj;
	DATAOBJECTSET         m_setDataObject;   // 用于记录复制的ole data指针。在RE销毁时释放，并提交到剪切板.

    REOleBase*   m_pHoverOle;
    REOleBase*   m_pPressOle;
};


}