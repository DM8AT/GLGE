/**
 * @file GLGEAsset_Texture.h
 * @author DM8AT
 * @brief define the API for a texture asset
 * @version 0.1
 * @date 2025-07-31
 * 
 * @copyright Copyright (c) 2025
 * 
 */

//header guard
#ifndef _GLGE_ASSET_TEXTURE_
#define _GLGE_ASSET_TEXTURE_

//include the settings
#include "../GLGESettings.h"

//check if graphics are included
#if GLGE_INCLUDE_GRAPHICS
//load the texture class from the graphic section
#include "../../GLGEGraphic/GLGETexture.h"
#endif

//check for C++ to create a class
#if GLGE_CPP

//say that an asset class will exist
class Asset;

/**
 * @brief this class contains the information on a loaded texture
 */
class AssetTexture
{
public:

    /**
     * @brief Construct a new Asset Texture
     * Always empty
     */
    AssetTexture() = default;

    /**
     * @brief create the actual asset texture
     * 
     * @if GLGE_INCLUDE_GRAPHICS == 1
     * @param purpose the purpose enum for the texture to create
     * @else
     * @param dummy just a value to make the count of parameters the same as if graphics are included. The value is ignored
     * @endif
     * 
     * @param asset a pointer to the asset to initalize to
     */
    void create(
        //if a graphic texture is present, add a setting for that
        #if GLGE_INCLUDE_GRAPHICS
        TexturePurpose purpose,
        #else
        //else, just add a dummy setting
        uint64_t dummy,
        #endif //end of graphic dependend switch
        Asset* asset) noexcept;

    /**
     * @brief Destroy the Asset Texture
     */
    ~AssetTexture();

    /**
     * @brief reload the contents from the file
     */
    void reloadFromFile() noexcept;

    //check if graphics exist
    #if GLGE_INCLUDE_GRAPHICS

    /**
     * @brief Get the graphical Texture
     * 
     * @return Texture* a pointer to the texture object
     */
    inline Texture* getTexture() const noexcept {return m_texture;}

    /**
     * @brief get if the texture owns an alpha channel
     * 
     * @return true : the texture owns an alpha channel
     * @return false : the texture does not own an alpha channel
     */
    inline bool hasAlpha() const noexcept {return m_texture->getGraphicTexture()->hasAlpha();}

    /**
     * @brief get if the texture is in high dynamic range or not
     * 
     * @return true : the texture is in high dynamic range
     * @return false : the texture is in not in high dynamic range
     */
    inline bool isHDR() const noexcept {return m_texture->getGraphicTexture()->isHDR();}

    /**
     * @brief Get the Raw Data
     * @warning the format depends on if the texture is in high dynamic range or not and if an alpha channel exists
     * 
     * @return void* the raw image data
     */
    inline void* getRawData() const noexcept {return m_texture->getGraphicTexture()->getData();}

    //end of graphic only section
    #else

    /**
     * @brief get if the texture owns an alpha channel
     * 
     * @return true : the texture owns an alpha channel
     * @return false : the texture does not own an alpha channel
     */
    inline bool hasAlpha() const noexcept {return m_hasAlpha;}

    /**
     * @brief get if the texture is in high dynamic range or not
     * 
     * @return true : the texture is in high dynamic range
     * @return false : the texture is in not in high dynamic range
     */
    inline bool isHDR() const noexcept {return m_isHDR;}

    /**
     * @brief Get the Raw Data
     * @warning the format depends on if the texture is in high dynamic range or not and if an alpha channel exists
     * 
     * @return void* the raw image data
     */
    inline void* getRawData() const noexcept {return m_data;}

    #endif //end of graphic dependend switch

protected:

    Asset* m_asset = 0;

    //check if graphics exist
    #if GLGE_INCLUDE_GRAPHICS

    //store a texture directly
    Texture* m_texture = 0;

    #else //end of graphic section

    /**
     * @brief store the raw image data
     */
    void* m_data = 0;
    /**
     * @brief store if the image is in high dynamic range
     */
    bool m_isHDR = false;
    /**
     * @brief store if the image uses an alpha channel
     */
    bool m_hasAlpha = false;

    #endif

};

#endif //end of C++ section

#endif