#pragma once


namespace UI { namespace RT {
 
    class Element;
    class Unit;
    class Run;
    class Doc;
    class Node;

    // richedit的选区类型
    // https://msdn.microsoft.com/zh-cn/library/windows/desktop/bb774037(v=vs.85).aspx

    // 网页规范
    // https://www.w3.org/TR/selection-api/
    // https://developer.mozilla.org/zh-CN/docs/Web/API/Selection
    //
    // DOMSelection 
    // http://www.netmite.com/android/mydroid/external/webkit/WebCore/page/DOMSelection.h
    //
    // 术语表
    //  锚点（anchor）
    //     锚指的是一个选区的起始点（不同于HTML中的锚点链接，译者注）。当我们使用鼠标框选一个区域的时候，锚点就是我们鼠标按下瞬间的那个点。在用户拖动鼠标时，锚点是不会变的。
    //  焦点（focus）
    //     选区的焦点是该选区的终点，当您用鼠标框选一个选区的时候，焦点是你的鼠标松开瞬间所记录的那个点。随着用户拖动鼠标，焦点的位置会随着改变。
    //  范围（range）
    //     范围指的是文档中连续的一部分。一个范围包括整个节点，也可以包含节点的一部分，例如文本节点的一部分。用户通常下只能选择一个范围
    //
    // 属性
    //   anchorNode
    //     返回该选区起点所在的节点（Node）。
    //   anchorOffset
    //    返回一个数字，其表示的是选区起点在 anchorNode 中的位置偏移量。
    //     如果 anchorNode 是文字节点，那么返回的就是从该文字节点的第一个字开始，直到被选中的第一个字之间的字数（如果第一个字就被选中，那么偏移量为零）。
    //     如果 anchorNode 是一个元素，那么返回的就是在选区第一个节点之前的同级节点总数。(这些节点都是 anchorNode 的子节点)
    //   focusOffset
    //     返回一个数字，其表示的是选区终点在 focusNode 中的位置偏移量。
    //      如果 focusNode 是文字节点，那么选区末尾未被选中的第一个字，在该文字节点中是第几个字（从0开始计），就返回它。
    //      如果 anchorNode 是一个元素，那么返回的就是在选区末尾之后第一个节点之前的同级节点总数。
    //   isCollapsed
    //      返回一个布尔值，用于判断选区的起始点和终点是否在同一个位置。


    // 每个对象本身也标识着自己的选中状态
    enum SelectionState {
        SelectionState_None,   // The object is not selected.
        SelectionState_Selected,
    };

    // 用于区分字符索引，光标的范围比字符长度大1
    class CaretPos  
    {
    public:
        //operator uint() { return pos; }
        CaretPos() : pos(0) {}
        CaretPos(uint n) : pos(n){}
        CaretPos& operator= (uint n) { pos = n; return *this; }

    public:
        uint pos;
    };
    inline void SortCaretPos(CaretPos& start, CaretPos& end)
    {
        if (start.pos > end.pos)
        {
            swap(start.pos, end.pos);
        }
    }

    //
    // 选区
    // 1. 以element为选区基本单位。因为run是布局对象，不是固定的，不能用来作为选区基本单元。
    // 2. 每个element自身有一个selection state，表明自己当前是否被选中，用于element绘制。
    //    每次选区发生改变时，会更新element的selection state。
    //
    class Selection //: public ISelection
    {
    public:
        Selection(Doc&);

//         virtual INode*  GetAnchorNode() override;
//         virtual INode*  GetFocusNode() override;
        Node*  GetAnchor();
        Node*  GetFocus();

        CaretPos  GetAnchorOffset();
        CaretPos  GetFocusOffset();

        void  SetAnchor(Node* p, CaretPos offset);
        void  SetFocus(Node* p, CaretPos offset);
        void  SetAnchorFocus(Node* p, CaretPos offset);
        void  Clear();
        bool  IsEmpty();
        void  ToString(String& str);

        bool  GetSelectionRange(Node* p, CaretPos& start, CaretPos& end);
        bool  GetSelectionRange(Run* p, CaretPos& start, CaretPos& end);

    private:
        void  clear_state();
        void  set_state_range(Node* p1, Node* p2, SelectionState s);

        void  update_state();

    private:
        Doc&  m_doc;

        Node*  m_pAnchor;
        CaretPos  m_nAnchorOffset;
        Node*  m_pFocus;
        CaretPos  m_nFocusOffset;
    };
}
}
