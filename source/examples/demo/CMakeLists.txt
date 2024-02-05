add_executable(demo)

target_sources(demo PRIVATE 
    ${SOURCE_EXAMPLES_DIRECTORY}/demo/private/demo.cpp)

# Create the project filters
GROUPSOURCES(${SOURCE_EXAMPLES_DIRECTORY}/demo/private private)

target_include_directories(demo PRIVATE ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/public)

target_link_libraries(demo processing_engine)

# Target properties
set_target_properties(demo PROPERTIES
                      FOLDER "examples")