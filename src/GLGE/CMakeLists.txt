#  $$$$$$\  $$$$$$$$\ $$$$$$$$\ $$\   $$\ $$$$$$$\  
# $$  __$$\ $$  _____|\__$$  __|$$ |  $$ |$$  __$$\ 
# $$ /  \__|$$ |         $$ |   $$ |  $$ |$$ |  $$ |
# \$$$$$$\  $$$$$\       $$ |   $$ |  $$ |$$$$$$$  |
#  \____$$\ $$  __|      $$ |   $$ |  $$ |$$  ____/ 
# $$\   $$ |$$ |         $$ |   $$ |  $$ |$$ |      
# \$$$$$$  |$$$$$$$$\    $$ |   \$$$$$$  |$$ |      
#  \______/ \________|   \__|    \______/ \__|      

# define the minimum cmake version
cmake_minimum_required(VERSION 3.10)
# define C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# GLGE project initalisation
project(GLGE
    VERSION 0.1
    DESCRIPTION "A 3D game engine designed in C++ for C and C++"
    LANGUAGES CXX)

#  $$$$$$\  $$$$$$$$\ $$$$$$$$\ $$$$$$$$\ $$$$$$\ $$\   $$\  $$$$$$\   $$$$$$\  
# $$  __$$\ $$  _____|\__$$  __|\__$$  __|\_$$  _|$$$\  $$ |$$  __$$\ $$  __$$\ 
# $$ /  \__|$$ |         $$ |      $$ |     $$ |  $$$$\ $$ |$$ /  \__|$$ /  \__|
# \$$$$$$\  $$$$$\       $$ |      $$ |     $$ |  $$ $$\$$ |$$ |$$$$\ \$$$$$$\  
#  \____$$\ $$  __|      $$ |      $$ |     $$ |  $$ \$$$$ |$$ |\_$$ | \____$$\ 
# $$\   $$ |$$ |         $$ |      $$ |     $$ |  $$ |\$$$ |$$ |  $$ |$$\   $$ |
# \$$$$$$  |$$$$$$$$\    $$ |      $$ |   $$$$$$\ $$ | \$$ |\$$$$$$  |\$$$$$$  |
#  \______/ \________|   \__|      \__|   \______|\__|  \__| \______/  \______/ 

# enable / disable Dear ImGui
option(GLGE_USE_DEAR_IMGUI "Specify if a imgui hook should be availabe. Default is ON" ON)
# enable / disable Assimp
option(GLGE_USE_ASSIMP "Specify if Assimp should be available for asset loading. WARNING: Disabeling removes a lot of import funcitonality!" ON)
# include / exclude graphics
option(GLGE_INCLUDE_GRAPHICS "Specify if the section responsible for graphics for GLGE should be included" ON)

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

# add pugixml
add_subdirectory(GLGE3rdParty/pugixml/pugixml)

# add assimp
if (GLGE_USE_ASSIMP)
    add_subdirectory(GLGE3rdParty/assimp)
endif()

# check for Dear ImGui
if (GLGE_USE_DEAR_IMGUI)

    # load all source files
    set(DEAR_IMGUI_SOURCE
            # add all generic header files
            GLGE3rdParty/imgui/imgui.cpp
            GLGE3rdParty/imgui/imgui_draw.cpp
            GLGE3rdParty/imgui/imgui_tables.cpp
            GLGE3rdParty/imgui/imgui_widgets.cpp
            GLGE3rdParty/imgui/imgui.cpp

            # add all needed implementation files
            GLGE3rdParty/imgui/backends/imgui_impl_opengl3.cpp
            GLGE3rdParty/imgui/backends/imgui_impl_sdl2.cpp
        )
else()

    # else load emtpy as the source files
    set(DEAR_IMGUI_SOURCE )

endif() # end of Dear ImGui file loading

#  $$$$$$\  $$\       $$$$$$\  $$$$$$$$\ 
# $$  __$$\ $$ |     $$  __$$\ $$  _____|
# $$ /  \__|$$ |     $$ /  \__|$$ |      
# $$ |$$$$\ $$ |     $$ |$$$$\ $$$$$\    
# $$ |\_$$ |$$ |     $$ |\_$$ |$$  __|   
# $$ |  $$ |$$ |     $$ |  $$ |$$ |      
# \$$$$$$  |$$$$$$$$\\$$$$$$  |$$$$$$$$\ 
#  \______/ \________|\______/ \________|

# list of all source files in GLGE
set(SOURCE_FILES_CPP
    # Core files
    GLGECore/GLGEInstance.cpp
    GLGECore/APIs/GLGEApi.cpp
    GLGECore/Logging/GLGEMessage.cpp
    GLGECore/Time/GLGEClock.cpp
    GLGECore/Time/GLGELimiter.cpp
    GLGECore/Files/GLGEFile.cpp
    GLGECore/Files/GLGEPath.cpp
    GLGECore/GLGELayers.cpp
    GLGECore/Color/GLGEColor.cpp
    # Subelement : Assets
    GLGECore/Assets/GLGEAsset.cpp
    GLGECore/Assets/GLGEAssetManager.cpp
    GLGECore/Assets/GLGEAssetObject.cpp
    # Subelement : Geometry
    GLGECore/Geometry/GLGEVertex.cpp
    GLGECore/Geometry/GLGEObject.cpp
        # Sub-Subelement : Worlds
        GLGECore/Geometry/World/GLGEWorld.cpp
        # Sub-Subelement : Mesh
        GLGECore/Geometry/Mesh/GLGEMesh.cpp
        # Sub-Subelement : Primitives
        GLGECore/Geometry/Primitives/GLGE_AABB.cpp
        GLGECore/Geometry/Primitives/GLGESphere.cpp
        GLGECore/Geometry/Primitives/GLGERay.cpp
        GLGECore/Geometry/Primitives/GLGETriangle.cpp
        GLGECore/Geometry/Primitives/GLGECollision.cpp


    # Math files
    # Vectors
    GLGEMath/Vectors/GLGEDoubleVec.cpp
    GLGEMath/Vectors/GLGEFloatVec.cpp
    GLGEMath/Vectors/GLGEIntVec.cpp
    GLGEMath/Vectors/GLGEUIntVec.cpp
    # Matricies
    GLGEMath/Matricies/GLGEDoubleMatricies.cpp
    GLGEMath/Matricies/GLGEFloatMatricies.cpp
    GLGEMath/Matricies/GLGEIntMatricies.cpp
    GLGEMath/Matricies/GLGEUIntMatricies.cpp
    # Rotation
    GLGEMath/Angles/GLGEEuler.cpp
    GLGEMath/Angles/GLGERotation.cpp
    # Quaternions
    GLGEMath/Imaginary/GLGEQuaternion.cpp

    # 3rd Party
        # Dear ImGui
        ${DEAR_IMGUI_SOURCE}
)

# check if graphics should be added
if (GLGE_INCLUDE_GRAPHICS)
    # add the source files for the graphic core
    set(SOURCE_FILES_CPP ${SOURCE_FILES_CPP}
            
        # Graphic files
        GLGEGraphic/GLGEWindow.cpp
        GLGEGraphic/GLGEGraphicShared.cpp
        GLGEGraphic/GLGETexture.cpp
        GLGEGraphic/GLGEFramebuffer.cpp
        GLGEGraphic/GLGERenderPipeline.cpp
        GLGEGraphic/GLGEBuffer.cpp
        GLGEGraphic/GLGEKeyboard.cpp
        GLGEGraphic/GLGERenderMaterial.cpp
        GLGEGraphic/GLGERenderMesh.cpp
        GLGEGraphic/GLGERenderVertexLayout.cpp
        GLGEGraphic/GLGEObjectRenderer.cpp
        GLGEGraphic/GLGECamera.cpp
        # Assets
        GLGEGraphic/Assets/GLGEAssetTexture.cpp
        GLGEGraphic/Assets/GLGEAssetShader.cpp
        # Shader stuff
        GLGEGraphic/Shader/GLGEShader.cpp
        GLGEGraphic/Shader/GLGEShaderProcessor.cpp
            # Shader extensions
            GLGEGraphic/Shader/shaderExtensionsCommon/Core/GLGE_MainStatement.cpp
            GLGEGraphic/Shader/shaderExtensionsCommon/Core/GLGE_VersionStatement.cpp
            GLGEGraphic/Shader/shaderExtensionsCommon/Util/GLGE_IncludeStatement.cpp
        # Subelement : Graphic API
        GLGEGraphic/GraphicAPI/GLGEGraphicAPI_Instance.cpp
        GLGEGraphic/GraphicAPI/GLGEGraphicAPI_Texture.cpp
        GLGEGraphic/GraphicAPI/GLGEGraphicAPI_Framebuffer.cpp
        GLGEGraphic/GraphicAPI/GLGEGraphicAPI_Window.cpp
        GLGEGraphic/GraphicAPI/GLGEGraphicAPI_CommandBuffer.cpp
        GLGEGraphic/GraphicAPI/GLGEGraphicAPI_Shader.cpp
        GLGEGraphic/GraphicAPI/GLGEGraphicAPI_RenderPipeline.cpp
        GLGEGraphic/GraphicAPI/GLGEGraphicAPI_MemoryArena.cpp
            # Graphic API: OpenGL 4.6
            GLGEGraphic/GraphicAPI/GraphicAPIs/API_OpenGL4_6/GLGE_OGL4_6_Instance.cpp
            GLGEGraphic/GraphicAPI/GraphicAPIs/API_OpenGL4_6/GLGE_OGL4_6_Window.cpp
            GLGEGraphic/GraphicAPI/GraphicAPIs/API_OpenGL4_6/GLGE_OGL4_6_CommandBuffer.cpp
            GLGEGraphic/GraphicAPI/GraphicAPIs/API_OpenGL4_6/GLGE_OGL4_6_Texture.cpp
            GLGEGraphic/GraphicAPI/GraphicAPIs/API_OpenGL4_6/GLGE_OGL4_6_Shader.cpp
            GLGEGraphic/GraphicAPI/GraphicAPIs/API_OpenGL4_6/GLGE_OGL4_6_RenderPipeline.cpp
            GLGEGraphic/GraphicAPI/GraphicAPIs/API_OpenGL4_6/GLGE_OGL4_6_Framebuffer.cpp
            GLGEGraphic/GraphicAPI/GraphicAPIs/API_OpenGL4_6/GLGE_OGL4_6_MemoryArena.cpp
            GLGEGraphic/GraphicAPI/GraphicAPIs/API_OpenGL4_6/GLGE_OGL4_6_VertexLayout.cpp
    )
endif() # end of graphic dependend source

# compile the library's source code
add_library(GLGE ${SOURCE_FILES_CPP})

# $$\       $$$$$$\ $$$$$$$\  $$$$$$$\   $$$$$$\  $$$$$$$\  $$$$$$\ $$$$$$$$\  $$$$$$\  
# $$ |      \_$$  _|$$  __$$\ $$  __$$\ $$  __$$\ $$  __$$\ \_$$  _|$$  _____|$$  __$$\ 
# $$ |        $$ |  $$ |  $$ |$$ |  $$ |$$ /  $$ |$$ |  $$ |  $$ |  $$ |      $$ /  \__|
# $$ |        $$ |  $$$$$$$\ |$$$$$$$  |$$$$$$$$ |$$$$$$$  |  $$ |  $$$$$\    \$$$$$$\  
# $$ |        $$ |  $$  __$$\ $$  __$$< $$  __$$ |$$  __$$<   $$ |  $$  __|    \____$$\ 
# $$ |        $$ |  $$ |  $$ |$$ |  $$ |$$ |  $$ |$$ |  $$ |  $$ |  $$ |      $$\   $$ |
# $$$$$$$$\ $$$$$$\ $$$$$$$  |$$ |  $$ |$$ |  $$ |$$ |  $$ |$$$$$$\ $$$$$$$$\ \$$$$$$  |
# \________|\______|\_______/ \__|  \__|\__|  \__|\__|  \__|\______|\________| \______/ 


# check for imgui to add the include directory for imgui
if (GLGE_USE_DEAR_IMGUI)
    # set the include directory
    target_include_directories(GLGE PUBLIC GLGE3rdParty/imgui)
endif()

# link pugixml
target_link_libraries(GLGE PUBLIC pugixml)

# check if the graphics should be included to add graphic specific libraries
if (GLGE_INCLUDE_GRAPHICS)
    # add SDL2
    # add the SDL include directory
    INCLUDE(FindPkgConfig)
    PKG_SEARCH_MODULE(SDL2IMAGE REQUIRED SDL2_image>=2.0.0)
    INCLUDE_DIRECTORIES(${SDL2_INCLUDE_DIRS} ${SDL2IMAGE_INCLUDE_DIRS})
    # add SDL2
    find_package(SDL2 REQUIRED)
    find_package(SDL_image REQUIRED)
    TARGET_LINK_LIBRARIES(GLGE PUBLIC SDL2 SDL2_image)
    # add vulkan
    find_package(Vulkan REQUIRED)
    TARGET_LINK_LIBRARIES(GLGE PUBLIC vulkan)
    # add OpenGL
    set(OpenGL_GL_PREFERENCE LEGACY)
    find_package(OpenGL REQUIRED)
    TARGET_LINK_LIBRARIES(GLGE PUBLIC OpenGL)
    # add GLEW
    find_package(GLEW REQUIRED)
    TARGET_LINK_LIBRARIES(GLGE PUBLIC GLEW)

    # check if Assimp should be added
    if (GLGE_USE_ASSIMP)
        # add assimp
        target_link_libraries(GLGE PUBLIC assimp)
    endif()

# end of graphic specific libraries
endif()