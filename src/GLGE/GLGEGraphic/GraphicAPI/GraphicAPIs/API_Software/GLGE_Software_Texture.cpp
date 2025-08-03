#include "GLGE_Software_Texture.h"
#include "../../../GLGETexture.h"

#include <SDL2/SDL.h>

void Software_Texture::onCreate()
{
    //create the texture surface
    SDL_Surface* surface = SDL_CreateRGBSurfaceWithFormat(0, m_texture->getSize().x, m_texture->getSize().y, 32, m_hasAlpha? SDL_PIXELFORMAT_RGBA8888 : SDL_PIXELFORMAT_RGB888);

    // if (!SDL_LockSurface(surface))
    // {
    //     for (size_t i = 0; i < (m_texture->getSize().x * m_texture->getSize().y); i++)
    //     {
    //         *((uint32_t*)surface->pixels+i) = SDL_MapRGBA(surface->format, 0, 0, 0, 255);
    //     }
    //     SDL_UnlockSurface(surface);
    // }

    //store the surface
    m_data = (void*) surface;
}

void Software_Texture::resize(const uvec2& size) noexcept
{
    SDL_FreeSurface((SDL_Surface*)m_data);

    //create the texture surface
    SDL_Surface* surface = SDL_CreateRGBSurfaceWithFormat(0, size.x, size.y, 32, m_hasAlpha? SDL_PIXELFORMAT_RGBA8888 : SDL_PIXELFORMAT_RGB888);

    //store the surface
    m_data = (void*) surface;
}