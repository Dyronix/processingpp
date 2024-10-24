add_executable(shapes)

target_sources(shapes PRIVATE 
    ${SOURCE_EXAMPLES_DIRECTORY}/shapes/private/shapes.cpp)

# Create the project filters
GROUPSOURCES(${SOURCE_EXAMPLES_DIRECTORY}/shapes/private private)

target_include_directories(shapes PRIVATE ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/public)

target_link_libraries(shapes processing_engine)

# Target properties
set_target_properties(shapes PROPERTIES
                      FOLDER "examples")