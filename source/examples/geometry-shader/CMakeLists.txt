add_executable(geometry-shader)

target_sources(geometry-shader PRIVATE 
    ${SOURCE_EXAMPLES_DIRECTORY}/geometry-shader/private/geometry-shader.cpp)

# Create the project filters
GROUPSOURCES(${SOURCE_EXAMPLES_DIRECTORY}/geometry-shader/private private)

target_include_directories(geometry-shader PRIVATE ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/public)

target_link_libraries(geometry-shader processing_engine)

# Target properties
set_target_properties(geometry-shader PROPERTIES
                      FOLDER "examples")