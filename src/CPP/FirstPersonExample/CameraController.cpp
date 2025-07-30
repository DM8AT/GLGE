/**
 * @file CameraController.cpp
 * @author DM8AT
 * @brief implement the simple camera controller
 * @version 0.1
 * @date 2025-07-25
 * 
 * @copyright Copyright (c) 2025
 * 
 */

//include the camera controller header
#include "CameraController.h"

CameraController::CameraController(Window& window)
 : m_speed(10.), m_window(window)
{
    //warp the mouse to the center
    m_window.warpMouseCursor(m_window.getSize() / 2);
    //capture the mouse
    m_window.captureMouse();
}

void CameraController::onUpdate() noexcept
{
    //only execute the movement if the controller is active
    if (m_active)
    {
        //store the delta time
        float delta = 1.f / m_object->getInstance()->getSDLLimiter().getIPS();
        float mult = m_speed * delta;
        //store the vector to move
        vec3 move;

        //check if the w key is pressed for forward movement
        if (m_object->getInstance()->getKeys().keys.w)
        {
            //get the forward vector of the object
            vec3 forward = m_object->getRot().rotate(-GLGE_ROTATION_GLOBAL_FORWARD);
            //project it to the xz plane and keep it a normal vector
            vec3 proj = vec3(forward.x, 0, forward.z).normalize();
            //queue the movement
            move += (proj * mult);
        }
        //check if the s key is pressed for backwards movement
        if (m_object->getInstance()->getKeys().keys.s)
        {
            //get the negated forward vector of the object
            vec3 forward = m_object->getRot().rotate(GLGE_ROTATION_GLOBAL_FORWARD);
            //project it to the xz plane and keep it a normal vector
            vec3 proj = vec3(forward.x, 0, forward.z).normalize();
            //queue the movement
            move += (proj * mult);
        }

        //check if the a key is pressed for left strafing
        if (m_object->getInstance()->getKeys().keys.a)
        {
            //get the left vector of the object
            vec3 forward = m_object->getRot().rotate(-GLGE_ROTATION_GLOBAL_RIGHT);
            //project it to the xz plane and keep it a normal vector
            vec3 proj = vec3(forward.x, 0, forward.z).normalize();
            //queue the movement
            move += (proj * mult);
        }
        //check if the d key is pressed for right strafing
        if (m_object->getInstance()->getKeys().keys.d)
        {
            //get the negated forward vector of the object
            vec3 forward = m_object->getRot().rotate(GLGE_ROTATION_GLOBAL_RIGHT);
            //project it to the xz plane and keep it a normal vector
            vec3 proj = vec3(forward.x, 0, forward.z).normalize();
            //queue the movement
            move += (proj * mult);
        }

        //check if the space key is pressed for flying up
        if (m_object->getInstance()->getKeys().keys.space)
        {
            //queue the movement
            move += (GLGE_ROTATION_GLOBAL_UP * mult);
        }
        //check if the shift key is pressed for flying down
        if (m_object->getInstance()->getKeys().keys.leftShift)
        {
            //queue the movement
            move += (-GLGE_ROTATION_GLOBAL_UP * mult);
        }

        //actually move the object if the movement is not zero
        if ((move.x != 0) || (move.y != 0) || (move.z != 0))
        {
            //update the position
            m_object->setPos(m_object->getPos() + move);
        }

        //get the mouse position relative to the window
        ivec2 moved = m_object->getInstance()->getMouseChange().pixel;

        //roate the camera object according to the movement
        m_angles -= Euler(moved.y * m_mouseSensetivety, moved.x * m_mouseSensetivety, 0);
        m_object->setRot(m_angles);
        //warp the mouse to the center
        m_window.warpMouseCursor(m_window.getSize() / 2);

        //check if the controller should no longer be active
        if (m_object->getInstance()->getChangedKeys().keys.e)
        {
            //release the mouse
            m_window.releaseMouse();
            //no longer active
            m_active = false;
        }
    }
    else
    {
        //else, check if the window inside is clicked (signals the controller is active again)
        //get the mouse state
        Mouse mouse = m_object->getInstance()->getMouse();
        //check if the mouse is in the window
        if ((mouse.pixel.x > 0) && (mouse.pixel.y > 0) && (mouse.pixel.x < (int32_t)m_window.getSize().x) && (mouse.pixel.y < (int32_t)m_window.getSize().y))
        {
            //check for a left button down
            if (mouse.leftButton)
            {
                //the controller is active again
                m_active = true;
                //warp the mouse to the center
                m_window.warpMouseCursor(m_window.getSize() / 2);
                //capture the mouse again
                m_window.captureMouse();
            }
        }
    }
}