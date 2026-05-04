/**
 * @file Instance.cpp
 * @author DM8AT
 * @brief implement the OpenGL 4.6 instance
 * @version 0.1
 * @date 2025-12-23
 * 
 * @copyright Copyright (c) 2025
 * 
 */
//include the instance
#include "Graphic/Backend/Builtin/Graphics/OpenGL/Instance.h"

//add graphic instances
#include "Graphic/Instance.h"
//add OpenGL windows
#include "Graphic/Backend/Builtin/Graphics/OpenGL/Window.h"
//add frontend windows
#include "Graphic/Window.h"

//include glad
#include "glad/glad.h"

//include profiling
#include "Core/Profiler.h"

//include the description
#include "Graphic/Backend/Builtin/Graphics/OpenGL/Description.h"

//add the OpenGL contract
#include "Graphic/Backend/Video/APIContracts/OpenGL.h"

//use the OpenGL backend namespace
using namespace GLGE::Graphic::Backend::Graphic::OpenGL;

/**
 * @brief a function for the OpenGL debug callback
 * 
 * @param source the source of the callback
 * @param type the type of callback
 * @param id the ID of the message
 * @param severity the severity
 * @param length the length of the message
 * @param message the actual message
 * @param userParam arbitrary user arguments
 */
static void APIENTRY openglDebugCallback(GLenum source,
                                  GLenum type,
                                  GLuint id,
                                  GLenum severity,
                                  GLsizei length,
                                  const GLchar* message,
                                  const void* userParam)
{
    if (severity == GL_DEBUG_SEVERITY_HIGH ||     // errors
        severity == GL_DEBUG_SEVERITY_MEDIUM ||   // warnings
        severity == GL_DEBUG_SEVERITY_LOW)        // perf warnings
    {
        std::cout << "[OpenGL] (" << id << ") "
                  << message << "\n"
                  << "  Type: " << type
                  << " | Severity: " << severity
                  << std::endl;
    }
}

Instance::Instance(GLGE::Graphic::Instance* instance)
 : GLGE::Graphic::Backend::Graphic::Instance(instance)
{
}

Instance::~Instance() {
}

void Instance::onBind() {
    
}

void Instance::onUnbind() {

}

void Instance::onRegisterWindow(GLGE::Graphic::Backend::Graphic::Window* window) {
    //increase the window count. If it was 0 create a new OpenGL context
    if (m_windowCount.fetch_add(1, std::memory_order_acq_rel) == 0) {
        //first, fetch the graphic instance
        GLGE::Graphic::Instance* inst = window->getWindow()->getGraphicInstance();
        //then, create the context using the settings from the graphic instance
        auto* contract = inst->getVideoBackendInstance()->getContract<GLGE::Graphic::Backend::Video::Contracts::OpenGL>();
        contract->createContext(window->getWindow()->getVideoWindow());

        //now, initialize OpenGL using glad

        //initialize GLAD
        if (!gladLoadGL())
        {throw GLGE::Exception("Failed to create the OpenGL context", "GLGE::Graphic::Backend::OpenGL::Instance::registerWindow");}

        //enable OpenGL debug callbacks for error catching
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        //set the callback function
        glDebugMessageCallback(openglDebugCallback, nullptr);

        //setup anisotropic sampling
        if (GLAD_GL_EXT_texture_filter_anisotropic) {
            //supports anisotropic sampling
            m_extensionInfo.anisotropic.supported = true;
            //quarry the maximum supported anisotropy level
            glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY, &m_extensionInfo.anisotropic.maxAnisotropy);
        } else {
            //anisotropic sampling not supported
            m_extensionInfo.anisotropic.supported = false;
            m_extensionInfo.anisotropic.maxAnisotropy = 1;
        }

        //setup multi sample extensions
        glGetIntegerv(GL_MAX_SAMPLES, &m_extensionInfo.multiSample.maxSampleCount);
        
        //enable depth testing
        glEnable(GL_DEPTH_TEST);

        //get the GPU information
        m_instance->getGraphicDescription()->initialize();

        //initialize the instance
        m_instance->onGraphicBackendInit();
    }

    //initialize the window
    window->onWindowSetup();
}

void Instance::onRemoveWindow(GLGE::Graphic::Backend::Graphic::Window* window) {
    //decrease the window count. If it was 1 (now is 0) then destroy the context
    if (m_windowCount.fetch_sub(1, std::memory_order_acq_rel) == 1) {
        //cleanup backend critical instance stuff
        m_instance->onGraphicBackendDestroy();
        //first, fetch the graphic instance
        GLGE::Graphic::Instance* inst = window->getWindow()->getGraphicInstance();
        //then destroy the context
        window->getWindow()->getGraphicInstance()->getVideoBackendInstance()->getContract<GLGE::Graphic::Backend::Video::Contracts::OpenGL>()->destroyContext();
    }
}