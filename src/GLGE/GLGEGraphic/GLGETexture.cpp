/**
 * @file GLGETexture.cpp
 * @author DM8AT
 * @brief implement the texture class
 * @version 0.1
 * @date 2025-04-19
 * 
 * @copyright Copyright (c) 2025
 * 
 */

//include the texture
#include "GLGETexture.h"
//include the API overloads
#include "GraphicAPI/GraphicAPIs/GLGE_AllAPIs.h"

//include SDL2 textures
#include <SDL2/SDL_image.h>

void Texture::create(std::string_view file, bool alpha)
{
    //load the image
    SDL_Surface* img = IMG_Load(file.data());
    //check if the texture was loaded successfully
    if (!img)
    {
        //return 
        return;
    }
    //format the surface
    SDL_Surface* conv;
    //check if an alpha channel should exist
    if (alpha)
    {
        //store the format with alpha
        conv = SDL_CreateRGBSurface(0, img->w, img->h, 32, 0x000000FF, 0x0000FF00, 0x00FF0000, 0xFF000000);
    }
    else
    {
        //store the format without alpha
        conv = SDL_CreateRGBSurface(0, img->w, img->h, 24, 0x000000FF, 0x0000FF00, 0x00FF0000, 0);
    }
    //store the data
    SDL_BlitSurface(img, 0, conv, 0);

    //switch over the graphic API
    if (m_instance->getAPI() == API_OPENGL_3_3)
    {
        //store the new texture
        m_texture = new OGL3_3_Texture(conv->pixels, false, alpha, uvec2(conv->w, conv->h), m_instance->getGraphicInstance());
    }

    //clean up
    SDL_FreeSurface(img);
    SDL_FreeSurface(conv);
}

void Texture::create(TexturePurpose purpose, const uvec2& size, bool isHDR, bool alpha)
{
    //just create the texture
    //switch over the graphic API
    if (m_instance->getAPI() == API_OPENGL_3_3)
    {
        //store the new texture
        m_texture = new OGL3_3_Texture(purpose, size, isHDR, alpha, m_instance->getGraphicInstance());
    }
}

Color Texture::getColor(const uvec2& pos)
{
    //switch over the correct format
    if ((m_texture->getPurpose() == TEXTURE_PURPOSE_IMAGE) || (m_texture->getPurpose() == TEXTURE_PURPOSE_RENDER))
    {
        //check if HDR is enabled
        if (m_texture->isHDR())
        {
            //check for an alpha channel
            if (m_texture->hasAlpha())
            {
                //store that the texture is HDR and has an alpha channel
                vec4* data = (vec4*)m_texture->getData();
                //get the requrested pixel
                vec4 pix = data[pos.x * m_texture->getSize().y + pos.y];
                //return the data as a color instance
                return Color(pix.x, pix.y, pix.z, pix.w, COLOR_SPACE_RGBA);
            }
            else
            {
                //store that the texture is HDR and has no alpha channel
                vec3* data = (vec3*)m_texture->getData();
                //get the requrested pixel
                vec3 pix = data[pos.x * m_texture->getSize().y + pos.y];
                //return the data as a color instance
                return Color(pix.x, pix.y, pix.z, 1, COLOR_SPACE_RGBA);
            }
        }
        else
        {
            //check for an alpha channel
            if (m_texture->hasAlpha())
            {
                //store that the texture is HDR and has an alpha channel
                uint8_t* data = (uint8_t*)m_texture->getData();
                //get the requrested pixel
                uint8_t* pix = (uint8_t*)&data[(pos.x * m_texture->getSize().y + pos.y) * (sizeof(uint8_t) * 4)];
                //return the data as a color instance
                return Color(pix[0], pix[1], pix[2], pix[3], COLOR_SPACE_RGBA);
            }
            else
            {
                //store that the texture is HDR and has no alpha channel
                uint8_t* data = (uint8_t*)m_texture->getData();
                //get the requrested pixel
                uint8_t* pix = (uint8_t*)&data[(pos.x * m_texture->getSize().y + pos.y) * (sizeof(uint8_t) * 3)];
                //return the data as a color instance
                return Color(pix[0], pix[1], pix[2], 1, COLOR_SPACE_RGBA);
            }
        }
    }
    //else, check if this is a depth texture
    else if (m_texture->getPurpose() == TEXTURE_PURPOSE_DEPTH)
    {
        //store that the texture is just floats
        float* data = (float*)m_texture->getData();
        //get the requrested pixel
        float pix = data[pos.x * m_texture->getSize().y + pos.y];
        //return the data as a color instance in grey scale
        return Color(pix, pix, pix, 1, COLOR_SPACE_RGBA);
    }

    //default: return black
    return Color(0, 0, 0, 0, COLOR_SPACE_RGBA);
}

void Texture::setColor(const uvec2& pos, const Color& color)
{
    //get the color
    vec4 rgba = color.getInSpace(COLOR_SPACE_RGBA);
    //switch over the correct format
    if ((m_texture->getPurpose() == TEXTURE_PURPOSE_IMAGE) || (m_texture->getPurpose() == TEXTURE_PURPOSE_RENDER))
    {
        //check if HDR is enabled
        if (m_texture->isHDR())
        {
            //check for an alpha channel
            if (m_texture->hasAlpha())
            {
                //store that the texture is HDR and has an alpha channel
                vec4* data = (vec4*)m_texture->getData();
                //set the requrested pixel
                data[pos.x * m_texture->getSize().y + pos.y] = rgba;
            }
            else
            {
                //store that the texture is HDR and has no alpha channel
                vec3* data = (vec3*)m_texture->getData();
                //set the requrested pixel
                data[pos.x * m_texture->getSize().y + pos.y] = vec3(rgba.x, rgba.y, rgba.z);
            }
        }
        else
        {
            //check for an alpha channel
            if (m_texture->hasAlpha())
            {
                //store that the texture is HDR and has an alpha channel
                uint8_t* data = (uint8_t*)m_texture->getData();
                //get the requrested pixel
                uint8_t* pix = (uint8_t*)&data[(pos.x * m_texture->getSize().y + pos.y) * (sizeof(uint8_t) * 4)];
                //set the requested pixel
                pix[0] = rgba.x;
                pix[1] = rgba.y;
                pix[2] = rgba.z;
                pix[3] = rgba.w;
            }
            else
            {
                //store that the texture is HDR and has no alpha channel
                uint8_t* data = (uint8_t*)m_texture->getData();
                //get the requrested pixel
                uint8_t* pix = (uint8_t*)&data[(pos.x * m_texture->getSize().y + pos.y) * (sizeof(uint8_t) * 3)];
                //set the requested pixel
                pix[0] = rgba.x;
                pix[1] = rgba.y;
                pix[2] = rgba.z;
            }
        }
    }
    //else, check if this is a depth texture
    else if (m_texture->getPurpose() == TEXTURE_PURPOSE_DEPTH)
    {
        //store that the texture is just floats
        float* data = (float*)m_texture->getData();
        //set the requested pixel using only the x component
        data[pos.x * m_texture->getSize().y + pos.y] = rgba.x;
    }
}
