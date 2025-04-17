/**
 * @file GLGEGraphicAPI_Window.h
 * @author DM8AT
 * @brief define the abstract API to interface with the graphic windows
 * @version 0.1
 * @date 2025-04-15
 * 
 * @copyright Copyright (c) 2025
 * 
 */

//header guard
#ifndef _GLGE_GRAPHIC_API_WINDOW_
#define _GLGE_GRAPHIC_API_WINDOW_

//include the settings
#include "../../GLGECore/GLGESettings.h"

//include instances
#include "GLGEGraphicAPI_Instance.h"
//include framebuffers
#include "GLGEGraphicAPI_Framebuffer.h"

//include vectors
#include "../../GLGEMath/Vectors/GLGEVectors.h"

//check if this is C++
#if GLGE_CPP
//windows will be defined later
class Window;

/**
 * @brief abstract interface for a window
 */
class GraphicWindow : public GraphicInstAttatchable
{
public:

    /**
     * @brief Construct a new Graphic Window
     */
    GraphicWindow() = default;

    /**
     * @brief Construct a new Graphic Window
     * 
     * @param window a pointer to the parent window
     * @param instance a pointer to the graphic instance the window will belong to
     */
    GraphicWindow(Window* window, GraphicInstance* instance) {create(window, instance);}

    /**
     * @brief Destroy the Graphic Window
     */
    virtual ~GraphicWindow() {destroy();}

    /**
     * @brief create a new window
     * 
     * @param window a pointer to the parent window
     * @param instance a pointer to the graphic instance the window will belong to
     */
    virtual void create(Window* window, GraphicInstance* instance);

    /**
     * @brief destroy the window
     */
    virtual void destroy();

    /**
     * @brief Get the Size of the window
     * 
     * @return const uvec2& the size of the graphic window
     */
    inline const uvec2& getSize() const noexcept {return m_size;}

    /**
     * @brief Get the Framebuffer of the window
     * 
     * @return GraphicFramebuffer* a pointer to the framebuffer of the window
     */
    inline GraphicFramebuffer* getFramebuffer() noexcept {return m_framebuffer;}

    /**
     * @brief get if the renderer is active
     * 
     * @return true : the renderer is active
     * @return false : the renderer is not active
     */
    inline bool isRendererActive() const noexcept {return m_runRenderer;}

    /**
     * @brief Get the Window the graphic window belongs to
     * 
     * @return Window* a pointer to the parent window
     */
    inline Window* getWindow() noexcept {return m_window;}

    /**
     * @brief Get the Command Buffer of the window
     * 
     * @return GraphicCommandBuffer* a pointer to the command buffer belonging to the window
     */
    inline GraphicCommandBuffer* getCommandBuffer() noexcept {return m_buffer;}

protected:

    /**
     * @brief a overloadable hook that is called when the framebuffer is created
     */
    virtual void onCreate() {}
    /**
     * @brief a overloadable hook that is called when the framebuffer is destroyed
     */
    virtual void onDestroy() {}

    /**
     * @brief store the size of the window
     */
    uvec2 m_size = 0;
    /**
     * @brief store the framebuffer of the window
     */
    GraphicFramebuffer* m_framebuffer = 0;
    /**
     * @brief store the command buffer for the window
     */
    GraphicCommandBuffer* m_buffer = 0;
    /**
     * @brief store if the render thread is active
     */
    bool m_runRenderer = false;
    /**
     * @brief store the render thread for the window
     */
    std::thread m_render;
    /**
     * @brief store the parent window
     */
    Window* m_window = 0;

};

#endif

#endif