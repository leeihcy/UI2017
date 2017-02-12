#pragma once

namespace UI
{
interface ISystemHScrollBar;
interface IScrollBarManager;


class HScrollbarCreator
{
public:
	HScrollbarCreator();
	~HScrollbarCreator();

	virtual void  CreateByEditor(CREATEBYEDITORDATA* pData, IObject* pParent) /*override*/;
	virtual void  Initialize(IObject* pParent, IScrollBarManager* pMgr) /*override*/;
	
private:
	ISystemHScrollBar*  m_pHScrollbar;
};

}