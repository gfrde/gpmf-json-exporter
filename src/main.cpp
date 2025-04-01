
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <cstdint>
#include <iostream>
#include <algorithm>

#include "GPMF_parser.h"
#include "demo/GPMF_mp4reader.h"

#include "coutputdata.h"
#include "helper_functions.h"

uint32_t load_meta = false;
uint32_t show_debug = false;
uint32_t show_info = false;
uint32_t only_second = -1;
bool show_json = false;
bool json_as_stream = false;
bool json_with_array = false;
bool json_with_filename = false;
bool split_data = false;

#define PRINT_DEBUG if (show_debug) printf
#define PRINT_INFO if (show_info) printf




GPMF_ERR readMP4File(char* filename, bool export_filename)
{
    GPMF_ERR ret = GPMF_OK;
    GPMF_stream metadata_stream = { 0 }, * ms = &metadata_stream;
    double metadatalength;
    uint32_t* payload = NULL;
    uint32_t payloadsize = 0;
    size_t payloadres = 0;
    size_t mp4handle;
    if (!load_meta) // Search for GPMF Track
        mp4handle = OpenMP4Source(filename, MOV_GPMF_TRAK_TYPE, MOV_GPMF_TRAK_SUBTYPE, 0);
    else // look for a global GPMF payload in the moov header, within 'udta'
        mp4handle = OpenMP4SourceUDTA(filename, 0);  //Search for GPMF payload with MP4's udta

    if (mp4handle == 0)
    {
        printf("error: %s is an invalid MP4/MOV or it has no GPMF data\n\n", filename);
        return GPMF_ERROR_BAD_STRUCTURE;
    }

    metadatalength = GetDuration(mp4handle);

    uint32_t index = -1;
    if (metadatalength > 0.0)
    {
        COutputData output_data;
        if (export_filename)
        {
            output_data.filename = filename;
        }
        output_data.split_data = split_data;

        if (json_with_array)
        {
            std::cout << "[" << std::endl;
        }



        uint32_t payloads = GetNumberPayloads(mp4handle);
        PRINT_INFO("found %.2fs of metadata, from %d payloads, within %s\n", metadatalength, payloads, filename);

        uint32_t fr_num, fr_dem;
        uint32_t frames = GetVideoFrameRateAndCount(mp4handle, &fr_num, &fr_dem);
        if (frames)
        {
            PRINT_INFO("VIDEO FRAMERATE:\n  %.3f with %d frames\n", (float)fr_num / (float)fr_dem, frames);
        }

        for (index = 0; index < payloads; index++)
        {
            ret = GPMF_OK;

            double in = 0.0, out = 0.0; //times
            payloadsize = GetPayloadSize(mp4handle, index);
            payloadres = GetPayloadResource(mp4handle, payloadres, payloadsize);
            payload = GetPayload(mp4handle, payloadres, index);
            PRINT_DEBUG("index=%u, Payload size: %u - payload: %p\n", index, payloadsize, payload);

            if (payload == nullptr)
                break;;


            ret = GetPayloadTime(mp4handle, index, &in, &out);
            if (ret != GPMF_OK)
            {
                PRINT_INFO("no payload time found");
                break;
            }

            ret = GPMF_Init(ms, payload, payloadsize);
            if (ret != GPMF_OK)
            {
                printf("initialization of struct failed");
                break;
            }

            if (only_second != -1 )
            {
                if (only_second != index)
                {
                    ret = GPMF_Next(ms, (GPMF_LEVELS)(GPMF_RECURSE_LEVELS | GPMF_TOLERANT));
                    if (GPMF_OK != ret)
                    {
                        printf("next() failed - stopping");
                        break;
                    }
                    continue;
                }
            }

            uint32_t key;
            GPMF_SampleType type;
            uint32_t samples;
            uint32_t elements;

            do
            {
                key = GPMF_Key(ms);
                type = GPMF_Type(ms);
                samples = GPMF_Repeat(ms);
                elements = GPMF_ElementsInStruct(ms);


                // write previous data, if existent
                if ((key == GPMF_KEY_DEVICE || key == GPMF_KEY_STREAM) && show_json && !output_data.export_data.empty())
                {
                    // if there is any data imported, write it now before cleanup
                    std::cout << output_data.buildJsonString() << std::endl;
                }


                uint32_t level = ms->nest_level;
                if (key != GPMF_KEY_DEVICE) level++;
                for (uint32_t i=0; i<level+1; i++)
                {
                    PRINT_DEBUG("  ");
                }

                if (type == GPMF_TYPE_NEST)
                {
                    PRINT_DEBUG("%c%c%c%c; type = nested\n", PRINTF_4CC(key));
                }
                else
                {
                    PRINT_DEBUG("%c%c%c%c; type = %c; struct-elements = %u; repeat = %u\n", PRINTF_4CC(key), type, elements, samples);
                }

                if (key == GPMF_KEY_DEVICE || key == GPMF_KEY_STREAM)
                {
                    output_data.reset_volatile();
                    output_data.payload_in = in;
                    output_data.payload_out = out;
                    output_data.payload_index = index;
                }

                if (key == GPMF_KEY_DEVICE_ID)
                {
                    output_data.device_id.clear();
                }
                else if (key == GPMF_KEY_DEVICE_NAME)
                {
                    output_data.device_name.clear();
                }

                char* data = (char*)GPMF_RawData(ms);
                uint32_t ssize = GPMF_StructSize(ms);
                uint32_t type_samples = GPMF_Repeat(ms);

                for (uint32_t i=0; i<level+1; i++)
                {
                    PRINT_DEBUG("  ");
                }
                PRINT_DEBUG("- struct size=%u - value=", ssize);


                // ########################################################################
                // main data handling
                if (type == GPMF_TYPE_NEST)
                {
                    PRINT_DEBUG("      is nested data\n");
                }
                else if (type == GPMF_TYPE_STRING_ASCII)
                {
                    std::string s;
                    for (int a=0; a<ssize*type_samples; a++)
                    {
                        PRINT_DEBUG("%c", data[a]);
                    }

                    bool single_string = type_samples == 1 || ssize == 1;
                    if (!single_string) s.append("[");

                    for (uint32_t i=0; i<type_samples; i++)
                    {
                        if (!single_string && i > 0) s.append(",");
                        if (!single_string) s.append("\"");

                        for (uint32_t j=0; j<ssize; j++)
                        {
                            // if (!single_string && j != 0) s.append(",");
                            if (data[i*ssize + j] > 31 && data[i*ssize + j] != '"') // ignore control characters
                            {
                                s.append(1, data[i*ssize + j]);
                            }
                        }

                        if (!single_string) s.append("\"");
                    }
                    if (!single_string) s.append("]");

                    if (key == GPMF_KEY_TYPE)
                    {
                        output_data.st_type = s;
                    }
                    else if (key == GPMF_KEY_STREAM_NAME)
                    {
                        output_data.st_name = s;
                    }
                    else if (key == GPMF_KEY_DEVICE_ID)
                    {
                        output_data.device_id = s;
                    }
                    else if (key == GPMF_KEY_DEVICE_NAME)
                    {
                        output_data.device_name = s;
                    }
                    else
                    {
                        if (single_string)
                            output_data.addExportData(convert_key_to_string(key), s);
                        else
                            output_data.addExportData(convert_key_to_string(key), s, true);
                    }

                }
                else if (type == GPMF_TYPE_FOURCC)
                {
                    PRINT_DEBUG("  %c%c%c%c ", PRINTF_4CC(*((uint32_t*)data)));

                    std::string s = convert_key_to_string(*((uint32_t*)data));
                    if (key == GPMF_KEY_DEVICE_ID)
                    {
                        output_data.device_id = s;
                    }
                    else
                    {
                        output_data.addExportData(convert_key_to_string(key), s);
                    }

                }
                else if (type == GPMF_TYPE_SIGNED_64BIT_INT)
                {
                    int64_t *j = (int64_t *)data;
                    //
                    // for (uint32_t i=0; i<samples; i++)
                    // {
                    //     printf(" %d", f[i]);
                    // }

                    PRINT_DEBUG("  %ld ", reinterpret_cast<int64_t>(BYTESWAP64(*j)));

                    if (key == GPMF_KEY_TIME_STAMP)
                    {
                        output_data.timestamp = BYTESWAP64(*j);
                    }
                    else
                    {
                        output_data.addExportData(convert_key_to_string(key), reinterpret_cast<int64_t>(BYTESWAP64(*j)));
                    }
                }
                else if (type == GPMF_TYPE_UNSIGNED_64BIT_INT)
                {
                    uint64_t *j = (uint64_t *)data;
                    PRINT_DEBUG("  %lu ", reinterpret_cast<uint64_t>(BYTESWAP64(*j)));

                    if (key == GPMF_KEY_TIME_STAMP)
                    {
                        output_data.timestamp = BYTESWAP64(*j);
                    }
                    else
                    {
                        output_data.addExportData(convert_key_to_string(key), reinterpret_cast<uint64_t>(BYTESWAP64(*j)));
                    }
                }
                else if (type == GPMF_TYPE_SIGNED_LONG || type == GPMF_TYPE_SIGNED_SHORT || type == GPMF_TYPE_SIGNED_BYTE)
                {
                    // value_ul = (uint32_t)data[0];
                    // printf("%lu", value_ul);

                    uint32_t num = samples * elements;
                    int32_t f[num];
                    ret = GPMF_ScaledData(ms, f, sizeof(f), 0, samples, GPMF_TYPE_SIGNED_LONG);
                    if (GPMF_OK == ret)
                    {
                        PRINT_DEBUG("(successful) ");
                        bool addToExportData = true;
                        if (key == GPMF_KEY_DEVICE_ID)
                        {
                            addToExportData = false;
                            output_data.device_id.append(std::to_string(f[0]));
                        }

                        // GPMF_FormattedData(ms, f, sizeof(f), 0, samples);
                        for (uint32_t i=0; i<num; i++)
                        {
                            PRINT_DEBUG(" %d", f[i]);
                        }

                        if (addToExportData)
                        {
                            if (num == 1)
                            {
                                output_data.addExportData(convert_key_to_string(key), f[0]);
                            }
                            else
                            {
                                uint32_t p = 0;
                                std::string s;
                                s.append("[");
                                for (uint32_t i=0; i<samples; i++)
                                {
                                    if (i>0) s.append(",");
                                    s.append("[");
                                    for (uint32_t j=0; j<elements; j++)
                                    {
                                        if (j>0) s.append(",");
                                        s.append(std::to_string(f[p++]));
                                    }
                                    s.append("]");
                                }
                                s.append("]");

                                output_data.addExportData(convert_key_to_string(key), s, true);
                            }
                        }
                    }
                    else
                    {
                        PRINT_DEBUG("(some problems) ------- %u", ret);
                    }

                }
                else if (type == GPMF_TYPE_UNSIGNED_LONG || type == GPMF_TYPE_UNSIGNED_SHORT || type == GPMF_TYPE_UNSIGNED_BYTE)
                {
                    // value_ul = (uint32_t)data[0];
                    // printf("%lu", value_ul);

                    uint32_t num = samples * elements;
                    uint32_t f[num];
                    ret = GPMF_ScaledData(ms, f, sizeof(f), 0, samples, GPMF_TYPE_UNSIGNED_LONG);
                    if (GPMF_OK == ret)
                    {
                        PRINT_DEBUG("(successful) ");
                        bool addToExportData = true;
                        if (key == GPMF_KEY_DEVICE_ID)
                        {
                            addToExportData = false;
                            output_data.device_id.append(std::to_string(f[0]));
                        }

                        // GPMF_FormattedData(ms, f, sizeof(f), 0, samples);
                        for (uint32_t i=0; i<num; i++)
                        {
                            PRINT_DEBUG(" %u", f[i]);
                        }


                        if (addToExportData)
                        {
                            if (addToExportData)
                            {
                                if (num == 1)
                                {
                                    output_data.addExportData(convert_key_to_string(key), f[0]);
                                }
                                else
                                {
                                    uint32_t p = 0;
                                    std::string s;
                                    s.append("[");
                                    for (uint32_t i=0; i<samples; i++)
                                    {
                                        if (i>0) s.append(",");
                                        s.append("[");
                                        for (uint32_t j=0; j<elements; j++)
                                        {
                                            if (j>0) s.append(",");
                                            s.append(std::to_string(f[p++]));
                                        }
                                        s.append("]");
                                    }
                                    s.append("]");

                                    output_data.addExportData(convert_key_to_string(key), s, true);
                                }
                            }
                        }
                    }
                    else
                    {
                        PRINT_DEBUG("(some problems) ------- %u", ret);
                    }

                }
                else if (type == GPMF_TYPE_FLOAT)
                {
                    uint32_t num = samples * elements;
                    double f[num];
                    ret = GPMF_ScaledData(ms, f, sizeof(f), 0, samples, GPMF_TYPE_DOUBLE);
                    if (GPMF_OK == ret)
                    {
                        PRINT_DEBUG("(successful) ");

                        // GPMF_FormattedData(ms, f, sizeof(f), 0, samples);
                        for (uint32_t i=0; i<num; i++)
                        {
                            PRINT_DEBUG(" %f", f[i]);
                        }


                        if (num == 1)
                        {
                            output_data.addExportData(convert_key_to_string(key), f[0]);
                        }
                        else
                        {
                            uint32_t p = 0;
                            std::string s;
                            s.append("[");
                            for (uint32_t i=0; i<samples; i++)
                            {
                                if (i>0) s.append(",");
                                s.append("[");
                                for (uint32_t j=0; j<elements; j++)
                                {
                                    if (j>0) s.append(",");
                                    s.append(std::to_string(f[p++]));
                                }
                                s.append("]");
                            }
                            s.append("]");
                            output_data.addExportData(convert_key_to_string(key), s, true);
                        }
                    }
                    else
                    {
                        PRINT_DEBUG("(some problems) ------- %u", ret);
                    }
                }
                else if (type == GPMF_TYPE_UTC_DATE_TIME)
                {
                    std::string s;
                    s.append("20");
                    uint32_t p = 0;
                    s.append(1, data[p++]);
                    s.append(1, data[p++]);
                    s.append("-");
                    s.append(1, data[p++]);
                    s.append(1, data[p++]);
                    s.append("-");
                    s.append(1, data[p++]);
                    s.append(1, data[p++]);
                    s.append("T");
                    s.append(1, data[p++]);
                    s.append(1, data[p++]);
                    s.append(":");
                    s.append(1, data[p++]);
                    s.append(1, data[p++]);
                    s.append(":");
                    s.append(1, data[p++]);
                    s.append(1, data[p++]);
                    s.append(1, data[p++]);
                    s.append(1, data[p++]);
                    s.append(1, data[p++]);
                    s.append(1, data[p++]);

                    output_data.addExportData(convert_key_to_string(key), s);
                }
                else if (type == GPMF_TYPE_COMPLEX)
                {
                    uint32_t num = samples * elements;
                    double f[num];
                    ret = GPMF_ScaledData(ms, f, sizeof(f), 0, samples, GPMF_TYPE_DOUBLE);
                    if (GPMF_OK == ret)
                    {
                        // printf("(successful) ");
                        // // GPMF_FormattedData(ms, f, sizeof(f), 0, samples);
                        // for (uint32_t i=0; i<num; i++)
                        // {
                        //     printf(" %f", f[i]);
                        // }

                        double* ptr, * tmpbuffer = f;
                        ptr = f;
                        int pos = 0;

                        uint32_t p = 0;
                        std::string s;
                        s.append("[");
                        for (uint32_t i = 0; i < samples; i++)
                        {
                            if (i>0) s.append(",");
                            s.append("[");
                            for (uint32_t j = 0; j < elements; j++)
                            {
                                if (j>0) s.append(",");

                                if (type == GPMF_TYPE_STRING_ASCII)
                                {
                                    printf("%c", data[pos]);
                                    pos++;
                                    ptr++;
                                }
                                else if (type_samples == 0) //no TYPE structure
                                {
                                    PRINT_DEBUG("%.5f, ", *ptr++);
                                    PRINT_DEBUG("%.8f, ", *ptr++);
                                }
                                else if (output_data.st_type[j] != 'F')
                                {
                                    s.append(std::to_string(*ptr++));
                                    PRINT_DEBUG("%.8f, ", *ptr++);
                                    pos += GPMF_SizeofType((GPMF_SampleType)output_data.st_type[j]);
                                }
                                else if (type_samples && output_data.st_type[j] == GPMF_TYPE_FOURCC)
                                {
                                    ptr++;
                                    PRINT_DEBUG("%c%c%c%c, ", data[pos], data[pos + 1], data[pos + 2], data[pos + 3]);


                                    s.append("\"");
                                    s.append(std::string(1, data[pos]));
                                    s.append(std::string(1, data[pos+1]));
                                    s.append(std::string(1, data[pos+2]));
                                    s.append(std::string(1, data[pos+3]));
                                    s.append("\"");

                                    pos += GPMF_SizeofType((GPMF_SampleType)output_data.st_type[j]);
                                }
                            }
                            s.append("]");
                        }
                        s.append("]");

                        output_data.addExportData(convert_key_to_string(key), s, true);
                    }
                    else
                    {
                        printf("(some problems) ------- %u", ret);
                    }

                }
                else
                {
                    printf("%c%c%c%c - unhandled type: %u --------------", PRINTF_4CC(key), type);

                    std::cerr << convert_key_to_string(key) << " - unhandled type: " << type << " --------------  index=" << index << std::endl;
                }
                PRINT_DEBUG("\n");

                ret = GPMF_Next(ms, (GPMF_LEVELS)(GPMF_RECURSE_LEVELS | GPMF_TOLERANT));
                // ret = GPMF_Next(ms, GPMF_CURRENT_LEVEL|GPMF_TOLERANT);
            } while (GPMF_OK == ret);
            // printf("iteration done: %u\n", ret);

            // write previous data, if existent
            if (show_json && !output_data.export_data.empty())
            {
                // if there is any data imported, write it now before cleanup
                std::cout << output_data.buildJsonString() << std::endl;
            }

            GPMF_ResetState(ms);

            // the upper iteration loops util "end of buffer" - so clear the last non-ok state
            ret = GPMF_OK;
        }

        // ---------------------------------------------------
        // cleanup all data
        if (payloadres) FreePayloadResource(mp4handle, payloadres);
        if (ms) GPMF_Free(ms);
        CloseSource(mp4handle);

        if (json_with_array)
        {
            std::cout << "]" << std::endl;
        }

    }

    return 0;
}


void printHelp(char* name)
{
    printf("usage: %s <file_with_GPMF> <optional features>\n", name);
    // printf("       -a - %s all payloads\n", SHOW_ALL_PAYLOADS ? "disable" : "show");
    // printf("       -g - %s GPMF structure\n", SHOW_GPMF_STRUCTURE ? "disable" : "show");
    // printf("       -i - %s index of the payload\n", SHOW_PAYLOAD_INDEX ? "disable" : "show");
    // printf("       -s - %s scaled data\n", SHOW_SCALED_DATA ? "disable" : "show");
    // printf("       -c - %s computed sample rates\n", SHOW_COMPUTED_SAMPLERATES ? "disable" : "show");
    // printf("       -v - %s video framerate\n", SHOW_VIDEO_FRAMERATE ? "disable" : "show");
    // printf("       -t - %s time of the payload\n", SHOW_PAYLOAD_TIME ? "disable" : "show");
    printf("       -f - filename as part of output - current=%s\n", !json_with_filename ? "disabled" : "showing");
    printf("       -j - write json to console - current=%s\n", !show_json ? "disabled" : "showing");
    printf("       -js - json as stream (without comma) - current=%s\n", !json_as_stream ? "disabled" : "showing");
    printf("       -ja - json with array braces - current=%s\n", !json_with_array ? "disabled" : "showing");
    printf("       -h - this help\n");
    printf("       \n");
    printf("       ver 2.0\n");
}


int main(int argc, char* argv[])
{

    // get file return data
    if (argc < 2)
    {
        printHelp(argv[0]);
        return -1;
    }


    for (int i = 1; i < argc; i++)
    {
        if (argv[i][0] == '-') //feature switches
        {
            switch (argv[i][1])
            {
            case 'd':
                {
                    show_debug = !show_debug;
                    if (show_debug)  show_info = true;
                }
                break;
            case 'f': json_with_filename = !json_with_filename;	  break;
            case 'u': split_data = !split_data;	  break;
            case 's':
                {
                    i ++;
                    only_second = atol(argv[i]);
                    printf("using explicit second to export: %d\n", only_second);
                }
                break;
            case 'j':
                {
                    switch (argv[i][2])
                    {
                    case 's': json_as_stream = !json_as_stream;		break;
                    case 'a': json_with_array = !json_with_array;		break;
                    default:
                        show_json = !show_json;
                    }

                }
                break;
            case 'm': load_meta = !load_meta;				break;
            case 'h':
                printHelp(argv[0]);
                return 0;

                //
                //                case 'M':  mp4fuzzchanges = atoi(&argv[i][2]);	break;
                //                case 'G':  gpmffuzzchanges = atoi(&argv[i][2]); break;
                //                case 'F':  fuzzloopcount = atoi(&argv[i][2]);	break;
            }
        }
    }

    if (GPMF_ERR ret = readMP4File(argv[1], json_with_filename); ret != GPMF_OK)
    {
        return 0;
    }
    else
    {
        return 0;
    }
}
