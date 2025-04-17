/**
 * @file GLGE_OGL3_3_Instance.cpp
 * @author DM8AT
 * @brief implement the OpenGL 3.3 instance
 * @version 0.1
 * @date 2025-04-16
 * 
 * @copyright Copyright (c) 2025
 * 
 */

//include the instance
#include "GLGE_OGL3_3_Instance.h"
//include command buffers
#include "../../GLGEGraphicAPI_CommandBuffer.h"

//include windows
#include "GLGE_OGL3_3_Window.h"
#include "../../../GLGEWindow.h"

//include OpenGL
#include <GL/glew.h>
//include SDL2
#include <SDL2/SDL.h>

void OGL3_3_Instance::onRender()
{
    //loop over all command buffers
    for (size_t i = 0; i < m_buffers.size(); ++i)
    {
        //execute the command buffer
        m_buffers[i]->play();
    }
}

void OGL3_3_Instance::initalizeGLEW(OGL3_3_Window* window)
{
    //activate the context
    SDL_GL_MakeCurrent((SDL_Window*)window->getWindow()->getSDL2Window(), m_context);
    //initalize GLEW
    GLenum res = glewInit();
    //check if GLEW initalized good
    if (res != GLEW_OK)
    {
        //get the error string
        char* str = (char*)glewGetErrorString(res);
        //print an error
        std::stringstream out;
        out << "Failed to initalize GLEW: " << str;
        m_instance->log(out, MESSAGE_TYPE_FATAL_ERROR);
        return;
    }

    //store that GLEW is initalized
    m_hasGLEW = true;
}

void OGL3_3_Instance::onCreate()
{
    //say that GLEW is not initalized
    m_hasGLEW = false;
}

void OGL3_3_Instance::onDestroy()
{
    //clear the context
    SDL_GL_DeleteContext(m_context);
    m_context = 0;
}