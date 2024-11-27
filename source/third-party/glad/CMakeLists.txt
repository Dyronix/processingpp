cmake_minimum_required(VERSION 3.0...3.20 FATAL_ERROR)

project(glad)

SET(GLAD_INCLUDE_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/include)
SET(GLAD_SOURCE_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/src)

add_library(glad RETAINED)

target_sources(glad PRIVATE ${GLAD_SOURCE_DIRECTORY}/glad.c)
target_sources(glad PUBLIC ${GLAD_INCLUDE_DIRECTORY}/glad/glad.h ${GLAD_INCLUDE_DIRECTORY}/KHR/khrplatform.h)

target_include_directories(glad PRIVATE ${GLAD_INCLUDE_DIRECTORY})

set_target_properties(glad PROPERTIES
                      FOLDER "third-party/GLAD")