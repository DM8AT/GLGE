/**
 * @file Window.cpp
 * @author DM8AT
 * @brief implement the functionality of a frontend window
 * @version 0.1
 * @date 2025-12-27
 * 
 * @copyright Copyright (c) 2025
 * 
 */
//include frontend windows
#include "Graphic/Window.h"

//include the window backend
#include "Graphic/Backend/Graphics/Window.h"
#include "Graphic/Backend/Video/Window.h"

//add profiling
#include "Core/Profiler.h"

//use the library namespace
using namespace GLGE::Graphic;

Window::Window(const std::string& name, const uvec2& size, const WindowSettings& settings)
 : BaseClass(), m_name(name), m_size(size), m_inst(getInstance()->getExtension<GLGE::Graphic::Instance>()), m_settings(settings)
{
    GLGE_PROFILER_SCOPE();

    //in debug sanity check the graphic instance
    #if GLGE_DEBUG
    if (m_inst == nullptr)
    {throw GLGE::Exception("To use a window a graphic instance must extend the currently bound instance", "GLGE::Graphic::Window::Window");}
    #endif

    //create the window backend
    m_vWin = m_inst->m_vDesc->createWindow(this);
    m_gWin = m_inst->m_gDesc->createWindow(this);
    //register the window
    m_inst->getGraphicBackendInstance()->onRegisterWindow(m_gWin.get());
    m_inst->m_windows.push_back(this);
}

Window::~Window() {
    GLGE_PROFILER_SCOPE();

    //remove the window from the instance
    m_inst->getGraphicBackendInstance()->onRemoveWindow(m_gWin.get());
    for (size_t i = 0; i < m_inst->m_windows.size(); ++i) {
        if (m_inst->m_windows[i] == this)
        {m_inst->m_windows.erase(m_inst->m_windows.begin() + i); break;}
    }
    //clean up the window
    delete m_vWin;
    m_vWin = nullptr;
}

void Window::update() {
    //tick the graphic window
    m_gWin->onUpdate();
}

void Window::notifyResolutionChange(const uvec2& newSize, const uvec2& newUsableSize, float pixelScale, const uvec2& resolution) {
    GLGE_PROFILER_SCOPE();

    //store the new size
    m_size = newSize;
    //store the usable size
    m_usableSize = newUsableSize;
    //store the pixel scale
    m_pixelScale = pixelScale;
    //store the new resolutions
    m_theoreticalResolution = uvec2(m_size.x * m_pixelScale, m_size.y * m_pixelScale);
    m_resolution = resolution;
}

bool Window::isHDRActive() const noexcept
{return m_gWin->isHDRActive();}

bool Window::setVSyncMode(VSync vsync)
{return m_gWin->onVSyncSet(vsync);}

VSync Window::getVSyncMode() const noexcept
{return m_gWin->getVSync();}

void Window::setMinimumSize(const uvec2& size) noexcept
{m_vWin->onSetMinimumSize(size);}

void Window::setMaximumSize(const uvec2& size) noexcept
{m_vWin->onSetMaximumSize(size);}