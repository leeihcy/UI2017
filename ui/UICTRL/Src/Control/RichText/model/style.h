#pragma once
#include "Inc\Interface\irichtext.h"

namespace UI
{
namespace RT
{

struct Style
{
    Style()
    {
		draw_link_underline = true;
		page_content_valign = VALIGN_top;
		page_content_halign = HALIGN_left;
		line_space = 0;
        SetRectEmpty(&page_padding);
    }

    // hover/press时自动绘制链接下划线
    bool  draw_link_underline;  

    // 内容纵向对齐方式
    VALIGN  page_content_valign;  

    // 内容横向对齐方式
    HALIGN  page_content_halign;  

    // 页面四周空白
    REGION4  page_padding;

    // 行间距
    long  line_space;  
};

}
}