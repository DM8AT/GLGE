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
//include colors
#include "GLGEColor.h"
//include textures
#include "GLGETexture.h"
//include layers
#include "../GLGECore/GLGELayers.h"

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
    /**
     * @brief store if the window is allways on top
     */
    bool alwaysOnTop;
} WindowSettings;

//create a constant default
const WindowSettings WINDOW_SETTINGS_DEFAULT = {false,false,true,true,false,false,false,false,true,true,false};

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
    inline ~Window() noexcept { if (m_window) {close();} }

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
    inline Limiter& getRenderLimiter() noexcept {return m_renderLimiter;}

    /**
     * @brief get the amount of currently opend windows
     * 
     * @return uint64_t the amount of currently opend windows
     */
    static uint64_t openWindowCount() noexcept;

    /**
     * @brief Get the Size of the window
     * 
     * @return const uvec2& the size of the window in pixels
     */
    inline const uvec2& getSize() const noexcept {return m_size;}

    /**
     * @brief Get the position of the window
     * 
     * @return const uvec2& the position of the window
     */
    inline const uvec2& getPos() const noexcept {return m_pos;}

    /**
     * @brief get the SDL2 window
     * 
     * @return void* a pointer to the SDL2 window
     */
    inline void* getSDL2Window() noexcept {return m_window;}

    /**
     * @brief Get the graphic window of the window
     * @warning ONLY USE WHEN YOU REALLY KNOW WHAT YOU'RE DOING
     * 
     * @return GraphicWindow* a pointer to the graphic window
     */
    inline GraphicWindow* getGraphicWindow() noexcept {return m_gWindow;}

    /**
     * @brief change the internal size because the SDL2 size change
     * 
     * @param newSize the new size of the window
     */
    inline void sdl_HandleResize(const uvec2& newSize) noexcept {m_size = newSize;}

    /**
     * @brief Set the icon of the window
     * 
     * @param icon a pointer to the texture containing the window icon
     */
    void setWindowIcon(Texture* icon);

    /**
     * @brief add a layer to the window's layer stack
     * @warning the layer stack will own the new layer
     * 
     * @param layer a pointer to the layer to add
     */
    inline void addLayer(Layer* layer) noexcept {m_layerStack.addLayer(layer);}

    /**
     * @brief send an event through the window's layer stack untill it is handled or reaches the bottom
     * 
     * @param event a pointer to the event to send
     * 
     * @return true : the event was handled
     * @return false : the event was not handled
     */
    inline bool handleEvent(Event* event) noexcept {return m_layerStack.sendEvent(event);}

    //add the window event layer as a friend class
    friend class WindowEventLayer;

    /**
     * @brief move the mouse cursor to a specific pixel inside the window
     * 
     * Mapping:
     * h : height
     * w : width
     * 0/0 -- w/0
     *  |      |
     * 0/h -- w/h
     * 
     * @param pixel the pixel coordinate to warp to according to the mapping
     */
    void warpMouseCursor(uvec2 pixel) const noexcept;

    /**
     * @brief use this window to capture the mouse
     */
    inline void captureMouse() const noexcept {m_instance->captureMouseByWindow(this);}

    /**
     * @brief release the mouse from any capture
     */
    inline void releaseMouse() const noexcept {m_instance->captureMouseByWindow(0);}

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
    /**
     * @brief store the graphic window
     */
    GraphicWindow* m_gWindow = 0;

    /**
     * @brief store a texture containing the window's icon
     */
    Texture* m_icon = 0;
    /**
     * @brief store if the icon is custom
     */
    bool m_customIcon = false;

    /**
     * @brief store the layer stack for the window
     */
    LayerStack m_layerStack;
};

/**
 * @brief the default layer all windows use to handle window events
 */
class WindowEventLayer : public Layer
{
public:

    /**
     * @brief Construct a new Window Event Layer
     * @warning explicitly deleted
     */
    WindowEventLayer() = delete;

    /**
     * @brief Construct a new Window Event Layer
     * 
     * @param window a pointer to the parent window
     */
    WindowEventLayer(Window* window) : m_window(window) {}

    /**
     * @brief handle an event
     * 
     * @param event a pointer to the event to handle
     * 
     * @return true : the event was handled successfully
     * @return false : the event was not handled
     */
    virtual bool onEvent(Event* event) noexcept override;

protected:

    /**
     * @brief store a pointer to the parent window
     */
    Window* m_window = 0;

};

//start a C-Section
extern "C" {
#endif //C++ section

/**
 * @brief store an instance of the window class
 */
typedef struct s_Window Window_t;

//check if a potential C-Section should end
#if GLGE_CPP
}
#endif

#endif