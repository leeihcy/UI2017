#pragma once
#include "unit.h"
#include "element.h"

namespace UI
{
namespace RT
{

//
// 为了统一调用，所有的unit都得有一个element接口，但这样会写object unit会很麻烦。
// 因此只为text类型保留element概念，对object类型的unit隐藏element概念
//
//
// 所有的object类型unit从这个对象派生
//

    class ObjectElement : public Element
    {
    public:
        ObjectElement()
        {

        }

        virtual NodeType  GetNodeType() {
            return NodeType::Object;
        }

        virtual void  Release()
        {
            // ObjectElement不需要释放，不是new出来的
        }
        virtual ElementType  GetElementType()
        {
            return ElementTypeObejct;
        }
        virtual void  Draw(HDC hDC, Run*, RECT* prcRun)
        {
            // return m_objUnit.Draw(hDC, prcRun);
        }
        virtual uint  GetWidth() override
        {
            UIASSERT(0);
            return 0;
        }
        virtual uint  GetHeight() override
        {
            UIASSERT(0);
            return 0;
        }
        virtual uint  GetCharacters() override
        {
            return 1;
        }
    };

#if 0
class ObjectUnit : public Unit
{
public:
    ObjectUnit(Doc& doc) : 
        Unit(doc), m_element(*this)
    {
        m_pFirstElement = &m_element;
    }

//     virtual uint  GetWidth() = 0;
//     virtual uint  GetHeight() = 0;

    // 在布局过程中，对象返回他所期望的大小。可根据当前page大小/行剩余宽度动态变化。
    virtual SIZE  GetLayoutSize(SIZE pageContentSize, int lineRemain) = 0;
    virtual void  Draw(HDC hDC, RECT* prc) = 0;

    // 由ObjectUnit进行封装，派生类只需实现GetLayoutSize, 不用实现LayoutMultiLine
    virtual void  LayoutMultiLine(MultiLineLayoutContext* pContext) final;
    virtual BOOL  OnSetCursor() override;

    Element&  GetElement()
    {
        return m_element;
    }

private:
    ObjectElement  m_element;
};
#endif
}
}