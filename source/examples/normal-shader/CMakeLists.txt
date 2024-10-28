add_executable(normal-shader)

target_sources(normal-shader PRIVATE 
    ${SOURCE_EXAMPLES_DIRECTORY}/normal-shader/private/normal-shader.cpp)

# Create the project filters
GROUPSOURCES(${SOURCE_EXAMPLES_DIRECTORY}/normal-shader/private private)

target_include_directories(normal-shader PRIVATE ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/public)

target_link_libraries(normal-shader processing_engine)

# Target properties
set_target_properties(normal-shader PROPERTIES
                      FOLDER "examples")