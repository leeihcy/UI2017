#pragma once

// 动画的封装实现在 UIAnimate.dll 中，uisdk.dll是依赖于 AnimateBase.dll，
// 不能直接调用 UIAnimate 模块的接口。因此在 uisdk 中预览回调接口，将实现
// 放在 UIAnimate 当中。


namespace UI
{
    class ListItemBase;
}

typedef bool (__stdcall *pfnListItemRectChangedCallback)(
                                IListItemBase& item,
                                LPCRECT prcOld, 
                                LPCRECT prcNew);
#if 0
class ListCtrlAnimateCallback
{
protected:
    ListCtrlAnimateCallback();
    ~ListCtrlAnimateCallback();

public:
    static ListCtrlAnimateCallback&  Get();

    static void  SetListItemRectChangedCallback(
                    pfnListItemRectChangedCallback);

    static bool  HandleItemRectChanged(
                    ListItemBase& item,
                    LPCRECT prcOld, 
                    LPCRECT prcNew);

private:
    pfnListItemRectChangedCallback  m_pfnListItemRectChangedCallback;
};
#endif