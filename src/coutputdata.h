//
// Created by georg on 12.01.25.
//

#ifndef COUTPUTDATA_H
#define COUTPUTDATA_H

#include <cstdint>
#include <map>
#include <string>
#include <vector>
#include "CValue.h"


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
    // std::vector<std::string> export_data;
    // std::map<std::string, std::string> export_data;
    std::map<std::string, std::unique_ptr<const CValue> > export_data;

    bool split_data = false;
    double frames_per_sec=-1;

    double timestamp_factor = 1000000;

public:
    std::vector<std::string> get_dest_attributes() const;
    void reset_volatile();
    void addExportData(const std::string& key, const std::string& s, bool isJson=false);
    void addExportData(const std::string& key, const uint64_t& s);
    void addExportData(const std::string& key, const int64_t& s);
    void addExportData(const std::string& key, const uint32_t& s);
    void addExportData(const std::string& key, const int32_t& s);
    void addExportData(const std::string& key, const double& s);
    void addExportData(const std::string& key, std::unique_ptr<const CValue> value);

    // void addExportData(const std::string& s);
    std::string buildJsonString();
    std::string buildJsonPart(int64_t timestamp, double frame_pos=-1) const;
};


#endif //COUTPUTDATA_H
