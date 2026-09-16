/**
 * @file Vulkan.cpp
 * @author DM8AT
 * @brief define the implementation for the vulkan sdl3 contract
 * @version 0.1
 * @date 2026-05-04
 * 
 * @copyright Copyright (c) 2026
 * 
 */
//add the vulkan sdl3 contract
#include "Graphic/Backend/Builtin/Video/SDL3/Contracts/Vulkan.h"

//add SDL3 vulkan
#include "SDL3/SDL_vulkan.h"

//add vulkan
#include "vulkan/vulkan.h"

//add SDL windows
#include "Graphic/Backend/Builtin/Video/SDL3/Window.h"

void GLGE::Graphic::Backend::Video::SDL3::Contracts::Vulkan::getRequiredInstanceExtensions(std::vector<const char*>& extensions) {
    //get the required extensions
    u32 count = 0;
    const char* const* beg = SDL_Vulkan_GetInstanceExtensions(&count);
    //add them to the vector
    extensions.insert(extensions.end(), beg, beg + count);
}

void GLGE::Graphic::Backend::Video::SDL3::Contracts::Vulkan::getRequiredDeviceExtensions(std::vector<const char*>& extensions) {
    //swapchain is required
    extensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
}

void GLGE::Graphic::Backend::Video::SDL3::Contracts::Vulkan::createWindowSurface(void* instance, void** surface, GLGE::Graphic::Backend::Video::Window* window) {
    SDL_Vulkan_CreateSurface(
        reinterpret_cast<SDL_Window*>(reinterpret_cast<GLGE::Graphic::Backend::Video::SDL3::Window*>(window)->getSDLWindow()), 
        reinterpret_cast<VkInstance>(instance), 
        nullptr, 
        reinterpret_cast<VkSurfaceKHR*>(surface)
    );
}

void GLGE::Graphic::Backend::Video::SDL3::Contracts::Vulkan::destroyWindowSurface(void* instance, void* surface, GLGE::Graphic::Backend::Video::Window* window) {
    SDL_Vulkan_DestroySurface(
        reinterpret_cast<VkInstance>(instance), 
        reinterpret_cast<VkSurfaceKHR>(surface), 
        nullptr
    );
}