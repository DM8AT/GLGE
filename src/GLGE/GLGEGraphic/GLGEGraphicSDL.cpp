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
    //initalize SDL2
    if (SDL_InitSubSystem(SDL_INIT_EVERYTHING) < 0)
    {
        //print an error
        if (logger) {logger->log("Failed to initalize SDL2", MESSAGE_TYPE_FATAL_ERROR);}
        //stop the function
        return;
    }

    //print a debug message
    GLGE_DEBUG_WRAPPER(
        //check if a logger exists
        if (logger)
        {
            //check if debugging is enabled
            if (logger->isDebug())
            {
                //log a debug message
                logger->log("Initalized SLD2", MESSAGE_TYPE_DEBUG);
            }
        }
    )

    //wait till there are windows
    while (__glge_all_windows_sdl.size() == 0) {std::this_thread::sleep_for(std::chrono::milliseconds(10));}

    //create a limiter to limit to 240 iterations per second
    Limiter sdlLimiter = 240;

    //start the limiter
    sdlLimiter.start();

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

        //end the tick
        sdlLimiter.endTick();
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