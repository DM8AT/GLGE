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
     * @param size the size of the window in pixels
     * @param instance a pointer to the graphic instance the window will belong to
     */
    inline GraphicWindow(uvec2 size, GraphicInstance* instance) {create(size, instance);}

    /**
     * @brief Destroy the Graphic Window
     */
    inline ~GraphicWindow() {destroy();}

    /**
     * @brief create a new window
     * 
     * @param size the size of the window in pixels
     * @param instance a pointer to the graphic instance the window will belong to
     */
    void create(uvec2 size, GraphicInstance* instance);

    /**
     * @brief destroy the window
     */
    void destroy();

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

};

#endif

#endif