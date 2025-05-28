/**
 * @file GLGEShaderCompiler.h
 * @author DM8AT
 * @brief define the layout for a shader processor class that converts from GLGE Shaders to GLSL
 * @version 0.1
 * @date 2025-05-13
 * 
 * @copyright Copyright (c) 2025
 * 
 */

//header guard
#ifndef _GLGE_SHADER_PROCESSOR_
#define _GLGE_SHADER_PROCESSOR_

//include the settings
#include "../../GLGECore/GLGESettings.h"
//include shaders
#include "GLGEShader.h"
//include shader extensions
#include "GLGEShaderExtension.h"

//check for C++
#if GLGE_CPP

/**
 * @brief a shader processor is a class that converts GLGE Shader code to GLSL
 */
class ShaderProcessor
{
public:

    /**
     * @brief Construct a new Shader Processor
     */
    ShaderProcessor();

    /**
     * @brief Destroy the Shader Processor
     */
    ~ShaderProcessor();

    /**
     * @brief load a new package
     * 
     * @param path the path to the file to load the package from
     * @param name the name of the package used when including it
     * 
     * @return true : the package was loaded successfully
     * @return false : the package file was not found
     */
    bool loadPackage(const Path& path, const std::string_view& name);

    /**
     * @brief check if a package is loaded
     * 
     * @param name the name of the package to check
     * @return true : the package is loaded
     * @return false : the package is not loaded
     */
    bool containsPackage(const std::string_view& name) const noexcept {return (m_packages.find(name) != m_packages.end());}

    /**
     * @brief print the shader processor and its contents
     * 
     * @param os the output stream to print to
     * @param sp the shader processor to print
     * @return std::ostream& 
     */
    friend std::ostream& operator<<(std::ostream& os, const ShaderProcessor& sp) noexcept;

    //add the shader as a friend class
    friend class Shader;

    /**
     * @brief Get all packages loaded to the shader processor
     * 
     * @return std::unordered_map<std::string_view, std::string>& 
     */
    inline const std::unordered_map<std::string_view, std::string>& getPackages() noexcept {return m_packages;}

    /**
     * @brief remove all comments from a specific string
     * 
     * @param source the string to remove the comments from
     */
    static void removeComments(std::string& source);

protected:

    /**
     * @brief process the code of a shader
     * 
     * @param shader the shader to process
     * 
     * @return true : the shader was successfully processed
     * @return false : failed to process the shader
     */
    bool processShader(Shader& shader) noexcept;

    /**
     * @brief store the package names and the corresponding source code
     */
    std::unordered_map<std::string_view, std::string> m_packages;
    /**
     * @brief store the shader extensions loaded to this shader processor
     */
    std::vector<ShaderExtension*> m_extensions;
    /**
     * @brief store the amount of loaded unique extensions
     */
    uint64_t m_uniqueExtensions = 0;

};

#endif

#endif