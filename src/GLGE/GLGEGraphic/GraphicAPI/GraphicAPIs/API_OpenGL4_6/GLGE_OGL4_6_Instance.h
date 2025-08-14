/**
 * @file GLGE_OGL4_6_Instance.h
 * @author DM8AT
 * @brief define instances for OpenGL 4.6
 * @version 0.1
 * @date 2025-04-16
 * 
 * @copyright Copyright (c) 2025
 * 
 */

//header guard
#ifndef _GLGE_OGL_4_6_INSTANCE_
#define _GLGE_OGL_4_6_INSTANCE_

//include instances
#include "../../GLGEGraphicAPI_Instance.h"
//include command buffers
#include "GLGE_OGL4_6_CommandBuffer.h"
//include memory arenas
#include "GLGE_OGL4_6_MemoryArena.h"

//check if this is C++
#if GLGE_CPP

//OpenGL 4.6 Windows will be defined later
class OGL4_6_Window;

/**
 * @brief define an instance for OpenGL 4.6
 */
class OGL4_6_Instance : public GraphicInstance
{
public:

    /**
     * @brief Construct a new OpenGL 4.6 Instance
     */
    OGL4_6_Instance() = default;

    /**
     * @brief Construct a new OpenGL 4.6 instance
     * 
     * @param context a pointer to the OpenGL context
     * @param instance a pointer to the instance the graphic instance belongs to
     */
    OGL4_6_Instance(void* context, Instance* instance)
     : GraphicInstance(instance), m_context(context)
    {onCreate();}

    /**
     * @brief Destroy the OpenGL instance
     */
    virtual ~OGL4_6_Instance() {}

    /**
     * @brief run the rendering for the OpenGL 4.6 instance
     */
    void onRender() override;

    /**
     * @brief Set the Context object
     * 
     * @param context the context for the OpenGL instance
     */
    inline void setContext(void* context) noexcept {m_context = context;}

    /**
     * @brief Get the OpenGL context
     * 
     * @return void* a pointer to the context
     */
    inline void* getContext() noexcept {return m_context;}

    /**
     * @brief get if GLEW is initalized
     * 
     * @return true : GLEW is initalized
     * @return false : GLEW is not initalized
     */
    inline bool isGlewInitalized() const noexcept {return m_hasGLEW;}

    /**
     * @brief initalize GLEW
     * 
     * @param window a pointer to the window to use for initalization
     */
    void initalizeGLEW(OGL4_6_Window* window);

    /**
     * @brief Get the Data Buffer
     * 
     * @return OGL4_6_CommandBuffer& the data buffer
     */
    inline OGL4_6_CommandBuffer& getDataBuffer() noexcept {return m_dataBuffer;}

protected:

    /**
     * @brief override the create hook
     */
    void onCreate() override;
    /**
     * @brief override the destruction hook
     */
    void onDestroy() override;

    /**
     * @brief store the data command buffer
     */
    OGL4_6_CommandBuffer m_dataBuffer;

    /**
     * @brief store that GLEW is not initalized
     */
    bool m_hasGLEW = false;

    /**
     * @brief store the OpenGL context
     */
    void* m_context = 0;
};

#endif //C++ section

#endif