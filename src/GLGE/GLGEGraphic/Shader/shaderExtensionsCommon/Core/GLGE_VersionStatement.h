/**
 * @file GLGE_VersionStatement.h
 * @author DM8AT
 * @brief allows for the usage of GLGE versions in shaders instead of GLSL version numbers
 * @version 0.1
 * @date 2025-05-28
 * 
 * @copyright Copyright (c) 2025
 * 
 */

//header guard
#ifndef _GLGE_SHADER_EXT_CORE_VERSION_STATEMENT_
#define _GLGE_SHADER_EXT_CORE_VERSION_STATEMENT_

//include shadder extensions
#include "../../GLGEShaderExtension.h"

//check for C++
#if GLGE_CPP

/**
 * @brief store a shader extension that adds GLGE versions
 */
class ShaderExt_Version : public ShaderExtension
{
public:

    /**
     * @brief Construct a new version shader extension
     */
    ShaderExt_Version() : ShaderExtension(identifyer) {}

    /**
     * @brief store a virtual function that is called when the identifyer is found in the source code
     * 
     * @param source the source code for the shader
     * @param processor a pointer to the shader processor this shader extension was found by
     * @param shader the shader the source code belongs to
     * @param elementIdx the index of the start of the identifyer
     * 
     * @return true : the shader extension compiled successfully
     * @return false : the shader extension compillation failed
     */
    virtual bool onFind(std::string& source, ShaderProcessor* processor, Shader* shader, uint64_t elementIdx) noexcept override;

    /**
     * @brief store the identifyer for the version statement
     */
    static const char* identifyer;

};

#endif //C++ seciont

#endif