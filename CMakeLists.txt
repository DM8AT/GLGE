cmake_minimum_required(VERSION 3.10)
project(MAIN_PROJ)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(EXEC_NAME MAIN)

# Add your source files
set(SOURCES
    src/main.cpp
)

# Create executable or library
add_executable(${EXEC_NAME} ${SOURCES})

set_target_properties(${EXEC_NAME} PROPERTIES
    CXX_STANDARD 23
    CXX_STANDARD_REQUIRED ON
)

# add submodules
add_subdirectory(GLGE)

# Link libraries if needed
target_link_libraries(${EXEC_NAME} PRIVATE GLGE)