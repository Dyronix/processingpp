cmake_minimum_required(VERSION 3.28)

project(processingpp)

option(PPP_ENABLE_TESTING "Enable testing for the Processing framework" ON)

MESSAGE(STATUS "PPP_ENABLE_TESTING: ${PPP_ENABLE_TESTING}")

# Use C++ 17 as a standard
set(CMAKE_CXX_STANDARD 17)

# Enable folder structure in the IDE
SET_PROPERTY(GLOBAL PROPERTY USE_FOLDERS ON)

# Set output directories for each configuration (Debug, Release, etc.)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/$<CONFIG>)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/$<CONFIG>)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/$<CONFIG>)

if(MSVC)
	# Remove the always defined _WINDOWS flag
	# CPP
	string (REPLACE "/D_WINDOWS" "" CMAKE_CUSTOM_CPP_FLAGS ${CMAKE_CXX_FLAGS})
	string (REPLACE "/DWIN32" "" CMAKE_CUSTOM_CPP_FLAGS ${CMAKE_CUSTOM_CPP_FLAGS}) 
	SET(CMAKE_CXX_FLAGS "${CMAKE_CUSTOM_CPP_FLAGS}" CACHE STRING "" FORCE)
	# C
	string (REPLACE "/D_WINDOWS" "" CMAKE_CUSTOM_C_FLAGS ${CMAKE_C_FLAGS})
	string (REPLACE "/DWIN32" "" CMAKE_CUSTOM_C_FLAGS ${CMAKE_CUSTOM_C_FLAGS})
	SET(CMAKE_C_FLAGS "${CMAKE_CUSTOM_C_FLAGS}" CACHE STRING "" FORCE)
endif()

# Debug postfix
SET (CMAKE_DEBUG_POSTFIX _d)

# Some easy access to folders within this repository
SET(SOURCE_RUNTIME_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/source/runtime)
SET(SOURCE_THIRDPARTY_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/source/third-party)
SET(SOURCE_EXAMPLES_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/source/examples)

MESSAGE(STATUS ${SOURCE_RUNTIME_DIRECTORY})
MESSAGE(STATUS ${SOURCE_THIRDPARTY_DIRECTORY})
MESSAGE(STATUS ${SOURCE_EXAMPLES_DIRECTORY})

add_subdirectory(${SOURCE_THIRDPARTY_DIRECTORY})
add_subdirectory(${SOURCE_RUNTIME_DIRECTORY})
add_subdirectory(${SOURCE_EXAMPLES_DIRECTORY})

# Add a custom target that always builds and runs the copy command
add_custom_target(copy-content ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_SOURCE_DIR}/content" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/content"
    COMMENT "Copying \"${CMAKE_SOURCE_DIR}/content\" folder to \"${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/content\""
    VERBATIM
)
# Add a custom target that always builds and runs the copy command
add_custom_target(copy-config ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_SOURCE_DIR}/config" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/config"
    COMMENT "Copying \"${CMAKE_SOURCE_DIR}/config\" folder to \"${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/config\""
    VERBATIM
)

set_property(TARGET copy-content PROPERTY FOLDER "programs")
set_property(TARGET copy-config PROPERTY FOLDER "programs")

if(PPP_ENABLE_TESTING)

	SET(SOURCE_TEST_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/source/test)
	
	MESSAGE(STATUS ${SOURCE_TEST_DIRECTORY})

	add_subdirectory(${SOURCE_TEST_DIRECTORY})

	# Run unit tests tests
	# Add a custom target that always builds and runs the batch script
	add_custom_target(unit-tests-run
		COMMAND ${CMAKE_COMMAND} -E cmake_echo_color --blue "Executing unit-tests-run.bat"
		COMMAND "${SOURCE_TEST_DIRECTORY}/unit-tests-run.bat" ${SOURCE_TEST_DIRECTORY}
		VERBATIM
	)

	set_property(TARGET unit-tests-run PROPERTY FOLDER "test/unit/tools")

	# Run integration tests
	# Add a custom target that always builds and runs the batch script
	add_custom_target(integration-tests-run
		COMMAND ${CMAKE_COMMAND} -E cmake_echo_color --blue "Executing integration-tests-run.bat"
		COMMAND "${SOURCE_TEST_DIRECTORY}/integration-tests-run.bat" ${SOURCE_TEST_DIRECTORY}
		VERBATIM
	)

	set_property(TARGET integration-tests-run PROPERTY FOLDER "test/integration/tools")

	# Generate new data for all the available tests
	# Add a custom target that always builds and runs the batch script
	add_custom_target(integration-tests-generate-data
		COMMAND ${CMAKE_COMMAND} -E cmake_echo_color --blue "Executing integration-tests-generate.bat"
		COMMAND "${SOURCE_TEST_DIRECTORY}/integration-tests-generate.bat" ${SOURCE_TEST_DIRECTORY}
		VERBATIM
	)

	set_property(TARGET integration-tests-generate-data PROPERTY FOLDER "test/integration/tools")

endif()