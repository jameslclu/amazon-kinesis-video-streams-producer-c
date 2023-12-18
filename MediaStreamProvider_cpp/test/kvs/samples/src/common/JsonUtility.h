#ifndef FXN_MIDDLEWARE_JSONRESULTUTILITY_H
#define FXN_MIDDLEWARE_JSONRESULTUTILITY_H
#include <string>
#include <vector>
#include <json/json.h>
#include "ResultType.h"

class JsonUtility {
public:
    static std::string JsonToString(Json::Value const& value);
    static std::string ResultToJsonString(ResultType result);
    static std::string ResultToJsonString(ResultType result, int value);
    static std::string ResultToJsonString(ResultType result, std::string const& value);
    static std::string ResultToJsonString(ResultType result, std::string const& key, int value);
    static std::string ResultToJsonString(ResultType result, std::string const& key, std::string const& value);

    static std::string AsyncResultToJsonString(ResultType result, std::string const& method);
    static std::string AsyncResultToJsonString(ResultType result, std::string const& method, int value);
    static std::string AsyncResultToJsonString(ResultType result, std::string const& method, std::string const& value);
    static std::string AsyncResultToJsonString(ResultType result, std::string const& method, std::string const& key, int value);
    static std::string AsyncResultToJsonString(ResultType result, std::string const& method, std::string const& key, std::string const& value);

    static std::string CMDToJsonString(std::string const& key, std::string const& value);
    static std::string CombineResultAndValue(ResultType result, Json::Value value);
    static ResultType JsonStringToResult(std::string const& str);
    static ResultType JsonStringToResult(std::string const& str, std::string const& key, std::string& value);
    static ResultType JsonStringToResult(std::string const& str, std::string const& key, int &value);
    static std::string ParseJsonFormatToCString(std::string const& str, std::string const& key);
    static uint32_t ParseJsonFormatToUInt(std::string str, std::string const& key);
    static std::vector<uint32_t> ParseJsonFormatToUIntArray(std::string str, std::string const& key);
    static bool Parse(const std::string& str, Json::Value& obj);
};

#endif