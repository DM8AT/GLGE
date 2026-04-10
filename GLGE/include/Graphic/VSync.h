/**
 * @file VSync.h
 * @author DM8AT
 * @brief store different modes that can be used for the window swap interval
 * @version 0.1
 * @date 2026-01-02
 * 
 * @copyright Copyright (c) 2026
 * 
 */
//header guard
#ifndef _GLGE_GRAPHIC_VSYNC_
#define _GLGE_GRAPHIC_VSYNC_

//add common stuff
#include "Core/Common.h"

//use the library namespace
namespace GLGE::Graphic {

    /**
     * @brief store different modes for VSync
     */
    enum VSync : u8 {
        /**
         * @brief the window flipping will not sync up with the vertical sync of the display
         */
        VSYNC_DISABLED = 0,
        /**
         * @brief the window flipping will wait for the window to signal the vertical sync befor continuing
         */
        VSYNC_ENABLED,
        /**
         * @brief the window flipping will dynamically adapt with the vertical sync where it MAY decide to ignore syncing on a per-frame basis
         */
        VSYNC_ADAPTIVE
    };

}

#endif