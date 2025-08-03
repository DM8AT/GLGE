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

Texture::~Texture()
{
    //if a texture dosn't exist, stop
    if (!m_texture) {return;}

    //else, check if the texture is a graphic texture or a SDL surface
    //this is checked by testing if a GPU Texture exists
    if (m_purpose == TEXTURE_PURPOSE_CPU_ONLY)
    {

    }
    else
    {

    }
}

void Texture::create(std::string_view file, TexturePurpose purpose, bool alpha)
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

    //store the size of the texture
    m_size = uvec2(img->w, img->h);

    //check if the texture should be CPU only. A CPU texture is stored as a SDL surface
    if (purpose == TEXTURE_PURPOSE_CPU_ONLY)
    {
        //store the SDL surface in the texture slot
        m_texture = (GraphicTexture*)conv;
    }
    //switch over the graphic API
    else if (m_instance->getAPI() == API_OPENGL_4_6)
    {
        //store the new texture
        m_texture = new OGL4_6_Texture(this, conv->pixels, false, alpha, m_instance->getGraphicInstance());
    }
    else if (m_instance->getAPI() == API_SOFTWARE)
    {
        m_texture = new Software_Texture(this, conv->pixels, false, alpha, m_instance->getGraphicInstance());
    }
    else
    {
        //log the missing texture creation as a fatal error
        std::stringstream stream;
        stream << "No overload for API " << m_instance->getAPI() << " for a texture was implemented";
        m_instance->log(stream, MESSAGE_TYPE_FATAL_ERROR);
    }

    //clean up
    SDL_FreeSurface(img);
    //only delete the converted surface
    if (purpose != TEXTURE_PURPOSE_CPU_ONLY) {SDL_FreeSurface(conv);}

    //store the texture's purpose
    m_purpose = purpose;
}

void Texture::create(TexturePurpose purpose, const uvec2& size, bool isHDR, bool alpha)
{
    //store the inputs
    m_purpose = purpose;
    m_size = size;
    //check if the purpose is a CPU texture
    if (m_purpose == TEXTURE_PURPOSE_CPU_ONLY)
    {
        //create a SDL surface
        m_texture = (GraphicTexture*)SDL_CreateRGBSurfaceWithFormat(0, m_size.x, m_size.y, (alpha) ? 32 : 24, (alpha) ? SDL_PIXELFORMAT_RGBA32 : SDL_PIXELFORMAT_RGB24);
    }
    //just create the texture
    //switch over the graphic API
    switch (m_instance->getAPI())
    {
    case API_OPENGL_4_6:
        //store the new texture
        m_texture = new OGL4_6_Texture(this, isHDR, alpha, m_instance->getGraphicInstance());
        break;
    case API_SOFTWARE:
        m_texture = new Software_Texture(this, isHDR, alpha, m_instance->getGraphicInstance());
        break;
    default:
    {
        //log the missing texture creation as a fatal error
        std::stringstream stream;
        stream << "No overload for API " << m_instance->getAPI() << " for a texture was implemented";
        m_instance->log(stream, MESSAGE_TYPE_FATAL_ERROR);
        break;
    }

    }
}

void Texture::create(void* data, TexturePurpose purpose, const uvec2& size, bool isHDR, bool alpha)
{
    //store the inputs
    m_purpose = purpose;
    m_size = size;
    //check if the purpose is a CPU texture
    if (m_purpose == TEXTURE_PURPOSE_CPU_ONLY)
    {
        //create a SDL surface
        m_texture = (GraphicTexture*)SDL_CreateRGBSurfaceWithFormatFrom(data, m_size.x, m_size.y, (alpha) ? 32 : 24, 
                                                                        m_size.x * ((alpha) ? 4 : 3), 
                                                                        (alpha) ? SDL_PIXELFORMAT_RGBA32 : SDL_PIXELFORMAT_RGB24);
    }
    //else, create the new texture
    else if (m_instance->getAPI() == API_OPENGL_4_6)
    {
        //store the texture
        m_texture = new OGL4_6_Texture(this, data, isHDR, alpha, m_instance->getGraphicInstance());
    }
    else
    {
        //log the missing texture creation as a fatal error
        std::stringstream stream;
        stream << "No overload for API " << m_instance->getAPI() << " for a texture was implemented";
        m_instance->log(stream, MESSAGE_TYPE_FATAL_ERROR);
    }
}

Color Texture::getColor(const uvec2& pos)
{
    //switch over the correct format
    if ((m_purpose == TEXTURE_PURPOSE_IMAGE) || (m_purpose == TEXTURE_PURPOSE_RENDER))
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
                vec4 pix = data[pos.x * m_size.y + pos.y];
                //return the data as a color instance
                return Color(pix.x, pix.y, pix.z, pix.w, COLOR_SPACE_RGBA);
            }
            else
            {
                //store that the texture is HDR and has no alpha channel
                vec3* data = (vec3*)m_texture->getData();
                //get the requrested pixel
                vec3 pix = data[pos.x * m_size.y + pos.y];
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
                uint8_t* pix = (uint8_t*)&data[(pos.x * m_size.y + pos.y) * (sizeof(uint8_t) * 4)];
                //return the data as a color instance
                return Color(pix[0], pix[1], pix[2], pix[3], COLOR_SPACE_RGBA);
            }
            else
            {
                //store that the texture is HDR and has no alpha channel
                uint8_t* data = (uint8_t*)m_texture->getData();
                //get the requrested pixel
                uint8_t* pix = (uint8_t*)&data[(pos.x * m_size.y + pos.y) * (sizeof(uint8_t) * 3)];
                //return the data as a color instance
                return Color(pix[0], pix[1], pix[2], 1, COLOR_SPACE_RGBA);
            }
        }
    }
    //else, check if this is a depth texture
    else if (m_purpose == TEXTURE_PURPOSE_DEPTH)
    {
        //store that the texture is just floats
        float* data = (float*)m_texture->getData();
        //get the requrested pixel
        float pix = data[pos.x * m_size.y + pos.y];
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
    if ((m_purpose == TEXTURE_PURPOSE_IMAGE) || (m_purpose == TEXTURE_PURPOSE_RENDER))
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
                data[pos.x * m_size.y + pos.y] = rgba;
            }
            else
            {
                //store that the texture is HDR and has no alpha channel
                vec3* data = (vec3*)m_texture->getData();
                //set the requrested pixel
                data[pos.x * m_size.y + pos.y] = vec3(rgba.x, rgba.y, rgba.z);
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
                uint8_t* pix = (uint8_t*)&data[(pos.x * m_size.y + pos.y) * (sizeof(uint8_t) * 4)];
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
                uint8_t* pix = (uint8_t*)&data[(pos.x * m_size.y + pos.y) * (sizeof(uint8_t) * 3)];
                //set the requested pixel
                pix[0] = rgba.x;
                pix[1] = rgba.y;
                pix[2] = rgba.z;
            }
        }
    }
    //else, check if this is a depth texture
    else if (m_purpose == TEXTURE_PURPOSE_DEPTH)
    {
        //store that the texture is just floats
        float* data = (float*)m_texture->getData();
        //set the requested pixel using only the x component
        data[pos.x * m_size.y + pos.y] = rgba.x;
    }
}









#if 0

Texture_t* texture_load(std::string_view file, bool alpha, TexturePurpose purpose, Instance* instance);
Texture_t* texture_create(TexturePurpose purpose, const uvec2& size, bool isHDR, bool alpha, Instance* instance);
Texture_t* texture_createFrom(void* data, TexturePurpose purpose, const uvec2& size, bool isHDR, bool alpha, Instance* instance);

const uvec2* texture_getSize(const Texture_t* texture);

Color_t* texture_getColor(const uvec2* pos, const Texture_t* texture);
void texture_setColor(const Color_t* color, const uvec2* pos, Texture_t* texture);

void texture_createMipMap(Texture_t* texture);

TexturePurpose texture_getPurpose(const Texture_t* texture);
#endif