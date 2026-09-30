/**
 * @file Description.cpp
 * @author DM8AT
 * @brief implement the description for SDL3
 * @version 0.1
 * @date 2025-12-29
 * 
 * @copyright Copyright (c) 2025
 * 
 */
//include the video backend description
#include "GLGE/Graphic/Backend/Builtin/Video/SDL3/Description.h"

//add SDL3 instances
#include "GLGE/Graphic/Backend/Builtin/Video/SDL3/Instance.h"
//add SDL3 windows
#include "GLGE/Graphic/Backend/Builtin/Video/SDL3/Window.h"
//add SDL3 cursors
#include "GLGE/Graphic/Backend/Builtin/Video/SDL3/Cursor.h"

//use the namespace
using namespace GLGE::Graphic::Builtin::Video;

/**
 * @brief store a list of all APIs that SDL3 does support
 */
inline static const std::vector<GLGE::Graphic::GraphicAPI> __SUPPORTED_APIS {
    //SDL3 does support OpenGL
    GLGE::Graphic::GraphicAPI::OPEN_GL,
    //SDL3 does support OpenGL ES
    GLGE::Graphic::GraphicAPI::OPEN_GL_ES,
    //SDL3 does support Vulkan
    GLGE::Graphic::GraphicAPI::VULKAN
};

const char* SDL3::getName() const noexcept
{return "GLGE_VIDEO_SDL_3_BUILTIN";}

GLGE::Graphic::Backend::Video::Instance* SDL3::createInstance(GLGE::Graphic::Instance* instance, GLGE::Graphic::GraphicAPI api, GLGE::Version version)
{return new GLGE::Graphic::Backend::Video::SDL3::Instance(instance, api, version);}

GLGE::Graphic::Backend::Video::Window* SDL3::createWindow(GLGE::Graphic::Window* window)
{return new GLGE::Graphic::Backend::Video::SDL3::Window(window);}

GLGE::Reference<GLGE::Graphic::Backend::Video::Cursor> SDL3::createCursor([[maybe_unused]] GLGE::Graphic::Backend::Video::Cursor::Defaults style)
{return Reference<Backend::Video::Cursor>(new Backend::Video::SDL3::Cursor(style));}

GLGE::Reference<GLGE::Graphic::Backend::Video::Cursor> SDL3::createCursor([[maybe_unused]] const ImageCPU& image, [[maybe_unused]] const uvec2& hot)
{return Reference<Backend::Video::Cursor>(new Backend::Video::SDL3::Cursor(image, hot));}

const std::vector<GLGE::Graphic::GraphicAPI>& SDL3::getSupportedAPIs() 
{return __SUPPORTED_APIS;}