//
// Created by georg on 12.01.25.
//

#include "coutputdata.h"

extern bool json_as_stream;
extern bool json_with_array;

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

void COutputData::addExportData(const std::string& s)
{
    export_data.push_back(s);
}

std::string COutputData::buildJsonString()
{
    exportCounter ++;

    std::string toWrite;
    if (!json_as_stream && (!json_with_array || exportCounter > 1))
    {
        toWrite.append(",");
    }
    toWrite.append("{");

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
    if (!st_type.empty())
    {
        toWrite.append(R"(,"st_type":")").append(st_type).append("\"");
    }
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
    toWrite.append(",\"data\":[");
    uint32_t p=0;
    for (const auto &s: export_data)
    {
        if (p>0) toWrite.append(",");
        p++;
        toWrite.append(s);
    }

    toWrite.append("]");
    toWrite.append("}");

    return toWrite;
}

