cmake_minimum_required(VERSION 3.14)

# set the name for the library
set(GLGE_LIB_NAME GLGE)
# set the name for the core library
set(GLGE_CORE_NAME GLGE_CORE)
# set the name for the graphic library
set(GLGE_GRAPHIC_NAME GLGE_GRAPHIC)
# set the name for the project
set(GLGE_PROJ_NAME GLGE)

# set the path to the source directory
set(GLGE_SRC_DIR src)
# set the path to the include directory
set(GLGE_INCLUDE_DIR include)

# an option that allows GLGE to be build in debug mode
option(GLGE_ALLOW_DEBUG
    "Enable optional debugging features"
    ON
)

# an option to disable GLGE Graphic
option(GLGE_INCLUDE_GRAPHICS
    "Enable the GLGE Graphic extension"
    ON
)

# an option to include the builtin OpenGL backend
option(GLGE_GRAPHICS_INCLUDE_OPENGL
    "Enable if the builtin OpenGL backend should be included"
    ON
)

# an option to include the builtin Vulkan backend
option(GLGE_GRAPHICS_INCLUDE_VULKAN
    "Enable if the builtin Vulkan backend should be included"
    ON
)

# an option to make command overriding possible
option(GLGE_GRAPHIC_ALLOW_ENGINE_COMMAND_OVERRIDE
    "Enable if the user may override engine-side commands with custom commands"
    OFF
)

# define the project
project(${GLGE_PROJ_NAME})

# Shared libraries are currently not supported
set(BUILD_SHARED_LIBS OFF)

###### DEPENDENCIES

# GLM is required for the core
# first, check if the OS has GLM installed
find_package(GLM QUIET)
# if not, fetch GLM from github
if (NOT GLM_FOUND)
    # print a message
    message(STATUS "GLM not found - fetching from GitHub")
    # use FetchContent to fetch GLM from github
    include(FetchContent)
    FetchContent_Declare(
        GLM
        GIT_REPOSITORY https://github.com/g-truc/glm.git
        GIT_TAG "1.0.3"
    )
    FetchContent_MakeAvailable(GLM)
endif()

# Tracy is required for the core
# first, check if the OS has tracy installed
find_package(TRACY QUIET)
# if not, fetch Tracy from github
if (NOT TRACY_FOUND)
    # print a message
    message(STATUS "Tracy not found - fetching from GitHub")
    # use FetchContent to fetch Tracy from github
    include(FetchContent)
    FetchContent_Declare(
        tracy
        GIT_REPOSITORY https://github.com/wolfpld/tracy.git
        GIT_TAG "master"
    )
    FetchContent_MakeAvailable(tracy)
endif()

# only required for GLGE Graphic
if (GLGE_INCLUDE_GRAPHICS)
    #### SDL3
    # first, check if the OS has SDL3 installed
    find_package(SDL3 QUIET)
    # if not, fetch SDL3 from github
    if (NOT SDL3_FOUND)
        # print a message
        message(STATUS "SDL3 not found - fetching from GitHub")
        # use FetchContent to fetch SDL3 from github
        include(FetchContent)
        FetchContent_Declare(
            SDL3
            GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
            GIT_TAG "release-3.4.0"
        )
        FetchContent_MakeAvailable(SDL3)
    endif()
    #### Assimp
    # first, check if the OS has assimp installed
    find_package(assimp QUIET)
    # if not, fetch SDL3 from github
    if (NOT assimp_FOUND)
        # print a message
        message(STATUS "assimp not found - fetching from GitHub")
        # use FetchContent to fetch SDL3 from github
        include(FetchContent)
        FetchContent_Declare(
            assimp
            GIT_REPOSITORY https://github.com/assimp/assimp
            GIT_TAG "v6.0.4"
        )
        FetchContent_MakeAvailable(assimp)
    endif()
    #### SPIRV-Reflect
    # first, check if the OS has SDL3 installed
    find_package(spirv-reflect QUIET)
    # if not, fetch SDL3 from github
    if (NOT spirv-reflect_FOUND)
        # set the option for a static library
        option(SPIRV_REFLECT_STATIC_LIB FORCE ON)
        option(SPIRV_REFLECT_INSTALL FORCE OFF)
        # print a message
        message(STATUS "SPIRV-Reflect not found - fetching from GitHub")
        # use FetchContent to fetch SDL3 from github
        include(FetchContent)
        FetchContent_Declare(
            spirv-reflect
            GIT_REPOSITORY https://github.com/KhronosGroup/SPIRV-Reflect
            GIT_TAG "main"
        )
        FetchContent_MakeAvailable(spirv-reflect)
    endif()
    #### Vulkan
    # only add vulkan if vulkan is requested
    if (GLGE_GRAPHICS_INCLUDE_VULKAN)
        # first, check if the OS has Vulkan installed
        find_package(Vulkan QUIET)

        # if vulkan was not found print a custom error message
        if (NOT ${Vulkan_FOUND})
            message(WARNING "Vulkan capabilities are requested, but Vulkan was not installed on the system. Disabling Vulkan. If you want to use Vulkan, please in stall the correct Vulkan SDK for your system. ")
            set(GLGE_GRAPHICS_INCLUDE_VULKAN OFF)
            message(STATUS "Deselected Vulkan. GLGE Graphics will not include the Vulkan backend. ")
        endif()
    endif()
endif()

###### GLGE

# create the main library
add_library(${GLGE_LIB_NAME} INTERFACE)

# add the include directory
target_include_directories(${GLGE_LIB_NAME} INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/${GLGE_INCLUDE_DIR}
)

###### GLGE_CORE

# add the source files to the core library
add_library(${GLGE_CORE_NAME}
    ${GLGE_SRC_DIR}/Core/Instance.cpp
    ${GLGE_SRC_DIR}/Core/BaseClass.cpp
    ${GLGE_SRC_DIR}/Core/Object.cpp
    ${GLGE_SRC_DIR}/Core/Keyboard.cpp
    ${GLGE_SRC_DIR}/Core/Mouse.cpp
)

# add the internal include directories
target_include_directories(${GLGE_CORE_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/${GLGE_INCLUDE_DIR}
)

# add a definition to the library if this is a debug build and debugging is allowed
target_compile_definitions(${GLGE_CORE_NAME} PUBLIC
    $<$<AND:$<CONFIG:Debug>,$<BOOL:${GLGE_ALLOW_DEBUG}>>:GLGE_DEBUG>
)

# specify that this is a C++23 library
set_target_properties(${GLGE_CORE_NAME} PROPERTIES
    CXX_STANDARD 23
    CXX_STANDARD_REQUIRED ON
)

# Compile TracyClient.cpp only in Debug
target_sources(GLGE_CORE PRIVATE
    $<$<CONFIG:Debug>:${tracy_SOURCE_DIR}/public/TracyClient.cpp>
)
# Define TRACY_ENABLE only in Debug
target_compile_definitions(GLGE_CORE PUBLIC
    $<$<CONFIG:Debug>:TRACY_ENABLE>
)

# add the required libraries to the core
target_include_directories(${GLGE_CORE_NAME} PUBLIC
    $<$<CONFIG:Debug>:${tracy_SOURCE_DIR}/public/tracy>
)

# add glm
target_link_libraries(${GLGE_CORE_NAME} PUBLIC glm::glm)

# compiler-specific release flags
if(MSVC)
    # MSVC (Visual Studio Compiler) release flags
    target_compile_options(${GLGE_LIB_NAME} INTERFACE $<$<CONFIG:Release>:/O2 /GL /DNDEBUG>)
    target_link_options(${GLGE_LIB_NAME} INTERFACE $<$<CONFIG:Release>:/LTCG /OPT:REF /OPT:ICF>)
else()
    # GCC / Clang Release flags
    target_compile_options(${GLGE_LIB_NAME} INTERFACE $<$<CONFIG:Release>:
        -Ofast -march=native -flto -DNDEBUG
        -ffunction-sections -fdata-sections
    >)
    target_link_options(${GLGE_LIB_NAME} INTERFACE $<$<CONFIG:Release>:
        -flto -Wl,--gc-sections
    >)
endif()

# add the library to the interface
target_link_libraries(${GLGE_LIB_NAME} INTERFACE ${GLGE_CORE_NAME})

###### GLGE_GRAPHIC

# check if graphics are wanted
if (GLGE_INCLUDE_GRAPHICS)
    # collect all the library source files
    set(GLGE_GRAPHIC_SOURCE 
        # Builtin::SDL
        ${GLGE_SRC_DIR}/Graphic/Backend/Builtin/Video/SDL3/Description.cpp
        ${GLGE_SRC_DIR}/Graphic/Backend/Builtin/Video/SDL3/Instance.cpp
        ${GLGE_SRC_DIR}/Graphic/Backend/Builtin/Video/SDL3/Window.cpp
        # SDL Contracts
        ${GLGE_SRC_DIR}/Graphic/Backend/Builtin/Video/SDL3/Contracts/OpenGL.cpp
        ${GLGE_SRC_DIR}/Graphic/Backend/Builtin/Video/SDL3/Contracts/Vulkan.cpp

        # Video backend
        ${GLGE_SRC_DIR}/Graphic/Backend/Video/Window.cpp

        # Graphic backend
        ${GLGE_SRC_DIR}/Graphic/Backend/Graphics/CommandTable.cpp
        ${GLGE_SRC_DIR}/Graphic/Backend/Graphics/CommandBuffer.cpp
        ${GLGE_SRC_DIR}/Graphic/Backend/Graphics/Framebuffer.cpp

        # Frontend
        ${GLGE_SRC_DIR}/Graphic/DisplaySetup.cpp
        ${GLGE_SRC_DIR}/Graphic/Instance.cpp
        ${GLGE_SRC_DIR}/Graphic/Window.cpp
        ${GLGE_SRC_DIR}/Graphic/RenderPipeline.cpp
        ${GLGE_SRC_DIR}/Graphic/ImageCPU.cpp
        ${GLGE_SRC_DIR}/Graphic/TextureCPU.cpp
        ${GLGE_SRC_DIR}/Graphic/Shader.cpp

        # Assets
        ${GLGE_SRC_DIR}/Graphic/Assets/ImageCPUAsset.cpp
        ${GLGE_SRC_DIR}/Graphic/Assets/TextureCPUAsset.cpp
        ${GLGE_SRC_DIR}/Graphic/Assets/MeshAsset.cpp
        # External: Miniz
        ${GLGE_SRC_DIR}/Graphic/Assets/miniz/miniz.c
    )

    # optionally add OpenGL
    if (GLGE_GRAPHICS_INCLUDE_OPENGL)
        # add the source files for OpenGL
        set(GLGE_GRAPHIC_SOURCE 
            # add all the previous source files
            ${GLGE_GRAPHIC_SOURCE}

            # Builtin::OpenGL
            ${GLGE_SRC_DIR}/Graphic/Backend/Builtin/Graphics/OpenGL/Description.cpp
            ${GLGE_SRC_DIR}/Graphic/Backend/Builtin/Graphics/OpenGL/Instance.cpp
            ${GLGE_SRC_DIR}/Graphic/Backend/Builtin/Graphics/OpenGL/Window.cpp
            ${GLGE_SRC_DIR}/Graphic/Backend/Builtin/Graphics/OpenGL/Sampler.cpp
            ${GLGE_SRC_DIR}/Graphic/Backend/Builtin/Graphics/OpenGL/Texture.cpp
            ${GLGE_SRC_DIR}/Graphic/Backend/Builtin/Graphics/OpenGL/Image.cpp
            ${GLGE_SRC_DIR}/Graphic/Backend/Builtin/Graphics/OpenGL/Framebuffer.cpp
            ${GLGE_SRC_DIR}/Graphic/Backend/Builtin/Graphics/OpenGL/ResourceSet.cpp
            ${GLGE_SRC_DIR}/Graphic/Backend/Builtin/Graphics/OpenGL/Shader.cpp
            ${GLGE_SRC_DIR}/Graphic/Backend/Builtin/Graphics/OpenGL/Buffer.cpp
            ${GLGE_SRC_DIR}/Graphic/Backend/Builtin/Graphics/OpenGL/SampledTexture.cpp
            ${GLGE_SRC_DIR}/Graphic/Backend/Builtin/Graphics/OpenGL/MeshPool.cpp
            ${GLGE_SRC_DIR}/Graphic/Backend/Builtin/Graphics/OpenGL/VertexLayout.cpp
            ${GLGE_SRC_DIR}/Graphic/Backend/Builtin/Graphics/OpenGL/Material.cpp
            ${GLGE_SRC_DIR}/Graphic/Backend/Builtin/Graphics/OpenGL/Renderer.cpp
            # Builtin::OpenGL::GLAD
            ${GLGE_SRC_DIR}/Graphic/Backend/Builtin/Graphics/OpenGL/glad/src/glad.c
            # Builtin::OpenGL::Translator
            ${GLGE_SRC_DIR}/Graphic/Backend/Builtin/Graphics/OpenGL/Translators.cpp

            # Internal mapping
            ${GLGE_SRC_DIR}/Graphic/Backend/Builtin/Graphics/OpenGL/__Mapper.cpp
        )
    endif()

    # optionally add Vulkan
    if (GLGE_GRAPHICS_INCLUDE_VULKAN)
        # add the source files for Vulkan
        set(GLGE_GRAPHIC_SOURCE
            # add all previous source files
            ${GLGE_GRAPHIC_SOURCE}

            # Builtin::Vulkan
            ${GLGE_SRC_DIR}/Graphic/Backend/Builtin/Graphics/Vulkan/Description.cpp
            ${GLGE_SRC_DIR}/Graphic/Backend/Builtin/Graphics/Vulkan/Instance.cpp
            ${GLGE_SRC_DIR}/Graphic/Backend/Builtin/Graphics/Vulkan/Window.cpp
        )
    endif()

    # compile the library
    add_library(${GLGE_GRAPHIC_NAME} ${GLGE_GRAPHIC_SOURCE})

    # add SDL3, assimp and spirv reflect
    target_link_libraries(${GLGE_GRAPHIC_NAME} PRIVATE SDL3::SDL3 assimp spirv-reflect-static ${GLGE_CORE_NAME})

    # if vulkan was requested link the vulkan SDK
    if (GLGE_GRAPHICS_INCLUDE_VULKAN)
        target_link_libraries(${GLGE_GRAPHIC_NAME} PRIVATE Vulkan::Vulkan)
    endif()

    # add the internal include directories
    target_include_directories(${GLGE_GRAPHIC_NAME} PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/${GLGE_INCLUDE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/${GLGE_SRC_DIR}/Graphic/Backend/Builtin/Graphics/OpenGL/glad/include
    )

    # add a definition to the library if this is a debug build and debugging is allowed
    target_compile_definitions(${GLGE_GRAPHIC_NAME} PRIVATE
        $<$<AND:$<CONFIG:Debug>,$<BOOL:${GLGE_ALLOW_DEBUG}>>:GLGE_DEBUG>
    )
    # if compilation with engine override was selected add the define for engine command overrides
    if (GLGE_GRAPHIC_ALLOW_ENGINE_COMMAND_OVERRIDE)
        target_compile_definitions(${GLGE_GRAPHIC_NAME} PRIVATE GLGE_GRAPHIC_ENGINE_COMMAND_OVERRIDE=1)
    endif()

    # specify that this is a C++23 library
    set_target_properties(${GLGE_GRAPHIC_NAME} PROPERTIES
        CXX_STANDARD 23
        CXX_STANDARD_REQUIRED ON
    )

    # link the library to the interface
    target_link_libraries(${GLGE_LIB_NAME} INTERFACE ${GLGE_GRAPHIC_NAME})

endif()