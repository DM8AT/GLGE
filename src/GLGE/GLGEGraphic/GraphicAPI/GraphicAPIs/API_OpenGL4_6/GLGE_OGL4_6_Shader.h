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
     * @brief a hook for the shader attatchment
     * 
     * @param cmdBuff the command buffer to enable the buffer on
     */
    virtual void onAttatch(GraphicCommandBuffer* cmdBuff) noexcept override;

    /**
     * @brief a hook to finish the usage of this shader
     * 
     * @param cmdBuff the command buffer the shader should be disabled in
     */
    virtual void onDetatch(GraphicCommandBuffer* cmdBuff) noexcept override;

protected:

    /**
     * @brief add a single shader stage
     * 
     * @param type the type of the shader stage to add
     * @param shader the shader to clean up
     * 
     * @return true : successfully added the shader stage
     * @return false : failed to add the shader stage
     */
    bool compileSubProgram(ShaderType type, uint32_t& shader) noexcept;

    /**
     * @brief compile the shader
     * 
     * @param cmdData the data for the shader
     * @param cmdDataSize the size of the data for the command
     */
    static void compileShader(void* cmdData, uint64_t cmdDataSize) noexcept;

    /**
     * @brief delete the shader
     * 
     * @param cmdData the data for the shader
     * @param cmdDataSize the size of the data for the command
     */
    static void destroyShader(void* cmdData, uint64_t cmdDataSize) noexcept;

    /**
     * @brief attatch the shader
     * 
     * @param cmdData the data for the shader
     * @param cmdDataSize the size of the data for the command
     */
    static void attatchShader(void* cmdData, uint64_t cmdDataSize) noexcept;

    /**
     * @brief detatch the shader
     * 
     * @param cmdData stores the shader
     * @param cmdDataSize unused
     */
    static void detatchShader(void* cmdData, uint64_t cmdDataSize) noexcept;

    /**
     * @brief store the OpenGL shader program
     */
    uint32_t m_program = 0;

};

#endif // end of C++ section

#endif