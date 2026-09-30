/**
 * @file Cursor.h
 * @author DM8AT
 * @brief define an abstraction for custom cursors
 * @version 0.1
 * @date 2026-09-30
 * 
 * @copyright Copyright (c) 2026
 * 
 */
//header guard
#ifndef _GLGE_GRAPHIC_BACKEND_VIDEO_CURSOR_
#define _GLGE_GRAPHIC_BACKEND_VIDEO_CURSOR_

//add CPU images
#include "GLGE/Graphic/ImageCPU.h"
//add resources
#include "GLGE/Core/Reference.h"

//use the backend video namespace
namespace GLGE::Graphic::Backend::Video {

    /**
     * @brief a structure to store custom cursors
     */
    class Cursor : public Referable {
    public:

        /**
         * @brief store identifiers to create system default cursors
         */
        enum class Defaults : u8 {
            /**
             * @brief identifier for the system default cursor
             */
            DEFAULT = 0,
            /**
             * @brief a cursor that is used when a text is hovered
             */
            TEXT,
            /**
             * @brief a cursor for moving (e.g. a window)
             */
            MOVE,
            /**
             * @brief style for resizing along north-south direction
             */
            NS_RESIZE,
            /**
             * @brief style for resizing along east-west direction
             */
            EW_RESIZE,
            /**
             * @brief style for resizing along north-east - south-west direction
             */
            NESW_RESIZE,
            /**
             * @brief style for resizing along north-west - south-east direction
             */
            NWSE_RESIZE,
            /**
             * @brief style if pointing at something (e.g. a button)
             */
            POINTER,
            /**
             * @brief style used while waiting for something
             */
            WAIT,
            /**
             * @brief style used to show that something is progressing
             */
            PROGRESS,
            /**
             * @brief style used to show that clicking something is not allowed
             */
            NOT_ALLOWED
        };

        /**
         * @brief Construct a new Cursor
         * 
         * @param style the style for the cursor to create
         */
        Cursor([[maybe_unused]] Cursor::Defaults style) 
        {}

        /**
         * @brief Construct a new Cursor
         * 
         * @param image the CPU image to load into the cursor
         * @param hot the position of the cursor hot spot (the spot for interaction)
         */
        Cursor([[maybe_unused]] const ImageCPU& image, [[maybe_unused]] const uvec2& hot) 
        {}

        /**
         * @brief Destroy the Cursor
         */
        virtual ~Cursor() = default;

        /**
         * @brief make this the currently used cursor
         */
        virtual void makeCurrent() const noexcept = 0;
        
    };

}

#endif