/**
 * @file Window.h
 * @author DM8AT
 * @brief define an overload for the window backend for OpenGL
 * @version 0.1
 * @date 2025-12-27
 * 
 * @copyright Copyright (c) 2025
 * 
 */
//header guard
#ifndef _GLGE_GRAPHIC_BACKEND_BUILTIN_GRAPHICS_OGL_WINDOW_
#define _GLGE_GRAPHIC_BACKEND_BUILTIN_GRAPHICS_OGL_WINDOW_

//add window backends
#include "Graphic/Backend/Graphics/Window.h"
//add events
#include "Core/Event.h"

//use the namespace
namespace GLGE::Graphic::Backend::Graphic::OpenGL {

    //instances are defined elsewhere
    class Instance;

    /**
     * @brief implement the backend window for OpenGL
     */
    class Window : public GLGE::Graphic::Backend::Graphic::Window {
    public:

        /**
         * @brief Construct a new Window
         * 
         * @param window a pointer to the frontend window the backend belongs to
         */
        Window(GLGE::Graphic::Window* window);

        /**
         * @brief Destroy the Window
         */
        virtual ~Window();

        /**
         * @brief a function that is called when the instance has successfully registered the window
         */
        virtual void onWindowSetup() override;

        /**
         * @brief a function that is called when the window is resized
         * 
         * @param size the new size
         * @param newUsableSize the new usable size of the window
         * @param resolution the pixel resolution of the window
         */
        virtual void onResolutionChange(const uvec2& size, const uvec2& newUsableSize, const uvec2& resolution) override;

        /**
         * @brief a function that is called when the HDR state of the window was changed
         * 
         * @param supportHDR `true` if HDR is now supported, `false` otherwise
         */
        virtual void onHDRChange(bool supportHDR) override 
        {/*just ignore the HDR state change*/}

        /**
         * @brief a function to set the vertical sync modus of the window
         * 
         * @param vsync the requested vsync mode
         * @return `true` when the vsync mode was selected successfully, `false` if the mode is not supported (will default to VSync enabled)
         */
        virtual bool onVSyncSet(VSync vsync) override;

    protected:

        //add OpenGL instances as a friend
        friend class GLGE::Graphic::Backend::Graphic::OpenGL::Instance;

        /**
         * @brief store the SDL window ID of the window
         */
        u32 m_id = 0;

    };

}

#endif