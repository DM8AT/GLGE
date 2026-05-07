/**
 * @file Window.h
 * @author DM8AT
 * @brief define an overload for the window backend for Vulkan
 * @version 0.1
 * @date 2026-05-04
 * 
 * @copyright Copyright (c) 2026
 * 
 */
//header guard
#ifndef _GLGE_GRAPHIC_BACKEND_BUILTIN_GRAPHICS_VK_WINDOW_
#define _GLGE_GRAPHIC_BACKEND_BUILTIN_GRAPHICS_VK_WINDOW_

//add window backends
#include "Graphic/Backend/Graphics/Window.h"
//add events
#include "Core/Event.h"

//use the namespace
namespace GLGE::Graphic::Backend::Graphic::Vulkan {

    /**
     * @brief the window backend class for Vulkan
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

        /**
         * @brief swap the window
         */
        void swap();

    protected:

        /**
         * @brief store the vulkan surface
         */
        void* m_surface = nullptr;

        /**
         * @brief store the vulkan swapchain
         */
        void* m_swapchain = nullptr;

    };

}

#endif