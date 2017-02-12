#pragma once
#include "node.h"
#include "Inc\Interface\irichtext.h"

namespace UI
{
namespace RT
{

	// 链接的实现修改为： Link对象所包含的子文本对象都属于链接

class Link : public Node, public ILink
{
public:
    Link();
    virtual ~Link();;

    virtual NodeType  GetNodeType() override { return NodeTypeLink; }
    virtual const CharFormat*  GetCharFormat() override;
    virtual BOOL  OnSetCursor() override;
    virtual void  OnClick(UINT nFlags, POINT ptDoc) override;
    virtual void  OnMouseEnter() override;
    virtual void  OnMouseLeave() override;

	virtual int  GetType() override;
	virtual void  SetType(int n) override;
    virtual void  SetId(LPCTSTR szId) override;
    virtual LPCTSTR  GetId() override;
    virtual void  SetToolTip(LPCTSTR) override;
    virtual LPCTSTR  GetToolTip() override;
    virtual LPCTSTR  GetReference() override;
    virtual void  SetReference(LPCTSTR) override;
	virtual IParagraph*  GetParagraph() override;

	void  SetWParam(WPARAM);
	void  SetLParam(LPARAM);
	WPARAM  GetWParam();
	LPARAM  GetLParam();

	void  SetCharFormat(const CharFormat* pcf);

protected:
	int  m_nType;
    String  m_strId;
	String  m_strToolTip;
    String  m_strRefefence;
	WPARAM  m_wParam;
	LPARAM  m_lParam;

	CharFormat*  m_pcf;  
};
}
}