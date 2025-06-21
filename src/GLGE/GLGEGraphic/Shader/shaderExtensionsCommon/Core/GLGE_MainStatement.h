/**
 * @file GLGE_MainStatement.h
 * @author DM8AT
 * @brief define a shader extension that handles the main statement
 * @version 0.1
 * @date 2025-05-22
 * 
 * @copyright Copyright (c) 2025
 * 
 */

//header guard
#ifndef _GLGE_SHADER_EXT_CORE_MAIN_STATEMENT_
#define _GLGE_SHADER_EXT_CORE_MAIN_STATEMENT_

//include shadder extensions
#include "../../GLGEShaderExtension.h"

//check for C++
#if GLGE_CPP

/**
 * @brief add a shader extension that adds main statements
 */
class ShaderExt_Main : public ShaderExtension
{
public:

    /**
     * @brief Construct a new main shader extension
     */
    ShaderExt_Main() : ShaderExtension(identifyer) {}

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
     * @brief store the identifyer for the main directive
     */
    static const char* identifyer;

};

#endif //C++ section

#endif