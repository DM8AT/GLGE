/**
 * @file GLGE_OGL4_6_Window.cpp
 * @author DM8AT
 * @brief implement the functionality for the OpenGL 4.6 window
 * @version 0.1
 * @date 2025-04-16
 * 
 * @copyright Copyright (c) 2025
 * 
 */

//include the OpenGL 4.6 window
#include "GLGE_OGL4_6_Window.h"
//include the OpenGL instance
#include "GLGE_OGL4_6_Instance.h"
//include the OpenGL command buffer
#include "GLGE_OGL4_6_CommandBuffer.h"
//include windows
#include "../../../GLGEWindow.h"

//include SDL2 and OpenGL
#include <SDL2/SDL.h>
#include <GL/glew.h>

void OGL4_6_Window::OGL4_6_WindowInitFunc(OGL4_6_Window* window, uint64_t) noexcept
{
    //check if GLEW is initalized
    if (!((OGL4_6_Instance*)window->getInstance())->isGlewInitalized())
    {
        //if not, initalize GLEw
        ((OGL4_6_Instance*)window->getInstance())->initalizeGLEW(window);
    }
    //make this the current window
    SDL_GL_MakeCurrent((SDL_Window*)window->m_window->getSDL2Window(), ((OGL4_6_Instance*)window->getInstance())->getContext());
}

void OGL4_6_Window::OGL4_6_WindowMakeCurrent(OGL4_6_Window* window, uint64_t) noexcept
{
    //make this the current window
    SDL_GL_MakeCurrent((SDL_Window*)window->m_window->getSDL2Window(), ((OGL4_6_Instance*)window->getInstance())->getContext());
    //check if the size changed
    if ((window->m_size.x != window->getWindow()->getSize().x) || (window->m_size.y != window->getWindow()->getSize().y))
    {
        //update the currently known size
        window->m_size = window->getWindow()->getSize();
        //set the viewport
        glViewport(0,0, window->m_size.x, window->m_size.y);
    }
}

void OGL4_6_Window::OGL4_6_WindowSwapFunc(OGL4_6_Window* window, uint64_t) noexcept
{
    //make sure to bind the window (required for Mac OS)
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    //swap the window's surface
    SDL_GL_SwapWindow((SDL_Window*)window->m_window->getSDL2Window());
}

void OGL4_6_Window::onCreate()
{
    //add the init command
    m_window->getInstance()->getGraphicInstance()->getBuffers()[0]->add(0, (void*)OGL4_6_WindowInitFunc, this, sizeof(this));
}

void OGL4_6_Window::makeCurrent(GraphicCommandBuffer* cmdBuff)
{
    //check if this is the currently active window. If not, queue the command
    if (cmdBuff->getCurrentWindow() != m_window)
    {
        //queue the make current command
        cmdBuff->add(0, (void*)OGL4_6_WindowMakeCurrent, this, sizeof(this));
        //set the new window pointer
        cmdBuff->setCurrentWindow(m_window);
    }
}

void OGL4_6_Window::swap(GraphicCommandBuffer* cmdBuff)
{
    //queue the swapping
    cmdBuff->add(0, (void*)OGL4_6_WindowSwapFunc, this, sizeof(this));
}