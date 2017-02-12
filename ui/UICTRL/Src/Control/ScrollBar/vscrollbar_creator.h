#pragma once


namespace UI
{
interface ISystemVScrollBar;
interface IScrollBarManager;

// interface IScrollbarCreator
// {
// 	virtual void  CreateByEditor(CREATEBYEDITORDATA* pData, IObject* pParent) PURE;
// 	virtual void  Initialize(IObject* pParent, IScrollBarManager* pMgr) PURE;
// };

class VScrollbarCreator// : public IScrollbarCreator
{
public:
	VScrollbarCreator();
	~VScrollbarCreator();

	virtual void  CreateByEditor(CREATEBYEDITORDATA* pData, IObject* pParent) /*override*/;
	virtual void  Initialize(IObject* pParent, IScrollBarManager* pMgr) /*override*/;
	
public:
	ISystemVScrollBar*  m_pVScrollBar;
};

}