add_library(processing_framework STATIC)

target_sources(processing_framework PRIVATE 
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_framework/private/sketch.cpp
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_framework/private/framework.cpp)

target_sources(processing_framework PUBLIC 
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_framework/public/framework.h
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_framework/public/sketch.h)

# Create the project filters
GROUPSOURCES(${SOURCE_RUNTIME_DIRECTORY}/ppp_framework/private private)
GROUPSOURCES(${SOURCE_RUNTIME_DIRECTORY}/ppp_framework/public public)

target_include_directories(processing_framework PRIVATE ${SOURCE_RUNTIME_DIRECTORY}/ppp_framework/public)
target_include_directories(processing_framework PRIVATE ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/public)

target_link_libraries(processing_framework processing_engine)

# Target properties
set_target_properties(processing_framework PROPERTIES
                      FOLDER "modules/runtime")