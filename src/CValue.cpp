//
// Created by georg on 02.04.25.
//

#include "CValue.h"

#include <utility>


// ********************************************************************************************************************
std::string CValue::getAsJsonValue(const std::vector<std::string> & attr_mapping) const
{
    return "???";
}




// ********************************************************************************************************************

CStringValue::CStringValue(std::string v, bool useRaw)
{
    this->value = std::move(v);
    this->useRaw = useRaw;
}

std::string CStringValue::getAsJsonValue(const std::vector<std::string> & attr_mapping) const
{
    // return value;
    if (useRaw)
    {
        return value;
    }
    else
    {
        return "\"" + value + "\"";
    }
}

// ********************************************************************************************************************
CLongValue::CLongValue(long v)
{
    this->value = v;
}

std::string CLongValue::getAsJsonValue(const std::vector<std::string> & attr_mapping) const
{
    return std::to_string(value);
}

// ********************************************************************************************************************
CUnsignedLongValue::CUnsignedLongValue(unsigned long v)
{
    this->value = v;
}

std::string CUnsignedLongValue::getAsJsonValue(const std::vector<std::string> & attr_mapping) const
{
    return std::to_string(value);
}

// ********************************************************************************************************************
CLongLongValue::CLongLongValue(long long v)
{
    this->value = v;
}

std::string CLongLongValue::getAsJsonValue(const std::vector<std::string> & attr_mapping) const
{
    return std::to_string(value);
}

// ********************************************************************************************************************
CUnsignedLongLongValue::CUnsignedLongLongValue(unsigned long long v)
{
    this->value = v;
}

std::string CUnsignedLongLongValue::getAsJsonValue(const std::vector<std::string> & attr_mapping) const
{
    return std::to_string(value);
}

// ********************************************************************************************************************
CDoubleValue::CDoubleValue(double v)
{
    this->value = v;
}

std::string CDoubleValue::getAsJsonValue(const std::vector<std::string> & attr_mapping) const
{
    return std::to_string(value);
}

// ********************************************************************************************************************
CListValue::CListValue()
= default;

std::string CListValue::getAsJsonValue(const std::vector<std::string> & attr_mapping) const
{
    int pos = 0;
    std::string res;
    attr_mapping.empty() ? res += "[" : res += "{";
    // res += "[";
    for (auto && v : value)
    {
        if (pos != 0)
        {
            res += ",";
        }
        if (pos < attr_mapping.size())
        {
        res += "\"" + attr_mapping[pos] + "\":";
        }
        res += v->getAsJsonValue(attr_mapping);;
        pos++;
    }
    attr_mapping.empty() ? res += "]" : res += "}";
    // res += "]";
    return res;
}

void CListValue::addValue(std::unique_ptr<const CValue> v)
{
    value.push_back(std::move(v));
}
