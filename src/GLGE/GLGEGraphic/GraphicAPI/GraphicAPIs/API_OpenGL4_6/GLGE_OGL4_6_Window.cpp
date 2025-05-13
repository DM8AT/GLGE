/**
 * @file GLGE_OGL3_3_Window.cpp
 * @author DM8AT
 * @brief implement the functionality for the OpenGL 3.3 window
 * @version 0.1
 * @date 2025-04-16
 * 
 * @copyright Copyright (c) 2025
 * 
 */

//include the OpenGL 3.3 window
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

void OGL3_3_RenderFunc(void* data, uint64_t)
{
    //extract the window
    OGL3_3_Window* window = (OGL3_3_Window*)data;

    //check if GLEW is initalized
    if (!((OGL3_3_Instance*)window->getInstance())->isGlewInitalized())
    {
        //if not, initalize GLEw
        ((OGL3_3_Instance*)window->getInstance())->initalizeGLEW(window);
    }
    //make this the current window
    SDL_GL_MakeCurrent((SDL_Window*)window->getWindow()->getSDL2Window(), ((OGL3_3_Instance*)window->getInstance())->getContext());

    SDL_GL_SwapWindow((SDL_Window*)window->getWindow()->getSDL2Window());
}

void OGL3_3_RenderThread(OGL3_3_Window* window)
{
    //loop while the instance is running
    while (window->isRendererActive())
    {
        //check if the command buffer is filled
        if (!window->getCommandBuffer()->isFilled())
        {
            //lock the command buffer
            window->getCommandBuffer()->begin();
            //record the command buffer
            window->getCommandBuffer()->add(0, (void*)OGL3_3_RenderFunc, (void*)window, sizeof(window));
            //stop the command buffer
            window->getCommandBuffer()->end();
        }

        //end the render thread for the window
        window->getWindow()->getRenderLimiter().endTick();
    }
}

void OGL3_3_Window::onCreate()
{
    //create the command buffer
    m_buffer = new OGL3_3_CommandBuffer(m_graphicInstance);
    //add the command buffer to the instance
    m_graphicInstance->addCommandBuffer(m_buffer);
    //start the render thread
    m_runRenderer = true;
    m_render = std::thread(OGL3_3_RenderThread, this);
}

void OGL3_3_Window::onDestroy()
{
    //stop the render thread
    m_runRenderer = false;
    m_render.join();
    //lock the buffer
    m_buffer->begin();
    //remove the buffer from the instance
    m_graphicInstance->removeCommandBuffer(m_buffer);
    //destroy the command buffer
    delete m_buffer;
    m_buffer = 0;
}