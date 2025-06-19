/**
 * @file GLGE_OGL4_6_Instance.cpp
 * @author DM8AT
 * @brief implement the OpenGL 4.6 instance
 * @version 0.1
 * @date 2025-04-16
 * 
 * @copyright Copyright (c) 2025
 * 
 */

//include the instance
#include "GLGE_OGL4_6_Instance.h"
//include command buffers
#include "../../GLGEGraphicAPI_CommandBuffer.h"

//include windows
#include "GLGE_OGL4_6_Window.h"
#include "../../../GLGEWindow.h"

//include OpenGL
#include <GL/glew.h>
//include SDL2
#include <SDL2/SDL.h>

/**
 * @brief a callback function for an OpenGL error
 * 
 * @param source the source of the error
 * @param type the type of the error
 * @param id the error's ID
 * @param severity the severity of the error
 * @param length the length of the error message
 * @param message the error message
 * @param instance a pointer to the instance the call came from
 */
static void oglMessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* instance)
{
    //extract the instance
    OGL4_6_Instance* inst = (OGL4_6_Instance*)instance;

    //stop junk from passing through
    if (severity == GL_DEBUG_SEVERITY_NOTIFICATION) {return;}
    
    //check if debugging is disabled
    #if !GLGE_ENABLE_DEBUGGING
    //allow only performance warnings and errors
    if (!((type == GL_DEBUG_TYPE_ERROR) || (type == GL_DEBUG_TYPE_PERFORMANCE)))
    {return;}
    #endif //no debug section

    //construct an ouput stream
    std::stringstream stream;
    //fill the stream with the error
    stream << "OpenGL error occourred at " << source << ". Severity: " << severity << ", callback type: " << type << ". Message: \n" << message << "\n";
    //log the stream
    inst->getInstance()->log(stream, MESSAGE_TYPE_INFO);
}

void OGL4_6_Instance::onRender()
{
    //loop over all command buffers
    for (size_t i = 0; i < m_buffers.size(); ++i)
    {
        //execute the command buffer
        m_buffers[i]->play();
    }
}

void OGL4_6_Instance::initalizeGLEW(OGL4_6_Window* window)
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

    //add error logging
    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(oglMessageCallback, this);

    //store all needed extensions
    const std::vector<std::string_view> needed = {
        //bindless textures are needed for textures
        "GL_ARB_bindless_texture"
    };
    //store the amount of found extensions
    uint64_t found = 0;

    //get all OpenGL extensions
    int32_t extensionCount = 0;
    glGetIntegerv(GL_NUM_EXTENSIONS, &extensionCount);
    //iterate over all extensions
    for (int32_t i = 0; i < extensionCount; ++i)
    {
        //get the name of the extension
        const GLubyte* name = glGetStringi(GL_EXTENSIONS, i);
        //convert the name to a const string view
        std::string_view view = (const char*)name;

        //check if the element is in the needed list
        if (std::find(needed.begin(), needed.end(), view) != needed.end())
        {
            //increment the amount of found extensions
            ++found;
        }
    }
    //check if not all extensions where found
    if (found != needed.size())
    {
        //fatal error!
        //inform about the error
        m_instance->log("Failed to find all neccecery OpenGL extensions. Nececcery are:\n - GL_ARB_bindless_texture", MESSAGE_TYPE_FATAL_ERROR);
        return;
    }

    //store that GLEW is initalized
    m_hasGLEW = true;
    //add the data command buffer. It must be buffer 0. 
    addCommandBuffer(&m_dataBuffer);

    //create the texture buffer
    m_textures = new OGL4_6_MemoryArena(0, true, MEMORY_USAGE_READ_WRITE, *m_instance);
    m_textures->onCreate();
    m_textures->setAPI(true);
    //create the image buffer
    m_images = new OGL4_6_MemoryArena(0, true, MEMORY_USAGE_READ_WRITE, *m_instance);
    m_images->onCreate();
    m_images->setAPI(true);

    //initalization is done
    m_isSetupFinished = true;
}

void OGL4_6_Instance::onCreate()
{
    //say that GLEW is not initalized
    m_hasGLEW = false;
    //create the data command buffer
    m_dataBuffer.create(this);
}

void OGL4_6_Instance::onDestroy()
{
    //remove the command buffer
    removeCommandBuffer(&m_dataBuffer);
    //delete the command buffer
    m_dataBuffer.destroy();
    //free the texture and image buffer
    delete m_textures;
    delete m_images;
    m_textures = 0;
    m_images = 0;
    //clear the context
    SDL_GL_DeleteContext(m_context);
    m_context = 0;
}