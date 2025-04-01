cmake_minimum_required (VERSION 3.8.2)
project (gpmf-json-exporter)

set(CMAKE_SUPPRESS_REGENERATION true)
set(CMAKE_CONFIGURATION_TYPES "Debug;Release")


add_subdirectory("externals/gpmf-parser")

file(GLOB EXPORTER_SOURCES
        src/main.cpp

        "externals/gpmf-parser/demo/GPMF_mp4reader.c"
)

add_executable(GPMF_EXPORTER_BIN ${EXPORTER_SOURCES}
        src/coutputdata.cpp
        src/coutputdata.h
        src/helper_functions.cpp
        src/helper_functions.h
)
set_target_properties(GPMF_EXPORTER_BIN PROPERTIES OUTPUT_NAME "${PROJECT_NAME}")
target_link_libraries( GPMF_EXPORTER_BIN GPMF_PARSER_LIB )

target_include_directories(GPMF_EXPORTER_BIN PRIVATE
        "externals/gpmf-parser"
        "externals/gpmf-parser/demo"
)
