cmake_minimum_required(VERSION 3.14)
project(MAIN_PROJ)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_executable(MAIN src/main.cpp)

add_subdirectory(Test)

target_link_libraries(MAIN TEST_LIB)

add_library(Test1 SHARED src/Test1.cpp)
set_target_properties(Test1 PROPERTIES 
                      LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/TestLib
                      RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/TestLib
                      CMAKE_CXX_STANDARD 23
                      CMAKE_CXX_STANDARD_REQUIRED ON
                      CMAKE_C_STANDARD 23
                      CMAKE_C_STANDARD_REQUIRED ON
                      CXX_VISIBILITY_PRESET hidden
                      VISIBILITY_INLINES_HIDDEN YES
                      )
target_link_libraries(Test1 PUBLIC TEST_LIB)
add_dependencies(MAIN Test1)
target_compile_definitions(Test1 PRIVATE TEST_BUILD=1)

add_dependencies(MAIN TestWorker)

if(MSVC)
    set_target_properties(MAIN PROPERTIES
        VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
    )
endif()