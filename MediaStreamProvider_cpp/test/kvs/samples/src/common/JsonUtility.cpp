#include "JsonUtility.h"

std::string JsonUtility::JsonToString(Json::Value const& value) {
    Json::Value obj;
    Json::StreamWriterBuilder builder;
    builder["indentation"] = "";
    return Json::writeString(builder, value);
}

std::string JsonUtility::ResultToJsonString(ResultType result) {
    std::string rtype = "invalid";
    Json::StreamWriterBuilder builder;
    builder["indentation"] = "";

    switch(result) {
        case ResultType::OK:
            rtype = "OK";
            break;
        case ResultType::ERROR:
            rtype = "Error";
            break;
        case ResultType::ERROR_BUSY:
            rtype = "Error-busy";
            break;
        case ResultType::ERROR_INVALID_JSON_FORMAT:
            rtype = "Error-InvalidJsonFormat";
            break;
        case ResultType::ERROR_INVALID_INPUT:
            rtype = "Error-InvalidInput";
            break;
        case ResultType::ERROR_OUT_OF_RANGE:
            rtype = "Error-OutOfRange";
            break;
        default:
            rtype = "invalid";
            break;
    }

    std::string r = "";
    if (rtype != "invalid") {
        Json::Value obj;
        obj["result"] = Json::Value(rtype);
        r = Json::writeString(builder, obj);
    }

    return r;
}

std::string JsonUtility::CombineResultAndValue(ResultType result, Json::Value value) {
    std::string resultstr;
    Json::Value obj;
    Json::StreamWriterBuilder builder;
    builder["indentation"] = "";
    std::string key = "value";

    if (result == ResultType::OK) {
        obj["result"] = Json::Value("OK");
        obj[key] = value;
        resultstr = Json::writeString(builder, obj);
    } else if (result == ResultType::ERROR) {
        obj["result"] = Json::Value("Error");
        obj[key] = value;
        resultstr = Json::writeString(builder, obj);
    } else {
        resultstr = "";
    }
    return resultstr;

}

std::string JsonUtility::ResultToJsonString(ResultType result, int value) {
    return JsonUtility::ResultToJsonString(result, "value", value);
}

std::string JsonUtility::ResultToJsonString(ResultType result, std::string const& value) {
    return JsonUtility::ResultToJsonString(result, "value", value);
}

std::string JsonUtility::ResultToJsonString(ResultType result, std::string const& key, int value) {
    std::string resultstr;
    Json::Value obj;
    Json::StreamWriterBuilder builder;
    builder["indentation"] = "";

    if (result == ResultType::OK) {
        obj["result"] = Json::Value("OK");
        obj[key] = Json::Value(value);
        resultstr = Json::writeString(builder, obj);
    } else if (result == ResultType::ERROR) {
        obj["result"] = Json::Value("Error");
        obj[key] = Json::Value(value);
        resultstr = Json::writeString(builder, obj);
    } else {
        resultstr = "";
    }
    return resultstr;
}

std::string JsonUtility::ResultToJsonString(ResultType result, std::string const& key, std::string const& value) {
    Json::Value obj;
    Json::StreamWriterBuilder builder;
    std::string resultstr;
    builder["indentation"] = "";

    if (result == ResultType::OK) {
        obj["result"] = Json::Value("OK");
        obj[key] =  Json::Value(value);
        resultstr = Json::writeString(builder, obj);
    } else if (result == ResultType::ERROR) {
        obj["result"] = Json::Value("Error");
        obj[key] =  Json::Value(value);
        resultstr = Json::writeString(builder, obj);
    } else {
        resultstr = "";
    }
    return resultstr;
}

std::string JsonUtility::CMDToJsonString(std::string const& key, std::string const& value) {
    Json::Value obj;
    Json::StreamWriterBuilder builder;
    builder["indentation"] = "";
    obj[key] =  Json::Value(value);
    return Json::writeString(builder, obj);
}

bool JsonUtility::Parse(const std::string& str, Json::Value& obj) {
    bool result = false;
    Json::CharReaderBuilder jsonBuilder;
    Json::CharReader* reader(jsonBuilder.newCharReader());

    if (reader->parse(str.c_str(), str.c_str() + str.size(), &obj, nullptr)) {
        if (obj.size()!=0U) {
            result = true;
        }
    }
    delete(reader);
    return result;
}

ResultType JsonUtility::JsonStringToResult(std::string const& str) {
    Json::Value obj;
    ResultType type;
    if (JsonUtility::Parse(str, obj)) {
        std::string result = obj["result"].asString();
        if (result == "OK") {
            type = ResultType::OK;
        } else if (result == "Error") {
            type = ResultType::ERROR;
        } else {
            type = ResultType::UKnown;
        }
    } else {
        type = ResultType::ERROR;
    }

    return type;
}

ResultType JsonUtility::JsonStringToResult(std::string const& str, std::string const& key, std::string& value) {
    Json::Value obj;
    ResultType type;

    if (JsonUtility::Parse(str, obj)) {
        std::string result = obj["result"].asString();
        if (result == "OK") {
            value = obj[key].asString();
            type = ResultType::OK;
        } else if (result == "Error") {
            value = obj[key].asString();
            type = ResultType::ERROR;
        } else {
            type = ResultType::UKnown;
        }
    } else {
        type = ResultType::ERROR;
    }

    return type;
}

ResultType JsonUtility::JsonStringToResult(std::string const& str, std::string const& key, int &value) {
    Json::Value obj;
    ResultType type;

    if (JsonUtility::Parse(str, obj)) {
        std::string result = obj["result"].asString();
        if (result == "OK") {
            value = obj[key].asInt();
            type = ResultType::OK;
        } else if (result == "Error") {
            value = obj[key].asInt();
            type = ResultType::ERROR;
        } else {
            type = ResultType::UKnown;
        }
    } else {
        type = ResultType::ERROR;
    }

    return type;
}

std::string JsonUtility::ParseJsonFormatToCString(std::string const& str, std::string const& key)
{
    Json::Value obj;
    std::string s = "";
    if (JsonUtility::Parse(str, obj)) {
        s = obj[key].asString();
    }
    return s;
}

uint32_t JsonUtility::ParseJsonFormatToUInt(std::string str, std::string const& key)
{
    Json::Value obj;
    uint32_t i = 0U;
    if (JsonUtility::Parse(str, obj)) {
        i = obj[key].asUInt();
    }

    return i;
}

std::vector<uint32_t> JsonUtility::ParseJsonFormatToUIntArray(std::string str, std::string const& key)
{
    static std::vector<uint32_t> key_vector;
    key_vector.clear();
    Json::Value obj;

    if (JsonUtility::Parse(str, obj)) {
        Json::Value json_array = obj[key];
        if (json_array.isArray()) {
            const uint32_t array_size = json_array.size();
            for (uint32_t i = 0U; i<array_size; i++) {
                key_vector.push_back(json_array[i].asUInt());
            }
        }
    }

    return key_vector;
}
