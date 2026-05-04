/**
 * @file OpenGL.cpp
 * @author DM8AT
 * @brief implement the OpenGL contract for SDL3
 * @version 0.1
 * @date 2026-05-04
 * 
 * @copyright Copyright (c) 2026
 * 
 */
//add the OpenGL - SDL3 contract
#include "Graphic/Backend/Builtin/Video/SDL3/Contracts/OpenGL.h"

//add SDL windows
#include "Graphic/Backend/Builtin/Video/SDL3/Window.h"

//add SDL3
#include "SDL3/SDL.h"

/**
 * @brief convert the VSync enum to an integer value SDL 3 can understand
 * 
 * @param v the vsync enum value
 * @return `GLGE::i32` the integer that SDL 3 can understand
 */
static GLGE::i32 __gl_VSyncToSDL(GLGE::Graphic::VSync v) {
    //switch over the known vsync modes
    switch (v)
    {
    //check for disabled mode
    case GLGE::Graphic::VSYNC_DISABLED:
        return 0;
        break;
    //check for enabled mode
    case GLGE::Graphic::VSYNC_ENABLED:
        return 1;
        break;
    //check for adaptive mode
    case GLGE::Graphic::VSYNC_ADAPTIVE:
        return -1;
        break;
    
    default:
        //in debug throw an error
        #if GLGE_DEBUG
        throw GLGE::Exception("Tried to select a not supported VSync mode", "GLGE::Graphic::Backend::Builtin::Video::SDL3::__gl_VSyncToSDL");
        #endif
        //just return 0 (disabled)
        return 0;
        break;
    }
}

void GLGE::Graphic::Backend::Video::SDL3::Contracts::OpenGL::createContext(GLGE::Graphic::Backend::Video::Window* window) {
    //set the version correctly
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, getInstance()->getInstance()->getGraphicDescription()->getAPIVersion().getMajor());
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, getInstance()->getInstance()->getGraphicDescription()->getAPIVersion().getMinor());
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, (getInstance()->getInstance()->getGraphicDescription()->getAPIVersion().getPatch() == 1) ? SDL_GL_CONTEXT_PROFILE_COMPATIBILITY : SDL_GL_CONTEXT_PROFILE_CORE);
    //explicitly enable double buffering
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    //request depth buffer
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    //setup default framebuffer attributes
    SDL_GL_SetAttribute(SDL_GL_FLOATBUFFERS, 0);
    SDL_GL_SetAttribute(SDL_GL_RED_SIZE,   8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE,  8);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);

    //request acceleration
    SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);

    //create the context
    m_context = SDL_GL_CreateContext(reinterpret_cast<SDL_Window*>(reinterpret_cast<GLGE::Graphic::Backend::Video::SDL3::Window*>(window)->getSDLWindow()));
}

void GLGE::Graphic::Backend::Video::SDL3::Contracts::OpenGL::destroyContext() {
    //delete the context
    SDL_GL_DestroyContext(reinterpret_cast<SDL_GLContext>(m_context));
}

void GLGE::Graphic::Backend::Video::SDL3::Contracts::OpenGL::makeCurrent(GLGE::Graphic::Backend::Video::Window* window) {
    //make the window the current one
    SDL_GL_MakeCurrent(reinterpret_cast<SDL_Window*>(reinterpret_cast<GLGE::Graphic::Backend::Video::SDL3::Window*>(window)->getSDLWindow()), reinterpret_cast<SDL_GLContext>(m_context));
}

void GLGE::Graphic::Backend::Video::SDL3::Contracts::OpenGL::swap(GLGE::Graphic::Backend::Video::Window* window) {
    //call the swap function
    SDL_GL_SwapWindow(reinterpret_cast<SDL_Window*>(reinterpret_cast<GLGE::Graphic::Backend::Video::SDL3::Window*>(window)->getSDLWindow()));
}

bool GLGE::Graphic::Backend::Video::SDL3::Contracts::OpenGL::setVsync(VSync swapInterval) {
    //set the vsync state
    SDL_GL_SetSwapInterval(__gl_VSyncToSDL(swapInterval));
    //return if the vsync state is as expected
    i32 val = 0;
    bool success = SDL_GL_GetSwapInterval(&val);
    return success ? (val == __gl_VSyncToSDL(swapInterval)) : false;
}