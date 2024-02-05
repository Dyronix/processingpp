cmake_minimum_required(VERSION 3.0...3.20 FATAL_ERROR)

project(noise)

SET(NOISE_INCLUDE_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/include)
SET(NOISE_SOURCE_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/src)

add_library(noise INTERFACE)

target_sources(noise PUBLIC 
    ${NOISE_INCLUDE_DIRECTORY}/noise/perlin_noise.h)

target_include_directories(noise INTERFACE ${NOISE_INCLUDE_DIRECTORY})

set_target_properties(noise PROPERTIES
                      FOLDER "third-party/NOISE")