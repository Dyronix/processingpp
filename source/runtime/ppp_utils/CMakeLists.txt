add_library(processing_utils INTERFACE)

# Sources
target_sources(processing_utils INTERFACE
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_utils/public/log.h
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_utils/public/types.h
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_utils/public/hash.h)

# Public includes
target_include_directories(processing_utils INTERFACE ${SOURCE_THIRDPARTY_DIRECTORY}/fmt/include)

# Include self
target_include_directories(processing_utils INTERFACE ${SOURCE_RUNTIME_DIRECTORY}/ppp_utils/public)