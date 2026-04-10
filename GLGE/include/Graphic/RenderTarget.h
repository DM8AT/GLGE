/**
 * @file RenderTarget.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2026-03-27
 * 
 * @copyright Copyright (c) 2026
 * 
 */

//header guard
#ifndef _GLGE_GRAPHIC_RENDER_TARGET_
#define _GLGE_GRAPHIC_RENDER_TARGET_

//add common stuff
#include "Core/Common.h"

//use the library namespace
namespace GLGE::Graphic {

    //windows and framebuffers are defined somewhere else
    class Window;
    class Framebuffer;

    /**
     * @brief define a resource that can be rendered to
     */
    class RenderTarget {
    public:

        /**
         * @brief define the type of render target 
         */
        enum Type : u8 {
            /**
             * @brief the target is a window
             */
            WINDOW = 0,
            /**
             * @brief the target is a framebuffer
             */
            FRAMEBUFFER
        };

        /**
         * @brief Construct a new Render Target
         * 
         * @param target a pointer to the framebuffer to use for storage
         */
        RenderTarget(Framebuffer* target)
         : m_type(FRAMEBUFFER), m_target(target)
        {}

        /**
         * @brief Construct a new Render Target
         * 
         * @param window a pointer to the window to use for storage
         */
        RenderTarget(Window* window)
         : m_type(WINDOW), m_target(window)
        {}

        /**
         * @brief Get the Type
         * 
         * @return `Type` the type of the render target
         */
        inline Type getType() const noexcept
        {return m_type;}

        /**
         * @brief Get the Target
         * 
         * @return `void*` a pointer to the target
         */
        inline void* getTarget() const noexcept
        {return m_target;}

    protected:

        /**
         * @brief store the type of render target
         */
        Type m_type = WINDOW;

        /**
         * @brief store a raw pointer to the target
         */
        void* m_target = nullptr;

    };

}

#endif