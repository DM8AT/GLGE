/**
 * @file Window.h
 * @author DM8AT
 * @brief define the generic API for video windows
 * @version 0.1
 * @date 2025-12-29
 * 
 * @copyright Copyright (c) 2025
 * 
 */
//header guard
#ifndef _GLGE_GRAPHIC_BACKEND_VIDEO_WINDOW_
#define _GLGE_GRAPHIC_BACKEND_VIDEO_WINDOW_

//include common stuff
#include "Core/Common.h"
//include frontend windows
#include "Graphic/Window.h"

//use the backend video namespace
namespace GLGE::Graphic::Backend::Video {

    /**
     * @brief this backend has instances
     */
    class Instance;

    /**
     * @brief define an abstract class for managing windows in the video backend
     */
    class Window {
    public:

        /**
         * @brief Construct a new Window
         * 
         * @param window a pointer to the frontend window the window will belong to
         */
        Window(GLGE::Graphic::Window* window);

        /**
         * @brief Destroy the Window
         */
        virtual ~Window() {}

        /**
         * @brief a function that is called when the window is resized
         * 
         * This does NOT mean that the displayed size changed. This function should also be called when the 
         * window was moved to a window with a different DPI setting or if the DPI of the window changed. 
         * This function should ALSO be called if the content scale of the window was changed. 
         * 
         * @param size the new size of the window
         * @param safeSize the size of the window that is safe for content
         * @param pixelScale the scale of each pixel in regards to the window sizes
         */
        virtual void onResize(const uvec2& size, const uvec2& safeSize, float pixelScale) = 0;

        /**
         * @brief a function that is called when the window was moved onto a new display
         * 
         * @param displayID the ID of the new display
         */
        virtual void onDisplayChange(u32 displayID) = 0;

        /**
         * @brief a function that is called when the window is maximized
         */
        virtual void onMinimize() = 0;

        /**
         * @brief a function that is called when the window was minimized
         */
        virtual void onMaximize() = 0;
        
        /**
         * @brief a function that is called when the window is restored (from minimized / maximized state)
         */
        virtual void onRestored() = 0;
        
        /**
         * @brief a function that is called when the window gets shown
         */
        virtual void onShown() = 0;
        
        /**
         * @brief a function that is called when the window gets hidden
         */
        virtual void onHidden() = 0;
        
        /**
         * @brief a function that is called when a part of the window gets exposed
         */
        virtual void onExposed() = 0;
        
        /**
         * @brief a function that is called when a part of the window gets occluded
         */
        virtual void onOccluded() = 0;
        
        /**
         * @brief a function that is called when the window gets moved
         * 
         * @param pos the new logical position of the window
         * @param safePos the new logical position of the window's safe area
         */
        virtual void onMoved(const ivec2& pos, const ivec2& safePos) = 0;
        
        /**
         * @brief the window has changed if it has mouse focus or not
         * 
         * @param focus `true` when mouse focus was gained, `false` when mouse focus was lost
         */
        virtual void onMouseFocusChange(bool focus) = 0;
        
        /**
         * @brief the window has changed if it has input focus or not
         * 
         * @param focus `true` when input focus was gained, `false` when input focus was lost
         */
        virtual void onInputFocusChange(bool focus) = 0;
        
        /**
         * @brief the window's fullscreen state was changed
         * 
         * @param fullscreen `true` when the window now is in fullscreen mode, `false` otherwise
         */
        virtual void onFullscreenUpdate(bool fullscreen) = 0;
        
        /**
         * @brief the HDR state of the monitor the window is on changed
         * 
         * @param hdr `true` if the monitor is HDR capable, `false` otherwise
         */
        virtual void onHDRStateChanged(bool hdr) = 0;

        /**
         * @brief set the minimal size of the window
         * 
         * @param size the minimal size of the window
         */
        virtual void onSetMinimumSize(const uvec2& size) = 0;

        /**
         * @brief set the maximum size of the window
         * 
         * @param size the maximal size of the window
         */
        virtual void onSetMaximumSize(const uvec2& size) = 0;

        /**
         * @brief Get the frontend window the backend belongs to
         * 
         * @return `GLGE::Graphic::Window*` a pointer to the frontend window
         */
        inline GLGE::Graphic::Window* getWindow() const noexcept
        {return m_window;}

        /**
         * @brief Get the Backend Instance the window belongs to
         * 
         * @return `GLGE::Graphic::Backend::Video::Instance*` a pointer to the backend instance the window belongs to
         */
        inline GLGE::Graphic::Backend::Video::Instance* getBackendInstance() const noexcept
        {return m_instance;}

    protected:

        /**
         * @brief notify the frontend window that a resize occurred
         * 
         * @param newSize the new size
         * @param newUsableSize the new usable size of the window
         * @param pixelScale the new pixel scaling factor of the window
         * @param resolution the pixel resolution of the window
         */
        void notifyResolutionChange(const uvec2& newSize, const uvec2& newUsableSize, float pixelScale, const uvec2& resolution)
        {m_window->notifyResolutionChange(newSize, newUsableSize, pixelScale, resolution);}

        /**
         * @brief notify the frontend window that a movement occurred
         * 
         * @param newPos the new position of the window
         */
        inline void notifyMove(const ivec2& newPos)
        {m_window->notifyMove(newPos);}

        /**
         * @brief notify the window that it was moved to a different display
         * 
         * @param id the ID of the display the window was moved to
         */
        inline void notifyDisplayChanged(u32 id)
        {m_window->notifyDisplayChanged(id);}

        /**
         * @brief notify the window that its visibility state changed
         * 
         * @param visible `true` if the window is visible, `false` if it is definitely not visible
         */
        inline void notifyVisibleChanged(bool visible)
        {m_window->notifyVisibleChanged(visible);}

        /**
         * @brief get an editable access for the window settings
         * 
         * @return `GLGE::Graphic::WindowSettings&` an editable reference to the window settings
         */
        GLGE::Graphic::WindowSettings& windowSettings()
        {return m_window->m_settings;}

    private:

        /**
         * @brief store the display ID of the window
         */
        u32 m_displayID = 0;

        /**
         * @brief store the window the video window belongs to
         */
        GLGE::Graphic::Window* m_window = nullptr;

        /**
         * @brief store a pointer to the backend instance this window belongs to
         */
        GLGE::Graphic::Backend::Video::Instance* m_instance = nullptr;

    };

}

#endif