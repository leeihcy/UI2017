#ifndef INCLUDED_JSON_WRAP
#define INCLUDED_JSON_WRAP

class JsonArray;
class JsonObject;

#include "../include/json.h"
#include <atlstr.h>

// json读取封装，用于解决类型读取错误时，进程崩溃的问题

class JsonDoc
{
public:
    bool  Parse(LPCWSTR, JsonObject&);
    bool  Parse(LPCSTR, JsonObject&);

private:
    Json::Reader  m_reader;
};

// 对象类型
class JsonObject
{
public:
    bool  GetString(const char* key, CStringA&);
    bool  GetString(const char* key, CString&);
    bool  GetInt(const char* key, int&);
	bool  GetBool(const char* key, bool&);
    bool  GetArray(const char* key, JsonArray&);
    bool  GetObject(const char* key, JsonObject&);

	bool  IsNull() const;
private:
    // 用于Json::Reader::parse时获取root value
    operator Json::Value&() { return m_value; }
    
private:
    Json::Value  m_value;
    friend class JsonArray;
    friend class JsonDoc;
};

// 数组类型
class JsonArray
{
public:
    bool  GetSubString(ULONG nIndex, CStringA&);
    bool  GetSubInt(ULONG nIndex, int&);
    bool  GetSubArray(ULONG nIndex, JsonArray&);
    bool  GetSubObject(ULONG nIndex, JsonObject&);
    ULONG  GetSize();

private:
    Json::Value  m_value;
    friend class JsonObject;
};

#endif // INCLUDED_JSON_WRAP