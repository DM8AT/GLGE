/**
 * @file GLGEMouse.h
 * @author DM8AT
 * @brief define the storage for mouse inputs
 * @version 0.1
 * @date 2025-06-23
 * 
 * @copyright Copyright (c) 2025
 * 
 */

//header guard
#ifndef _GLGE_MOUSE_
#define _GLGE_MOUSE_

//include vectors
#include "../GLGEMath/Vectors/GLGEVectors.h"

/**
 * @brief store the state of a mouse
 */
typedef struct s_Mouse {
    /**
     * @brief store if the left mouse button is pressed
     */
    bool leftButton;
    /**
     * @brief store if the middle mouse button is pressed
     */
    bool middleButton;
    /**
     * @brief store if the right mouse button is pressed
     */
    bool rightButton;

    /**
     * @brief store how the mouse weel was scrolled
     */
    int8_t scroll;

    /**
     * @brief store the pixel the mouse is over. The pixel is relative to the window. 
     * 
     * Mapping:
     * h : height
     * w : width
     * 0/0 -- w/0
     *  |      |
     * 0/h -- w/h
     */
    ivec2 pixel;

    /**
     * @brief store the pixel the mouse is over. The pixel is relative to the desktop enviroment. 
     * The mapping is the same as for "pixel". 
     */
    ivec2 pixelDesktop;

    //check for C++ to add a constructor
    #if GLGE_CPP

    /**
     * @brief default constructor sets everything to 0
     */
    s_Mouse()
     : leftButton(false), middleButton(false), rightButton(false), scroll(0), 
       pixel(0), pixelDesktop(0)
    {}

    #endif //end of C++ section
} Mouse;

#endif