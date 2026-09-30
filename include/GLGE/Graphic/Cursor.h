/**
 * @file Cursor.h
 * @author DM8AT
 * @brief define a system to interface with cursors
 * @version 0.1
 * @date 2026-09-30
 * 
 * @copyright Copyright (c) 2026
 * 
 */
//header guard
#ifndef _GLGE_GRAPHIC_CURSOR_
#define _GLGE_GRAPHIC_CURSOR_

//add CPU images
#include "ImageCPU.h"
//add the cursor backend
#include "Backend/Video/Cursor.h"

//add base class
#include "GLGE/Core/BaseClass.h"

//add graphic instance
#include "Instance.h"

//library namespace
namespace GLGE::Graphic {

    /**
     * @brief define a structure to store custom cursors
     */
    class Cursor : public BaseClass {
    public:

        /**
         * @brief use the defaults of the backend
         */
        using Defaults = Backend::Video::Cursor::Defaults;

        /**
         * @brief Construct a new Cursor
         */
        Cursor() = default;

        /**
         * @brief Construct a new Cursor
         * 
         * @param style the style for the cursor to create
         */
        Cursor(Defaults style)
         : BaseClass(), m_cursor(getInstance()->getExtension<Graphic::Instance>()->getVideoDescription()->createCursor(style)) 
        {}

        /**
         * @brief Construct a new Cursor
         * 
         * @param image the CPU image to load into the cursor
         * @param hot the position of the cursor hot spot (the spot for interaction)
         */
        Cursor(const ImageCPU& image, const uvec2& hot)
         : BaseClass(), m_cursor(getInstance()->getExtension<Graphic::Instance>()->getVideoDescription()->createCursor(image, hot)) 
        {}

        /**
         * @brief make the cursor the current one
         */
        void makeCurrent() const noexcept
        {m_cursor->makeCurrent();}

        /**
         * @brief check if the cursor is valid
         * 
         * @return `true` if the value is valid, `false` if not
         */
        inline bool isValid() const noexcept
        {return m_cursor.get() != nullptr;}

        /**
         * @brief check if the cursor is valid
         * 
         * @return `true` if the value is valid, `false` if not
         */
        inline operator bool() const noexcept
        {return isValid();}

    protected:

        /**
         * @brief store the cursor
         */
        Reference<Backend::Video::Cursor> m_cursor;

    };

}

#endif