#pragma once

namespace UI
{

// 整数的一些删除插入操作
class NumberOP
{
public:
    NumberOP(LPCTSTR szText)
    {
        if (szText)
            m_strText = szText;
    }

    void  Remove(int nPos, int nLen)
    {
        m_strText.erase(nPos, nLen);
    }
    void  Insert(int nPos, TCHAR c)
    {
        if (nPos >= (int)m_strText.length())
            m_strText.append(1, c);
        else
            m_strText.insert(nPos, 1, c);
    }
    void  Insert(int nPos, LPCTSTR str)
    {
        m_strText.insert(nPos, str);
    }

    bool  UintValue(UINT* pUint)
    {
        errno = 0;

        UINT nValue = _tcstoul(m_strText.c_str(), NULL, 10);
        if (errno == ERANGE)
            return false;

        *pUint = nValue;
        return true;
    }
    bool  IntValue(int* pint)
    {
        errno = 0;

        int nValue = _ttoi(m_strText.c_str());
        if (errno == ERANGE)
            return false;

        *pint = nValue;
        return true;
    }

    bool  IsSameText(LPCTSTR szText)
    {
        if (!szText)
            return false;

        if (m_strText == szText)
            return true;

        return false;
    }
    bool IsZeroPrefix()
    {
        if (m_strText.length() > 0 && 
            m_strText[0] == TEXT('0'))
            return true;

        return false;
    }

private:
    String  m_strText;
};
}