
#include "helper_functions.h"
#include "GPMF_parser.h"

std::string convert_key_to_string(uint32_t key)
{
    char buf[10];
    sprintf(buf, "%c%c%c%c", PRINTF_4CC(key));

    return (buf);
}

std::string create_json_string(const std::string& attr, uint64_t v)
{
    return "{\"" + attr + "\":" + std::to_string(v) + "}";
}

std::string create_json_string(const std::string& attr, int64_t v)
{
    return "{\"" + attr + "\":" + std::to_string(v) + "}";
}

std::string create_json_string(const std::string& attr, uint32_t v)
{
    return "{\"" + attr + "\":" + std::to_string(v) + "}";
}

std::string create_json_string(const std::string& attr, int32_t v)
{
    return "{\"" + attr + "\":" + std::to_string(v) + "}";
}

std::string create_json_string(const std::string& attr, double v)
{
    return "{\"" + attr + "\":" + std::to_string(v) + "}";
}

std::string create_json_string(const std::string& attr, const std::string& v)
{
    return "{\"" + attr + "\":\"" + v + "\"}";
}

std::string create_json_element(const std::string& attr, const std::string& v)
{
    return "{\"" + attr + "\":" + v + "}";
}

std::string create_json_object_entry(const std::string& attr, const std::string& v)
{
    return "\"" + attr + "\":" + v;
}
