/**
 * @file GLGEGraphicSDL.h
 * @author DM8AT
 * @brief implement the SDL dependand stuff like the update thread
 * @version 0.1
 * @date 2025-04-13
 * 
 * @copyright Copyright (c) 2025
 * 
 */

//include the shared stuff
#include "GLGEGraphicShared.h"

//include SDL2
#include <SDL2/SDL.h>

//check for Dear ImGui usage
#if GLGE_3RD_PARTY_INCLUDE_DEAR_IMGUI

//include Dear ImGui
#include "../GLGE3rdParty/imgui/imgui.h"
#include "../GLGE3rdParty/imgui/backends/imgui_impl_sdl2.h"
#include "../GLGE3rdParty/imgui/backends/imgui_impl_opengl3.h"

#endif

/**
 * @brief main thread for SDL, this has nothing to do with the 
 */
void SDL_Main_Thread(Logger* logger)
{
    //wait till there are windows
    while (__glge_all_windows_sdl.size() == 0) {std::this_thread::sleep_for(std::chrono::milliseconds(10));}

    //start the sdl limiter
    Instance::getSDLLimiter().start();

    //loop while there are windows available
    while (__glge_all_windows_sdl.size() > 0)
    {
        //get the SDL mode
        SDL_Keymod mode = SDL_GetModState();
        //get the capslock state
        bool capslock = mode & KMOD_CAPS;
        //get the numpad state
        bool numpad = mode & KMOD_NUM;
        //SDL update for all instances
        for (uint64_t i = 0; i < __glge_all_instances->size(); ++i)
        {
            //de-register all changed keys
            (*__glge_all_instances)[i]->getChangedKeys().keys.clear();

            //update the capslock and numpad trigger
            (*__glge_all_instances)[i]->getChangedKeys().states.capslock = (*__glge_all_instances)[i]->getKeys().states.capslock != capslock;
            (*__glge_all_instances)[i]->getChangedKeys().states.numpad = (*__glge_all_instances)[i]->getKeys().states.numpad != numpad;

            //store the new states for the keys
            (*__glge_all_instances)[i]->getKeys().states.capslock = capslock;
            (*__glge_all_instances)[i]->getKeys().states.numpad = numpad;

            //reset the delta mouse
            bzero((void*)&((*__glge_all_instances)[i]->getMouseChange()), sizeof(Mouse));
        }

        //store the current event
        SDL_Event event;
        //get all events
        while (SDL_PollEvent(&event))
        {
            //check for ImGui support
            #if GLGE_3RD_PARTY_INCLUDE_DEAR_IMGUI

            //check if a context exists
            //a context is created on initalization of ImGui
            if (SDL_GL_GetCurrentContext())
            {
                //handle the event with ImGui
                ImGui_ImplSDL2_ProcessEvent(&event);
            }

            #endif //ImGui section

            //store an effected window
            Window* effected = 0;
            //switch over the event type
            switch (event.type)
            {
            //check if this is a windowevent
            case SDL_WINDOWEVENT:
                //get the window that is effected by the event
                effected = __glge_all_windows_sdl[event.window.windowID];
                //if the effected window exists, pass the event
                if (effected) {effected->handleEvent(new WindowEvent(event));}
                break;

            //check if this is a keyboard event
            //Key down event
            case SDL_KEYDOWN:
                //iterate over all instances and send the event
                for (uint64_t i = 0; i < __glge_all_instances->size(); ++i)
                {(*__glge_all_instances)[i]->handleEvent(new KeyboardKeyEvent(true, event.key.keysym.scancode));}
                break;

            //Key up event
            case SDL_KEYUP:
                //iterate over all instances and send the event
                for (uint64_t i = 0; i < __glge_all_instances->size(); ++i)
                {(*__glge_all_instances)[i]->handleEvent(new KeyboardKeyEvent(false, event.key.keysym.scancode));}
                break;

            //mouse button press event
            case SDL_MOUSEBUTTONDOWN:
                //iterate over all instances and send the event
                for (uint64_t i = 0; i < __glge_all_instances->size(); ++i)
                {(*__glge_all_instances)[i]->handleEvent(new MouseEvent(event.button.button, true));}
                break;

            //mouse button release event
            case SDL_MOUSEBUTTONUP:
                //iterate over all instances and send the event
                for (uint64_t i = 0; i < __glge_all_instances->size(); ++i)
                {(*__glge_all_instances)[i]->handleEvent(new MouseEvent(event.button.button, false));}
                break;

            //mouse motion event
            case SDL_MOUSEMOTION:
                //iterate over all instances and send the event
                for (uint64_t i = 0; i < __glge_all_instances->size(); ++i)
                {(*__glge_all_instances)[i]->handleEvent(new MouseEvent(uvec2(event.motion.x, event.motion.y)));}
                break;

            default:
                break;
            }
        }

        //loop over all instances
        for (size_t i = 0; i < __glge_all_instances->size(); ++i)
        {
            //render the instance
            (*__glge_all_instances)[i]->getGraphicInstance()->onRender();
        }

        //end the tick
        Instance::getSDLLimiter().endTick();
    }

    //close SDL
    SDL_Quit();

    //print a debug message
    GLGE_DEBUG_WRAPPER(
        //check if a logger exists
        if (logger)
        {
            //check if debugging is enabled
            if (logger->isDebug())
            {
                //log a debug message
                logger->log("Closed SDL2", MESSAGE_TYPE_DEBUG);
            }
        }
    )
}