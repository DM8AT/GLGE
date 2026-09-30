/**
 * @file Cursor.h
 * @author DM8AT
 * @brief define a structure to store a cursor style
 * @version 0.1
 * @date 2026-09-30
 * 
 * @copyright Copyright (c) 2026
 * 
 */
//header guard
#ifndef _GLGE_GRAPHIC_BACKEND_BUILTIN_VIDEO_SDL3_CURSOR_
#define _GLGE_GRAPHIC_BACKEND_BUILTIN_VIDEO_SDL3_CURSOR_

//include the cursor backend base
#include "GLGE/Graphic/Backend/Video/Cursor.h"

//use the namespace
namespace GLGE::Graphic::Backend::Video::SDL3 {

    /**
     * @brief a structure to store custom cursors
     */
    class Cursor : public Backend::Video::Cursor {
    public:

        /**
         * @brief Construct a new Cursor
         * 
         * @param style the style for the cursor to create
         */
        Cursor(Defaults style);

        /**
         * @brief Construct a new Cursor
         * 
         * @param image the CPU image to load into the cursor
         * @param hot the position of the cursor hot spot (the spot for interaction)
         */
        Cursor(const ImageCPU& image, const uvec2& hot);

        /**
         * @brief Destroy the Cursor
         */
        virtual ~Cursor();

        /**
         * @brief make this the currently used cursor
         */
        virtual void makeCurrent() const noexcept override;

    protected:

        /**
         * @brief store a handle for the OS cursor
         */
        void* m_cursor = nullptr;
        /**
         * @brief store the surface that contains the content of the cursor
         */
        void* m_surface = nullptr;
        /**
         * @brief store the image data parallel to the surface
         * 
         * The image must remain alive while the surface is
         */
        ImageCPU m_img;
        
    };

}

#endif