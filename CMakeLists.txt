cmake_minimum_required(VERSION 3.14)

# set the name for the library
set(GLGE_LIB_NAME GLGE)
# set the name for the core library
set(GLGE_CORE_NAME GLGE_CORE)
# set the name for the graphic library
set(GLGE_GRAPHIC_NAME GLGE_GRAPHIC)
# set the name for the physics library
set(GLGE_PHYSICS_NAME GLGE_PHYSICS)
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

option(GLGE_GRAPHIC_INCLUDE_IMGUI
    "Enable if ImGUI should be automatically downloaded and if ImGUI bindings to the GUI system should be included"
    ON
)

# an option to disable GLGE Physics
option(GLGE_INCLUDE_PHYSICS
    "Enable the GLGE Physics extension"
    ON
)

# an option to disable the Jolt implementation
option(GLGE_PHYSICS_INCLUDE_JOLT
    "Enable Jolt backend"
    ON
)

option(GLGE_PHYSICS_INCLUDE_PHYSX
    "Enable PhysX backend"
    OFF
)
# an option to disable the bullet implementation
option(GLGE_PHYSICS_INCLUDE_BULLET
    "Enable Bullet backend"
    ON
)

# detect total system RAM in bytes
# there were issues with the host information query, so OS-Specific alternatives exist. 
if(WIN32)
    execute_process(
        COMMAND powershell -NoProfile -Command
            "(Get-CimInstance Win32_ComputerSystem).TotalPhysicalMemory"
        OUTPUT_VARIABLE TOTAL_RAM_MIB
        OUTPUT_STRIP_TRAILING_WHITESPACE
        RESULT_VARIABLE RAM_RESULT
    )
elseif(APPLE)
    execute_process(
        COMMAND sysctl -n hw.memsize
        OUTPUT_VARIABLE TOTAL_RAM_MIB
        OUTPUT_STRIP_TRAILING_WHITESPACE
        RESULT_VARIABLE RAM_RESULT
    )

elseif(UNIX)
    execute_process(
        COMMAND sh -c "awk '/MemTotal/ {print $2 * 1024}' /proc/meminfo"
        OUTPUT_VARIABLE TOTAL_RAM_MIB
        OUTPUT_STRIP_TRAILING_WHITESPACE
        RESULT_VARIABLE RAM_RESULT
    )
else()
    cmake_host_system_information(RESULT TOTAL_RAM_MIB QUERY TOTAL_PHYSICAL_MEMORY)
    if (NOT TOTAL_RAM_MIB)
        message(WARNING "Failed to query memory. Assuming 1 GB.")
        set(TOTAL_RAM_MIB 1024)
    endif()
endif()

# Validate detection
if(NOT TOTAL_RAM_MIB MATCHES "^[0-9]+$" OR TOTAL_RAM_MIB EQUAL 0)
    message(FATAL_ERROR "Could not detect total system RAM.")
endif()
# convert to GB
math(EXPR TOTAL_RAM_GB "${TOTAL_RAM_MIB} / 1024 / 1024 / 1024")
# calculate heavy compile task limit
math(EXPR DEFAULT_HEAVY_COMPILE_LIMIT "${TOTAL_RAM_GB} / 7")
# ensure at least one task
if(DEFAULT_HEAVY_COMPILE_LIMIT LESS 1)
    set(DEFAULT_HEAVY_COMPILE_LIMIT 1)
endif()

# define how many heavy compile tasks GLGE will allow to run in parallel
set(GLGE_HEAVY_COMPILE_TASK_LIMIT
    ${DEFAULT_HEAVY_COMPILE_LIMIT}
    CACHE STRING
    "The maximum amount of heavy compile tasks to run concurrently"
)
# sanity check
if(NOT GLGE_HEAVY_COMPILE_TASK_LIMIT MATCHES "^[1-9][0-9]*$")
    message(FATAL_ERROR
        "GLGE_HEAVY_COMPILE_TASK_LIMIT must be a positive integer. "
        "Got: '${GLGE_HEAVY_COMPILE_TASK_LIMIT}'"
    )
endif()

# some debug info
message(STATUS "Detected RAM: ${TOTAL_RAM_GB} GB (${TOTAL_RAM_MIB} MiB)")
message(STATUS "Heavy compile task limit: ${GLGE_HEAVY_COMPILE_TASK_LIMIT}")

# set GNU compiler flags
if ((CMAKE_CXX_COMPILER_ID STREQUAL "GNU")) 
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-cast-user-defined -Wno-ignored-attributes")
endif()

# configure job pools
set_property(GLOBAL PROPERTY JOB_POOLS heavy_compile=${GLGE_HEAVY_COMPILE_TASK_LIMIT})

# Tracy profiling option
option(GLGE_ENABLE_TRACY "Enable Tracy profiler integration" ON)

# define the project
project(${GLGE_PROJ_NAME})

# Shared libraries are currently not supported unless the host enables GLGE_BUILD_SHARED_LIBS
option(GLGE_BUILD_SHARED_LIBS "Build GLGE_CORE / GLGE_GRAPHIC as shared libraries (required for out-of-tree launcher plugins)" OFF)
if(GLGE_BUILD_SHARED_LIBS)
    set(GLGE_CORE_LIBRARY_TYPE SHARED)
    set(GLGE_GRAPHIC_LIBRARY_TYPE SHARED)
    set(GLGE_PHYSICS_LIBRARY_TYPE SHARED)
else()
    set(GLGE_CORE_LIBRARY_TYPE STATIC)
    set(GLGE_GRAPHIC_LIBRARY_TYPE STATIC)
    set(GLGE_PHYSICS_LIBRARY_TYPE STATIC)
endif()

# check if physics should be enabled
set(GLGE_PHYSICS_ENABLED FALSE)
# at least one backend must enabled
if(GLGE_PHYSICS_INCLUDE_JOLT)
    set(GLGE_PHYSICS_ENABLED TRUE)
endif()
if(GLGE_PHYSICS_INCLUDE_PHYSX)
    set(GLGE_PHYSICS_ENABLED TRUE)
endif()
if(GLGE_PHYSICS_INCLUDE_BULLET)
    set(GLGE_PHYSICS_ENABLED TRUE)
endif()
if(NOT GLGE_PHYSICS_ENABLED)
    set(GLGE_INCLUDE_PHYSICS OFF)
    message(STATUS "No physics backends selected. Physics disabled.")
endif()

###### DEPENDENCIES

# Persistent cache shared across build directories.
set(DEPENDENCY_CACHE_DIR "${CMAKE_BINARY_DIR}/../.cache" CACHE PATH "Persistent dependency cache")
include(FetchContent)

# a function to load a dependency and cache it
function(fetch_dependency NAME GIT_REPOSITORY GIT_TAG SHALLOW)
    string(TOUPPER "${NAME}" NAME_UPPER)
    set(ENTRY_SOURCE_DIR "${DEPENDENCY_CACHE_DIR}/${NAME}/src")

    # check if the source is cached
    if(EXISTS "${ENTRY_SOURCE_DIR}/CMakeLists.txt")
        # cached: fetch from cache
        message(STATUS "Using cached ${NAME}: ${ENTRY_SOURCE_DIR}")
        FetchContent_Declare(
            ${NAME} 
            SOURCE_DIR "${ENTRY_SOURCE_DIR}"
        )
        FetchContent_MakeAvailable(${NAME})

        # export the paths
        set(${NAME}_SOURCE_DIR "${${NAME}_SOURCE_DIR}" PARENT_SCOPE)
        set(${NAME}_BINARY_DIR "${${NAME}_BINARY_DIR}" PARENT_SCOPE)
        set(${NAME}_POPULATED "${${NAME}_POPULATED}" PARENT_SCOPE)

        # print where the source and binaries will be / are
        message(STATUS "Source for ${NAME}: ${${NAME}_SOURCE_DIR}")
        message(STATUS "Binary for ${NAME}: ${${NAME}_BINARY_DIR}")
        return()
    endif()

    # No cache -> check internet connection
    message(STATUS "No cache for ${NAME}. Checking internet connectivity...")
    # github connection check
    set(CONNECTIVITY_PATH "${CMAKE_BINARY_DIR}/${NAME}_connectivity_check.html")
    # fetch file
    file(
        DOWNLOAD
        "https://github.com"
        ${CONNECTIVITY_PATH}
        STATUS CONNECTION_STATUS
        TIMEOUT 5
        INACTIVITY_TIMEOUT 5
        TLS_VERIFY ON
    )
    list(GET CONNECTION_STATUS 0 CONNECTION_CODE)
    # clean up the file
    file(REMOVE ${CONNECTIVITY_PATH})

    # Success -> fetch from github
    if(CONNECTION_CODE EQUAL 0)
        message(STATUS "Internet available. Fetching ${NAME} into ${ENTRY_SOURCE_DIR}...")
        FetchContent_Declare(
            ${NAME}
            GIT_REPOSITORY "${GIT_REPOSITORY}"
            GIT_TAG        "${GIT_TAG}"
            GIT_SHALLOW    "${SHALLOW}"
            SOURCE_DIR     "${ENTRY_SOURCE_DIR}"
        )
        FetchContent_MakeAvailable(${NAME})
    else()
        # Failed to get dependency
        message(FATAL_ERROR
            "Cannot fetch dependency '${NAME}'.\n"
            "Internet is unavailable and no cached source was found.\n"
            "Expected cache directory: ${ENTRY_SOURCE_DIR}"
        )
    endif()

    # export the variables
    set(${NAME}_SOURCE_DIR "${${NAME}_SOURCE_DIR}" PARENT_SCOPE)
    set(${NAME}_BINARY_DIR "${${NAME}_BINARY_DIR}" PARENT_SCOPE)
    set(${NAME}_POPULATED "${${NAME}_POPULATED}" PARENT_SCOPE)

    # print where the source is / the binaries are
    message(STATUS "Source for ${NAME}: ${${NAME}_SOURCE_DIR}")
    message(STATUS "Binary for ${NAME}: ${${NAME}_BINARY_DIR}")
endfunction()

# GLM is required for the core
fetch_dependency(
    GLM
    "https://github.com/g-truc/glm.git"
    "1.0.3"
    ON
)

# only download tracy in debug if tracy is enabled (maybe mood?)
fetch_dependency(
    "tracy"
    "https://github.com/wolfpld/tracy.git"
    "v0.13.1"
    ON
)

# oneTBB is used for advanced thread primitives
# configure for static build
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(TBB_BUILD_SHARED ON CACHE BOOL "" FORCE)
set(TBB_BUILD_STATIC OFF CACHE BOOL "" FORCE)
set(TBBMALLOC_BUILD OFF CACHE BOOL "" FORCE)
set(TBBMALLOC_PROXY_BUILD OFF CACHE BOOL "" FORCE)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
# disable whatever is not needed
set(TBB_TEST OFF CACHE BOOL "" FORCE)
set(TBB_STRICT OFF CACHE BOOL "" FORCE)
set(TBB_DIR "${tbb_BINARY_DIR}" CACHE PATH "" FORCE) # enforce that Embree finds this tbb target
fetch_dependency(
    oneTBB
    "https://github.com/oneapi-src/oneTBB.git"
    "v2022.2.0"
    ON
)

# Mesh optimizer is used to optimize meshes
fetch_dependency(
    meshoptimizer
    "https://github.com/zeux/meshoptimizer.git"
    "v1.2"
    ON
)

#### Assimp
# Force Assimp to build static no matter what
set(ASSIMP_BUILD_SHARED_LIBS ON CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_STATIC_LIBS OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(ASSIMP_INSTALL OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_ZLIB ON CACHE BOOL "" FORCE)
fetch_dependency(
    assimp
    "https://github.com/assimp/assimp"
    "v6.0.4"
    ON
)
# do some sanity checks
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_compile_options(assimp PRIVATE
        -Wno-error=maybe-uninitialized
    )
endif()

# setup embree options
set(EMBREE_ISPC_SUPPORT OFF CACHE BOOL "" FORCE)
set(EMBREE_TUTORIALS OFF CACHE BOOL "" FORCE)
set(EMBREE_TESTING OFF CACHE BOOL "" FORCE)
set(EMBREE_BACKFACE_CULLING OFF CACHE BOOL "" FORCE)
set(EMBREE_MAX_ISA AVX2 CACHE STRING "" FORCE)
fetch_dependency(
    embree
    "https://github.com/RenderKit/embree.git"
    "v4.4.0"
    ON
)
# helper function to configure all embree targets
function(configure_embree_targets DIR)
    get_property(TARGETS
        DIRECTORY "${DIR}"
        PROPERTY BUILDSYSTEM_TARGETS
    )

    foreach(TARGET IN LISTS TARGETS)
        get_target_property(TARGET_TYPE ${TARGET} TYPE)

        # Only configure targets that actually compile code
        if(   TARGET_TYPE STREQUAL "STATIC_LIBRARY" 
           OR TARGET_TYPE STREQUAL "SHARED_LIBRARY" 
           OR TARGET_TYPE STREQUAL "MODULE_LIBRARY" 
           OR TARGET_TYPE STREQUAL "OBJECT_LIBRARY" 
           OR TARGET_TYPE STREQUAL "EXECUTABLE"
           )
            message(STATUS "Configuring Embree target: ${TARGET}")

            set_target_properties(${TARGET} PROPERTIES JOB_POOL_COMPILE heavy_compile)

            if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
                target_compile_options(${TARGET} PRIVATE -Wno-cast-user-defined)
            endif()
        endif()
    endforeach()

    # Process subdirectories recursively
    get_property(SUBDIRS
        DIRECTORY "${DIR}"
        PROPERTY SUBDIRECTORIES
    )

    # recursion!
    foreach(SUBDIR IN LISTS SUBDIRS)
        configure_embree_targets("${SUBDIR}")
    endforeach()
endfunction()

# configure embree
configure_embree_targets("${embree_SOURCE_DIR}")

# only required for GLGE Graphic
if (GLGE_INCLUDE_GRAPHICS)
    # setup save OpenGL loading
    # this does not load OpenGL, it only makes the newer OpenGL loading the preferred
    if(POLICY CMP0072)
        cmake_policy(SET CMP0072 NEW)
    endif()
    set(OpenGL_GL_PREFERENCE GLVND)

    #### SDL3
    fetch_dependency(
        SDL3
        "https://github.com/libsdl-org/SDL.git"
        "release-3.4.0"
        ON
    )

    #### SPIRV-Reflect
    # set the option for a static library
    option(SPIRV_REFLECT_STATIC_LIB FORCE ON)
    option(SPIRV_REFLECT_INSTALL FORCE OFF)
    fetch_dependency(
        spirv-reflect
        "https://github.com/KhronosGroup/SPIRV-Reflect"
        "vulkan-sdk-1.4.350.0"
        ON
    )
    
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

        #### VMA
        fetch_dependency(
            VMA
            "https://github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator.git"
            "v3.1.0"
            ON
        )
        # FetchContent exposes VMA source dir here
        set(VMA_INCLUDE_DIR ${VMA_SOURCE_DIR}/include)
    endif()

    #### ImGUI
    # only add if ImGUI is requested
    if (GLGE_GRAPHIC_INCLUDE_IMGUI)
        # ImGUI is a common GUI library
        fetch_dependency(
            "imgui"
            "https://github.com/ocornut/imgui.git"
            "v1.92.9b-docking"
            ON
        )
    endif()
endif()

if (GLGE_INCLUDE_PHYSICS)
    ### Jolt
    if(GLGE_PHYSICS_INCLUDE_JOLT)
        fetch_dependency(
            JoltPhysics
            "https://github.com/jrouwe/JoltPhysics.git"
            "v5.3.0"
            OFF
        )
        FetchContent_GetProperties(JoltPhysics)
        if(NOT JoltPhysics_POPULATED)
            add_subdirectory(
                ${joltphysics_SOURCE_DIR}/Build
                ${joltphysics_BINARY_DIR}
            )
        endif()
    endif()

    ### PhysX
    if (GLGE_PHYSICS_INCLUDE_PHYSX)
        message(WARNING "Physics is currently NOT supported")
    endif()

    ### Bullet
    if (GLGE_PHYSICS_INCLUDE_BULLET)
        # setup build
        set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
        set(BUILD_UNIT_TESTS OFF CACHE BOOL "" FORCE)
        set(BUILD_EXTRAS OFF CACHE BOOL "" FORCE)
        set(BUILD_BULLET2_DEMOS OFF CACHE BOOL "" FORCE)
        set(BUILD_CPU_DEMOS OFF CACHE BOOL "" FORCE)
        set(INSTALL_LIBS OFF CACHE BOOL "" FORCE)

        # disable multi threading
        set(BULLET2_MULTITHREADING OFF CACHE BOOL "" FORCE)

        # get from github
        fetch_dependency(
            bullet
            "https://github.com/bulletphysics/bullet3.git"
            "master" # Note: Master is used since the newest release uses a very old CMake version that does no longer compile everywhere
            OFF
        )
        if ((CMAKE_CXX_COMPILER_ID STREQUAL "GNU")) 
            target_compile_options(BulletDynamics PRIVATE -Wno-ignored-attributes)
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
add_library(${GLGE_CORE_NAME} ${GLGE_CORE_LIBRARY_TYPE}
    ${GLGE_SRC_DIR}/Core/Common.cpp
    ${GLGE_SRC_DIR}/Core/Instance.cpp
    ${GLGE_SRC_DIR}/Core/BaseClass.cpp
    ${GLGE_SRC_DIR}/Core/Keyboard.cpp
    ${GLGE_SRC_DIR}/Core/Mouse.cpp
    ${GLGE_SRC_DIR}/Core/CompoundAsset.cpp
    ${GLGE_SRC_DIR}/Core/utils/RecursiveThreadMutexShared.cpp
    ${GLGE_SRC_DIR}/Core/WorldAsset.cpp
    ${GLGE_SRC_DIR}/Core/Transform.cpp
    ${GLGE_SRC_DIR}/Core/Application.cpp
    ${GLGE_SRC_DIR}/Core/LayerManager.cpp
    ${GLGE_SRC_DIR}/Core/Mesh.cpp
    ${GLGE_SRC_DIR}/Core/MeshAsset.cpp
    ${GLGE_SRC_DIR}/Core/RayCasting.cpp
    # External: Miniz
    ${GLGE_SRC_DIR}/external/miniz/miniz.c
)

# add the internal include directories
target_include_directories(${GLGE_CORE_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/${GLGE_INCLUDE_DIR}
    ${GLGE_SRC_DIR}/external
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
if(WIN32 AND GLGE_BUILD_SHARED_LIBS)
    set_target_properties(${GLGE_CORE_NAME} PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS ON)
endif()

# check for tracy integration
if (GLGE_ENABLE_TRACY)
    # Compile TracyClient.cpp only in Debug
    target_sources(GLGE_CORE PRIVATE
        $<$<CONFIG:Debug>:${tracy_SOURCE_DIR}/public/TracyClient.cpp>
    )
    # Define TRACY_ENABLE only in Debug
    target_compile_definitions(GLGE_CORE PUBLIC
        $<$<CONFIG:Debug>:TRACY_ENABLE GLGE_PROFILER_ENABLE=1>
    )

    # add the required libraries to the core
    target_include_directories(${GLGE_CORE_NAME} PUBLIC
        $<$<CONFIG:Debug>:${tracy_SOURCE_DIR}/public/tracy>
    )

    # add libraries for tracy if on windows
    target_link_libraries(GLGE_CORE PUBLIC
        $<$<AND:$<PLATFORM_ID:Windows>,$<CONFIG:Debug>>:
            ws2_32
            dbghelp
            secur32
        >
    )

endif()

# add libs
target_link_libraries(${GLGE_CORE_NAME} PUBLIC glm::glm tbb meshoptimizer embree assimp)

# compiler-specific release flags
if(MSVC)
    # MSVC / Visual Studio
    target_compile_options(${GLGE_LIB_NAME} INTERFACE $<$<CONFIG:Release>: /O2 /GL /Gw /Gy /DNDEBUG>)
    target_link_options(${GLGE_LIB_NAME} INTERFACE $<$<CONFIG:Release>: /LTCG /OPT:REF /OPT:ICF>)
else()
    # GCC / Clang
    target_compile_options(${GLGE_LIB_NAME} INTERFACE $<$<CONFIG:Release>: -O3 -flto -DNDEBUG -ffunction-sections -fdata-sections>)
    target_link_options(${GLGE_LIB_NAME} INTERFACE $<$<CONFIG:Release>: -flto -Wl,--gc-sections -Wl,--as-needed>)
endif()

# add the library to the interface
target_link_libraries(${GLGE_LIB_NAME} INTERFACE ${GLGE_CORE_NAME})

# set whole archive flags
if (MSVC)
    # target_link_options(${GLGE_LIB_NAME} INTERFACE "/WHOLEARCHIVE:${GLGE_CORE_NAME}")
else()
    target_link_options(${GLGE_LIB_NAME} INTERFACE "-Wl,--whole-archive" 
                                                   "$<TARGET_FILE:${GLGE_CORE_NAME}>"
                                                   "-Wl,--no-whole-archive"
                                                   )
endif()

###### GLGE_GRAPHIC

# check if graphics are wanted
if (GLGE_INCLUDE_GRAPHICS)
    # collect all the library source files
    set(GLGE_GRAPHIC_SOURCE 
        # Builtin::SDL
        ${GLGE_SRC_DIR}/Graphic/Backend/Builtin/Video/SDL3/Description.cpp
        ${GLGE_SRC_DIR}/Graphic/Backend/Builtin/Video/SDL3/Instance.cpp
        ${GLGE_SRC_DIR}/Graphic/Backend/Builtin/Video/SDL3/Window.cpp
        ${GLGE_SRC_DIR}/Graphic/Backend/Builtin/Video/SDL3/Cursor.cpp
        # SDL Contracts
        ${GLGE_SRC_DIR}/Graphic/Backend/Builtin/Video/SDL3/Contracts/OpenGL.cpp
        ${GLGE_SRC_DIR}/Graphic/Backend/Builtin/Video/SDL3/Contracts/Vulkan.cpp

        # Video backend
        ${GLGE_SRC_DIR}/Graphic/Backend/Video/Window.cpp

        # Graphic backend
        ${GLGE_SRC_DIR}/Graphic/Backend/Graphics/CommandTable.cpp
        ${GLGE_SRC_DIR}/Graphic/Backend/Graphics/CommandBuffer.cpp
        ${GLGE_SRC_DIR}/Graphic/Backend/Graphics/Framebuffer.cpp
        ${GLGE_SRC_DIR}/Graphic/Backend/Graphics/GeometryPool.cpp
        ${GLGE_SRC_DIR}/Graphic/Backend/Graphics/MeshManager.cpp

        # Frontend
        ${GLGE_SRC_DIR}/Graphic/DisplaySetup.cpp
        ${GLGE_SRC_DIR}/Graphic/Instance.cpp
        ${GLGE_SRC_DIR}/Graphic/Window.cpp
        ${GLGE_SRC_DIR}/Graphic/RenderPipeline.cpp
        ${GLGE_SRC_DIR}/Graphic/ImageCPU.cpp
        ${GLGE_SRC_DIR}/Graphic/TextureCPU.cpp
        ${GLGE_SRC_DIR}/Graphic/Shader.cpp
        ${GLGE_SRC_DIR}/Graphic/Command.cpp
        ${GLGE_SRC_DIR}/Graphic/Resource.cpp
        ${GLGE_SRC_DIR}/Graphic/Image.cpp
        ${GLGE_SRC_DIR}/Graphic/Texture.cpp
        ${GLGE_SRC_DIR}/Graphic/Sampler.cpp
        ${GLGE_SRC_DIR}/Graphic/DebugRenderer.cpp
        ${GLGE_SRC_DIR}/Graphic/DebugContext.cpp
        ${GLGE_SRC_DIR}/Graphic/GUIContext.cpp

        # Assets
        ${GLGE_SRC_DIR}/Graphic/Assets/ImageCPUAsset.cpp
        ${GLGE_SRC_DIR}/Graphic/Assets/TextureCPUAsset.cpp
        # External: Miniz
        ${GLGE_SRC_DIR}/external/miniz/miniz.c
    )

    # optionally add ImGUI
    if (GLGE_GRAPHIC_INCLUDE_IMGUI)
        # add the ImGUI source files
        set(GLGE_GRAPHIC_SOURCE
            # add all the previous source files
            ${GLGE_GRAPHIC_SOURCE}

            # add the ImGUI provider
            ${GLGE_SRC_DIR}/Graphic/DefaultGuiProviders/ImGUIProvider.cpp

            # add ImGUI sources
            ${imgui_SOURCE_DIR}/imgui.cpp
            ${imgui_SOURCE_DIR}/imgui_draw.cpp
            ${imgui_SOURCE_DIR}/imgui_tables.cpp
            ${imgui_SOURCE_DIR}/imgui_widgets.cpp
            ${imgui_SOURCE_DIR}/imgui_demo.cpp
        )
    endif()

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
            ${GLGE_SRC_DIR}/Graphic/Backend/Builtin/Graphics/OpenGL/Material.cpp
            ${GLGE_SRC_DIR}/Graphic/Backend/Builtin/Graphics/OpenGL/Renderer.cpp
            ${GLGE_SRC_DIR}/Graphic/Backend/Builtin/Graphics/OpenGL/CommandBuffer.cpp
            ${GLGE_SRC_DIR}/Graphic/Backend/Builtin/Graphics/OpenGL/GeometryPoolStream.cpp
            ${GLGE_SRC_DIR}/Graphic/Backend/Builtin/Graphics/OpenGL/CommandExecutor.cpp
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
            ${GLGE_SRC_DIR}/Graphic/Backend/Builtin/Graphics/Vulkan/CommandBuffer.cpp
            ${GLGE_SRC_DIR}/Graphic/Backend/Builtin/Graphics/Vulkan/Image.cpp
            ${GLGE_SRC_DIR}/Graphic/Backend/Builtin/Graphics/Vulkan/Translator.cpp
            ${GLGE_SRC_DIR}/Graphic/Backend/Builtin/Graphics/Vulkan/Texture.cpp
            ${GLGE_SRC_DIR}/Graphic/Backend/Builtin/Graphics/Vulkan/Shader.cpp
            ${GLGE_SRC_DIR}/Graphic/Backend/Builtin/Graphics/Vulkan/ResourceSet.cpp
            ${GLGE_SRC_DIR}/Graphic/Backend/Builtin/Graphics/Vulkan/Framebuffer.cpp
            ${GLGE_SRC_DIR}/Graphic/Backend/Builtin/Graphics/Vulkan/Buffer.cpp
            ${GLGE_SRC_DIR}/Graphic/Backend/Builtin/Graphics/Vulkan/Material.cpp
            ${GLGE_SRC_DIR}/Graphic/Backend/Builtin/Graphics/Vulkan/Renderer.cpp
            ${GLGE_SRC_DIR}/Graphic/Backend/Builtin/Graphics/Vulkan/Sampler.cpp
            ${GLGE_SRC_DIR}/Graphic/Backend/Builtin/Graphics/Vulkan/SampledTexture.cpp
            ${GLGE_SRC_DIR}/Graphic/Backend/Builtin/Graphics/Vulkan/GeometryPoolStream.cpp
            ${GLGE_SRC_DIR}/Graphic/Backend/Builtin/Graphics/Vulkan/CommandExecutor.cpp
        )
    endif()

    # compile the library
    add_library(${GLGE_GRAPHIC_NAME} ${GLGE_GRAPHIC_LIBRARY_TYPE} ${GLGE_GRAPHIC_SOURCE})

    # add SDL3, assimp and spirv reflect
    target_link_libraries(${GLGE_GRAPHIC_NAME} PRIVATE SDL3::SDL3 spirv-reflect-static ${GLGE_CORE_NAME})

    # if vulkan was requested link the vulkan SDK
    if (GLGE_GRAPHICS_INCLUDE_VULKAN)
        target_link_libraries(${GLGE_GRAPHIC_NAME} PRIVATE Vulkan::Vulkan)
        message(STATUS "VMA include dir ${VMA_INCLUDE_DIR}")
        target_include_directories(${GLGE_GRAPHIC_NAME} PRIVATE ${VMA_INCLUDE_DIR})
    endif()
    # if ImGUI was added, add ImGUI include directory
    if (GLGE_GRAPHIC_INCLUDE_IMGUI)
        target_include_directories(${GLGE_GRAPHIC_NAME} PUBLIC ${imgui_SOURCE_DIR})
        target_compile_definitions(${GLGE_GRAPHIC_NAME} PUBLIC GLGE_HAS_IMGUI)
    endif()

    # add the internal include directories
    target_include_directories(${GLGE_GRAPHIC_NAME} PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/${GLGE_INCLUDE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/${GLGE_SRC_DIR}/Graphic/Backend/Builtin/Graphics/OpenGL/glad/include
        ${GLGE_SRC_DIR}/external
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
    if(WIN32 AND GLGE_BUILD_SHARED_LIBS)
        set_target_properties(${GLGE_GRAPHIC_NAME} PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS ON)
    endif()

    # link the library to the interface
    target_link_libraries(${GLGE_LIB_NAME} INTERFACE ${GLGE_GRAPHIC_NAME})
    
endif()

# check if physics is enabled
if (GLGE_INCLUDE_PHYSICS)

    # define all always-present physics sources
    set(GLGE_PHYSICS_SOURCE 
            ${GLGE_SRC_DIR}/Physics/Instance.cpp
            ${GLGE_SRC_DIR}/Physics/Backend/Instance.cpp
            ${GLGE_SRC_DIR}/Physics/Backend/SimulatedEntity.cpp
    )

    # if jolt is enabled, add jolt sources
    if (GLGE_PHYSICS_INCLUDE_JOLT)
        # append to the source
        set (GLGE_PHYSICS_SOURCE
            ${GLGE_PHYSICS_SOURCE}
            ${GLGE_SRC_DIR}/Physics/Backend/Builtin/Jolt/Description.cpp
            ${GLGE_SRC_DIR}/Physics/Backend/Builtin/Jolt/Collider.cpp
            ${GLGE_SRC_DIR}/Physics/Backend/Builtin/Jolt/Instance.cpp
            ${GLGE_SRC_DIR}/Physics/Backend/Builtin/Jolt/World.cpp
            ${GLGE_SRC_DIR}/Physics/Backend/Builtin/Jolt/SimulatedEntity.cpp
        )
    endif()

    # if bullet is enabled, add bullet sources
    if (GLGE_PHYSICS_INCLUDE_BULLET)
        # append to the source
        set (GLGE_PHYSICS_SOURCE
            ${GLGE_PHYSICS_SOURCE}
            ${GLGE_SRC_DIR}/Physics/Backend/Builtin/Bullet/Description.cpp
            ${GLGE_SRC_DIR}/Physics/Backend/Builtin/Bullet/Instance.cpp
            ${GLGE_SRC_DIR}/Physics/Backend/Builtin/Bullet/World.cpp
            ${GLGE_SRC_DIR}/Physics/Backend/Builtin/Bullet/SimulatedEntity.cpp
        )
    endif()

    # compile the library
    add_library(${GLGE_PHYSICS_NAME} ${GLGE_PHYSICS_LIBRARY_TYPE} ${GLGE_PHYSICS_SOURCE})

    # if jolt is enabled, add jolt libs
    if (GLGE_PHYSICS_INCLUDE_JOLT)
    target_link_libraries(${GLGE_PHYSICS_NAME} PRIVATE Jolt)
    target_include_directories(${GLGE_PHYSICS_NAME} PRIVATE ${joltphysics_SOURCE_DIR})
    endif()

    # if physx is enabled, add physx libs
    if (GLGE_PHYSICS_INCLUDE_PHYSX)
    endif()

    # if bullet is enabled, add bullet libs
    if (GLGE_PHYSICS_INCLUDE_BULLET)
    target_link_libraries(${GLGE_PHYSICS_NAME} PRIVATE BulletDynamics BulletCollision LinearMath)
    target_include_directories(${GLGE_PHYSICS_NAME} PRIVATE ${bullet_SOURCE_DIR}/src)
    endif()

    # add the internal include directories
    target_include_directories(${GLGE_PHYSICS_NAME} PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/${GLGE_INCLUDE_DIR}
        ${GLGE_SRC_DIR}/external
    )

    # specify that this is a C++23 library
    set_target_properties(${GLGE_PHYSICS_NAME} PROPERTIES
        CXX_STANDARD 23
        CXX_STANDARD_REQUIRED ON
    )

    # add the core
    target_link_libraries(${GLGE_PHYSICS_NAME} PUBLIC ${GLGE_CORE_NAME})

    # link the library to the interface
    target_link_libraries(${GLGE_PHYSICS_NAME} PUBLIC ${GLGE_CORE_NAME})
    target_link_libraries(${GLGE_LIB_NAME} INTERFACE ${GLGE_PHYSICS_NAME})
    
endif()

# set the definitions
target_compile_definitions(${GLGE_LIB_NAME} INTERFACE GLGE_HAS_GRAPHICS=$<BOOL:${GLGE_INCLUDE_GRAPHICS}>)
target_compile_definitions(${GLGE_LIB_NAME} INTERFACE GLGE_HAS_PHYSICS=$<BOOL:${GLGE_INCLUDE_PHYSICS}>)
target_compile_definitions(${GLGE_LIB_NAME} INTERFACE GLGE_GRAPHICS_HAS_OPENGL=$<AND:$<BOOL:${GLGE_GRAPHICS_INCLUDE_OPENGL}>,$<BOOL:${GLGE_INCLUDE_GRAPHICS}>>)
target_compile_definitions(${GLGE_LIB_NAME} INTERFACE GLGE_GRAPHICS_HAS_VULKAN=$<AND:$<BOOL:${GLGE_GRAPHICS_INCLUDE_VULKAN}>,$<BOOL:${GLGE_INCLUDE_GRAPHICS}>>)
target_compile_definitions(${GLGE_LIB_NAME} INTERFACE GLGE_PHYSICS_HAS_PHYSX=$<AND:$<BOOL:${GLGE_PHYSICS_INCLUDE_PHYSX}>,$<BOOL:${GLGE_INCLUDE_PHYSICS}>>)
target_compile_definitions(${GLGE_LIB_NAME} INTERFACE GLGE_PHYSICS_HAS_JOLT=$<AND:$<BOOL:${GLGE_PHYSICS_INCLUDE_JOLT}>,$<BOOL:${GLGE_INCLUDE_PHYSICS}>>)
target_compile_definitions(${GLGE_LIB_NAME} INTERFACE GLGE_PHYSICS_HAS_BULLET=$<AND:$<BOOL:${GLGE_PHYSICS_INCLUDE_BULLET}>,$<BOOL:${GLGE_INCLUDE_PHYSICS}>>)