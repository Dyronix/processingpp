cmake_minimum_required(VERSION 3.0...3.20 FATAL_ERROR)

project(inih)

SET(INIH_INCLUDE_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/include)
SET(INIH_SOURCE_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/src)

add_library(inih STATIC)

target_sources(inih PRIVATE ${INIH_SOURCE_DIRECTORY}/ini.c)
target_sources(inih PUBLIC ${INIH_INCLUDE_DIRECTORY}/ini.h)

target_include_directories(inih PRIVATE ${INIH_INCLUDE_DIRECTORY})

set_target_properties(inih PROPERTIES
                      FOLDER "third-party/INIH")