/**
 * @file GLGEAsset_Shader.h
 * @author DM8AT
 * @brief define the asset layout for shader assets
 * @version 0.1
 * @date 2025-08-01
 * 
 * @copyright Copyright (c) 2025
 * 
 */

//header guard
#ifndef _GLGE_ASSET_SHADER_
#define _GLGE_ASSET_SHADER_

//include the settings
#include "../../GLGECore/GLGESettings.h"
//include shader
#include "../Shader/GLGEShader.h"

//check for C++ to create a class
#if GLGE_CPP

//assets will be defined later
class Asset;

/**
 * @brief load a shader from an external file
 */
class AssetShader
{
public:

    /**
     * @brief Construct a new Asset Shader
     * prevent an empty asset from existing
     */
    AssetShader() = delete;

    /**
     * @brief Construct a new Asset Shader
     * 
     * @param processor the shader processor to compile the shader with
     * @param textures a mapping from names to textures the shader will be able to access
     * @param buffers a mapping from names to buffer bindings the shader will be able to access
     * @param asset the asset the shader is loaded to
     */
    AssetShader(ShaderProcessor* processor, 
            std::unordered_map<std::string_view, Texture*> textures, 
            std::unordered_map<std::string_view, BufferShaderBinding> buffers, Asset* asset) noexcept;

    /**
     * @brief Get the actual shader object
     * 
     * @return Shader* a pointer to the created shader
     */
    inline Shader* getShader() const noexcept {return m_shader;}

    /**
     * @brief reload the contents from the file
     */
    void reloadFromFile() noexcept;

protected:

    /**
     * @brief store a pointer to the parent asset
     */
    Asset* m_asset = 0;
    /**
     * @brief store a pointer to the shader processor the shader compiles with
     */
    ShaderProcessor* m_processor = 0;
    /**
     * @brief store the created shader
     */
    Shader* m_shader = 0;

};

#endif //end of C++ section

#endif