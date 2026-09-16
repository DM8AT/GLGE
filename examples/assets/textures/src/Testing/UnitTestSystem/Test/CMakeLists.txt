cmake_minimum_required(VERSION 3.14)
project(TEST_LIB_PROJ)

add_library(TEST_LIB_INTERN src/TestExecImpl.cpp)
add_library(TEST_LIB INTERFACE)

target_include_directories(TEST_LIB_INTERN PUBLIC include)

set_target_properties(TEST_LIB_INTERN PROPERTIES 
                      CMAKE_CXX_STANDARD 23
                      CMAKE_CXX_STANDARD_REQUIRED ON
                      CMAKE_C_STANDARD 23
                      CMAKE_C_STANDARD_REQUIRED ON
                      )
                      
add_executable(TestWorker src/Worker.cpp)
set_target_properties(TestWorker PROPERTIES 
                      RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/TestLib
                      CMAKE_CXX_STANDARD 23
                      CMAKE_CXX_STANDARD_REQUIRED ON
                      CMAKE_C_STANDARD 23
                      CMAKE_C_STANDARD_REQUIRED ON
                      )
target_link_libraries(TestWorker PRIVATE TEST_LIB_INTERN)

add_dependencies(TEST_LIB TestWorker)
target_link_libraries(TEST_LIB INTERFACE TEST_LIB_INTERN)