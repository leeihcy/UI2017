#pragma once
#include "..\oleelement.h"

namespace UI
{

class TextOleElement : public OleElement
{
public:
    enum {TYPE = 140281055};

    TextOleElement();
    ~TextOleElement();

    static TextOleElement*  CreateInstance();

public:
    virtual long  GetType() override;
    virtual void  Draw(HDC hDC, int xOffset, int yOffset) override;

protected:
    String  m_strText;
};

}