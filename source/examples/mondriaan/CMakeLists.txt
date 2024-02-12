add_executable(mondriaan)

target_sources(mondriaan PRIVATE 
    ${SOURCE_EXAMPLES_DIRECTORY}/mondriaan/private/mondriaan.cpp)

# Create the project filters
GROUPSOURCES(${SOURCE_EXAMPLES_DIRECTORY}/mondriaan/private private)

target_include_directories(mondriaan PRIVATE ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/public)

target_link_libraries(mondriaan processing_engine)

# Target properties
set_target_properties(mondriaan PROPERTIES
                      FOLDER "examples")