cmake_minimum_required(VERSION 3.14)
project(MAIN_PROJ)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Static GLGE archives are embedded into shared example plugin modules.
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

# ---------------------------------------------------------------------------
# Example plugin configuration
# ---------------------------------------------------------------------------
option(EXAMPLE_PLUGIN_BUILD_DEFAULT "Build the DefaultExample plugin module" ON)
option(UNIT_TEST_BUILD_DEFAULT "Build the unit test test modules" ON)

# Extend with additional `option(...)` + `add_example_plugin(...)` pairs.
set(EXAMPLE_PLUGIN_TARGETS "")
# Extend with additional `option(...)` + `add_unit_test(...)` pairs.
set(UNIT_TEST_TARGETS "")

# a function to add an example plugin
function(add_example_plugin target output_name)
    set(sources ${ARGN})
    if(sources STREQUAL "")
        message(FATAL_ERROR "add_example_plugin(${target}): no sources given")
    endif()

    add_library(${target} MODULE ${sources})
    set_target_properties(${target} PROPERTIES
        CXX_STANDARD 23
        CXX_STANDARD_REQUIRED ON
        PREFIX ""
        OUTPUT_NAME "${output_name}"
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/plugins"
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/plugins"
    )
    target_include_directories(${target} PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Examples
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Examples/Plugins
        ${CMAKE_CURRENT_SOURCE_DIR}/GLGE/include
    )
    # say that this is a plugin build
    target_compile_definitions(${target} PRIVATE EXAMPLE_PLUGIN_BUILD=1)

    # link GLGE
    target_link_libraries(${target} PRIVATE GLGE)
endfunction()

# a function to add a test
function(add_unit_test target output_name)
    set(sources ${ARGN})
    if(sources STREQUAL "")
        message(FATAL_ERROR "add_unit_test(${target}): no sources given")
    endif()

    add_library(${target} MODULE ${sources})
    set_target_properties(${target} PROPERTIES
        CXX_STANDARD 23
        CXX_STANDARD_REQUIRED ON
        PREFIX ""
        OUTPUT_NAME "${output_name}"
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/Tests"
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/Tests"
    )
    target_include_directories(${target} PRIVATE
        ${CMAKE_SOURCE_DIR}/src
        ${CMAKE_SOURCE_DIR}/src/Examples
        ${CMAKE_SOURCE_DIR}/src/Examples/Plugins
        ${CMAKE_SOURCE_DIR}/GLGE/include
    )

    # link GLGE
    target_link_libraries(${target} PRIVATE GLGE)
    # link testing
    target_link_libraries(${target} PRIVATE TEST_LIB)
endfunction()

# add GLGE
add_subdirectory(GLGE)
# add unit testing
add_subdirectory(${CMAKE_SOURCE_DIR}/src/Testing/UnitTestSystem/Test)

# add the default example
if(EXAMPLE_PLUGIN_BUILD_DEFAULT)
    # Default
    add_example_plugin(ExamplePlugin_DefaultExample DefaultExample
        src/Examples/Plugins/DefaultExample.cpp
    )
    list(APPEND EXAMPLE_PLUGIN_TARGETS ExamplePlugin_DefaultExample)
    # Windows
    add_example_plugin(ExamplePlugin_WindowExample WindowExample
        src/Examples/Plugins/WindowExample.cpp
    )
    list(APPEND EXAMPLE_PLUGIN_TARGETS ExamplePlugin_DefaultExample)
    # Assets
    add_example_plugin(ExamplePlugin_AssetExample AssetExample
        src/Examples/Plugins/AssetExample.cpp
    )
    list(APPEND EXAMPLE_PLUGIN_TARGETS ExamplePlugin_AssetExample)
endif()

# add unit tests
if (UNIT_TEST_BUILD_DEFAULT)
    # math test suite
    add_unit_test(UnitTest_MathSuite MathTestSuite
        src/Testing/Tests/MathTestSuite.cpp
    )
    list(APPEND UNIT_TEST_TARGETS UnitTest_MathSuite)
    # core test suite
    add_unit_test(UnitTest_CoreSuite CoreTestSuite
        src/Testing/Tests/CoreTestSuite.cpp
    )
    list(APPEND UNIT_TEST_TARGETS UnitTest_MathSuite)
endif()

# define the launcher executable
add_executable(MAIN src/main.cpp src/Examples/ExamplePluginLoader.cpp src/Testing/TestLauncher.cpp)

# setup the C++ standard to use
set_target_properties(MAIN PROPERTIES
    CXX_STANDARD 23
    CXX_STANDARD_REQUIRED ON
)

# add all required include directories
target_include_directories(MAIN PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Examples
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# add all dynamic libraries
if(CMAKE_DL_LIBS)
    target_link_libraries(MAIN PRIVATE ${CMAKE_DL_LIBS})
endif()
# add the test library
target_link_libraries(MAIN PRIVATE TEST_LIB)

# if example plugin targets exist, add them all and add them to compilation
if(EXAMPLE_PLUGIN_TARGETS)
    add_dependencies(MAIN ${EXAMPLE_PLUGIN_TARGETS})
    add_custom_command(TARGET MAIN POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_BINARY_DIR}/plugins"
        COMMENT "Ensure example plugin output directory exists"
    )
endif()

# if example plugin targets exist, add them all and add them to compilation
if(UNIT_TEST_TARGETS)
    add_dependencies(MAIN ${UNIT_TEST_TARGETS})
    add_custom_command(TARGET MAIN POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_BINARY_DIR}/Tests"
        COMMENT "Ensure unit test output directory exists"
    )
endif()
