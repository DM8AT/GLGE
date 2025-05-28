/**
 * @file GLGEShaderExtension.h
 * @author DM8AT
 * @brief define a class that can be used to extend the shader processor functionality
 * @version 0.1
 * @date 2025-05-22
 * 
 * @copyright Copyright (c) 2025
 * 
 */

//header guard
#ifndef _GLGE_SHADER_EXTENSION_
#define _GLGE_SHADER_EXTENSION_

//include the settings
#include "../../GLGECore/GLGESettings.h"

//check for C++
#if GLGE_CPP

//say that shaders will exist
class Shader;
//say that shader processors will exist
class ShaderProcessor;

/**
 * @brief store a single shader processor functionality
 */
class ShaderExtension
{
public:

    /**
     * @brief Construct a new Shader Extension
     * 
     * @param identifyer the identifyer for the shader extension
     */
    ShaderExtension(const std::string_view& identifyer) : m_identifyer(identifyer) {}

    /**
     * @brief Destroy the Shader Extension
     */
    virtual ~ShaderExtension() {}

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
    virtual bool onFind(std::string&, ShaderProcessor*, Shader*, uint64_t) noexcept {return false;}

    /**
     * @brief Get the Identifyer
     * 
     * @return const std::string_view& the identifyer for the extension class
     */
    const std::string_view& getIdentifyer() const noexcept {return m_identifyer;}

protected:

    /**
     * @brief store the identifyer for this 
     */
    std::string_view m_identifyer = "";

};

#endif //C++ section

#endif