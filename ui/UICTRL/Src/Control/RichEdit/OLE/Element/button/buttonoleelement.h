#pragma once
#include "..\oleelement.h"
#include "..\UISDK\Inc\Util\iimage.h"

namespace UI
{

class ButtonOleElement : public OleElement
{
public:
    enum {TYPE = 141362118};

    ButtonOleElement();
    ~ButtonOleElement();

    static ButtonOleElement* CreateInstance();

public:
	void  SetImage(
		ImageWrap* pNormal,
		ImageWrap* pHover,
		ImageWrap* pPress, 
		ImageWrap* pDisable);

    void  SetTooltip(LPCTSTR szTooltip);

public:
    virtual long  GetType() override;
    virtual void  Draw(HDC hDC, int xOffset, int yOffset) override;
	virtual LRESULT  ProcessMessage(UINT msg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) override;

protected:
	void  DrawImage(ImageWrap* pImage, HDC hDC, int xOffset, int yOffset);
    void  OnClick();

private:
    ImageWrap*  m_pImageNormal;
	ImageWrap*  m_pImageHover;
	ImageWrap*  m_pImagePress;
	ImageWrap*  m_pImageDisable;

    String  m_strTooltip;
    bool  m_bHover;
    bool  m_bPress;
};

}