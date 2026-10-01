/**
 * @file Cursor.cpp
 * @author DM8AT
 * @brief implement the cursor
 * @version 0.1
 * @date 2026-09-30
 * 
 * @copyright Copyright (c) 2026
 * 
 */
//add the SDL cursor
#include "GLGE/Graphic/Backend/Builtin/Video/SDL3/Cursor.h"

//add SDL
#include <SDL3/SDL.h>

static SDL_SystemCursor __getSysCursor(GLGE::Graphic::Backend::Video::Cursor::Defaults style) {
    switch (style) {
    case GLGE::Graphic::Backend::Video::Cursor::Defaults::DEFAULT:     return SDL_SYSTEM_CURSOR_DEFAULT;
    case GLGE::Graphic::Backend::Video::Cursor::Defaults::TEXT:        return SDL_SYSTEM_CURSOR_TEXT;
    case GLGE::Graphic::Backend::Video::Cursor::Defaults::MOVE:        return SDL_SYSTEM_CURSOR_MOVE;
    case GLGE::Graphic::Backend::Video::Cursor::Defaults::NS_RESIZE:   return SDL_SYSTEM_CURSOR_NS_RESIZE;
    case GLGE::Graphic::Backend::Video::Cursor::Defaults::EW_RESIZE:   return SDL_SYSTEM_CURSOR_EW_RESIZE;
    case GLGE::Graphic::Backend::Video::Cursor::Defaults::NESW_RESIZE: return SDL_SYSTEM_CURSOR_NESW_RESIZE;
    case GLGE::Graphic::Backend::Video::Cursor::Defaults::NWSE_RESIZE: return SDL_SYSTEM_CURSOR_NWSE_RESIZE;
    case GLGE::Graphic::Backend::Video::Cursor::Defaults::POINTER:     return SDL_SYSTEM_CURSOR_POINTER;
    case GLGE::Graphic::Backend::Video::Cursor::Defaults::WAIT:        return SDL_SYSTEM_CURSOR_WAIT;
    case GLGE::Graphic::Backend::Video::Cursor::Defaults::PROGRESS:    return SDL_SYSTEM_CURSOR_PROGRESS;
    case GLGE::Graphic::Backend::Video::Cursor::Defaults::NOT_ALLOWED: return SDL_SYSTEM_CURSOR_NOT_ALLOWED;
    
    default: std::unreachable();
    }
}

GLGE::Graphic::Backend::Video::SDL3::Cursor::Cursor(Defaults style) 
 : GLGE::Graphic::Backend::Video::Cursor::Cursor(style)
{
    //just create the system cursor
    m_cursor = SDL_CreateSystemCursor(__getSysCursor(style));
}

GLGE::Graphic::Backend::Video::SDL3::Cursor::Cursor(const ImageCPU& image, const uvec2& hot) 
 : GLGE::Graphic::Backend::Video::Cursor::Cursor(image, hot)
{
    //make sure that the data is in RGBA_8888 format
    m_img = image.toFormat(PIXEL_FORMAT_RGBA_8_UNORM);
    //create the surface
    m_surface = SDL_CreateSurfaceFrom(image.getSize().x, image.getSize().y, SDL_PIXELFORMAT_RGBA32, m_img.getRaw(), m_img.getSize().x * 4 /*4 bytes per pixels*/);

    //create the cursor from the surface
    m_cursor = SDL_CreateColorCursor(reinterpret_cast<SDL_Surface*>(m_surface), hot.x, hot.y);
}

GLGE::Graphic::Backend::Video::SDL3::Cursor::~Cursor() {
    //clean up the cursor
    if (m_cursor) {
        SDL_DestroyCursor(reinterpret_cast<SDL_Cursor*>(m_cursor));
    }

    //clean the surface
    if (m_surface) {
        SDL_DestroySurface(reinterpret_cast<SDL_Surface*>(m_surface));
    }
}

void GLGE::Graphic::Backend::Video::SDL3::Cursor::makeCurrent() const noexcept {
    //make a cursor the current one
    SDL_SetCursor(reinterpret_cast<SDL_Cursor*>(m_cursor));
}