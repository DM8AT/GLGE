/**
 * @file GLGE_Software_Window.h
 * @author JuNi4
 * @brief overload the API for the window
 * @version 0.1
 * @date 2025-07-01
 * 
 * @copyright Copyright (c) 2025
 * 
 */

//header guard
#ifndef _GLGE_SOFTWARE_WINDOW_
#define _GLGE_SOFTWARE_WINDOW_

//include the window
#include "../../GLGEGraphicAPI_Window.h"

//check if this is C++
#if GLGE_CPP

/**
 * @brief the window for the software rasterizer API
 */
class Software_Window : public GraphicWindow
{
public:

    /**
     * @brief Construct a new software rasterizer window
     */
    Software_Window() = default;

    /**
     * @brief Construct a new software rasterizer window
     * 
     * @param window a pointer to the parent window
     * @param instance a pointer to the instance the window will belong to
     */
    Software_Window(Window* window, GraphicInstance* instance) : GraphicWindow(window, instance) {onCreate();}

    /**
     * @brief Destroy the software rasterizer window
     */
    ~Software_Window() override {destroy();}

    /**
     * @brief override the create hook
     */
    void onCreate() override;

    /**
     * @brief swap this window's contents with the new renderd one
     * 
     * @param cmdBuff a pointer to the command buffer to queue the command into
     */
    void swap(GraphicCommandBuffer* cmdBuff) override;

    /**
     * @brief make this the currently active window
     * 
     * @param cmdBuff a pointer to the command buffer to queue the command in
     */
    void makeCurrent(GraphicCommandBuffer* cmdBuff) override;

    inline void* getWindowSurface() { return m_windowSurface; }

protected:
    
    /**
     * @brief store a function that is called from a command buffer to initalize the window
     * 
     * @param window a pointer to the software rasterizer window
     */
    static void Software_WindowInitFunc(Software_Window* window, uint64_t) noexcept;
    /**
     * @brief store a function to update the window's surface
     * 
     * @param window a pointer to the software rasterizer window to swap
     */
    static void Software_WindowSwapFunc(Software_Window* window, uint64_t) noexcept;
    
    static void Software_WindowMakeCurrent(Software_Window* window, uint64_t) noexcept;

    /**
     * @brief store the currently known size of the window
     * @warning this may not actually be the current window's size. It is used to detect size changes
     */
    uvec2 m_size = 0;
    
    /**
     * @brief a pointer to the surface that is rendered into
     */
    void* m_windowSurface = 0;
};

#endif //C++ section

#endif