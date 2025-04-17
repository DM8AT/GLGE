/**
 * @file GLGE_OGL3_3_Window.h
 * @author DM8AT
 * @brief overload the API for the window
 * @version 0.1
 * @date 2025-04-16
 * 
 * @copyright Copyright (c) 2025
 * 
 */

//header guard
#ifndef _GLGE_OGL_3_3_WINDOW_
#define _GLGE_OGL_3_3_WINDOW_

//include the window
#include "../../GLGEGraphicAPI_Window.h"

//check if this is C++
#if GLGE_CPP

/**
 * @brief the window for the OpenGL 3.3 API
 */
class OGL3_3_Window : public GraphicWindow
{
public:

    /**
     * @brief Construct a new OpenGL 3.3 window
     */
    OGL3_3_Window() = default;

    /**
     * @brief Construct a new OpenGL 3.3 window
     * 
     * @param window a pointer to the parent window
     * @param instance a pointer to the instance the window will belong to
     */
    OGL3_3_Window(Window* window, GraphicInstance* instance) : GraphicWindow(window, instance) {onCreate();}

    /**
     * @brief Destroy the OpenGL 3.3 window
     */
    ~OGL3_3_Window() override {destroy();}

    /**
     * @brief override the create hook
     */
    void onCreate() override;
    /**
     * @brief override the destruction hook
     */
    void onDestroy() override;

protected:

};

#endif //C++ section

#endif