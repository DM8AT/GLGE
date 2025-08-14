#  $$$$$$\  $$$$$$$$\ $$$$$$$$\ $$\   $$\ $$$$$$$\  
# $$  __$$\ $$  _____|\__$$  __|$$ |  $$ |$$  __$$\ 
# $$ /  \__|$$ |         $$ |   $$ |  $$ |$$ |  $$ |
# \$$$$$$\  $$$$$\       $$ |   $$ |  $$ |$$$$$$$  |
#  \____$$\ $$  __|      $$ |   $$ |  $$ |$$  ____/ 
# $$\   $$ |$$ |         $$ |   $$ |  $$ |$$ |      
# \$$$$$$  |$$$$$$$$\    $$ |   \$$$$$$  |$$ |      
#  \______/ \________|   \__|    \______/ \__|      

# default setup for CMake
cmake_minimum_required(VERSION 3.10)
# define C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
# store the compiler options
set(COMPILER_OPTIONS -Wall -Wextra -Wpedantic -Wno-unused-function)# -ggdb)
# Copy to compiler and link libraries for more debug info
# -fsanitize=address
# store some libraries for linking
set(LINK_LIBS )

# path to GLGE
set(GLGE_PATH src/GLGE)
# add pugixml - it is neccecery for loading assets
set(PUGIXML_LIB ../${GLGE_PATH}/GLGE3rdParty/pugixml/pugixml_build)
# store all pathes for GLGE libraries
set(GLGE_LIBRARY_PATHS ${PUGIXML_LIB})

#  $$$$$$\  $$$$$$$$\ $$$$$$$$\ $$$$$$$$\ $$$$$$\ $$\   $$\  $$$$$$\   $$$$$$\  
# $$  __$$\ $$  _____|\__$$  __|\__$$  __|\_$$  _|$$$\  $$ |$$  __$$\ $$  __$$\ 
# $$ /  \__|$$ |         $$ |      $$ |     $$ |  $$$$\ $$ |$$ /  \__|$$ /  \__|
# \$$$$$$\  $$$$$\       $$ |      $$ |     $$ |  $$ $$\$$ |$$ |$$$$\ \$$$$$$\  
#  \____$$\ $$  __|      $$ |      $$ |     $$ |  $$ \$$$$ |$$ |\_$$ | \____$$\ 
# $$\   $$ |$$ |         $$ |      $$ |     $$ |  $$ |\$$$ |$$ |  $$ |$$\   $$ |
# \$$$$$$  |$$$$$$$$\    $$ |      $$ |   $$$$$$\ $$ | \$$ |\$$$$$$  |\$$$$$$  |
#  \______/ \________|   \__|      \__|   \______|\__|  \__| \______/  \______/ 

# enable / disable Dear ImGui
set(GLGE_USE_DEAR_IMGUI true)
# enable / disable Assimp
set(GLGE_USE_ASSIMP false)
# include / exclude graphics
set(GLGE_INCLUDE_GRAPHICS true)


#  $$$$$$\  $$$$$$$\  $$$$$$$\        $$$$$$$\                      $$\               
# $$ ___$$\ $$  __$$\ $$  __$$\       $$  __$$\                     $$ |              
# \_/   $$ |$$ |  $$ |$$ |  $$ |      $$ |  $$ |$$$$$$\   $$$$$$\ $$$$$$\   $$\   $$\ 
#   $$$$$ / $$$$$$$  |$$ |  $$ |      $$$$$$$  |\____$$\ $$  __$$\\_$$  _|  $$ |  $$ |
#   \___$$\ $$  __$$< $$ |  $$ |      $$  ____/ $$$$$$$ |$$ |  \__| $$ |    $$ |  $$ |
# $$\   $$ |$$ |  $$ |$$ |  $$ |      $$ |     $$  __$$ |$$ |       $$ |$$\ $$ |  $$ |
# \$$$$$$  |$$ |  $$ |$$$$$$$  |      $$ |     \$$$$$$$ |$$ |       \$$$$  |\$$$$$$$ |
#  \______/ \__|  \__|\_______/       \__|      \_______|\__|        \____/  \____$$ |
#                                                                           $$\   $$ |
#                                                                           \$$$$$$  |
#                                                                            \______/ 

# check for Dear ImGui
if (GLGE_USE_DEAR_IMGUI)

    # load all source files
    set(DEAR_IMGUI_SOURCE
            # add all generic header files
            ${GLGE_PATH}/GLGE3rdParty/imgui/imgui.cpp
            ${GLGE_PATH}/GLGE3rdParty/imgui/imgui_draw.cpp
            ${GLGE_PATH}/GLGE3rdParty/imgui/imgui_tables.cpp
            ${GLGE_PATH}/GLGE3rdParty/imgui/imgui_widgets.cpp
            ${GLGE_PATH}/GLGE3rdParty/imgui/imgui.cpp

            # add all needed implementation files
            ${GLGE_PATH}/GLGE3rdParty/imgui/backends/imgui_impl_opengl3.cpp
            ${GLGE_PATH}/GLGE3rdParty/imgui/backends/imgui_impl_sdl2.cpp
        )
else()

    # else load emtpy as the source files
    set(DEAR_IMGUI_SOURCE )

endif() # end of Dear ImGui file loading

# check for Assimp
if (GLGE_USE_ASSIMP)
    # store the path to the Assimp library
    set(ASSIMP_LIB ${GLGE_PATH}/GLGE3rdParty/assimp/lib/)
endif() # end of assimp addition

# SDL2 Can not be disabled since it is neccecery for GLGE
# search SDL2
INCLUDE(FindPkgConfig)
PKG_SEARCH_MODULE(SDL2 REQUIRED sdl2)
PKG_SEARCH_MODULE(SDL2IMAGE REQUIRED SDL2_image>=2.0.0)
INCLUDE_DIRECTORIES(${SDL2_INCLUDE_DIRS} ${SDL2IMAGE_INCLUDE_DIRS})

#  $$$$$$\  $$\       $$$$$$\  $$$$$$$$\ 
# $$  __$$\ $$ |     $$  __$$\ $$  _____|
# $$ /  \__|$$ |     $$ /  \__|$$ |      
# $$ |$$$$\ $$ |     $$ |$$$$\ $$$$$\    
# $$ |\_$$ |$$ |     $$ |\_$$ |$$  __|   
# $$ |  $$ |$$ |     $$ |  $$ |$$ |      
# \$$$$$$  |$$$$$$$$\\$$$$$$  |$$$$$$$$\ 
#  \______/ \________|\______/ \________|

# GLGE project initalisation
project(GLGE
    VERSION 0.1
    DESCRIPTION "A 3D game engine designed in C++ for C and C++"
    LANGUAGES CXX)
# make the project independend from the location
# set_property(TARGET ${objs} PROPERTY POSITION_INDEPENDENT_CODE 1)
# list of all source files in GLGE
set(SOURCE_FILES_CPP
    # Core files
    ${GLGE_PATH}/GLGECore/GLGEInstance.cpp
    ${GLGE_PATH}/GLGECore/APIs/GLGEApi.cpp
    ${GLGE_PATH}/GLGECore/Logging/GLGEMessage.cpp
    ${GLGE_PATH}/GLGECore/Time/GLGEClock.cpp
    ${GLGE_PATH}/GLGECore/Time/GLGELimiter.cpp
    ${GLGE_PATH}/GLGECore/Files/GLGEFile.cpp
    ${GLGE_PATH}/GLGECore/Files/GLGEPath.cpp
    ${GLGE_PATH}/GLGECore/GLGELayers.cpp
    ${GLGE_PATH}/GLGECore/Color/GLGEColor.cpp
    # Subelement : Assets
    ${GLGE_PATH}/GLGECore/Assets/GLGEAsset.cpp
    ${GLGE_PATH}/GLGECore/Assets/GLGEAssetManager.cpp
    ${GLGE_PATH}/GLGECore/Assets/GLGEAssetObject.cpp
    # Subelement : Geometry
    ${GLGE_PATH}/GLGECore/Geometry/GLGEVertex.cpp
    ${GLGE_PATH}/GLGECore/Geometry/GLGEObject.cpp
        # Sub-Subelement : Worlds
        ${GLGE_PATH}/GLGECore/Geometry/World/GLGEWorld.cpp
        # Sub-Subelement : Mesh
        ${GLGE_PATH}/GLGECore/Geometry/Mesh/GLGEMesh.cpp
        # Sub-Subelement : Primitives
        ${GLGE_PATH}/GLGECore/Geometry/Primitives/GLGE_AABB.cpp
        ${GLGE_PATH}/GLGECore/Geometry/Primitives/GLGESphere.cpp
        ${GLGE_PATH}/GLGECore/Geometry/Primitives/GLGERay.cpp
        ${GLGE_PATH}/GLGECore/Geometry/Primitives/GLGETriangle.cpp
        ${GLGE_PATH}/GLGECore/Geometry/Primitives/GLGECollision.cpp


    # Math files
    # Vectors
    ${GLGE_PATH}/GLGEMath/Vectors/GLGEDoubleVec.cpp
    ${GLGE_PATH}/GLGEMath/Vectors/GLGEFloatVec.cpp
    ${GLGE_PATH}/GLGEMath/Vectors/GLGEIntVec.cpp
    ${GLGE_PATH}/GLGEMath/Vectors/GLGEUIntVec.cpp
    # Matricies
    ${GLGE_PATH}/GLGEMath/Matricies/GLGEDoubleMatricies.cpp
    ${GLGE_PATH}/GLGEMath/Matricies/GLGEFloatMatricies.cpp
    ${GLGE_PATH}/GLGEMath/Matricies/GLGEIntMatricies.cpp
    ${GLGE_PATH}/GLGEMath/Matricies/GLGEUIntMatricies.cpp
    # Rotation
    ${GLGE_PATH}/GLGEMath/Angles/GLGEEuler.cpp
    ${GLGE_PATH}/GLGEMath/Angles/GLGERotation.cpp
    # Quaternions
    ${GLGE_PATH}/GLGEMath/Imaginary/GLGEQuaternion.cpp

    # 3rd Party
        # Dear ImGui
        ${DEAR_IMGUI_SOURCE}
)

# check if the graphics should be included
if (GLGE_INCLUDE_GRAPHICS)
    # add the source files for the graphic core
    set(SOURCE_FILES_CPP ${SOURCE_FILES_CPP}
            
        # Graphic files
        ${GLGE_PATH}/GLGEGraphic/GLGEWindow.cpp
        ${GLGE_PATH}/GLGEGraphic/GLGEGraphicShared.cpp
        ${GLGE_PATH}/GLGEGraphic/GLGETexture.cpp
        ${GLGE_PATH}/GLGEGraphic/GLGEFramebuffer.cpp
        ${GLGE_PATH}/GLGEGraphic/GLGERenderPipeline.cpp
        ${GLGE_PATH}/GLGEGraphic/GLGEBuffer.cpp
        ${GLGE_PATH}/GLGEGraphic/GLGEKeyboard.cpp
        ${GLGE_PATH}/GLGEGraphic/GLGERenderMaterial.cpp
        ${GLGE_PATH}/GLGEGraphic/GLGERenderMesh.cpp
        ${GLGE_PATH}/GLGEGraphic/GLGERenderVertexLayout.cpp
        ${GLGE_PATH}/GLGEGraphic/GLGEObjectRenderer.cpp
        ${GLGE_PATH}/GLGEGraphic/GLGECamera.cpp
        # Assets
        ${GLGE_PATH}/GLGEGraphic/Assets/GLGEAssetTexture.cpp
        ${GLGE_PATH}/GLGEGraphic/Assets/GLGEAssetShader.cpp
        # Shader stuff
        ${GLGE_PATH}/GLGEGraphic/Shader/GLGEShader.cpp
        ${GLGE_PATH}/GLGEGraphic/Shader/GLGEShaderProcessor.cpp
            # Shader extensions
            ${GLGE_PATH}/GLGEGraphic/Shader/shaderExtensionsCommon/Core/GLGE_MainStatement.cpp
            ${GLGE_PATH}/GLGEGraphic/Shader/shaderExtensionsCommon/Core/GLGE_VersionStatement.cpp
            ${GLGE_PATH}/GLGEGraphic/Shader/shaderExtensionsCommon/Util/GLGE_IncludeStatement.cpp
        # Subelement : Graphic API
        ${GLGE_PATH}/GLGEGraphic/GraphicAPI/GLGEGraphicAPI_Instance.cpp
        ${GLGE_PATH}/GLGEGraphic/GraphicAPI/GLGEGraphicAPI_Texture.cpp
        ${GLGE_PATH}/GLGEGraphic/GraphicAPI/GLGEGraphicAPI_Framebuffer.cpp
        ${GLGE_PATH}/GLGEGraphic/GraphicAPI/GLGEGraphicAPI_Window.cpp
        ${GLGE_PATH}/GLGEGraphic/GraphicAPI/GLGEGraphicAPI_CommandBuffer.cpp
        ${GLGE_PATH}/GLGEGraphic/GraphicAPI/GLGEGraphicAPI_Shader.cpp
        ${GLGE_PATH}/GLGEGraphic/GraphicAPI/GLGEGraphicAPI_RenderPipeline.cpp
        ${GLGE_PATH}/GLGEGraphic/GraphicAPI/GLGEGraphicAPI_MemoryArena.cpp
            # Graphic API: OpenGL 4.6
            ${GLGE_PATH}/GLGEGraphic/GraphicAPI/GraphicAPIs/API_OpenGL4_6/GLGE_OGL4_6_Instance.cpp
            ${GLGE_PATH}/GLGEGraphic/GraphicAPI/GraphicAPIs/API_OpenGL4_6/GLGE_OGL4_6_Window.cpp
            ${GLGE_PATH}/GLGEGraphic/GraphicAPI/GraphicAPIs/API_OpenGL4_6/GLGE_OGL4_6_CommandBuffer.cpp
            ${GLGE_PATH}/GLGEGraphic/GraphicAPI/GraphicAPIs/API_OpenGL4_6/GLGE_OGL4_6_Texture.cpp
            ${GLGE_PATH}/GLGEGraphic/GraphicAPI/GraphicAPIs/API_OpenGL4_6/GLGE_OGL4_6_Shader.cpp
            ${GLGE_PATH}/GLGEGraphic/GraphicAPI/GraphicAPIs/API_OpenGL4_6/GLGE_OGL4_6_RenderPipeline.cpp
            ${GLGE_PATH}/GLGEGraphic/GraphicAPI/GraphicAPIs/API_OpenGL4_6/GLGE_OGL4_6_Framebuffer.cpp
            ${GLGE_PATH}/GLGEGraphic/GraphicAPI/GraphicAPIs/API_OpenGL4_6/GLGE_OGL4_6_MemoryArena.cpp
            ${GLGE_PATH}/GLGEGraphic/GraphicAPI/GraphicAPIs/API_OpenGL4_6/GLGE_OGL4_6_VertexLayout.cpp
    )
# end of graphic dependend source code
endif()
# add the executables to the library
add_library(GLGE STATIC ${SOURCE_FILES_CPP})
# specify compiler settings for default
target_compile_definitions(GLGE PRIVATE USE_NEW_AUDIO_ENGINE)
target_compile_options(GLGE PRIVATE ${COMPILER_OPTIONS})
# add all library pathes
target_link_libraries(GLGE PRIVATE -L${GLGE_LIBRARY_PATHS})
# check for imgui to add the include directory for imgui
if (GLGE_USE_DEAR_IMGUI)
    # set the include directory
    target_include_directories(GLGE PRIVATE ${GLGE_PATH}/GLGE3rdParty/imgui)
endif()
# check if the graphics should be included to add graphic specific libraries
if (GLGE_INCLUDE_GRAPHICS)
    # add SDL2
    TARGET_LINK_LIBRARIES(GLGE PRIVATE SDL2 SDL2_image)
    # add vulkan
    TARGET_LINK_LIBRARIES(GLGE PRIVATE vulkan)
    # add OpenGL
    TARGET_LINK_LIBRARIES(GLGE PRIVATE GL)
    # add GLEW
    TARGET_LINK_LIBRARIES(GLGE PRIVATE GLEW ${LINK_LIBS})

    # check if Assimp should be added
    if (GLGE_USE_ASSIMP)
        # add the library as an include directory
        target_link_libraries(GLGE PRIVATE -L${ASSIMP_LIB})
        # add the assimp library to the include stream
        TARGET_LINK_LIBRARIES(GLGE PRIVATE assimp)
    endif()

# end of graphic specific libraries
endif()
TARGET_LINK_LIBRARIES(GLGE PUBLIC pugixml)

# enable threading
target_compile_options(GLGE PUBLIC -pthread)
target_link_options(GLGE PUBLIC -pthread)

# $$$$$$$$\                    $$\               
# \__$$  __|                   $$ |              
#    $$ | $$$$$$\   $$$$$$$\ $$$$$$\    $$$$$$$\ 
#    $$ |$$  __$$\ $$  _____|\_$$  _|  $$  _____|
#    $$ |$$$$$$$$ |\$$$$$$\    $$ |    \$$$$$$\  
#    $$ |$$   ____| \____$$\   $$ |$$\  \____$$\ 
#    $$ |\$$$$$$$\ $$$$$$$  |  \$$$$  |$$$$$$$  |
#    \__| \_______|\_______/    \____/ \_______/ 

# Unused
# # C - Test:
#     project(TEST_CPP
#     VERSION 1.0
#     DESCRIPTION "A simple test for GLGE in C++"
#     LANGUAGES C)
#     # executable for test project
#     add_executable(TEST_C
#     src/main.c)
#     # define compiler settings
#     target_compile_definitions(TEST_C PRIVATE USE_NEW_AUDIO_ENGINE)
#     target_compile_options(TEST_C PRIVATE ${COMPILER_OPTIONS})
#     # set the C version
#     set_property(TARGET TEST_C PROPERTY C_STANDARD 99)
#     # add GLGE
#     target_link_libraries(TEST_C GLGE)

# First person example test
    project(FP_EXAMPLE_CPP
        VERSION 1.0
        DESCRIPTION "A simple test a first person camera in C++ with GLGE"
        LANGUAGES CXX)
    # executable for test project
    add_executable(FP_EXAMPLE_CPP
        src/CPP/FirstPersonExample/main.cpp
        src/CPP/FirstPersonExample/CameraController.cpp
    )
    # define compiler settings
    target_compile_definitions(FP_EXAMPLE_CPP PRIVATE USE_NEW_AUDIO_ENGINE)
    target_compile_options(FP_EXAMPLE_CPP PRIVATE ${COMPILER_OPTIONS})
    # add GLGE
    target_link_libraries(FP_EXAMPLE_CPP GLGE)

# Unit tests
    project(UNIT_TEST_CPP
        VERSION 1.0
        DESCRIPTION "A list of automated tests for GLGE"
        LANGUAGES CXX)
    # executable for test project
    add_executable(UNIT_TEST_CPP
        src/CPP/UnitTest/main.cpp
        src/CPP/UnitTest/tests.cpp
    )
    # define compiler settings
    target_compile_definitions(UNIT_TEST_CPP PRIVATE USE_NEW_AUDIO_ENGINE)
    target_compile_options(UNIT_TEST_CPP PRIVATE ${COMPILER_OPTIONS})
    # add GLGE
    target_link_libraries(UNIT_TEST_CPP GLGE)

# Unit tests
    project(ASSET_LOADING_CPP
        VERSION 1.0
        DESCRIPTION "A simple asset loader made with GLGE"
        LANGUAGES CXX)
    # executable for test project
    add_executable(ASSET_LOADING_CPP
        src/CPP/AssetLoading/main.cpp
    )
    # define compiler settings
    target_compile_definitions(ASSET_LOADING_CPP PRIVATE USE_NEW_AUDIO_ENGINE)
    target_compile_options(ASSET_LOADING_CPP PRIVATE ${COMPILER_OPTIONS})
    # add GLGE
    target_link_libraries(ASSET_LOADING_CPP GLGE)

# Example hub
    project(EXAMPLE_HUB_CPP
        VERSION 1.0
        DESCRIPTION "A hub to start other example projects from"
        LANGUAGES CXX)
    # executable for test project
    add_executable(EXAMPLE_HUB_CPP
        src/main.cpp
    )
    # define compiler settings
    target_compile_definitions(EXAMPLE_HUB_CPP PRIVATE USE_NEW_AUDIO_ENGINE)
    target_compile_options(EXAMPLE_HUB_CPP PRIVATE ${COMPILER_OPTIONS})
    # add GLGE
    target_link_libraries(EXAMPLE_HUB_CPP GLGE)