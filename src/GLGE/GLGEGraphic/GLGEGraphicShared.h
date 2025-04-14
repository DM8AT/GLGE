/**
 * @file GLGEGraphicShared.h
 * @author DM8AT
 * @brief store all the shared data for the graphic module
 * @version 0.1
 * @date 2025-04-13
 * 
 * @copyright Copyright (c) 2025
 * 
 */

//header guard
#ifndef _GLGE_GRAPHIC_SHARED_
#define _GLGE_GRAPHIC_SHARED_

//include the settings
#include "../GLGECore/GLGESettings.h"

//include windows
#include "GLGEWindow.h"

//check for C++
#ifdef GLGE_CPP

//store all the windows
extern std::unordered_map<uint32_t, Window*> __glge_all_windows_sdl;

//store the SDL2 thread
extern std::thread* __glge_sdl_2_thread;

//store the amount of open windows
extern uint32_t* __glge_all_window_count;

#endif

#endif