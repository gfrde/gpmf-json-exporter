//
// Created by georg on 12.01.25.
//

#include "coutputdata.h"

#include <ostream>
#include <set>

#include "helper_functions.h"

extern bool json_as_stream;
extern bool json_with_array;
extern std::set<std::string> ignoreTypes;


void COutputData::reset_volatile()
{
    stmp.clear();
    st_name.clear();
    st_type.clear();
    export_data.clear();

    payload_index = 0;
    payload_in = -1.0;
    payload_out = -1.0;
}

void COutputData::addExportData(const std::string& key, std::unique_ptr<const CValue> value)
{
    export_data[key] = std::move(value);
    // export_data.insert(std::pair<std::string, std::unique_ptr<const CValue> >(key, value));
}


void COutputData::addExportData(const std::string& key, const std::string& s, bool isJson)
{
    // export_data.push_back(s);#
    std::string s1 = s;
    if (isJson)
    {
        // export_data[key] = s;
        export_data[key] = std::make_unique<CStringValue>(s);
    }
    else
    {
        // export_data[key] = "\"" + s + "\"";
        export_data[key] = std::make_unique<CStringValue>("\"" + s + "\"");
    }
}

void COutputData::addExportData(const std::string& key, const uint64_t& s)
{
    // export_data.push_back(s);
    // export_data[key] = std::to_string(s);
    export_data[key] = std::make_unique<CLongLongValue>(s);
}
void COutputData::addExportData(const std::string& key, const int64_t& s)
{
    // export_data.push_back(s);
    // export_data[key] = std::to_string(s);
    export_data[key] = std::make_unique<CLongLongValue>(s);
}
void COutputData::addExportData(const std::string& key, const uint32_t& s)
{
    // export_data.push_back(s);
    // export_data[key] = std::to_string(s);
    export_data[key] = std::make_unique<CLongValue>(s);
}
void COutputData::addExportData(const std::string& key, const int32_t& s)
{
    // export_data.push_back(s);
    // export_data[key] = std::to_string(s);
    export_data[key] = std::make_unique<CLongValue>(s);
}
void COutputData::addExportData(const std::string& key, const double& s)
{
    // export_data.push_back(s);
    // export_data[key] = std::to_string(s);
    export_data[key] = std::make_unique<CDoubleValue>(s);
}

std::string COutputData::buildJsonPart(int64_t timestamp, double frame_pos) const
{
    std::string toWrite;

    toWrite.append("\"static\":{");
    toWrite.append(R"("devid":")").append(device_id).append("\"");
    toWrite.append(R"(,"devname":")").append(device_name).append("\"");
    if (!filename.empty())
    {
        toWrite.append(R"(,"filename":")").append(filename).append("\"");
    }
    toWrite.append("}");

    toWrite.append(",\"meta\":{");
    toWrite.append(R"("st_name":")").append(st_name).append("\"");
    toWrite.append(R"(,"timestamp":)").append(std::to_string(timestamp));
    if (frame_pos > 0)
    {
        toWrite.append(R"(,"frame":)").append(std::to_string(frame_pos));
    }
    // if (!st_type.empty())
    // {
    //     toWrite.append(R"(,"st_type":")").append(st_type).append("\"");
    // }
    toWrite.append(R"(,"index":")").append(std::to_string(payload_index)).append("\"");
    if (payload_in >= 0)
    {
        toWrite.append(R"(,"time_in":")").append(std::to_string(payload_in)).append("\"");
    }
    if (payload_out >= 0)
    {
        toWrite.append(R"(,"time_out":")").append(std::to_string(payload_out)).append("\"");
    }
    toWrite.append("}");

    return toWrite;
}

std::string COutputData::buildJsonString()
{
    std::string toWrite;
    if (this->split_data)
    {
        double timestep = (payload_out - payload_in) / export_data.size();
        int step = -1;

        // first check, if there any of the entries needs to be exploded
        for (const auto &s: export_data)
        {

        }

        //
        for (const auto &s: export_data)
        {
            if (ignoreTypes.find(s.first) != ignoreTypes.end())
            {
                continue;
            }
            step ++;

            exportCounter ++;

            if (!json_as_stream && (!json_with_array || exportCounter > 1))
            {
                toWrite.append(",");
            }
            toWrite.append("{");

            if (frames_per_sec > 0 )
            {
                double t = timestamp + step*timestep;
                toWrite.append(this->buildJsonPart(static_cast<int64_t>(t), t / frames_per_sec));
            } else
            {
                toWrite.append(this->buildJsonPart(static_cast<int64_t>(timestamp + step*timestep)));
            }

            toWrite.append(",\"data\":");
            toWrite.append(create_json_element(s.first, s.second->getAsJsonValue()));
            // toWrite.append(s);
            toWrite.append("}");
            toWrite.append("\n");
        }
    } else
    {
        exportCounter ++;

        if (!json_as_stream && (!json_with_array || exportCounter > 1))
        {
            toWrite.append(",");
        }
        toWrite.append("{");

        toWrite.append(this->buildJsonPart(timestamp));

        toWrite.append(",\"data\":[");
        uint32_t p=0;
        for (const auto &s: export_data)
        {
            if (ignoreTypes.find(s.first) != ignoreTypes.end())
            {
                continue;
            }
            if (p>0) toWrite.append(",");
            p++;
            toWrite.append(create_json_element(s.first, s.second->getAsJsonValue()));
            // toWrite.append(s);
        }

        toWrite.append("]");
        toWrite.append("}");
    }
    return toWrite;
}

