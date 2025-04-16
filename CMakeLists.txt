# default setup for CMake
cmake_minimum_required(VERSION 3.2...3.31)
# define C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# path to GLGE
set(GLGE_PATH src/GLGE)
# search SDL2
INCLUDE(FindPkgConfig)
PKG_SEARCH_MODULE(SDL2 REQUIRED sdl2)
PKG_SEARCH_MODULE(SDL2IMAGE REQUIRED SDL2_image>=2.0.0)
INCLUDE_DIRECTORIES(${SDL2_INCLUDE_DIRS} ${SDL2IMAGE_INCLUDE_DIRS})

# GLGE project initalisation
project(GLGE
    VERSION 0.1
    DESCRIPTION "A 3D game engine designed in C++ for C and C++"
    LANGUAGES CXX)
# list of all source files in GLGE
set(SOURCE_FILES_CPP
    # Core files
    ${GLGE_PATH}/GLGECore/GLGEInstance.cpp
    ${GLGE_PATH}/GLGECore/GraphicAPIs/GLGEApi.cpp
    ${GLGE_PATH}/GLGECore/Logging/GLGEMessage.cpp
    ${GLGE_PATH}/GLGECore/Time/GLGEClock.cpp
    ${GLGE_PATH}/GLGECore/Time/GLGELimiter.cpp
    ${GLGE_PATH}/GLGECore/Files/GLGEFile.cpp

    # Graphic files
    ${GLGE_PATH}/GLGEGraphic/GLGEWindow.cpp
    ${GLGE_PATH}/GLGEGraphic/GLGEGraphicShared.cpp
    # Subelement : Graphic API
    ${GLGE_PATH}/GLGEGraphic/GraphicAPI/GLGEGraphicAPI_Instance.cpp
    ${GLGE_PATH}/GLGEGraphic/GraphicAPI/GLGEGraphicAPI_Texture.cpp
    ${GLGE_PATH}/GLGEGraphic/GraphicAPI/GLGEGraphicAPI_Framebuffer.cpp
    ${GLGE_PATH}/GLGEGraphic/GraphicAPI/GLGEGraphicAPI_CommandBuffer.cpp

    # Math files
    ${GLGE_PATH}/GLGEMath/Vectors/GLGEDoubleVec.cpp
    ${GLGE_PATH}/GLGEMath/Vectors/GLGEFloatVec.cpp
    ${GLGE_PATH}/GLGEMath/Vectors/GLGEIntVec.cpp
    ${GLGE_PATH}/GLGEMath/Vectors/GLGEUIntVec.cpp
)
# add the executables to the library
add_library(GLGE STATIC ${SOURCE_FILES_CPP})
# specify compiler settings for default
target_compile_definitions(GLGE PRIVATE USE_NEW_AUDIO_ENGINE)
target_compile_options(GLGE PRIVATE -Wall -Wextra -Wpedantic -Wno-unused-function -ggdb)
# add SDL2
TARGET_LINK_LIBRARIES(GLGE ${SDL2_LIBRARIES} ${SDL2IMAGE_LIBRARIES})
# add vulkan
TARGET_LINK_LIBRARIES(GLGE vulkan)
# add OpenGL
TARGET_LINK_LIBRARIES(GLGE GL)
# enable threading
target_compile_options(GLGE PRIVATE -pthread)
target_link_options(GLGE PRIVATE -pthread)

# C - Test:
    project(TEST_CPP
    VERSION 1.0
    DESCRIPTION "A simple test for GLGE in C++"
    LANGUAGES C)
    # executable for test project
    add_executable(TEST_C
    src/main.c)
    # define compiler settings
    target_compile_definitions(TEST_C PRIVATE USE_NEW_AUDIO_ENGINE)
    target_compile_options(TEST_C PRIVATE -Wall -Wextra -Wpedantic -ggdb)
    # set the C version
    set_property(TARGET TEST_C PROPERTY C_STANDARD 99)
    # add GLGE
    target_link_libraries(TEST_C GLGE)

# C++ - Test:
    project(TEST_CPP
        VERSION 1.0
        DESCRIPTION "A simple test for GLGE in C++"
        LANGUAGES CXX)
    # executable for test project
    add_executable(TEST_CPP
        src/main.cpp)
    # define compiler settings
    target_compile_definitions(TEST_CPP PRIVATE USE_NEW_AUDIO_ENGINE)
    target_compile_options(TEST_CPP PRIVATE -Wall -Wextra -Wpedantic -ggdb)
    # add GLGE
    target_link_libraries(TEST_CPP GLGE)