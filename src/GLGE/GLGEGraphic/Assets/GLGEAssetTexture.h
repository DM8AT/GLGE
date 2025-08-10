/**
 * @file GLGEAssetTexture.h
 * @author DM8AT
 * @brief define a texture asset
 * @version 0.1
 * @date 2025-08-04
 * 
 * @copyright Copyright (c) 2025
 * 
 */

//header guard
#ifndef _GLGE_ASSET_TEXTURE_
#define _GLGE_ASSET_TEXTURE_

//inlcude the asset base class
#include "../../GLGECore/Assets/GLGEAsset.h"
//include textures
#include "../GLGETexture.h"

//check for C++ to create a class
#if GLGE_CPP

/**
 * @brief store an asset that wraps a single texture
 */
class AssetTexture final : public Asset
{
public:

    /**
     * @brief Construct a new Asset Texture
     * empty assets should not exist
     */
    AssetTexture() = delete;

    /**
     * @brief Construct a new Asset Texture
     * 
     * @param path a path to the file to load the texture from (can be a texture asset or a texture file)
     * @param purpose the purpose of the texture
     * @param storage a pointer to the storage the asset belongs to
     */
    AssetTexture(std::filesystem::path path, TexturePurpose purpose, AssetStorage* storage);

    /**
     * @brief Destroy the Asset Texture
     */
    ~AssetTexture() {}

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
    static const char* getTypeNameStatic() noexcept {return "GLGE_TEXTURE_ASSET";}

    /**
     * @brief Get the Type Name of the asset (but overloadable)
     * 
     * @return const char* the type name of the asset
     */
    virtual const char* getTypeName() const noexcept override {return getTypeNameStatic();}

    /**
     * @brief Get the Texture of the asset
     * 
     * @return Texture* a pointer to the loaded texture or 0 if the texture is not loaded
     */
    inline Texture* getTexture() const noexcept {return m_texture;}

    /**
     * @brief use the texture asset like a texture pointer
     * 
     * @return Texture* a pointer to the texture object
     */
    operator Texture*() const noexcept {return m_texture;}

    /**
     * @brief use the texture asset like a texture reference
     * 
     * @return Texture& a reference to the texture
     */
    operator Texture&() noexcept {return *m_texture;}

protected:

    /**
     * @brief load the content for the texture
     */
    void loadContent();

    /**
     * @brief store the actual texture object
     */
    Texture* m_texture = 0;
    /**
     * @brief store the purpose of the texture
     */
    TexturePurpose m_purpose = TEXTURE_PURPOSE_IMAGE;

};

#endif

#endif