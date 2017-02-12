#include "stdafx.h"
#include "textoleelement.h"

using namespace UI;

TextOleElement::TextOleElement()
{
    m_style.bEnable = false; // 不接收鼠标消息
}
TextOleElement::~TextOleElement()
{

}

TextOleElement*  TextOleElement::CreateInstance()
{
    return new TextOleElement();
}

long  TextOleElement::GetType()
{
    return TextOleElement::TYPE;
}
void  TextOleElement::Draw(HDC hDC, int xOffset, int yOffset)
{
    RECT  rcDraw = m_rcInOle;
    OffsetRect(&rcDraw, xOffset, yOffset);

    DrawText(
        hDC, 
        m_strText.c_str(),
        m_strText.length(),
        &rcDraw,
        DT_SINGLELINE);
}
