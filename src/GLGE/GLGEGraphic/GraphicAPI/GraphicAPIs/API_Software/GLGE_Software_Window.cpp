/**
 * @file GLGE_Software_Window.cpp
 * @author JuNi4
 * @brief 
 * @version 0.1
 * @date 2025-07-02
 * 
 * @copyright Copyright (c) 2025
 * 
 */

//include the software window
#include "GLGE_Software_Window.h"
//include the software instance
#include "GLGE_Software_Instance.h"
//include the software command buffer
// #include "GLGE_Software_CommandBuffer.h"
//include windows
#include "../../../GLGEWindow.h"

//include SDL2 and OpenGL
#include <SDL2/SDL.h>
#include <GL/glew.h>

//check if ImGui is included
#if GLGE_3RD_PARTY_INCLUDE_DEAR_IMGUI

//include Dear ImGui
#include "../../../../GLGE3rdParty/imgui/imgui.h"
#include "../../../../GLGE3rdParty/imgui/backends/imgui_impl_sdl2.h"
#include "../../../../GLGE3rdParty/imgui/backends/imgui_impl_opengl3.h"

#endif //end of ImGui section

void Software_Window::onCreate()
{
    //queue window initialization
    m_graphicInstance->getBuffers()[0]->add(0, (void*)Software_WindowInitFunc, this, sizeof(this));
}

void Software_Window::Software_WindowInitFunc(Software_Window* window, uint64_t) noexcept
{
    //create the window surface
    window->m_windowSurface = SDL_GetWindowSurface((SDL_Window*)(window->m_window->getSDL2Window()));

    //check if GLEW is initalized
    if (!((Software_Instance*)window->getInstance())->isImGuiInitialized())
    {
        //if not, initalize GLEw
        ((Software_Instance*)window->getInstance())->initializeImGui(window);
    }
}

void Software_Window::Software_WindowSwapFunc(Software_Window* window, uint64_t) noexcept
{
    //swap the surfaces
    SDL_UpdateWindowSurface((SDL_Window*)(window->m_window->getSDL2Window()));
}

void Software_Window::swap(GraphicCommandBuffer* cmdBuff)
{
    //queue the swapping
    cmdBuff->add(0, (void*)Software_WindowSwapFunc, this, sizeof(this));
}

void Software_Window::makeCurrent(GraphicCommandBuffer* cmdBuff)
{
    //check if this is the currently active window. If not, queue the command
    if (cmdBuff->getCurrentWindow() != m_window)
    {
        //queue the make current command
        cmdBuff->add(0, (void*)Software_WindowMakeCurrent, this, sizeof(this));
        //set the new window pointer
        cmdBuff->setCurrentWindow(m_window);
    }
}

void Software_Window::Software_WindowMakeCurrent(Software_Window* window, uint64_t size) noexcept
{
    //check if the size changed
    if ((window->m_size.x != window->getWindow()->getSize().x) || (window->m_size.y != window->getWindow()->getSize().y))
    {
        //update the currently known size
        window->m_size = window->getWindow()->getSize();
        //set the viewport
        window->m_windowSurface = SDL_GetWindowSurface((SDL_Window*)(window->m_window->getSDL2Window()));
    }
}