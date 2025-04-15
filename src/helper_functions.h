
#ifndef HELPER_FUNCTIONS_H
#define HELPER_FUNCTIONS_H

#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <cstdint>
#include <iostream>

std::string convert_key_to_string(uint32_t key);
std::string create_json_string(const std::string& attr, uint64_t v);
std::string create_json_string(const std::string& attr, int64_t v);
std::string create_json_string(const std::string& attr, uint32_t v);
std::string create_json_string(const std::string& attr, int32_t v);
std::string create_json_string(const std::string& attr, double v);
std::string create_json_string(const std::string& attr, const std::string& v);
std::string create_json_element(const std::string& attr, const std::string& v);
std::string create_json_object_entry(const std::string& attr, const std::string& v);


#endif // HELPER_FUNCTIONS_H