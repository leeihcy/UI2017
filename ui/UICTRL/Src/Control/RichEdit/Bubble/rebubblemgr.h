#pragma 
#include "UI\UICTRL\Inc\Interface\irichedit.h"
#include "UI\UISDK\Inc\Util\iimage.h"
#include "UI\UISDK\Inc\Util\struct.h"

namespace UI
{
class REBubble;
class WindowlessRichEdit;

class BubbleManager
{
public:
	BubbleManager(WindowlessRichEdit*  p);
    ~BubbleManager();

public:
	void  Init();
	REBubble*  CreateBubble(BubbleType eType, BubbleAlign eAlign, int nStartCP, int nEndCP);
	WindowlessRichEdit*  GetRE();
	ImageWrap*  GetLeftBubbleImage();
	ImageWrap*  GetRightBubbleImage();
    C9Region*  Get9RegionLeftBubble();
    C9Region*  Get9RegionRightBubble();

    void  SetLeftImageResItem(IImageResItem* p);
    void  SetRightImageResItem(IImageResItem* p);
    IImageResItem*  GetLeftImageResItem();
    IImageResItem*  GetRightImageResItem();

    REBubble*  FindBubble(int ncp);
	bool  IsInBubble(int ncp, __out REBubble** ppBubble);

    void  OnSize();
    void  OnDraw(HDC hDC);

    void  InvalidBubbleSize(REBubble* pBubble);

protected:
    void  Clear();
    bool  PreCalcBubblePos();
    void  PostCalcBubblePos(bool bPreRet);

    void  UpdateSingleBubblePos(REBubble* pBubble);
    void  RefreshInvalidBubbleSize();

private:
	WindowlessRichEdit*  m_pWindowlessRE;

    IImageResItem*  m_pLeftImageResItem;
    IImageResItem*  m_pRightImageResItem;

	ImageWrap  m_imageLeftBubble;
	ImageWrap  m_imageRightBubble;
    C9Region  m_9regionLeftBubble;
    C9Region  m_9regionRightBubble;

    typedef list<REBubble*>::iterator _MyIter;
    typedef list<REBubble*>::reverse_iterator _MyReverseIter;
    
    // 气泡列表，建议按顺序排列，否则cp查找时存在问题
    list<REBubble*>  m_bubbleList;  

    // 需要更新内容大小的气泡列表，如刚插入时、气泡内ole大小改变时
    list<REBubble*>  m_bubbleSizeInvalidList;
};

}