/**
 * @file GLGEAsset_Texture.cpp
 * @author DM8AT
 * @brief implement the texture asset class
 * @version 0.1
 * @date 2025-07-31
 * 
 * @copyright Copyright (c) 2025
 * 
 */

//include the texture assets
#include "GLGEAsset_Texture.h"
//include the base asset class
#include "GLGEAssets.h"

//include SDL2 for texture loading
#include <SDL2/SDL_image.h>


void AssetTexture::create(
    //if a graphic texture is present, add a setting for that
    #if GLGE_INCLUDE_GRAPHICS
    TexturePurpose purpose,
    #else
    //else, just add a dummy setting
    uint64_t,
    #endif //end of graphic dependend switch
    Asset* asset) noexcept
{
    //store the parent asset
    m_asset = asset;

    //initalize a texture context
    IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG | IMG_INIT_AVIF | IMG_INIT_JXL | IMG_INIT_TIF | IMG_INIT_WEBP);
    
    //load the texture data using SDL2
    SDL_Surface* surf = IMG_Load(m_asset->getPath().c_str());
    
    //check if the surface was created successfully
    if (!surf)
    {
        //create the error message
        std::stringstream stream;
        stream << "Failed to load the requested asset file " << asset->getPath() << ". An SDL error occoured: " << IMG_GetError();
        //log the error
        asset->getInstance()->log(stream, MESSAGE_TYPE_ERROR);
        return;
    }

    //store the creation flags
    bool isHDR = false;
    bool hasAlpha = false;
    //switch over the bits per pixel count to select the correct flags
    switch (surf->format->BitsPerPixel)
    {
    case 24:
        //everything is set correctly, just stop
        break;
    case 32:
        //an alpha channel exists
        hasAlpha = true;
        break;
    case 32*3:
        //the texture is in HDR (high dynamic range)
        isHDR = true;
        break;
    case 32*4:
        //the texture is in HDR (high dynamic range) and has an alpha channel
        isHDR = true;
        hasAlpha = true;
        break;
    
    default:
        {
            //unknown format, throw an error
            std::stringstream stream;
            stream << "Loaded a texture with " << surf->format->BitsPerPixel << " bits per pixel. Failed to determine the format of the texture.";
            m_asset->getInstance()->log(stream, MESSAGE_TYPE_ERROR);
            //clean up
            SDL_FreeSurface(surf);
            IMG_Quit();
            return;
        }
        break;
    }

    //destroy the texture loading context (clean up)
    IMG_Quit();

    //check if graphics exist (then the texture is a texture class)
    #if GLGE_INCLUDE_GRAPHICS

    //create the actual texture
    m_texture = new Texture(surf->pixels, purpose, uvec2(surf->w, surf->h), isHDR, hasAlpha, *m_asset->getInstance());

    #else //end of graphic dependend section

    //store the flags
    m_hasAlpha = hasAlpha;
    m_isHDR = isHDR;
    //store the raw data
    uint64_t size = surf->w * surf->h * (m_isHDR ? sizeof(float) : sizeof(uint8_t)) * (m_hasAlpha ? 4 : 3);
    m_data = malloc(size);
    memcpy(m_data, surf->pixels, size);

    #endif //end of graphic dependend switch

    //free the surface
    SDL_FreeSurface(surf);
}

AssetTexture::~AssetTexture()
{
    //check for graphics to check if a texture must be deleted
    #if GLGE_INCLUDE_GRAPHICS

    //delete the texture if it exists
    if (m_texture)
    {
        delete m_texture;
        m_texture = 0;
    }

    #else //end of graphic section

    //free the internal data
    if (m_data)
    {
        free(m_data);
        m_data = 0;
    }

    #endif //end of graphic switch
}

void AssetTexture::reloadFromFile() noexcept
{
    //mostly the same as the creation code (ctrl c + ctrl v)
    //initalize a texture context
    IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG | IMG_INIT_AVIF | IMG_INIT_JXL | IMG_INIT_TIF | IMG_INIT_WEBP);
    
    //load the texture data using SDL2
    SDL_Surface* surf = IMG_Load(m_asset->getPath().c_str());
    
    //check if the surface was created successfully
    if (!surf)
    {
        //create the error message
        std::stringstream stream;
        stream << "Failed to load the requested asset file " << m_asset->getPath() << ". An SDL error occoured: " << IMG_GetError();
        //log the error
        m_asset->getInstance()->log(stream, MESSAGE_TYPE_ERROR);
        return;
    }

    //store the creation flags
    bool isHDR = false;
    bool hasAlpha = false;
    //switch over the bits per pixel count to select the correct flags
    switch (surf->format->BitsPerPixel)
    {
    case 24:
        //everything is set correctly, just stop
        break;
    case 32:
        //an alpha channel exists
        hasAlpha = true;
        break;
    case 32*3:
        //the texture is in HDR (high dynamic range)
        isHDR = true;
        break;
    case 32*4:
        //the texture is in HDR (high dynamic range) and has an alpha channel
        isHDR = true;
        hasAlpha = true;
        break;
    
    default:
        {
            //unknown format, throw an error
            std::stringstream stream;
            stream << "Loaded a texture with " << surf->format->BitsPerPixel << " bits per pixel. Failed to determine the format of the texture.";
            m_asset->getInstance()->log(stream, MESSAGE_TYPE_ERROR);
            //clean up
            SDL_FreeSurface(surf);
            IMG_Quit();
            return;
        }
        break;
    }

    //destroy the texture loading context (clean up)
    IMG_Quit();

    //check if graphics exist (then the texture is a texture class)
    #if GLGE_INCLUDE_GRAPHICS

    //at this point, reload the texture
    m_texture->reload(surf->pixels, uvec2(surf->w, surf->h), isHDR, hasAlpha);

    #else //end of graphic dependend section

    //free the old data
    if (m_data) {free(m_data);}
    //store the flags
    m_hasAlpha = hasAlpha;
    m_isHDR = isHDR;
    //store the raw data
    uint64_t size = surf->w * surf->h * (m_isHDR ? sizeof(float) : sizeof(uint8_t)) * (m_hasAlpha ? 4 : 3);
    m_data = malloc(size);
    memcpy(m_data, surf->pixels, size);


    #endif //end of graphic dependend switch

    //free the surface
    SDL_FreeSurface(surf);
}