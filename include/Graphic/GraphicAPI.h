/**
 * @file GraphicAPI.h
 * @author DM8AT
 * @brief define an enum that is used as a universal API identifier
 * 
 * This does not identify the backend, it only identifies the API that the backend implementation uses. 
 * 
 * @version 0.1
 * @date 2025-12-29
 * 
 * @copyright Copyright (c) 2025
 * 
 */
//header guard
#ifndef _GLGE_GRAPHIC_GRAPHIC_API_
#define _GLGE_GRAPHIC_GRAPHIC_API_

//include common stuff
#include "Core/Common.h"

//use the library namespace
namespace GLGE::Graphic {

    /**
     * @brief a enum that holds some pre-named graphic APIs
     * 
     * Not all APIs must have an entry in this enum. It is only a utility that contains some common APIs
     */
    enum class GraphicAPI : u32 {
        /**
         * @brief Just an error value. If this value appears, something went terribly wrong. 
         */
        UNDEFINED = 0,
        /**
         * @brief The [OpenGL](https://www.opengl.org/) graphic API
         * 
         * Recommended for older desctop hardware
         * 
         * Works on all platforms except modern Apple
         */
        OPEN_GL,
        /**
         * @brief The [OpenGL ES](https://www.khronos.org/opengles/) graphic API
         * 
         * A version of OpenGL that is focused on mobile devices
         * 
         * Works on all platforms except modern Apple
         */
        OPEN_GL_ES,
        /**
         * @brief The [Vulkan](https://www.vulkan.org/) graphic API
         * 
         * Recommended for modern desctop or mobile hardwares
         * 
         * Works on all platforms, for Apple MoltenVk is required
         */
        VULKAN,
        /**
         * @brief [Apples Graphic API](https://developer.apple.com/metal/)
         * 
         * Recommended for modern apple hardware
         * 
         * Works only on apple
         */
        METAL,
        /**
         * @brief The ninth version of the [DirectX graphic API](https://en.wikipedia.org/wiki/DirectX)
         * 
         * Recommended for really old windows
         * 
         * Works only on windows and microsoft platforms
         */ 
        DIRECT_X9,
        /**
         * @brief The tenth version of the [DirectX graphic API](https://en.wikipedia.org/wiki/DirectX)
         * 
         * Recommended for old windows
         * 
         * Works only on windows and microsoft platforms
         */
        DIRECT_X10,
        /**
         * @brief The eleventh version of the [DirectX graphic API](https://en.wikipedia.org/wiki/DirectX)
         * 
         * Recommended for more modern, but not quite up to date windows
         * 
         * Works only on windows and microsoft platforms
         */
        DIRECT_X11,
        /**
         * @brief The twelfth version of the [DirectX graphic API](https://en.wikipedia.org/wiki/DirectX)
         * 
         * Recommended for modern windows
         * 
         * Works only on windows and microsoft platforms
         */
        DIRECT_X12,

        /**
         * @brief A value for the fist custom API. 
         * 
         * All APIs that are not officially listed here should be this value plus some offset larger than 0
         */
        CUSTOM = 0xfff
    };

}

#endif