#include "stdafx.h"
#include "rebubble.h"
#include "Inc\Interface\iricheditbubble.h"
using namespace UI;

IREBubble::IREBubble(REBubble* p)
{
	m_pBubbleImpl = p;
}

IREPortraitBubble::IREPortraitBubble(REPortraitBubble* p):IREBubble(p)
{
	m_pImpl = p;
}

IREInfoBubble::IREInfoBubble(REInfoBubble* p):IREBubble(p)
{
	m_pImpl = p;
}