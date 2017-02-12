#pragma once

namespace UI
{
// 遍历RichEdit文本，分拆Ole与文字
class RETextEnum
{
public:
    enum ENUMRESULT
    {
        FINISH = 0,
        TEXT = 1,
        OLE = 2
    };

    RETextEnum(WindowlessRichEdit* pRE, LPCTSTR szText, int ncpOffset)
    {
        m_pRE = pRE;
        m_szBuffer = szText;
        m_ncpOffset = ncpOffset;
        m_nBufferSize = _tcslen(szText);
        Reset();
    }

    void  Reset()
    {
        m_iPos = 0;
        m_nTextStartPos = 0;
        m_pCurOleObj = NULL;
        m_curText.clear();
    }

    ENUMRESULT Next()
    {
        if (-1 == m_iPos)
            return FINISH;

        for (; m_iPos < m_nBufferSize; )
        {
            if ((WCHAR)m_szBuffer[m_iPos] == WCH_EMBEDDING)
            {
                int nTextStartPosSave = m_nTextStartPos;
                m_nTextStartPos = m_iPos + 1;

                m_pCurOleObj = NULL;
                m_curText.clear();

                int nTextLength = m_iPos - nTextStartPosSave;
                if (nTextLength > 0)
                {
                    m_curText.append(m_szBuffer + nTextStartPosSave, nTextLength);
                    return TEXT;
                }

                m_pCurOleObj = m_pRE->GetOleByCharPos(m_iPos + m_ncpOffset);
                m_iPos ++;
                return OLE;
            }

            m_iPos ++;
        }

        m_pCurOleObj = NULL;
        m_curText.clear();

        // 最后一段文本
        int nTextLength = m_iPos - m_nTextStartPos;
        if (nTextLength > 0)
        {
            m_curText.append(m_szBuffer + m_nTextStartPos, nTextLength);

            m_iPos = -1;  // 下一次直接结束
            return TEXT;
        }

        return FINISH;
    }

    REOleBase*  GetOle()
    {
        return m_pCurOleObj;
    }
    LPCTSTR  GetText()
    {
        return m_curText.c_str();
    }
    int  GetTextLength()
    {
        return m_curText.length();
    }


private:
    LPCTSTR  m_szBuffer;
    WindowlessRichEdit*  m_pRE;
    int  m_ncpOffset;       // 当前分析的文本串在RE内容中的偏移，用于取ole

    int  m_nBufferSize;
    int  m_iPos;            // 当前遍历位置
    int  m_nTextStartPos; // 这一段文本开始的位置

    // Get
    REOleBase*  m_pCurOleObj;
    String  m_curText;
};

}