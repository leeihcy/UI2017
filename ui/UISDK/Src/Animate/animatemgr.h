#pragma once

namespace UIA
{
// http://msdn.microsoft.com/zh-cn/magazine/cc163397.aspx  扩展 WPF 动画类
// http://www.cnblogs.com/Jax/archive/2008/03/26/1123778.html 《Programming WPF》翻译 第8章 动画

// http://msdn.microsoft.com/en-us/library/system.windows.media.animation.doubleanimationbase_members(v=vs.85).aspx  DoubleAnimationBase Members

// http://gamebabyrocksun.blog.163.com/blog/static/571534632010219544464/  游戏引擎循环机制

/*  motion blur<<运动模糊>>

● 游戏中为什么需要模糊处理？清晰一点不是更好吗？

	为什么每秒24帧的速度对于电影来说已经足以获得很流畅的视觉效果，而
	对于电脑游戏来说却会显得磕磕碰碰呢？原因很简单，摄像机在工作的时
	候并非一帧一帧静止的拍摄，它所摄下的每一帧已经包含了1/24秒以内的
	所有视觉信息，包括物体的位移。如果在看录像的时候按下暂停键，我们
	得所到的并不是一幅清晰的静止画面，而是一张模糊的图像，原因就在于
	此。电脑做不到这一点，游戏里的每一帧就是一幅静止画面，如果你在运
	动的过程中抓一张图片下来，得到的肯定是一幅清晰的静态图。运动模糊
	技术的目的就在于增强快速移动场景的真实感，这一技术并不是在两帧之
	间插入更多的位移信息，而是将当前帧同前一帧混合在一起所获得的一种
	效果。
*/

// 2015.10.18
// WaitableTimer + SleepEx/WaitForSingleObject + while(1)结合时，发现
// FPS有时最大只在60帧左右,但有时有能够正常。
// 后测试发现，打开chrome浏览器时，fps能上去。关闭chrome时，fps只能是60
// 2016.3.10
// 使用timeBeginPeriod可恢复精度。

class Storyboard;
interface IAnimateManager;
interface IAnimateEventCallback;
interface IAnimateTimerCallback;

class AnimateManager
{
public:
	AnimateManager();
	~AnimateManager();

	void  AddStoryboard(Storyboard* p);
	void  AddStoryboardBlock(Storyboard* p);
    void  AddStoryboardFullCpu(Storyboard* p);
	void  ClearStoryboardOfNotify(IAnimateEventCallback*);
    void  RemoveStoryboardByNotityAndId(IAnimateEventCallback* pCallback, int nId);
	void  RemoveStoryboard(Storyboard* p);
    void  CancelStoryboard(Storyboard* p);
	
    Storyboard*  CreateStoryboard(
            IAnimateEventCallback* pNotify, 
            int nId = 0, WPARAM wParam = 0, LPARAM lParam = 0);

    IAnimateManager*  GetIAnimateManager();
    void  SetTimerCallback(IAnimateTimerCallback*);
	void  SetUIApplication(UIApplication*);

public:
	int     GetFps() { return m_nFps; }
	int     SetFps(int n);

	void    OnTick();

protected:
    void  Destroy();

	void  SetTimer();
	void  KillTimer();

protected:
    IAnimateManager*  m_pIAnimateManager;
    IAnimateTimerCallback*  m_pITimerCallback;
	UIApplication*  m_pUIApplication;

	LARGE_INTEGER     m_liPerFreq;     // 用于帧数计算

	HANDLE m_hTimer;
	int    m_nFps;
    bool   m_bTimerStart;
	bool   m_bHandlingTimerCallback;   // 避免在遍历stl过程中外部又删除一个对象，导致崩溃

	list<Storyboard*>  m_listStoryboard;
    HMODULE  m_hModuleWinmm;
};
}

