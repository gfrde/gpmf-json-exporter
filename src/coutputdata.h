//
// Created by georg on 12.01.25.
//

#ifndef COUTPUTDATA_H
#define COUTPUTDATA_H

#include <cstdint>
#include <string>
#include <vector>


class COutputData
{
public:
    std::string device_id;
    std::string device_name;

    uint32_t exportCounter = 0;
    std::string filename;

    //
    uint32_t payload_index = 0;
    double payload_in = -1.0;
    double payload_out = -1.0;
    int64_t timestamp;  // micro seconds since video start
    std::string stmp;
    std::string st_name;
    std::string st_type;
    std::vector<std::string> export_data;

public:
    void reset_volatile();
    void addExportData(const std::string& s);
    std::string buildJsonString();
};


#endif //COUTPUTDATA_H
