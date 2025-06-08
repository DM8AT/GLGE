/**
 * @file GLGE_OGL4_6_Window.h
 * @author DM8AT
 * @brief overload the API for the window
 * @version 0.1
 * @date 2025-04-16
 * 
 * @copyright Copyright (c) 2025
 * 
 */

//header guard
#ifndef _GLGE_OGL_4_6_WINDOW_
#define _GLGE_OGL_4_6_WINDOW_

//include the window
#include "../../GLGEGraphicAPI_Window.h"

//check if this is C++
#if GLGE_CPP

/**
 * @brief the window for the OpenGL 4.6 API
 */
class OGL4_6_Window : public GraphicWindow
{
public:

    /**
     * @brief Construct a new OpenGL 4.6 window
     */
    OGL4_6_Window() = default;

    /**
     * @brief Construct a new OpenGL 4.6 window
     * 
     * @param window a pointer to the parent window
     * @param instance a pointer to the instance the window will belong to
     */
    OGL4_6_Window(Window* window, GraphicInstance* instance) : GraphicWindow(window, instance) {onCreate();}

    /**
     * @brief Destroy the OpenGL 4.6 window
     */
    ~OGL4_6_Window() override {destroy();}

    /**
     * @brief override the create hook
     */
    void onCreate() override;

    /**
     * @brief make this the currently active window
     * 
     * @param cmdBuff a pointer to the command buffer to queue the command in
     */
    void makeCurrent(GraphicCommandBuffer* cmdBuff) override;

    /**
     * @brief swap this window's contents with the new renderd one
     * 
     * @param cmdBuff a pointer to the command buffer to queue the command into
     */
    void swap(GraphicCommandBuffer* cmdBuff) override;

protected:

    /**
     * @brief store a function that is called from a command buffer to initalize the window
     * 
     * @param window a pointer to the OpenGL 4.6 window
     */
    static void OGL4_6_WindowInitFunc(OGL4_6_Window* window, uint64_t) noexcept;
    /**
     * @brief store a function that is called to make a window the current window in a command buffer
     * 
     * @param window a pointer to the OpenGL 4.6 window to make the current one
     */
    static void OGL4_6_WindowMakeCurrent(OGL4_6_Window* window, uint64_t) noexcept;
    /**
     * @brief store a function to update the window's surface
     * 
     * @param window a pointer to the OpenGL 4.6 window to swap
     */
    static void OGL4_6_WindowSwapFunc(OGL4_6_Window* window, uint64_t) noexcept;

    /**
     * @brief store the currently known size of the window
     * @warning this may not actually be the current window's size. It is used to detect size changes
     */
    uvec2 m_size = 0;
};

#endif //C++ section

#endif