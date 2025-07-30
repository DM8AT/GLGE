/**
 * @file CameraController.h
 * @author DM8AT
 * @brief a simple class for a 3D first person camera controller
 * @version 0.1
 * @date 2025-07-25
 * 
 * @copyright Copyright (c) 2025
 * 
 */

//only conpile once
#pragma once

//GLGE is needed for the objects
#include "../../GLGE/GLGE.h"

/**
 * @brief a simple class to controll a 3D camera
 */
class CameraController final : public ObjectAttatchable
{
public:

    /**
     * @brief Construct a new Camera Controller
     * 
     * @param window a reference to the window the image will be presented to
     */
    CameraController(Window& window);

    /**
     * @brief update the camera controller
     */
    virtual void onUpdate() noexcept;

protected:

    /**
     * @brief store if the controller is active
     */
    bool m_active = true;
    /**
     * @brief store the movement speed of the camera
     */
    float m_speed = 10.f;
    /**
     * @brief store the factor for the mouse sensetivety
     */
    float m_mouseSensetivety = 0.2;
    /**
     * @brief store the angles of the camera
     * Used for numeric stabilety and acceleration. Saves one quaternion -> euler conversion
     */
    Euler m_angles;
    /**
     * @brief store a reference to the window the camera renders to
     */
    Window& m_window;

};