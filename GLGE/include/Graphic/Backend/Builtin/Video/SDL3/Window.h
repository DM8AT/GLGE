/**
 * @file Window.h
 * @author DM8AT
 * @brief define an overload for the video backend class for SDL3
 * @version 0.1
 * @date 2025-12-30
 * 
 * @copyright Copyright (c) 2025
 * 
 */
//header guard
#ifndef _GLGE_GRAPHIC_BACKEND_BUILTIN_VIDEO_SDL3_WINDOW_
#define _GLGE_GRAPHIC_BACKEND_BUILTIN_VIDEO_SDL3_WINDOW_

//include the window backend base
#include "Graphic/Backend/Video/Window.h"

//use the namespace
namespace GLGE::Graphic::Backend::Video::SDL3 {

    /**
     * @brief the SDL3 overload for a video backend window
     */
    class Window : public GLGE::Graphic::Backend::Video::Window {
    public:

        /**
         * @brief Construct a new Window
         */
        Window(GLGE::Graphic::Window* window);

        /**
         * @brief Destroy the Window
         */
        virtual ~Window();

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
        virtual void onResize(const uvec2& size, const uvec2& safeSize, float pixelScale) override;

        /**
         * @brief a function that is called when the window was moved onto a new display
         * 
         * @param displayID the ID of the new display
         */
        virtual void onDisplayChange(u32 displayID) override;

        /**
         * @brief a function that is called when the window is maximized
         */
        virtual void onMinimize() override;

        /**
         * @brief a function that is called when the window was minimized
         */
        virtual void onMaximize() override;
        
        /**
         * @brief a function that is called when the window is restored (from minimized / maximized state)
         */
        virtual void onRestored() override;
        
        /**
         * @brief a function that is called when the window gets shown
         */
        virtual void onShown() override;
        
        /**
         * @brief a function that is called when the window gets hidden
         */
        virtual void onHidden() override;
        
        /**
         * @brief a function that is called when a part of the window gets exposed
         */
        virtual void onExposed() override;
        
        /**
         * @brief a function that is called when a part of the window gets occluded
         */
        virtual void onOccluded() override;
        
        /**
         * @brief a function that is called when the window gets moved
         * 
         * @param pos the new logical position of the window
         * @param safePos the new logical position of the window's safe area
         */
        virtual void onMoved(const ivec2& pos, const ivec2& safePos) override;
        
        /**
         * @brief the window has changed if it has mouse focus or not
         * 
         * @param focus `true` when mouse focus was gained, `false` when mouse focus was lost
         */
        virtual void onMouseFocusChange(bool focus) override;
        
        /**
         * @brief the window has changed if it has input focus or not
         * 
         * @param focus `true` when input focus was gained, `false` when input focus was lost
         */
        virtual void onInputFocusChange(bool focus) override;
        
        /**
         * @brief the window's fullscreen state was changed
         * 
         * @param fullscreen `true` when the window now is in fullscreen mode, `false` otherwise
         */
        virtual void onFullscreenUpdate(bool fullscreen) override;
        
        /**
         * @brief the HDR state of the monitor the window is on changed
         * 
         * @param hdr `true` if the monitor is HDR capable, `false` otherwise
         */
        virtual void onHDRStateChanged(bool hdr) override;

        /**
         * @brief a function to bind this window to a context
         * 
         * This function is required by some backends (e.g. OpenGL) for context management. 
         * It is not required for all backends and can safely be ignored. 
         * 
         * @param context the opaque context handle
         */
        virtual void onMakeCurrent(void* context) override;

        /**
         * @brief set the vsync state for the window
         * 
         * This function is required by some backends (e.g. OpenGL). 
         * It is not required for all backends and can safely be ignored. 
         * 
         * @param vsync the desired VSync mode
         * @param context the context the window belongs to
         * @return `true` if the vsync mode was set successfully, `false` if it failed and defaulted to `VSYNC_DISABLED`
         */
        virtual bool onSetVSync(VSync vsync, void* context) override;

        /**
         * @brief a function to swap the window
         * 
         * This function is only required by some backends (e.g. OpenGL). 
         * It is not required for all backends and can safely be ignored. 
         */
        virtual void onSwapWindow() override;

        /**
         * @brief get the SDL window the window controls
         * 
         * @return `void*` a pointer to the SDL window
         */
        inline void* getSDLWindow() const noexcept
        {return m_win;}

    protected:

        /**
         * @brief Create a new Window
         * 
         * This function runs only on the main thread. It is responsible to create the actual SDL 3 window and set it up fully. 
         * 
         * @param caller the window that called the function
         */
        static void createWindow(GLGE::Graphic::Backend::Video::SDL3::Window* caller);

        /**
         * @brief Destroy a window
         * 
         * This function runs only on the main thread. It is responsible for destroying a SDL3 window
         * 
         * @param caller the window that called the function
         */
        static void destroyWindow(GLGE::Graphic::Backend::Video::SDL3::Window* caller);
        
        /**
         * @brief store the actual SDL window
         */
        void* m_win = nullptr;

    };

}

#endif