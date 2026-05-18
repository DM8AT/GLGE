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

# Extend with additional `option(...)` + `add_example_plugin(...)` pairs.
set(EXAMPLE_PLUGIN_TARGETS "")

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
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Examples
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Examples/Plugins
        ${CMAKE_CURRENT_SOURCE_DIR}/GLGE/include
    )
    # say that this is a plugin build
    target_compile_definitions(${target} PRIVATE EXAMPLE_PLUGIN_BUILD=1)

    # link GLGE
    target_link_libraries(${target} PRIVATE GLGE)
endfunction()

# add GLGE
add_subdirectory(GLGE)

# add the default example
if(EXAMPLE_PLUGIN_BUILD_DEFAULT)
    add_example_plugin(ExamplePlugin_DefaultExample DefaultExample
        src/Examples/Plugins/DefaultExample.cpp
    )
    list(APPEND EXAMPLE_PLUGIN_TARGETS ExamplePlugin_DefaultExample)
endif()

# define the launcher executable
add_executable(MAIN src/main.cpp src/Examples/ExamplePluginLoader.cpp)

# setup the C++ standard to use
set_target_properties(MAIN PROPERTIES
    CXX_STANDARD 23
    CXX_STANDARD_REQUIRED ON
)

# add all required include directories
target_include_directories(MAIN PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Examples
)

# add all dynamic libraries
if(CMAKE_DL_LIBS)
    target_link_libraries(MAIN PRIVATE ${CMAKE_DL_LIBS})
endif()

# if example plugin targets exist, add them all and add them to compilation
if(EXAMPLE_PLUGIN_TARGETS)
    add_dependencies(MAIN ${EXAMPLE_PLUGIN_TARGETS})
    add_custom_command(TARGET MAIN POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_BINARY_DIR}/plugins"
        COMMENT "Ensure example plugin output directory exists"
    )
endif()
