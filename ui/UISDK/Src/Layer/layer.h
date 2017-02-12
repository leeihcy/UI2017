#pragma once
#include "Src\Util\RectArray\rectarray.h"
#include "Inc\Interface\irenderlayer.h"
#include "transform3d.h"

namespace UI
{
    class Object;
    class Compositor;
}

// 2016.6.4 重写渲染机制
// 默认以支持硬件加载为基础，然后兼容软件渲染
//
// 每个layer将对应一个显存上的纹理，用于最后硬件合成
//
// layer不依赖于object，可以单独存在。object与渲染无直接关系，只是用于
// 提供layer的数据以及鼠标键盘消息等

// http://ariya.ofilabs.com/2013/06/optimizing-css3-for-gpu-compositing.html
// To minimize the amount of texture uploads, you can only animate or
// transition the following properties: opacity, transform, and filter.
// Anything else may trigger a layer update. 
// 什么样的动画不需要上传数据到gpu: 修改透明度、变换、滤镜??
// 变换背景颜色是不行的，需要重新上传背景图，例如http://codepen.io/ariya/full/xuwgy
// 这种动画就会很卡。这种动画可以通过用两个颜色叠加来改善，并不断改变其中一个的透明度

// https://wiki.mozilla.org/Gecko:Layers
// Scrolling
// What should we do to scroll?
// Bas: Use a tile cache.
//
// 问题：layer也要受限于父object的区域限制，例如listitem的layer永远要在listctrl里面，
//       但listctrl不一定开启了layer。所以将layer作为一级渲染会存在这个问题。
// 在 Compositor 的时候，如果需要剪裁这个子layer，则需要用parent object rect来调用clip
//
//
//
// 问题. root layer从哪里来？
// 每创建一个layer时，都去遍历出整棵layer tree的第一个结点作为root layer
//

namespace UI
{

class Object;

interface ILayerContent
{
    virtual bool  IsChildOf(Object*) = 0;
    virtual bool  IsSelfVisible() = 0;
    virtual void  Draw(UI::IRenderTarget*) = 0;
    virtual void  GetWindowRect(RECT* prcOut) = 0;
    virtual void  GetParentWindowRect(RECT* prcOut) = 0;
};

enum LayerType
{
    Layer_Software,
    Layer_Hardware,
};

class Layer : public UIA::IAnimateEventCallback
{
protected:
	enum {
		ANIMATE_DURATION = 250,
	};

public:
	Layer();
    virtual ~Layer();
	
    ILayer*  GetILayer();
    void  SetCompositorPtr(Compositor*);

    IRenderTarget*  GetRenderTarget();
   
    bool  AddSubLayer(Layer*, Layer* pInsertBefore);
    void  MoveLayer2NewParentEnd(Layer* pOldParent, Layer* pNewParent);
    void  RemoveMeInTheTree();

    void  PostCompositorRequest();
	void  Invalidate(LPCRECT prcDirtyInLayer);
	void  Invalidate(LPCRECT prcArray, uint nCount);
    void  CopyDirtyRect(RectArray& arr);

    void  SetContent(ILayerContent*);
    ILayerContent*  GetContent();

    Layer*  GetNext();
    Layer*  GetParent() {
               return m_pParent; }
    Layer*  GetFirstChild();

	void  OnSize(uint nWidth, uint nHeight);

// 	void  SetColor(color);
// 	void  SetTransform(matrix);

    // property
	byte  GetOpacity();
	void  SetOpacity(byte, LayerAnimateParam*);
    
    void  SetYRotate(float);
    float  GetYRotate();
    void  SetTranslate(float x, float y, float z);
    float  GetXTranslate();
    float  GetYTranslate();
    float  GetZTranslate();

    virtual void  UpdateDirty() {}
	virtual void  MapView2Layer(POINT* pPoint){};
    virtual LayerType  GetType() = 0;

    bool  IsAutoAnimate();
    void  EnableAutoAnimate(bool);
//     void  SetAnimateFinishCallback(pfnLayerAnimateFinish, long*);

protected:
	virtual UIA::E_ANIMATE_TICK_RESULT  OnAnimateTick(UIA::IStoryboard*) override;
    virtual void  OnAnimateEnd(UIA::IStoryboard*, UIA::E_ANIMATE_END_REASON e) override;
    virtual void  virtualOnSize(uint nWidth, uint nHeight) {};

private:
    bool  do_add_sub_layer(Layer*);
    bool  do_insert_sub_layer(Layer* p, Layer* pInsertAfter);
    void  on_layer_tree_changed();

protected:
    ILayer  m_iLayer;
    Compositor*  m_pCompositor;

    IRenderTarget*  m_pRenderTarget;

	// Layer Tree
	Layer*  m_pParent;
	Layer*  m_pFirstChild;
	Layer*  m_pNext;
	Layer*  m_pPrev;

	// 
    SIZE  m_size;
	RectArray  m_dirtyRectangles;
	
    ILayerContent*  m_pLayerContent;

    // 将layer剪裁到父对象中(注：不是父layer中)
    bool  m_bClipLayerInParentObj;

 	long  m_bAutoAnimate;
//     pfnLayerAnimateFinish  m_pAnimateFinishCallback;
//     long*  m_pAnimateFinishCallbackUserData;

	// 属性
	byte  m_nOpacity;         // 设置的值
	byte  m_nOpacity_Render;  // 动画过程中的值

 	Transform3D  m_transfrom3d;  // 动画过程中的值
 	float  m_fyRotate;   // 设置的值
    float  m_xTranslate; // 设置的值
    float  m_yTranslate; // 设置的值
    float  m_zTranslate; // 设置的值
};
}