/**
 * @file ExampleBackendFactory.h
 * @author DM8AT
 * @brief define mappings from names to actual backend factories
 * @version 0.1
 * @date 2026-05-18
 * 
 * @copyright Copyright (c) 2026
 * 
 * Backend objects must be constructed in the same module that runs the example.
 * Passing Description pointers from the launcher causes duplicate static GLGE/SDL
 * state and windows that never show up.
 */
//header guard
#ifndef _EXAMPLE_BACKEND_FACTORY_
#define _EXAMPLE_BACKEND_FACTORY_

//add GLGE
#define GLGE_NO_THANKS_MSG
#include "GLGE.h"

#include <cstring>
#include <memory>
#include <stdexcept>
#include <string>

/**
 * @brief Create a Graphic Backend Description
 * 
 * @param name the name of the description to create
 * @return `std::unique_ptr<GLGE::Graphic::Backend::Graphic::Description>` a unique pointer to the graphic backend description
 */
inline std::unique_ptr<GLGE::Graphic::Backend::Graphic::Description> createGraphicBackendDescription(const char* name) {
    //sanity check for null
    if (name == nullptr)
    {throw std::invalid_argument("Graphic backend name is null");}
    //switch over known names
    if (std::strcmp(name, "OpenGL") == 0)
    {return std::make_unique<GLGE::Graphic::Builtin::Graphics::OpenGL>();}
    if (std::strcmp(name, "Vulkan") == 0)
    {return std::make_unique<GLGE::Graphic::Builtin::Graphics::Vulkan>();}
    //throw on unknown argument
    throw std::invalid_argument(std::string("Unknown graphic backend: ") + name);
}

/**
 * @brief Create a Video Backend Description
 * 
 * @param name the name of the description to create
 * @return `std::unique_ptr<GLGE::Graphic::Backend::Video::Description>` a unique pointer to the video backend description
 */
inline std::unique_ptr<GLGE::Graphic::Backend::Video::Description> createVideoBackendDescription(const char* name) {
    //sanity check for null
    if (name == nullptr)
    {throw std::invalid_argument("Video backend name is null");}
    //switch over known names
    if (std::strcmp(name, "SDL3") == 0)
    {return std::make_unique<GLGE::Graphic::Builtin::Video::SDL3>();}
    //thrown on unknown argument
    throw std::invalid_argument(std::string("Unknown video backend: ") + name);
}

#endif
