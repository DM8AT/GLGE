/**
 * @file GLGE_OGL3_3_Instance.h
 * @author DM8AT
 * @brief define instances for OpenGL 3.3
 * @version 0.1
 * @date 2025-04-16
 * 
 * @copyright Copyright (c) 2025
 * 
 */

//header guard
#ifndef _GLGE_OGL_3_3_INSTANCE_
#define _GLGE_OGL_3_3_INSTANCE_

//include instances
#include "../../GLGEGraphicAPI_Instance.h"
//include command buffers
#include "GLGE_OGL3_3_CommandBuffer.h"

//check if this is C++
#if GLGE_CPP

//OpenGL 3.3 Windows will be defined later
class OGL3_3_Window;

/**
 * @brief define an instance for OpenGL 3.3
 */
class OGL3_3_Instance : public GraphicInstance
{
public:
    /**
     * @brief Construct a new OpenGL 3.3 instance
     */
    OGL3_3_Instance() = default;

    /**
     * @brief Construct a new OpenGL 3.3 instance
     * 
     * @param context a pointer to the OpenGL context
     * @param instance a pointer to the instance the graphic instance belongs to
     */
    OGL3_3_Instance(void* context, Instance* instance) : GraphicInstance(instance), m_context(context) {onCreate();}

    /**
     * @brief run the rendering for the OpenGL 3.3 instance
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
    void initalizeGLEW(OGL3_3_Window* window);

    /**
     * @brief Get the Data Buffer
     * 
     * @return OGL3_3_CommandBuffer& the data buffer
     */
    inline OGL3_3_CommandBuffer& getDataBuffer() noexcept {return m_dataBuffer;}

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
    OGL3_3_CommandBuffer m_dataBuffer;

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