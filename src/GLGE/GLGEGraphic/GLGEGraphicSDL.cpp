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
        //store the current event
        SDL_Event event;
        //get all events
        while (SDL_PollEvent(&event))
        {
            //store an effected window
            Window* effected = 0;
            //switch over the event type
            switch (event.type)
            {
            //check if this is a windowevent
            case SDL_WINDOWEVENT:
                //switch over the windowevent types
                switch (event.window.event)
                {
                case SDL_WINDOWEVENT_CLOSE:
                    //get the effected window
                    effected = __glge_all_windows_sdl[event.window.windowID];
                    //check if the window exists
                    if (effected)
                    {
                        //pass the call
                        effected->setQueuedClosing(true);
                    }
                    break;
                
                default:
                    break;
                }
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