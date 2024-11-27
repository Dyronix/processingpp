cmake_minimum_required(VERSION 3.0...3.20 FATAL_ERROR)

project(stb)

SET(STB_INCLUDE_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/include)
SET(STB_SOURCE_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/src)

add_library(stb RETAINED)

target_sources(stb PRIVATE ${STB_SOURCE_DIRECTORY}/stb_image.cpp ${STB_SOURCE_DIRECTORY}/stb_image_write.cpp)
target_sources(stb PUBLIC ${STB_INCLUDE_DIRECTORY}/stb/stb_image.h ${STB_INCLUDE_DIRECTORY}/stb/stb_image_write.h)

target_include_directories(stb PRIVATE ${STB_INCLUDE_DIRECTORY})

set_target_properties(stb PROPERTIES
                      FOLDER "third-party/STB")