add_executable(raycasting)

target_sources(raycasting PRIVATE 
    ${SOURCE_EXAMPLES_DIRECTORY}/raycasting/private/boundary.h
    ${SOURCE_EXAMPLES_DIRECTORY}/raycasting/private/boundary.cpp
    ${SOURCE_EXAMPLES_DIRECTORY}/raycasting/private/ray.h
    ${SOURCE_EXAMPLES_DIRECTORY}/raycasting/private/ray.cpp
    ${SOURCE_EXAMPLES_DIRECTORY}/raycasting/private/particle.h
    ${SOURCE_EXAMPLES_DIRECTORY}/raycasting/private/particle.cpp
    ${SOURCE_EXAMPLES_DIRECTORY}/raycasting/private/raycasting.cpp)

# Create the project filters
GROUPSOURCES(${SOURCE_EXAMPLES_DIRECTORY}/raycasting/private private)

target_include_directories(raycasting PRIVATE ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/public)

target_link_libraries(raycasting processing_engine)

# Target properties
set_target_properties(raycasting PROPERTIES
                      FOLDER "examples")