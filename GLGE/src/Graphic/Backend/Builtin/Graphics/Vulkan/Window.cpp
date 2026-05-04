/**
 * @file Window.cpp
 * @author DM8AT
 * @brief implement the vulkan window backend
 * @version 0.1
 * @date 2026-05-04
 * 
 * @copyright Copyright (c) 2026
 * 
 */
//include the Vulkan window
#include "Graphic/Backend/Builtin/Graphics/Vulkan/Window.h"

//include Vulkan instances
#include "Graphic/Backend/Builtin/Graphics/Vulkan/Instance.h"
//add video windows
#include "Graphic/Backend/Video/Window.h"

//include profiling
#include "Core/Profiler.h"

//use the namespace
using namespace GLGE::Graphic::Backend::Graphic::Vulkan;

Window::Window(GLGE::Graphic::Window* window) 
 : GLGE::Graphic::Backend::Graphic::Window(window)
{
    
}

Window::~Window() {
    
}

void Window::onWindowSetup() {
    //make a resolution change to the correct resolution
    onResolutionChange(getWindow()->getSize(), getWindow()->getUsableSize(), getWindow()->getResolution());
}

void Window::onResolutionChange(const uvec2& size, const uvec2& newUsableSize, const uvec2& res) {
    GLGE_PROFILER_SCOPE();

    //early out if nothing changed
    if (res.x == m_resolution.x && res.y == m_resolution.y)
    {return;}

    //store the new rendering resolution
    m_resolution = res;
}

bool Window::onVSyncSet(VSync vsync) {

    return false;
}