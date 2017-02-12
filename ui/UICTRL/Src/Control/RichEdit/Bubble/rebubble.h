#pragma 
#include "..\Ole\richeditolemgr.h"
#include "Inc\Interface\iricheditbubble.h"
#include "..\UISDK\Inc\Util\iimage.h"

// richedit气泡背景
//
// 气泡并没有采用ole来实现，因为只需要维护startcp/endcp就行了
// 
// buglist:
// 1.  2014.8.6. libo
//     如果内容中包含TAB制表符，会导致右对齐时获取行宽度不正确，导致文字跑出气泡了!
//     如何解决? 目前临时采用空格替换的方式
//       strContent.Replace(TEXT("\t"), TEXT("    "));
//     但如何解决copy该内容时应该还原为tab？？
//
// 2. 2014.10.30 libo
//     将RE高度调整为刚好容纳下一个很长的右侧气泡，气泡内包含一些大的OLE对象，如图片
//     然后将窗口大小拉小，将出现气泡位置闪烁的现象。
//     原因是RE变小后，RE的宽度放不下右侧气泡，因此右侧气泡内容被迫变高，导致滚动条出现，
//     RE客户区变小，内容整体向左偏移。出现滚动条后，气泡的新位置调整都是基于新的客户
//     区大小，导致无法归位到无滚动条的状态。
//
//     如何解决？看QQ的解决办法是：气泡模式下，滚动条一直占有该区域，RE客户区域不变
//     参见： RichEdit::EnableBubbleMode
//    
// 3. Q: 有时候气泡绘制的位置与内容的位置不一致，是什么原因？
//    A: 内容的位置调整好像有时是在txdraw中触发，因此txdraw之前绘制气泡得到的结果肯定与之后
//       显示的内容位置不一致。因此画气泡之前必须先调用一次txdraw
//
interface ITextPara;
namespace UI
{
class BubbleManager;

class REBubble
{
public:
	REBubble();
	virtual ~REBubble();

public:
	void  SetBubbleManager(BubbleManager* pMgr);
    void  SetContentCPInfo(int nStart, int nEnd);
	
    IREBubble*  GetIREBubble();
    void  CreateIxxx(IREBubble* p);

protected:
	void  GetDrawRegion(RECT* prc);
    void  CalcBubblePos();
	void  SetDesiredAlign(BubbleAlign e);
    void  UpdateRightBubbleIndents(ITextPara*  pPara);
	void  UpdateCenterBubbleIndents(ITextPara*  pPara);

protected:
	// 绘制气泡
	virtual void  Draw(HDC hDC, RECT* prcDraw);
	// 获取内容与气泡之间的间距
	virtual void  GetContentPadding(REGION4* pPadding);

protected:
	friend class BubbleManager;
	BubbleManager*  m_pBubbleMgr;
    IREBubble*  m_pIREBubble;
    bool  m_bNeedDeleteIREBubble;

    int     m_nContentWidth;
    int     m_nContentHeight;
    int     m_ncpStart;
    int     m_ncpEnd;
	
	BubbleAlign  m_eAlignDesired;  // 期望的对齐方式
	BubbleAlign  m_eAlignReal;     // 实际的对齐方式
};

// 气泡+头像
class REPortraitBubble : public REBubble
{
public:
	REPortraitBubble();
    ~REPortraitBubble();
	
	// 定义头像与气泡的偏移
	enum
	{
		PORTRAIT_OFFSET_LEFT_X = 30,
		PORTRAIT_OFFSET_LEFT_T = 0,
		PORTRAIT_OFFSET_RIGHT_X = 30,
		PORTRAIT_OFFSET_RIGHT_Y = 0,
	};

	virtual void  Draw(HDC hDC, RECT* prcDraw);

	void  SetPortrait(ImageWrap* pPortrait);
    IREPortraitBubble*  GetIREPortraitBubble();
    void  CreateIxxx(IREPortraitBubble* p);

protected:
	IREPortraitBubble*   m_pIREPortraitBubble;
    bool  m_bNeedDeleteIREPortraitBubble;

	ImageWrap*  m_pImagePortrait;
};

class REInfoBubble : public REBubble
{
public:
	virtual void  Draw(HDC hDC, RECT* prcDraw){};
	virtual void  GetContentPadding(REGION4* pPadding){};
};


}