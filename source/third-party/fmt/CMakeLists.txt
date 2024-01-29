cmake_minimum_required(VERSION 3.0...3.20 FATAL_ERROR)

project(fmt)

SET(FMT_INCLUDE_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/include)
SET(FMT_SOURCE_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/src)

add_library(fmt INTERFACE)

target_sources(fmt PUBLIC 
    ${FMT_INCLUDE_DIRECTORY}/fmt/args.h 
    ${FMT_INCLUDE_DIRECTORY}/fmt/chrono.h 
    ${FMT_INCLUDE_DIRECTORY}/fmt/color.h 
    ${FMT_INCLUDE_DIRECTORY}/fmt/compile.h 
    ${FMT_INCLUDE_DIRECTORY}/fmt/core.h 
    ${FMT_INCLUDE_DIRECTORY}/fmt/format-inl.h 
    ${FMT_INCLUDE_DIRECTORY}/fmt/format.h 
    ${FMT_INCLUDE_DIRECTORY}/fmt/os.h 
    ${FMT_INCLUDE_DIRECTORY}/fmt/ostream.h 
    ${FMT_INCLUDE_DIRECTORY}/fmt/printf.h 
    ${FMT_INCLUDE_DIRECTORY}/fmt/ranges.h 
    ${FMT_INCLUDE_DIRECTORY}/fmt/std.h 
    ${FMT_INCLUDE_DIRECTORY}/fmt/xchar.h)

target_include_directories(fmt INTERFACE ${FMT_INCLUDE_DIRECTORY})

set_target_properties(fmt PROPERTIES
                      FOLDER "third-party/FMT")