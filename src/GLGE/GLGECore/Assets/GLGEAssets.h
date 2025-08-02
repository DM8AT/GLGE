/**
 * @file GLGEAsset.h
 * @author DM8AT
 * @brief an asset is some recource loaded into memory for GLGE to understand it
 * @version 0.1
 * @date 2025-07-31
 * 
 * @copyright Copyright (c) 2025
 * 
 */

//header guard
#ifndef _GLGE_ASSET_
#define _GLGE_ASSET_

//include the settings
#include "../GLGESettings.h"
//include instances for the asset
#include "../GLGEInstance.h"

//include the texture asset type
#include "GLGEAsset_Texture.h"

//check for the graphic core to inlcude graphic dependend assets
#if GLGE_INCLUDE_GRAPHICS

//include the shader asset
#include "../../GLGEGraphic/Assets/GLGEAsset_Shader.h"

#endif //end of graphic dependend section

/**
 * @brief store what type of asset is stored in the asset class
 */
typedef enum e_AssetType
{
    /**
     * @brief say that the purpose of the asset is still unknown
     */
    ASSET_TYPE_UNKNOWN = 0,
    /**
     * @brief say that the loaded asset is a texture
     */
    ASSET_TYPE_TEXTURE,
    /**
     * @brief say that the loaded asset is a material
     */
    ASSET_TYPE_MATERIAL,
    /**
     * @brief say that the loaded asset is a shader source file
     */
    ASSET_TYPE_SHADER,
    /**
     * @brief say that the loaded asset is an object
     */
    ASSET_TYPE_OBJECT,
    /**
     * @brief say that the loaded asset is a world (scene)
     */
    ASSET_TYPE_WORLD,
    /**
     * @brief say that the asset is a combination of multiple different asset types (as example if compound textures and a material where loaded during importing)
     */
    ASSET_TYPE_COMPOUND
} AssetType;

//check for C++ to create a class
#if GLGE_CPP

/**
 * @brief a class that is used to load recources from the file system to GLGE
 */
class Asset final : public InstAttachableClass
{
public:

    /**
     * @brief Construct a new Asset
     * Prevent empty assets from existing
     */
    Asset() = delete;

    /**
     * @brief Destroy the Asset
     */
    virtual ~Asset();

    /**
     * @brief Construct a new Asset texture
     * 
     * @param path the path to the texture to load
     * 
     * @if GLGE_INCLUDE_GRAPHICS == 1
     * @param purpose the purpose enum for the texture to create
     * @else
     * @param dummy just a value to make the count of parameters the same as if graphics are included. The value is ignored
     * @endif
     * 
     * @param autoReload true : the asset will automatically reload | false : the asset will not automatically reload
     * @param instance a reference to the instance the asset will belong to
     */
    Asset(std::filesystem::path path, 
        //if a graphic texture is present, add a setting for that
        #if GLGE_INCLUDE_GRAPHICS
        TexturePurpose purpose,
        #else
        //else, just add a dummy setting
        uint64_t dummy,
        #endif //end of graphic dependend switch
        bool autoReload, Instance& instance);

    //graphics specific constructor
    #if GLGE_INCLUDE_GRAPHICS

    /**
     * @brief Construct a new Asset - load a shader
     * 
     * @param path the path to the shader to load
     * @param processor a pointer to the shader processor that should be used for the shader
     * @param textures a map of textures and names to store all textures the shader can access
     * @param buffers a map of buffer mappings and names to store all buffers the shader can access
     * @param autoReload true : the asset will automatically reload | false : the asset will not automatically reload
     * @param instance a reference to the instance the shader will belong to
     */
    Asset(std::filesystem::path path, ShaderProcessor* processor, 
          std::unordered_map<std::string_view, Texture*> textures, 
          std::unordered_map<std::string_view, BufferShaderBinding> buffers, 
          bool autoReload,
          Instance& instance);

    #endif //end of graphic specific constructor section

    /**
     * @brief update the asset data. Used for auto reloading the asset's contents
     * 
     * @return true : always
     * @return false : never returned
     */
    virtual bool onUpdate() noexcept override;

    /**
     * @brief Get the Path of the asset
     * 
     * @return const std::filesystem::path& a constant reference to the path
     */
    inline const std::filesystem::path& getPath() const noexcept {return m_path;}

    /**
     * @brief Get the loaded asset Texture
     * 
     * @return AssetTexture* a pointer to the asset texture or 0 if the type is not a texture
     */
    inline AssetTexture* getTextureAsset() const noexcept {return ((m_type == ASSET_TYPE_TEXTURE) ? (AssetTexture*)&m_data.texture : (AssetTexture*)0);}

    //check for graphic - it dictates if a texture can be optained
    #if GLGE_INCLUDE_GRAPHICS

    /**
     * @brief Get the Texture instance maintained by the asset
     * 
     * @return Texture* a pointer to the texture
     */
    inline Texture* getTexture() const noexcept {return ((m_type == ASSET_TYPE_TEXTURE) ? m_data.texture.getTexture() : 0);}

    /**
     * @brief Get the loaded Shader Asset
     * 
     * @return AssetShader* a pointer to the loaded shader asset or 0 if the asset type is not a shader
     */
    inline AssetShader* getShaderAsset() const noexcept {return ((m_type == ASSET_TYPE_SHADER) ? (AssetShader*)&m_data.shader : (AssetShader*)0);}

    /**
     * @brief Get the Shader instance maintained by the asset
     * 
     * @return Shader* a pointer to the shader
     */
    inline Shader* getShader() const noexcept {return ((m_type == ASSET_TYPE_SHADER) ? m_data.shader.getShader() : 0);}

    #endif //end of graphic only section

protected:

    /**
     * @brief store the type of the loaded asset
     */
    AssetType m_type = ASSET_TYPE_UNKNOWN;
    /**
     * @brief store the file path to the file the asset was loaded from
     */
    std::filesystem::path m_path = "";
    /**
     * @brief store if the asset should reload when the asset file was changed
     */
    bool m_autoReload = true;
    /**
     * @brief store the last time the asset was loaded
     */
    std::filesystem::file_time_type m_loadTime = std::filesystem::__file_clock::now();

    /**
     * @brief store the actual loaded data of the asset
     */
    union AssetData {
        /**
         * @brief store the potential texture data
         */
        AssetTexture texture;

        //graphic dependend stuff
        #if GLGE_INCLUDE_GRAPHICS

        /**
         * @brief store a potential shader asset's data
         */
        AssetShader shader;

        #endif //end of graphic only section

        /**
         * @brief Construct a new Asset Data
         */
        AssetData() {}
        /**
         * @brief Destroy the Asset Data
         */
        ~AssetData() {}
    } m_data;

};

#endif //end of C++ section

#endif