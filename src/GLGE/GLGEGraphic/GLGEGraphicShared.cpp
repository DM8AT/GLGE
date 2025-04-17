/**
 * @file GLGEGraphicShared.cpp
 * @author DM8AT
 * @brief initalize all the shared stuff for the graphic module
 * @version 0.1
 * @date 2025-04-13
 * 
 * @copyright Copyright (c) 2025
 * 
 */

//inculude the shared stuff
#include "GLGEGraphicShared.h"

//store all the windows
std::unordered_map<uint32_t, Window*> __glge_all_windows_sdl;

//store the SDL2 thread
std::thread* __glge_sdl_2_thread = 0;

//store the amount of open windows
uint32_t* __glge_all_window_count = 0;

//store all instances
std::vector<Instance*>* __glge_all_instances = 0;
