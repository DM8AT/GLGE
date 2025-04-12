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

//include the settings
#include "../GLGECore/GLGESettings.h"

//check for C++
#if GLGE_CPP

//include vectors
#include "../GLGEMath/Vectors/GLGEVectors.h"

/**
 * @brief a simple window class
 */
class Window
{
public:

    /**
     * @brief Construct a new Window
     */
    Window() = default;

protected:

    /**
     * @brief store the size of the window in pixels
     */
    uvec2 m_size = 0;
    /**
     * @brief store the name of the window
     */
    std::string_view m_name;

};

#endif //C++ section

#endif