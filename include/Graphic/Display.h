/**
 * @file Display.h
 * @author DM8AT
 * @brief define a class to get information of displays
 * @version 0.1
 * @date 2025-12-28
 * 
 * @copyright Copyright (c) 2025
 * 
 */
//header guard
#ifndef _GLGE_GRAPHIC_DISPLAY_
#define _GLGE_GRAPHIC_DISPLAY_

//include common stuff
#include "Core/Common.h"
//include color information
#include "ColorInfo.h"

//use the library namespace
namespace GLGE::Graphic {

    /**
     * @brief A class to store information and handle displays
     */
    class Display {
    public:

        /**
         * @brief store information about the display orientation
         */
        enum class Orientation : u8 {
            /**
             * @brief the orientation of the display is unknown
             */
            UNKNOWN = 0,
            /**
             * @brief the display is in landscape mode
             * 
             * This usually means that the bottom edge is larger than the vertical edge
             */
            LANDSCAPE,
            /**
             * @brief similar to landscape, but the display is flipped
             * 
             * This usually means that the bottom edge is larger than the vertical edge, but also that top and bottom are swapped
             */
            LANDSCAPE_FLIPPED,
            /**
             * @brief the display is in portrait mode
             * 
             * This usually means that the bottom edge is shorter than the vertical edge
             */
            PORTRAIT,
            /**
             * @brief similar to portrait, but the display is flipped
             * 
             * This usually means that the bottom edge is shorter that the vertical edge, but also that top and bottom are swapped
             */
            PORTRAIT_FLIPPED
        };

        /**
         * @brief Construct a new Display
         * 
         * Everything is initialized as empty
         */
        Display() = default;

        /**
         * @brief store the name of the display
         */
        const char* name = "UNNAMED_DISPLAY";
        /**
         * @brief store the index of the display
         */
        u32 displayID = 0;
        /**
         * @brief store the position of the display relative to the virtual desktop origen
         */
        uvec2 pos {0,0};
        /**
         * @brief store the size of the display in pixels
         */
        uvec2 logicalSize {0,0};
        /**
         * @brief store the size of the display in pixels but taking into account the amount of OS-Owned pixels
         */
        uvec2 usableSize {0,0};
        /**
         * @brief store the starting position of the usable display portion
         */
        uvec2 usablePosition {0,0};
        /**
         * @brief store the resolution of the display in pixels
         */
        uvec2 resolution {0,0};
        /**
         * @brief store the scaling factor of the content seen on this display
         */
        float scaleFactor = 0.f;
        /**
         * @brief store the pixel density of the display
         */
        float pixelDensity = 0.f;
        /**
         * @brief store the refresh rate of the display
         */
        float refreshRate = 0;
        /**
         * @brief store the pixel format of the display
         */
        PixelFormat format;
        /**
         * @brief store the orientation of the display
         */
        Orientation orientation;

    };

}

#endif