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
         * @brief Get the surface
         * 
         * @return `void*` the vulkan surface
         */
        inline void* getSurface() const noexcept
        {return m_surface;}

        /**
         * @brief Get the swapchain
         * 
         * @return `void*` the vulkan swapchain
         */
        inline void* getSwapchain() const noexcept
        {return m_swapchain;}

        /**
         * @brief Get all the images from the swapchain
         * 
         * @return `const std::vector<void*>&` a constant reference to the vector of images
         */
        inline const std::vector<void*>& getImages() const noexcept
        {return m_imgs;}

        /**
         * @brief Get the image views from the swapchain
         * 
         * @return `const std::vector<void*>&` a constant reference to the vector of image views
         */
        inline const std::vector<void*>& getImageViews() const noexcept
        {return m_imgViews;}

        /**
         * @brief Get the format
         * 
         * @return `i32` the image format enum value
         */
        inline i32 getFormat() const noexcept
        {return m_format;}

    protected:

        /**
         * @brief store the vulkan surface
         */
        void* m_surface = nullptr;

        /**
         * @brief store the vulkan swapchain
         */
        void* m_swapchain = nullptr;

        /**
         * @brief store the images of the swapchain
         */
        std::vector<void*> m_imgs;
        /**
         * @brief store views to the swapchain images
         */
        std::vector<void*> m_imgViews;

        /**
         * @brief store the selected image format
         */
        i32 m_format = 0;

    };

}

#endif