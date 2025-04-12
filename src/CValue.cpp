//
// Created by georg on 02.04.25.
//

#include "CValue.h"

#include <utility>


// ********************************************************************************************************************
std::string CValue::getAsJsonValue() const
{
    return "???";
}




// ********************************************************************************************************************

CStringValue::CStringValue(std::string v, bool useRaw)
{
    this->value = std::move(v);
    this->useRaw = useRaw;
}

std::string CStringValue::getAsJsonValue() const
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

std::string CLongValue::getAsJsonValue() const
{
    return std::to_string(value);
}

// ********************************************************************************************************************
CLongLongValue::CLongLongValue(long long v)
{
    this->value = v;
}

std::string CLongLongValue::getAsJsonValue() const
{
    return std::to_string(value);
}

// ********************************************************************************************************************
CDoubleValue::CDoubleValue(double v)
{
    this->value = v;
}

std::string CDoubleValue::getAsJsonValue() const
{
    return std::to_string(value);
}

// ********************************************************************************************************************
CListValue::CListValue()
{
}

std::string CListValue::getAsJsonValue() const
{
    int pos = 0;
    std::string res = "[";
    for (auto && v : value)
    {
        if (pos != 0)
        {
            res += ",";
        }
        res += v->getAsJsonValue();;
        pos++;
    }
    res += "]";
    return res;
}

void CListValue::addValue(std::unique_ptr<const CValue> v)
{
    value.push_back(std::move(v));
}
