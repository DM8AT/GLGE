/**
 * @file GLGE_OGL4_6_Shader.h
 * @author DM8AT
 * @brief implement the shader overload for OpenGL 4.6
 * @version 0.1
 * @date 2025-05-13
 * 
 * @copyright Copyright (c) 2025
 * 
 */

//header guard
#ifndef _GLGE_OGL_4_6_SHADER_
#define _GLGE_OGL_4_6_SHADER_

//include the settings
#include "../../GLGEGraphicAPI_Instance.h"
//include grpahic shader
#include "../../GLGEGraphicAPI_Shader.h"

//check for C++
#if GLGE_CPP

/**
 * @brief overload the shader
 */
class OGL4_6_Shader : public GraphicShader
{
public:

    /**
     * @brief Construct a new ogl4 6 shader
     */
    OGL4_6_Shader() = default;

    /**
     * @brief Construct a new OpenGL 4.6 shader
     * 
     * @param shader a pointer to the parent shader
     * @param instance a pointer to the instance the shader will belong to
     */
    OGL4_6_Shader(Shader* shader, GraphicInstance* instance) : GraphicShader(shader, instance) {}

    /**
     * @brief overrid the hook for creation
     */
    virtual void onCreate() noexcept override;

    /**
     * @brief override the destruction hook
     */
    virtual void onDestroy() noexcept override;

    /**
     * @brief override the attatch hook
     */
    virtual void onAttatch() noexcept override;

protected:

    /**
     * @brief store the OpenGL shader program
     */
    uint32_t m_program = 0;

};

#endif // end of C++ section

#endif