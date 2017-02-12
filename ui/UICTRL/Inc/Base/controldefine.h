#ifndef _UI_CONTROLDEFINE_H_
#define _UI_CONTROLDEFINE_H_

// 导入导出定义
#ifdef UICTRL_EXPORTS
#define UICTRL_API __declspec(dllexport)
#else
#define UICTRL_API __declspec(dllimport)
#endif

#define UICTRL_API_UUID(guid)  __declspec(uuid(#guid)) UICTRL_API

namespace UI
{
// 最后两位是Control Sub Type标识

#if 0

// richedit style
#define RICHEDIT_STYLE_TEXTSHADOW       0x0100    // 绘制文本阴影。用在透明背景上，避免文字与背景颜色太接近，增加阴影进行区分

    // tree style
#define TREE_STYLE_HASLINE              0x0100    // 绘制虚线

    // listbox style

    // menu style
#define MENU_STYLE_AUTO_DELETE_SUBMENU  0x1000    // 父窗口销毁时，是否自动释放子菜单内存。主要用于从xml中load的菜单
    // 因为是内部创建的子菜单，内部负责销毁子菜单。而外部自己create出来

// tab ctrl
#define TABCTRL_STYLE_BOTTOM            0x0100    // 按钮位于下侧
#endif
}

#endif  // _UI_CONTROLDEFINE_H_