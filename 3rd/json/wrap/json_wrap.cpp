#include "stdafx.h"
#include "json_wrap.h"
#include <assert.h>

//
// 1. 采用dll的方式共享json代码，避免json代码重复
// 2. 采用md的共享库，解决外部调用json时，json返回CStringA类型后，出现释放崩溃问题
//
#ifdef _DEBUG
#pragma  comment(lib, "lib_json_md.lib")
#else
#pragma  comment(lib, "lib_json_md.lib")
#endif


// bool  CJsonReader::Parse(const char* utf8Content)
// {
// 	ASSERTEX(utf8Content);
// 	if (!utf8Content)
// 		return false;
// 
//     if (m_pReader)
//         return false;
// 
//     m_pReader = new Json::Reader;
//     if (!m_pReader->parse(utf8Content, m_value))
//         return false;
// 
//     return true;
// }

bool  JsonDoc::Parse(LPCWSTR text, JsonObject& o)
{
    if (!text)
        return false;

    return this->Parse(CW2A(text, CP_UTF8), o);
}
bool  JsonDoc::Parse(LPCSTR text, JsonObject& o)
{
    if (!text)
        return false;

    if (!m_reader.parse(text, o))
        return false;

    if (!o.m_value.isObject())
        return false;

    return true;
}

bool  parse_string_in_value(Json::Value& v, CStringA& text)
{
    if (v.isString())
    {
        text = v.asString().c_str();
        return true;
    }

    if (v.isInt())
    {
        char format[16] = {0};
        sprintf(format, "%d", v.asInt());
        text = format;

        return true;
    }

    return false;
}

bool  parse_int_in_value(Json::Value& v, int& integer)
{
    if (v.isInt())
    {
        integer = v.asInt();
        return true;
    }

    if (v.isString())
    {
        integer = atoi(v.asString().c_str());
        return true;
    }

    return false;
}

bool  JsonObject::GetString(const char* key, CStringA& text)
{
    if (!key || !key[0])
        return false;

    if (!m_value.isMember(key))
        return false;

    Json::Value v = m_value[key];
    return parse_string_in_value(v, text);
}

bool  JsonObject::GetString(const char* key, CString& text)
{
    CStringA utf8;
    if (!GetString(key, utf8))
        return false;

    text.SetString(CA2W(utf8, CP_UTF8));
    return true;
}

bool  JsonObject::GetInt(const char* key, int& integer)
{
    if (!key || !key[0])
        return false;

    if (!m_value.isMember(key))
        return false;

    Json::Value v = m_value[key];
    return parse_int_in_value(v, integer);
}

bool  JsonObject::GetBool(const char* key, bool& bValue)
{
	if (!key || !key[0])
		return false;

	if (!m_value.isMember(key))
		return false;

	Json::Value v = m_value[key];

	if (v.isBool())
	{
		bValue = v.asBool();
		return true;
	}

	if (v.isInt())
	{
		bValue = v.asInt() ? true:false;
		return true;
	}

	if (v.isString())
	{
		std::string str = v.asString();
		if (0 == stricmp(str.c_str(), "true"))
		{
			bValue = true;
		}
		else if (0 == stricmp(str.c_str(), "false"))
		{
			bValue = false;
		}
		return true;
	}

	return false;
}

bool  JsonObject::GetArray(const char* key, JsonArray& j)
{
    if (!key || !key[0])
        return false;

    if (!m_value.isMember(key))
        return false;

    if (!m_value[key].isArray())
        return false;

    j.m_value = m_value[key];
    return true;
}

bool  JsonObject::GetObject(const char* key, JsonObject& j)
{
    if (!key || !key[0])
        return false;

    if (!m_value.isMember(key))
        return false;

    if (!m_value[key].isObject())
        return false;

    j.m_value = m_value[key];
    return true;
}

bool  JsonObject::IsNull() const
{
	return m_value.isNull();
}

bool  JsonArray::GetSubString(ULONG nIndex, CStringA& text)
{
    if (!m_value.isArray())
        return false;

    if (nIndex >= m_value.size())
        return false;

    return parse_string_in_value(m_value[(int)nIndex], text);
}

bool  JsonArray::GetSubInt(ULONG nIndex, int& integer)
{
    if (!m_value.isArray())
        return false;

    if (nIndex >= m_value.size())
        return false;

    return parse_int_in_value(m_value[(int)nIndex], integer);
}

bool  JsonArray::GetSubObject(ULONG nIndex, JsonObject& j)
{
    if (!m_value.isArray())
        return false;

    if (nIndex >= m_value.size())
        return false;

    if (!m_value[(int)nIndex].isObject())
        return false;

    j.m_value = m_value[(int)nIndex];
    return true;
}

bool  JsonArray::GetSubArray(ULONG nIndex, JsonArray& j)
{
    if (!m_value.isArray())
        return false;

    if (nIndex >= m_value.size())
        return false;

    if (!m_value[(int)nIndex].isArray())
        return false;

    j.m_value = m_value[(int)nIndex];
    return true;
}

ULONG  JsonArray::GetSize()
{
    if (!m_value.isArray())
        return 0;

    return m_value.size();
}

#ifdef _UNITTESTxxx

#include <iostream>
using namespace std;

//
//  json格式：
//  http://www.jsonin.com/ json校验
//
//  1. 数组前面必须有key:，不能直接就出现一个数组
//  2. 数组中的内容，不带key:  如果要带key:则得用{}包起来做成一个对象
//  3. [int]类型的操作符仅是用于访问array类型的value，而且array类型的也不能用["key"]来访问
//  4. "key":["b":1,2,3]，<--格式错误，数X组中不能出现:（冒号），只期望出现，（逗号）
//      在jsoncpp中这个数组会被当成有4个成员，"b"是第1个字符串类型，1是第2个。
//  5. "key":[{"b":1},2,3]，{"b":1}会被当成一个object，再取["b"]是int 1;
//
int _tmain()
{
	CStringA strJson(
        "{\"list\":"
            "["
                "{\"id\":\"100\", \"type\":\"embed_web\",\"path\":\"www.baidu.com\",\"name\":\"百度\",\"key\":\"test@baidu.com\",\"icon\":\"https://www.baidu.com/favicon.ico\"},"
                "{\"id\":\"200\", \"type\":\"embed_web\",\"path\":\"www.qq.com\",\"name\":\"腾讯\",\"key\":\"test@qq.com\",\"icon\":\"http://mat1.gtimg.com/www/icon/favicon2.ico\"},"
                "{\"id\":\"300\", \"type\":\"embed_web\",\"path\":\"www.sina.com\",\"name\":\"新浪\",\"key\":\"test@sina.com\",\"icon\":\"http://i3.sinaimg.cn/home/2013/0331/U586P30DT20130331093840.png\"},"
                "{\"id\":\"400\", \"type\":\"embed_web\",\"path\":\"www.10086.cn\",\"name\":\"移动\",\"key\":\"test@10086.com\",\"icon\":\"http://www.10086.cn/favicon.ico\"},"
                "{\"id\":\"500\", \"type\":\"embed_web\",\"path\":\"www.huawei.com\",\"name\":\"华为\",\"key\":\"test@huawei.com\",\"icon\":\"http://www.huawei.com/Assets/img/img_ent_en_logo_ico.ico\"}"
        "]}");

#if 0
	Json::Reader reader;
	Json::Value jValue;

	assert(reader.parse(strJson.c_str(), jValue));

    cout << jValue["result_code"].asInt() << endl;   
    cout << jValue["result_desc"].asString().c_str() << endl;   

    cout << jValue["result_desc"].asInt() << endl;  // 崩溃了 
    cout << jValue["result_code"].asString().c_str() << endl;  // 崩溃了
#else

    JsonDoc doc;
    JsonObject json;
    assert(doc.Parse(strJson.c_str(), json));

    // 整形，字符串互读
    CStringA str;
    int i;
    assert (json.GetInt("result_code", i));
    assert (json.GetString("result_desc", str));
    
    cout << "result_code=" << i << endl;
    cout << "result_desc=" << str.c_str() << endl;

    assert (json.GetInt("result_desc", i));
    assert (json.GetString("result_code", str));

    // 数组的读取
    JsonArray partsArray;
    assert (json.GetArray("parts", partsArray));

    for (ULONG i = 0; i < partsArray.GetSize(); i++)
    {
        JsonObject partObj;
        assert (partsArray.GetSubObject(i, partObj));

        CStringA part;
        assert (partObj.GetString("part", part));
        cout << "part=" << part.c_str() << endl;

        cout << "means=" ;
        JsonArray meansArray;
        assert (partObj.GetArray("means", meansArray));
        for (ULONG j = 0; j < meansArray.GetSize(); j++)
        {
            CStringA str;
            assert (meansArray.GetSubString(j, str));
            cout << str.c_str() << " ";
        }
        cout << endl;
    }

#endif

    system("pause");
	return 0;
}
#endif