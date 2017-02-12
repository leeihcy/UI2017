#pragma once

// 设置控件位置
class SetObjectPosCommand : public CommandBase
{
public:
	SetObjectPosCommand()
	{
		m_pObject = NULL;
		m_rcOld.SetRectEmpty();
		m_rcNew.SetRectEmpty();
	}
	~SetObjectPosCommand()
	{
	}

	long*  GetKey()
	{
		return (long*)m_pObject->GetWindowObject();
	}

	static ICommand*  CreateInstance(IObject*  pObj, RECT* prcOld, RECT* prcNew)
	{
		if (!pObj || !prcOld || !prcNew)
			return NULL;

		if (EqualRect(prcNew, prcOld))
			return NULL;

		SetObjectPosCommand* p = new SetObjectPosCommand;
		p->AddRef();

		p->m_pObject = pObj;
		p->m_rcNew.CopyRect(prcNew);
		p->m_rcOld.CopyRect(prcOld);

		return p;
	}

	void  SetObjectPos(CRect* prc)
	{
		m_pObject->SetObjectPos(prc, SWP_NOREDRAW);
	}

	virtual long execute()
	{
		this->SetObjectPos(&m_rcNew);
		on_post_do(&m_rcNew);

		return S_OK;
	}

	virtual long undo()
	{
		this->SetObjectPos(&m_rcOld);
		on_post_do(&m_rcOld);

		return S_OK;
	}

protected:
	void  on_post_do(CRect* pDst)
	{
		// 设置dirty属性
        IUIElement* pUIElement = GetUIEditor()->GetObjectXmlElem(m_pObject);
		SetDirty(pUIElement);

		// 更新xml属性
		GetUIEditor()->SaveAttribute2Xml(m_pObject);

		// 刷新界面
		UE_RefreshUIWindow(m_pObject);
        UE_ReloadObjectAttribute(m_pObject);
		UE_RefreshLayoutView(m_pObject);

        IObjectDescription* pDesc = m_pObject->GetDescription();

		// 提示
		String  str = _T("修改对象坐标 ");
		str.append(m_pObject->GetId());

        if (pDesc)
        {
            str.append(_T("["));
            str.append(pDesc->GetTagName());
            str.append(_T("] ："));
        }

		TCHAR  szPos[64] = {0};
		_stprintf(szPos, _T("pos: %d,%d  width: %d,%d"), pDst->left, pDst->top, pDst->Width(), pDst->Height());
		str.append(szPos);
		g_pGlobalData->m_pStatusBar->SetStatusText2(str.c_str());
	}
public:
	IObject*      m_pObject;
	CRect         m_rcOld;
	CRect         m_rcNew;
};