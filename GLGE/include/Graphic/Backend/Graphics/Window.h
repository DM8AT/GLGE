/**
 * @file Window.h
 * @author DM8AT
 * @brief define a 
 * @version 0.1
 * @date 2025-12-27
 * 
 * @copyright Copyright (c) 2025
 * 
 */
//header guard
#ifndef _GLGE_GRAPHIC_BACKEND_GRAPHICS_WINDOW_
#define _GLGE_GRAPHIC_BACKEND_GRAPHICS_WINDOW_

//add common stuff
#include "Core/Common.h"
//add references
#include "Core/Reference.h"
//add the VSync enum
#include "Graphic/VSync.h"

//create the window
namespace GLGE::Graphic 
{class Window;}

//use the backend namespace
namespace GLGE::Graphic::Backend::Graphic {

    /**
     * @brief the window backend is responsible for creating a surface to render to and to manage the actual window
     */
    class Window : public Referable {
    public:

        /**
         * @brief Construct a new backend Window
         * 
         * @param window the pointer to the frontend window
         */
        Window(GLGE::Graphic::Window* window)
         : m_window(window)
        {}

        /**
         * @brief Destroy the Window
         */
        virtual ~Window() {}

        /**
         * @brief a function that is called when the instance has successfully registered the window
         */
        virtual void onWindowSetup() = 0;

        /**
         * @brief tick the graphic window
         */
        virtual void onUpdate() {
            //make sure a resize is triggered during the first tick
            if (m_initialized) 
            {m_resized = false;}
            else
            {m_initialized = false; m_resized = true;}
        }

        /**
         * @brief a function that is called when the size of the window was changed
         * 
         * @param size the new size
         * @param newUsableSize the new usable size of the window
         * @param resolution the pixel resolution of the window
         */
        virtual void onResolutionChange(const uvec2& size, const uvec2& newUsableSize, const uvec2& resolution) = 0;

        /**
         * @brief a function that is called when the HDR state of the window was changed
         * 
         * @param supportHDR `true` if HDR is now supported, `false` otherwise
         */
        virtual void onHDRChange(bool supportHDR) = 0;

        /**
         * @brief Get the frontend window
         * 
         * @return `GLGE::Graphic::Window*` a pointer to the frontend window the backend belongs to
         */
        inline GLGE::Graphic::Window* getWindow() const noexcept
        {return m_window;}

        /**
         * @brief get if the window is currently rendering to an HDR target
         * 
         * @return `true` if an HDR target is selected, `false` otherwise
         */
        inline bool isHDRActive() const noexcept
        {return m_isHDR;}

        /**
         * @brief Get the Resolution of the actual window render target
         * 
         * @return const uvec2& the size in pixels of the target surface
         */
        inline const uvec2& getResolution() const noexcept
        {return m_resolution;}

        /**
         * @brief a function to set the vertical sync modus of the window
         * 
         * @param vsync the requested vsync mode
         * @return `true` when the vsync mode was selected successfully, `false` if the mode is not supported (will default to VSync enabled)
         */
        virtual bool onVSyncSet(VSync vsync) = 0;

        /**
         * @brief get the current vertical sync modus
         * 
         * @return `VSync` the current vertical sync modus
         */
        inline VSync getVSync() const noexcept
        {return m_vsync;}

        /**
         * @brief get if the window was resized
         * 
         * @return `true` if the window was resized, `false` if the window was not resized
         */
        inline bool didResize() const noexcept
        {return m_resized;}

    protected:

        /**
         * @brief store if the window is using HDR
         * 
         * This is NOT if the window could support it, but if the backend uses it
         */
        bool m_isHDR = false;
        /**
         * @brief store the current rendering resolution
         */
        uvec2 m_resolution = {0,0};
        /**
         * @brief store the current VSync state
         */
        VSync m_vsync = VSYNC_ENABLED;
        /**
         * @brief store if the window was resized
         */
        bool m_resized = true;

        /**
         * @brief store if the window is initialized
         */
        bool m_initialized = false;

    private:

        /**
         * @brief store a pointer to the frontend window
         */
        GLGE::Graphic::Window* m_window = nullptr;

    };

}

#endif