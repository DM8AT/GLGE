/**
 * @file Window.h
 * @author DM8AT
 * @brief define the frontend window class
 * @version 0.1
 * @date 2025-12-27
 * 
 * @copyright Copyright (c) 2025
 * 
 */
//header guard
#ifndef _GLGE_GRAPHIC_WINDOW_
#define _GLGE_GRAPHIC_WINDOW_

//add common stuff
#include "Core/Common.h"
//add base classes
#include "Core/BaseClass.h"

//add the vsync enum
#include "VSync.h"
//add the window settings
#include "WindowSettings.h"

//add graphic instances
#include "Instance.h"

//use the library namespace
namespace GLGE::Graphic {

    //backend windows are defined else where
    namespace Backend::Graphic {class Window;}
    namespace Backend::Video {class Window;}

    /**
     * @brief a class to manage a window
     */
    class Window : public GLGE::BaseClass {
    public:

        /**
         * @brief Construct a new Window
         * 
         * @param name the name of the window to create
         * @param size the initial window size
         * @param settings the initial settings for the window
         */
        Window(const std::string& name, const uvec2& size, const WindowSettings& settings = WindowSettings{});

        /**
         * @brief Destroy the Window
         */
        ~Window();

        /**
         * @brief tick the window
         */
        void update();

        /**
         * @brief Get the Graphic Instance the window belongs to
         * 
         * @return `GLGE::Graphic::Instance*` a pointer to the graphic instance the window belongs to
         */
        inline GLGE::Graphic::Instance* getGraphicInstance() const noexcept
        {return m_inst;}

        /**
         * @brief Get the Name of the window
         * 
         * @return `const std::string&` a constant reference to the name
         */
        inline const std::string& getName() const noexcept
        {return m_name;}

        /**
         * @brief store that the window is requested to close
         */
        inline void requestClosing() noexcept
        {m_closingRequested = true;}

        /**
         * @brief store that closing is no longer requested
         */
        inline void ignoreClosingRequest() noexcept
        {m_closingRequested = false;}

        /**
         * @brief get if the window is requested to be closed
         * 
         * @return `true` when closing is requested, `false` otherwise
         */
        inline bool isClosingRequested() const noexcept
        {return m_closingRequested;}

        /**
         * @brief Get the logical size of the window
         * 
         * @warning this is not the resolution of the window
         * 
         * This is the logical size of the window reported by the window manager. It is constant across all displays. 
         * 
         * @return `const uvec2&` the logical size of the window
         */
        inline const uvec2& getSize() const noexcept
        {return m_size;}

        /**
         * @brief Get the usable size of the window
         * 
         * This is similar to the size, but only the section not occupied by OS-Content
         * 
         * @return `const uvec2&` the usable logical size of the window
         */
        inline const uvec2& getUsableSize() const noexcept
        {return m_usableSize;}

        /**
         * @brief Get the Theoretical Resolution of the window if the whole window
         * 
         * @return `const uvec2&` The amount of pixels the window takes up on screen
         */
        inline const uvec2& getTheoreticalResolution() const noexcept
        {return m_theoreticalResolution;}

        /**
         * @brief Get the Resolution of the window
         * 
         * @return `const uvec2&` the pixel resolution of the window
         */
        inline const uvec2& getResolution() const noexcept
        {return m_resolution;}

        /**
         * @brief Get the pixel scale of the window
         * 
         * This is not constant and depends on the users window scale setting as well as the DPI of the display the window is on. 
         * This is the factor used to compute the resolution from the size
         * 
         * @return float the current window content scale
         */
        inline float getPixelScale() const noexcept
        {return m_pixelScale;}

        /**
         * @brief Get the unique ID of the display the window is currently on
         * 
         * @return `u32` the unique ID of the display the window is currently on
         */
        inline u32 getDisplayID() const noexcept
        {return m_displayID;}

        /**
         * @brief Get the logical position of the window
         * 
         * @return const uvec2& the window in logical units as reported by the window manager
         */
        inline const ivec2& getPosition() const noexcept
        {return m_pos;}

        /**
         * @brief Get the current state of the window settings
         * 
         * They may change due to user interactions or commands to the window
         * 
         * @return `const WindowSettings&` a constant reference to the window settings
         */
        inline const WindowSettings& getSettings() const noexcept
        {return m_settings;}

        /**
         * @brief get if the window is currently rendering to an HDR target
         * 
         * @return `true` if an HDR target is selected, `false` otherwise
         */
        bool isHDRActive() const noexcept;

        /**
         * @brief Get the Graphic Window owned by the window frontend
         * 
         * @return `Reference<GLGE::Graphic::Backend::Graphic::Window>` a reference to the graphic window backend instance
         */
        inline Reference<GLGE::Graphic::Backend::Graphic::Window> getGraphicWindow() const noexcept
        {return m_gWin;}

        /**
         * @brief Get the Video Window owned by the window frontend
         * 
         * @return `GLGE::Graphic::Backend::Video::Window*` a pointer to the video window backend instance
         */
        inline GLGE::Graphic::Backend::Video::Window* getVideoWindow() const noexcept
        {return m_vWin;}

        /**
         * @brief update the VSync mode of the window
         * 
         * @param vsync the new selected VSync mode
         * @return `true` if the vsync state was set successfully, `false` if it failed (then it defaults to enabled)
         */
        bool setVSyncMode(VSync vsync);

        /**
         * @brief get the vertical sync mode of the window
         * 
         * @return `VSync` the current vertical sync mode of the window
         */
        VSync getVSyncMode() const noexcept;

        /**
         * @brief Set the minimum size
         * 
         * @param size the new minimum size of the window
         */
        void setMinimumSize(const uvec2& size) noexcept;

        /**
         * @brief Set the maximum size
         * 
         * @param size the new maximum size of the window
         */
        void setMaximumSize(const uvec2& size) noexcept;

        /**
         * @brief check if the window did resize
         * 
         * @return `true` if the window resized, `false` if it didn't
         */
        inline bool didResize() const noexcept
        {return m_gWin->didResize();}

    private:

        /**
         * @brief notify the window of a resize
         * 
         * @param newSize the new size
         * @param newUsableSize the new usable size of the window
         * @param pixelScale the new pixel scaling factor of the window
         * @param resolution the pixel resolution of the window
         */
        void notifyResolutionChange(const uvec2& newSize, const uvec2& newUsableSize, float pixelScale, const uvec2& resolution);

        /**
         * @brief notify the window of a movement
         * 
         * @param newPos the new position
         */
        inline void notifyMove(const ivec2& newPos)
        {m_pos = newPos;}

        /**
         * @brief notify the window that its visibility state changed
         * 
         * @param visible `true` if the window is visible, `false` if it is definitely not visible
         */
        inline void notifyVisibleChanged(bool visible)
        {m_visible = visible;}

        /**
         * @brief notify the window that it was moved to a different display
         * 
         * @param id the ID of the display the window was moved to
         */
        inline void notifyDisplayChanged(u32 id)
        {m_displayID = id;}

        /**
         * @brief add the backend window as a friend class
         */
        friend class GLGE::Graphic::Backend::Graphic::Window;
        friend class GLGE::Graphic::Backend::Video::Window;

        /**
         * @brief store the name of the window
         */
        std::string m_name = "Yes.";
        /**
         * @brief store the position of the window
         */
        ivec2 m_pos {0,0};

        /**
         * @brief store the fully unique ID of the display the window is currently on
         */
        u32 m_displayID = 0;
        /**
         * @brief store if the window is visible
         */
        bool m_visible = false;

        /**
         * @brief store the size of the window
         */
        uvec2 m_size {0,0};
        /**
         * @brief store the actually usable size of the window
         */
        uvec2 m_usableSize {0,0};
        /**
         * @brief store the theoretical resolution of the window
         * 
         * This is the resolution of the whole window size
         */
        uvec2 m_theoreticalResolution = {0,0};
        /**
         * @brief store the resolution of the renderable area of the window
         */
        uvec2 m_resolution {0,0};
        /**
         * @brief store by how much the actual renderable size is larger than the window size
         */
        float m_pixelScale = 0;

        /**
         * @brief store the current window settings
         */
        WindowSettings m_settings;
        /**
         * @brief store a pointer to the graphic instance
         */
        Instance* m_inst = nullptr;
        /**
         * @brief store a pointer to the graphic backend window
         */
        Reference<GLGE::Graphic::Backend::Graphic::Window> m_gWin;
        /**
         * @brief store a pointer to the video backend window
         */
        Backend::Video::Window* m_vWin = nullptr;
        /**
         * @brief store if window closing is requested
         */
        bool m_closingRequested = false;

    };

}

#endif