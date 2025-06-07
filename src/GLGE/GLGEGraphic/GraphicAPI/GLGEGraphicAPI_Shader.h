/**
 * @file GLGEGraphicAPI_Shader.h
 * @author DM8AT
 * @brief define a wrapper that abstracts the API dependend behaviour of shaders
 * @version 0.1
 * @date 2025-05-13
 * 
 * @copyright Copyright (c) 2025
 * 
 */

//header guard
#ifndef _GLGE_GRAPHIC_API_SHADER_
#define _GLGE_GRAPHIC_API_SHADER_

//include the settings
#include "../../GLGECore/GLGESettings.h"
//include instances
#include "GLGEGraphicAPI_Instance.h"
//include command buffers
#include "GLGEGraphicAPI_CommandBuffer.h"

/**
 * @brief define the type of the shader. The mapping is forced as follows:
 *  0 : Vertex Shader
 *  1 : Fragment Shader
 *  2 : Geometry Shader
 *  3 : Tesselation controll Shader
 *  4 : Tesselatoin evaluation Shader
 *  5 : Compute Shader
 */
typedef enum e_ShaderType {
    /**
     * @brief specify that the shader type is a vertex shader
     */
    SHADER_TYPE_VERTEX = 0,
    /**
     * @brief specify that the shader type is a fragment shader
     */
    SHADER_TYPE_FRAGMENT = 1,
    /**
     * @brief specify that the shader type is a geometry shader
     */
    SHADER_TYPE_GEOMETRY = 2,
    /**
     * @brief specify that the shader is a tesselation controll shader
     */
    SHADER_TYPE_TESSELATION_CONTROLL = 3,
    /**
     * @brief specify that the shader is a tesselation evaluation shader
     */
    SHADER_TYPE_TESSELATION_EVALUATION = 4,
    /**
     * @brief specify that the shader is a compute shader
     */
    SHADER_TYPE_COMPUTE = 5
} ShaderType;

//check for C++
#if GLGE_CPP

//say that shaders will be defined
class Shader;

/**
 * @brief store an abstracted shader class
 */
class GraphicShader : GraphicInstAttatchable
{
public:

    GraphicShader() = default;

    /**
     * @brief Construct a new Graphic Shader
     * 
     * @param shader a pointer to the parent shader
     * @param instance the graphic instance the shader will belong to
     */
    GraphicShader(Shader* shader, GraphicInstance* instance) : GraphicInstAttatchable(instance), m_shader(shader) {}

    /**
     * @brief Destroy the Graphic Shader
     */
    virtual ~GraphicShader();

    /**
     * @brief Get the parent shader
     * 
     * @return Shader* a pointer to the parent shader
     */
    Shader* getShader() const noexcept {return m_shader;}

    /**
     * @brief a hook for creation
     */
    virtual void onCreate() noexcept {}

    /**
     * @brief a hook for destruction
     */
    virtual void onDestroy() noexcept {}

    /**
     * @brief a hook for the shader attatchment
     * 
     * @param cmdBuff the command buffer to enable the buffer on
     */
    virtual void onAttatch(GraphicCommandBuffer*) noexcept {}

protected:

    /**
     * @brief store the parent shader
     */
    Shader* m_shader = 0;

};

#endif // end of C++ section

#endif