/**
 * @file GLGE_IncludeStatement.h
 * @author DM8AT
 * @brief define a shader extension to manage include statements
 * @version 0.1
 * @date 2025-05-23
 * 
 * @copyright Copyright (c) 2025
 * 
 */

//header guard
#ifndef _GLGE_SHADER_EXT_UTIL_INCLUDE_
#define _GLGE_SHADER_EXT_UTIL_INCLUDE_

//include shadder extensions
#include "../../GLGEShaderExtension.h"

//check for C++
#if GLGE_CPP

/**
 * @brief specify an extension that adds an include statement to GLGE shaders
 */
class ShaderExt_Include : public ShaderExtension
{
public:

    /**
     * @brief Construct a new shader inclusion extension
     */
    ShaderExt_Include() : ShaderExtension(identifyer) {}

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
    virtual bool onFind(std::string& source, ShaderProcessor* processor, Shader* shader, uint64_t elementIdx) noexcept;

    /**
     * @brief store the identifyer for the include directive
     */
    static const char* identifyer;

};

#endif //C++ section

#endif