/**
 * @file GLGE_Software_Framebuffer.cpp
 * @author JuNi4
 * @brief 
 * @version 0.1
 * @date 2025-07-02
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#include "GLGE_Software_Framebuffer.h"
#include "../../../GLGEFramebuffer.h"

#include <SDL2/SDL.h>

void Software_Framebuffer::onCreate()
{}

void Software_Framebuffer::onDestroy()
{}

void Software_Framebuffer::clear(GraphicCommandBuffer* cmdBuff)
{
    cmdBuff->add(0, (void*)Software_Framebuffer::software_clear, this, sizeof(this));
}

void Software_Framebuffer::resize(GraphicCommandBuffer* cmdBuff, const uvec2& newSize) noexcept
{
    cmdBuff->add(0, (void*)Software_Framebuffer::software_resize, this, *((uint64_t*)&newSize));
}

void Software_Framebuffer::software_clear(Software_Framebuffer* fbuff, uint64_t) noexcept
{
    for (uint64_t i = 0; i < fbuff->m_fbuff->getColorAttatchmentCount(); i++)
    {
        SDL_Surface* data = (SDL_Surface*)(fbuff->m_fbuff->getColorAttatchment(i)->getGraphicTexture()->getData());

        if (data->pixels == nullptr) {continue;}

        uvec2 size = fbuff->m_fbuff->getSize();
        vec4 c = fbuff->m_fbuff->getClearColor().getInSpace(COLOR_SPACE_RGBA) * 255;

        for (size_t x = 0; x < (size.x * size.y); x++)
            *((uint32_t*)(data->pixels )+x) = SDL_MapRGBA(data->format, c.x, c.y, c.z, c.w);
    }
}

void Software_Framebuffer::software_resize(Software_Framebuffer* fbuff, uint64_t nsize) noexcept
{
    uvec2 size = *((uvec2*)&nsize);

    for (size_t i = 0; i < fbuff->m_fbuff->getColorAttatchmentCount(); i++)
    {
        Software_Texture* tex = (Software_Texture*)fbuff->m_fbuff->getColorAttatchment(i)->getGraphicTexture();
        tex->setParentSize(size);
        tex->resize(size);
    }
    //extract the depth attachment
    Texture* depth = (Texture*)fbuff->m_fbuff->getDepthAttatchment();
    //resize the depth buffer if it exists
    if (depth)
    {
        //extract the depth texture
        Software_Texture* depthTex = (Software_Texture*)depth->getGraphicTexture();
        //first, directly resize the texture
        depthTex->setParentSize(size);
        depthTex->resize(size);
    }
}