/**
 * @file GLGEWindow.h
 * @author DM8AT
 * @brief define the layout for the window class
 * @version 0.1
 * @date 2025-04-11
 * 
 * @copyright Copyright (c) 2025
 * 
 */

//header guard
#ifndef _GLGE_GRAPHIC_WINDOW_
#define _GLGE_GRAPHIC_WINDOW_

//include the core
#include "../GLGECore/GLGECore.h"
//include mathematics
#include "../GLGEMath/GLGEMath.h"
//include the graphic API
#include "GraphicAPI/GLGEGraphicAPI.h"

/**
 * @brief specify some settings for a window
 */
typedef struct s_WindowSettings {
    /**
     * @brief specify if the window is borderless
     */
    bool borderless;
    /**
     * @brief specify if the window is in fullscreen mode
     */
    bool fullscreen;
    /**
     * @brief specify if the window can be resized
     */
    bool resizable;
    /**
     * @brief specify if the window can be moved
     */
    bool movable;
    /**
     * @brief store if the window is minimized
     */
    bool minimized;
    /**
     * @brief store if the window is maximized
     */
    bool maximized;
    /**
     * @brief store if the mouse is grabbed
     */
    bool mouseGrabbed;
    /**
     * @brief store if the input is grabbed
     */
    bool inputGrabbed;
    /**
     * @brief store if the input is focused
     */
    bool inputFocused;
    /**
     * @brief store if the mouse is focused
     */
    bool mouseFocused;
} WindowSettings;

//create a constant default
const WindowSettings WINDOW_SETTINGS_DEFAULT = {false,false,true,true,false,false,false,false,true,true};

//check for C++
#if GLGE_CPP

/**
 * @brief a simple window class
 */
class Window : public InstAttachableClass
{
public:

    /**
     * @brief Construct a new Window
     */
    Window() = default;

    /**
     * @brief Construct a new Window
     * 
     * @param name the name of the window
     * @param size the size of the window in pixels
     * @param pos the position of the window in pixels
     * @param settings the settings for the window
     * @param instance the instance for the window
     */
    inline Window(std::string_view name, const uvec2& size, const uvec2& pos, const WindowSettings& settings, Instance& instance) noexcept : InstAttachableClass()
    {open(name, size, pos, settings, instance);}

    /**
     * @brief Destroy the Window
     */
    inline ~Window() noexcept { if (m_window) {close(); }}

    /**
     * @brief open a window
     * @warning make sure the that the window is not open allready
     * 
     * @param name the name of the window
     * @param size the size of the window in pixels
     * @param pos the position of the window in pixels
     * @param settings the settings for the window
     * @param instance the instance for the window
     */
    void open(std::string_view name, const uvec2& size, const uvec2& pos, const WindowSettings& settings, Instance& instance) noexcept;

    /**
     * @brief close the window
     */
    void close() noexcept;

    /**
     * @brief get if the window is open
     * 
     * @return true : the window is open
     * @return false : the window is closed
     */
    inline bool isOpen() const noexcept {return m_window;}

    /**
     * @brief Set if the window is queued to close
     * 
     * @param queued true : the window will close on the next update | false : the window will not close on the next update
     */
    inline void setQueuedClosing(bool queued) noexcept {m_shoudlClose = queued;}

    /**
     * @brief get if closing is queued
     * 
     * @return true : closing is queued
     * @return false : closing is not queued
     */
    inline bool isQueuingClosed() const noexcept {return m_shoudlClose;}

    /**
     * @brief update the window
     * 
     * @return true : the window still exists
     * @return false : the window no longer exists
     */
    virtual bool onUpdate() noexcept override;

    /**
     * @brief get a reference to the render limiter 
     * 
     * @return Limiter& a reference to the render limiter
     */
    inline Limiter& renderLimiter() noexcept {return m_renderLimiter;}

    /**
     * @brief get the amount of currently opend windows
     * 
     * @return uint64_t the amount of currently opend windows
     */
    static uint64_t openWindowCount() noexcept;

protected:

    /**
     * @brief store the size of the window in pixels
     */
    uvec2 m_size = 0;
    /**
     * @brief store the position of the window in pixels
     */
    uvec2 m_pos = 0;
    /**
     * @brief store the window settings
     */
    WindowSettings m_settings = WINDOW_SETTINGS_DEFAULT;

    /**
     * @brief the limiter to limit the rendering
     */
    Limiter m_renderLimiter;

    /**
     * @brief store if the window should close
     */
    bool m_shoudlClose = false;

    /**
     * @brief store the actual SDL2 window
     */
    void* m_window = 0;

};

#endif //C++ section

#endif