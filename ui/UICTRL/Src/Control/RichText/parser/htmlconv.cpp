#include "stdafx.h"
#include "htmlconv.h"
#include <atlenc.h>

namespace UI
{

void XmlEscape(LPCTSTR szText, String& strOut)
{
	TCHAR szStack[128] = {0};
	int nLen = ATL::EscapeXML(szText, _tcslen(szText), szStack, 127, ATL_ESC_FLAG_ATTR);
	if (nLen >= 127)
	{
		TCHAR* szText = new TCHAR[nLen+1];
		memset(szText, 0, (nLen+1)*sizeof(TCHAR));
		ATL::EscapeXML(szText, _tcslen(szText), szText, nLen, ATL_ESC_FLAG_ATTR);
		strOut.assign(szText);
	}
	else
	{
		strOut.assign(szStack);
	}
}

void XmlUnEscape(LPCTSTR szText, String& strOut)
{
	strOut.clear();

	LPCTSTR p = szText;
	LPCTSTR p2 = p;  // p2->p为一段不需要解析的字符串
	
	while (*p)
	{
		if (*p != TEXT('&'))
		{
			p++;
			continue;
		}
		if (p != p2)
			strOut.append(p2, p-p2);

		p++;  // 跳过&

		// 数值类型
		if (*p == TEXT('#'))	// &#...
		{
			p++;
			int nRadix = 10;
			if (*p == 'x') // &#x... 16进制
			{
				nRadix = 16;
				p++;
			}

			long lValue = 0;
			while (*p)
			{
				if (*p >= TEXT('0') && *p <= TEXT('9'))
				{
					lValue *= nRadix;
					lValue += *p - TEXT('0');
					p++;
					continue;
				}
				else if (nRadix == 16)
				{
					if (*p >= TEXT('A') && *p <= TEXT('F'))
					{
						lValue *= nRadix; 
						lValue += 10 + *p - TEXT('A');
						p++;
						continue;
					}
					else if (*p >= TEXT('a') && *p <= TEXT('f'))
					{
						lValue *= nRadix; 
						lValue += 10 + *p - TEXT('a');
						p++;
						continue;
					}

					break;;
				}
				break;
			}
			if (*p == TEXT(';'))
			{
				strOut.push_back((TCHAR)(unsigned short)lValue);
				p++;
			}
		}

		// 特殊字符
		// & -- &amp;
		else if (p[0] == TEXT('a') && 
				 p[1] == TEXT('m') &&
				 p[2] == TEXT('p') &&
				 p[3] == TEXT(';'))
		{
			strOut.push_back(TEXT('&'));
			p += 4;
		}
		// ' -- &apos;
		else if (p[0] == TEXT('a') && 
				 p[1] == TEXT('p') &&
				 p[2] == TEXT('o') &&
				 p[3] == TEXT('s') &&
				 p[4] == TEXT(';'))
		{
			strOut.push_back(TEXT('\''));
			p += 5;
		}
		// > -- &gt;
		else if (p[0] == TEXT('g') && 
				 p[1] == TEXT('t') &&
				 p[2] == TEXT(';'))
		{
			strOut.push_back(TEXT('>'));
			p += 3;
		}
		// < -- &lt;
		else if (p[0] == TEXT('l') && 
				 p[1] == TEXT('t') &&
				 p[2] == TEXT(';'))
		{
			strOut.push_back(TEXT('<'));
			p += 3;
		}
		// " -- &quot;
		else if (p[0] == TEXT('q') && 
				 p[1] == TEXT('u') &&
				 p[2] == TEXT('o') &&
				 p[3] == TEXT('t') &&
				 p[4] == TEXT(';'))
		{
			strOut.push_back(TEXT('\"'));
			p += 5;
		}

		// 重置字符串起始位置
		p2 = p;
	}

	if (p != p2)
	{
		strOut.append(p2, p-p2);
		p2 = p;
	}
}

}