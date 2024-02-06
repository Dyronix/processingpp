add_executable(perlin-noise-flow-field)

target_sources(perlin-noise-flow-field PRIVATE 
    ${SOURCE_EXAMPLES_DIRECTORY}/perlin-noise-flow-field/private/particle.h
    ${SOURCE_EXAMPLES_DIRECTORY}/perlin-noise-flow-field/private/particle.cpp
    ${SOURCE_EXAMPLES_DIRECTORY}/perlin-noise-flow-field/private/perlin-noise-flow-field.cpp)

# Create the project filters
GROUPSOURCES(${SOURCE_EXAMPLES_DIRECTORY}/perlin-noise-flow-field/private private)

target_include_directories(perlin-noise-flow-field PRIVATE ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/public)

target_link_libraries(perlin-noise-flow-field processing_engine)

# Target properties
set_target_properties(perlin-noise-flow-field PROPERTIES
                      FOLDER "examples")