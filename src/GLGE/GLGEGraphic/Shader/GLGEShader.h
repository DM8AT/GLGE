/**
 * @file GLGEShader.h
 * @author DM8AT
 * @brief Store the deffination for a shader class
 * @version 0.1
 * @date 2025-05-13
 * 
 * @copyright Copyright (c) 2025
 * 
 */

//header guard
#ifndef _GLGE_SHADER_
#define _GLGE_SHADER_

//include the settings
#include "../../GLGECore/GLGESettings.h"
//include instances
#include "../../GLGECore/GLGEInstance.h"
//include the API Shader
#include "../GraphicAPI/GLGEGraphicAPI_Shader.h"
//include pathes
#include "../../GLGECore/Files/GLGEPath.h"

//check for C++
#if GLGE_CPP

//say that a shader processor will be defined
class ShaderProcessor;

/**
 * @brief a shader is a code program that runs on the GPU
 */
class Shader : public InstAttachableClass
{
public:

    /**
     * @brief Construct a new Shader
     */
    Shader() = default;

    /**
     * @brief Construct a new Shader
     * 
     * @param path the path to the file to load as a shader
     * @param processor the shader processor to use for compillation
     * @param instance the instance the shader will belong to
     */
    Shader(const Path& path, ShaderProcessor* processor, const Instance& instance);

    /**
     * @brief Destroy the Shader
     */
    virtual ~Shader();

    //add the shader processor as a friend class
    friend class ShaderProcessor;

    /**
     * @brief Set or add an entry point
     * @warning DO NOT USE THIS MANUALY!
     * 
     * @param type the type of shader process to add the entry point for
     * @param string the function name of the entry point for this shader stage
     */
    void setEntryPoint(ShaderType type, const std::string& string) {m_shaders[type] = string;}

    /**
     * @brief Get the file path to the shader file
     * 
     * @return const std::string& the path to the shader file
     */
    const std::string& getFile() const noexcept {return m_file;}

    /**
     * @brief make this the current shader. This updates all needed uniforms. 
     * 
     * @param cmdBuffer the command buffer to attach the shader on
     */
    void attatch(GraphicCommandBuffer* cmdBuffer) noexcept;

    /**
     * @brief Get the Shader Stages and the corresponding entry points the shader contains
     * 
     * @return const std::unordered_map<ShaderType, std::string>& the shader's stored shader stages and corresponding entry point names
     */
    inline std::unordered_map<ShaderType, std::string>& getShaderStages() noexcept {return m_shaders;}

    /**
     * @brief prepare a sourcecode snipped for a shader stage
     * 
     * @param src the source code of the stage to prepare
     * @param stage the shader stage to prepare for
     */
    static void prepareForStage(std::string& src, ShaderType stage) noexcept;

    //add the graphic shader class as a friend class
    friend class GraphicShader;

    /**
     * @brief Get the source processed by GLGE from the shader file
     * 
     * @return const std::string& a constant reference to the shader source code
     */
    inline const std::string& getSource() const noexcept {return m_glgeSource;}

protected:

    /**
     * @brief store the absolute path to the file
     */
    std::string m_file = "";
    /**
     * @brief store the unprocessed GLGE shader source code
     */
    std::string m_glgeSource;
    /**
     * @brief store all the shader stages
     */
    std::unordered_map<ShaderType, std::string> m_shaders;
    /**
     * @brief store the actual graphic shader
     */
    GraphicShader* m_shader = 0;

};

#endif //end of C++ section

#endif