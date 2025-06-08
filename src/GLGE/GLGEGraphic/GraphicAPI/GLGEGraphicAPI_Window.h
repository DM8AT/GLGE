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
//include command buffers
#include "GLGEGraphicAPI_CommandBuffer.h"

//include vectors
#include "../../GLGEMath/Vectors/GLGEVectors.h"

//check if this is C++
#if GLGE_CPP
//windows will be defined later
class Window;

//say that pipelines will exist
class RenderPipeline;

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
     * @brief make a window the currently active window
     * 
     * @param cmdBuff the command buffer to queue the activation in
     */
    virtual void makeCurrent(GraphicCommandBuffer*) {}

    /**
     * @brief swap this window's contents with the new renderd one
     * 
     * @param cmdBuff a pointer to the command buffer to queue the command into
     */
    virtual void swap(GraphicCommandBuffer*) {}

    /**
     * @brief Get the Framebuffer of the window
     * 
     * @return GraphicFramebuffer* a pointer to the framebuffer of the window
     */
    inline GraphicFramebuffer* getFramebuffer() noexcept {return m_framebuffer;}

    /**
     * @brief Get the Window the graphic window belongs to
     * 
     * @return Window* a pointer to the parent window
     */
    inline Window* getWindow() noexcept {return m_window;}

    /**
     * @brief get wich command buffers reference this window
     * 
     * @return std::vector<RenderPipeline*> a vector containing pointrs to all command buffers that reference this window
     */
    inline std::vector<RenderPipeline*>& isReferencedBy() noexcept {return m_referencedBy;}

    /**
     * @brief add that the window was referenced by a command buffer
     * 
     * @param cmdBuff a pointer to the command buffer that referenced this window
     */
    void addReference(RenderPipeline* dBuff);

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
     * @brief store the framebuffer of the window
     */
    GraphicFramebuffer* m_framebuffer = 0;
    /**
     * @brief store the parent window
     */
    Window* m_window = 0;

    /**
     * @brief store all command buffer that reference this window
     */
    std::vector<RenderPipeline*> m_referencedBy;
};

#endif

#endif