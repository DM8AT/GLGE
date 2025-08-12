/**
 * @file GLGEAssetShader.h
 * @author DM8AT
 * @brief define the asset class for a shader asset
 * @version 0.1
 * @date 2025-08-04
 * 
 * @copyright Copyright (c) 2025
 * 
 */

//header guard
#ifndef _GLGE_ASSET_SHADER_
#define _GLGE_ASSET_SHADER_

//inlcude the asset base class
#include "../../GLGECore/Assets/GLGEAsset.h"
//include the shader class to encapsulate
#include "../Shader/GLGEShader.h"

//check for C++ to create a class
#if GLGE_CPP

class AssetShader final : public Asset
{
public:

    /**
     * @brief Construct a new Asset Shader
     * Explicetly deleted - emtpy assets should not exist
     */
    AssetShader() = delete;

    /**
     * @brief Construct a new Asset Shader
     * 
     * @param path the file to load the asset from
     * @param processor a pointer to a shader processor to load the shader with
     * @param storage a pointer to the storage the asset belongs to
     */
    AssetShader(std::filesystem::path path, ShaderProcessor* processor, AssetStorage* storage);

    /**
     * @brief Construct a new Asset Shader
     * 
     * @param path the file to load the asset from
     * @param processor a pointer to a shader processor to load the shader with
     * @param textures store an unorderd map that maps names to texture pointers the shader can access
     * @param buffers store an unorderd map that maps names to buffer binding solutions
     * @param storage a pointer to the storage the asset belongs to
     */
    AssetShader(std::filesystem::path path, ShaderProcessor* processor, std::unordered_map<std::string_view, Texture*> textures, 
                std::unordered_map<std::string_view, BufferShaderBinding> buffers, AssetStorage* storage);

    /**
     * @brief Destroy the Asset Shader
     */
    virtual ~AssetShader() noexcept;

    /**
     * @brief re-load the asset from the disk
     */
    virtual void reload() noexcept override;

    /**
     * @brief Get the Type Name of the asset
     * @warning Even if this function is not virtual, sub-classes MUST have a function with this name
     * 
     * @return const char* the constant name of the asset
     */
    static const char* getTypeNameStatic() noexcept {return "GLGE_SHADER_ASSET";}

    /**
     * @brief Get the Type Name of the asset (but overloadable)
     * 
     * @return const char* the type name of the asset
     */
    virtual const char* getTypeName() const noexcept override {return getTypeNameStatic();}

    /**
     * @brief Get the stored shader object
     * 
     * @return Shader* a pointer to the stored shader object
     */
    inline Shader* getShader() const noexcept {return m_shader;}

    /**
     * @brief use the shader asset like a shader pointer
     * 
     * @return Shader* a pointer to the stored shader object
     */
    inline operator Shader*() const noexcept {return m_shader;}

    /**
     * @brief use the shader asset like a reference to the stored shader object
     * 
     * @return Shader& a reference to the stored shader object
     */
    inline operator Shader&() noexcept {return *m_shader;}

    /**
     * @brief use the shader asset like a constant reference to the store shader object
     * 
     * @return const Shader& a constant reference to the stored shader object
     */
    inline operator const Shader&() const noexcept {return *m_shader;}

protected:

    /**
     * @brief load or re-load the shader
     */
    void loadShader() noexcept;

    /**
     * @brief store a pointer to the actually stored shader object
     */
    Shader* m_shader = 0;
    /**
     * @brief a pointer to the shader processor the asset is compiled with. 
     * @warning the shader processor must stay allive till the asset is released
     */
    ShaderProcessor* m_processor = 0;

    /**
     * @brief store additional textures to pass to the shader
     */
    std::unordered_map<std::string_view, Texture*> m_textures;
    /**
     * @brief store additional buffers to pass to the shader
     */
    std::unordered_map<std::string_view, BufferShaderBinding> m_buffers;

};

#endif //end of C++ section

#endif