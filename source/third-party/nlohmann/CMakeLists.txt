cmake_minimum_required(VERSION 3.0...3.20 FATAL_ERROR)

project(nlohmann)

SET(NLOHMANN_INCLUDE_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/include)

add_library(nlohmann INTERFACE)

target_sources(nlohmann PUBLIC ${NLOHMANN_INCLUDE_DIRECTORY}/nlohmann/json.hpp ${NLOHMANN_INCLUDE_DIRECTORY}/nlohmann/json_fwd.hpp)

target_include_directories(nlohmann INTERFACE ${NLOHMANN_INCLUDE_DIRECTORY})

set_target_properties(nlohmann PROPERTIES
                      FOLDER "third-party/NLOHMANN")