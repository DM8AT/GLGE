/**
 * @file Window.cpp
 * @author DM8AT
 * @brief implement the OpenGL window
 * @version 0.1
 * @date 2025-12-27
 * 
 * @copyright Copyright (c) 2025
 * 
 */
//include the OpenGL window
#include "Graphic/Backend/Builtin/Graphics/OpenGL/Window.h"

//add OpenGL
#include "glad/glad.h"

//include OpenGL instances
#include "Graphic/Backend/Builtin/Graphics/OpenGL/Instance.h"
//add video windows
#include "Graphic/Backend/Video/Window.h"

//include profiling
#include "Core/Profiler.h"

//use the namespace
using namespace GLGE::Graphic::Backend::Graphic::OpenGL;

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

    //make this the current window
    getWindow()->getVideoWindow()->onMakeCurrent(getWindow()->getGraphicInstance()->getGraphicBackendInstance()->getContext());
    //early out if nothing changed
    if (res.x == m_resolution.x && res.y == m_resolution.y)
    {return;}

    //set the viewport
    glViewport(0, 0, res.x, res.y);
    //swap the window
    getWindow()->getVideoWindow()->onSwapWindow();
    //store the new rendering resolution
    m_resolution = res;
}

bool Window::onVSyncSet(VSync vsync) {
    //use the windowing engine to set the vsync mode
    bool s = getWindow()->getVideoWindow()->onSetVSync(vsync, getWindow()->getGraphicInstance()->getGraphicBackendInstance()->getContext());
    m_vsync = s ? vsync : VSYNC_ENABLED;
    return s;
}