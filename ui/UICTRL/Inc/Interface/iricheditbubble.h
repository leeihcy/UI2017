#ifndef UI_IRICHEDIT_BUBBLE_H_LEEIHCY_201411162122
#define UI_IRICHEDIT_BUBBLE_H_LEEIHCY_201411162122

namespace UI
{

enum  BubbleAlign
{
	BUBBLE_ALIGN_LEFT,
	BUBBLE_ALIGN_RIGHT,
	BUBBLE_ALIGN_CENTER,
};

enum  BubbleType
{
	BUBBLE_NORMAL,
	BUBBLE_PORTRAIT,
	BUBBLE_INFO
};

class REBubble;
interface UICTRL_API IREBubble
{
	IREBubble(REBubble* p);
private:
	REBubble*  m_pBubbleImpl;
};

class REPortraitBubble;
interface UICTRL_API IREPortraitBubble : public IREBubble
{
	IREPortraitBubble(REPortraitBubble* p);

private:
	REPortraitBubble*  m_pImpl;
};

class REInfoBubble;
interface UICTRL_API IREInfoBubble : public IREBubble
{
	IREInfoBubble(REInfoBubble* p);

private:
	REInfoBubble*  m_pImpl;
};


}


#endif // UI_IRICHEDIT_BUBBLE_H_LEEIHCY_201411162122