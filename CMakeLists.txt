cmake_minimum_required(VERSION 3.10)
project(MAIN_PROJ)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Create executable or library
add_executable(MAIN src/main.cpp)

set_target_properties(MAIN PROPERTIES
    CXX_STANDARD 23
    CXX_STANDARD_REQUIRED ON
)

# create a helper library
add_library(MAIN_LIB 
    src/OpenGL/gl_example1.cpp
    src/Vulkan/vk_example1.cpp
)
set_target_properties(MAIN_LIB PROPERTIES CXX_STANDARD 23 CXX_STANDARD_REQUIRED ON)
target_link_libraries(MAIN PUBLIC MAIN_LIB)

# add submodules
add_subdirectory(GLGE)

# Link libraries if needed
target_link_libraries(MAIN_LIB PRIVATE GLGE)