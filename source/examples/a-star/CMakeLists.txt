add_executable(a-star)

target_sources(a-star PRIVATE 
    ${SOURCE_EXAMPLES_DIRECTORY}/a-star/private/a-star-grid.h
    ${SOURCE_EXAMPLES_DIRECTORY}/a-star/private/a-star-grid-drawer.h
    ${SOURCE_EXAMPLES_DIRECTORY}/a-star/private/a-star.cpp)

# Create the project filters
GROUPSOURCES(${SOURCE_EXAMPLES_DIRECTORY}/a-star/private private)

target_include_directories(a-star PRIVATE ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/public)

target_link_libraries(a-star processing_engine)

# Target properties
set_target_properties(a-star PROPERTIES
                      FOLDER "examples")