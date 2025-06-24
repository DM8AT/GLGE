/**
 * @file GLGEGraphicShared.cpp
 * @author DM8AT
 * @brief initalize all the shared stuff for the graphic module
 * @version 0.1
 * @date 2025-04-13
 * 
 * @copyright Copyright (c) 2025
 * 
 */

//inculude the shared stuff
#include "GLGEGraphicShared.h"

//store all the windows
std::unordered_map<uint32_t, Window*> __glge_all_windows_sdl;

//store the SDL2 thread
std::thread* __glge_sdl_2_thread = 0;

//store the amount of open windows
uint32_t* __glge_all_window_count = 0;

//store all instances
std::vector<Instance*>* __glge_all_instances = 0;




bool KeyboardEventLayer::onEvent(Event* event) noexcept
{
    //check if this event is on the correct channel
    if (event->getIdentifyer().library != GLGE_EVENT_IDENTIFYER_INPUT)
    {
        //stop the function. The event is not ment for this layer. 
        return false;
    }

    //check if the event is a keyboard pressed / released event
    if (event->getIdentifyer().event == GLGE_KEYBOARD_EVENT_KEY_STATE_CHANGE)
    {
        //convert the event to an actual key event
        KeyboardKeyEvent* keyEvent = (KeyboardKeyEvent*)event;

        //update the tick keyboard
        m_instance->m_toggled.keys.keyUpdate(keyEvent->getKey(), m_instance->m_keyboard.keys.getKey(keyEvent->getKey()) != keyEvent->wasReleased());
        //update the key for this instance
        m_instance->m_keyboard.keys.keyUpdate(keyEvent->getKey(), keyEvent->wasReleased());

        //the event was successfully handled
        return true;
    }

    //event was not handled
    return false;
}


bool MouseEventLayer::onEvent(Event* event) noexcept
{
    //check if this event is on the correct channel
    if (event->getIdentifyer().library != GLGE_EVENT_IDENTIFYER_INPUT)
    {
        //wrong channel, discard the event
        return false;
    }

    //check if this is a mouse button event
    if (event->getIdentifyer().event == GLGE_MOUSE_EVENT_MOUSE_BUTTON_STATE_CHANGE)
    {
        //convert the event to a mouse event
        MouseEvent* mouseEvent = (MouseEvent*)event;

        //switch over the button
        switch (mouseEvent->data.button.identifyer)
        {
        //left button
        case SDL_BUTTON_LEFT:
            {
                //button state was changed
                m_instance->m_deltaMouse.leftButton = true;
                //store the new button state
                m_instance->m_mouse.leftButton = mouseEvent->data.button.state;
            }
            break;
        //middle button
        case SDL_BUTTON_MIDDLE:
            {
                //button state was changed
                m_instance->m_deltaMouse.middleButton = true;
                //store the new button state
                m_instance->m_mouse.middleButton = mouseEvent->data.button.state;
            }
            break;
        //right button
        case SDL_BUTTON_RIGHT:
            {
                //button state was changed
                m_instance->m_deltaMouse.rightButton = true;
                //store the new button state
                m_instance->m_mouse.rightButton = mouseEvent->data.button.state;
            }
            break;
        
        default:
            //not a default mouse keycode. Event was not handled. 
            return false;
            break;
        }

        //button was handled successfully
        return true;
    }

    //now, check if the event is a mouse movement event
    if (event->getIdentifyer().event == GLGE_MOUSE_EVENT_MOUSE_MOVEMENT)
    {
        //convert the event to a mouse event
        MouseEvent* mouseEvent = (MouseEvent*)event;

        //compute the difference from the last position of the mouse
        m_instance->m_deltaMouse.pixel = ivec2(mouseEvent->data.pixel.x, mouseEvent->data.pixel.y) - m_instance->m_mouse.pixel;
        //relative motion is not hinderd by reference frame
        m_instance->m_deltaMouse.pixelDesktop = m_instance->m_deltaMouse.pixel;

        //get the global mouse position
        SDL_GetGlobalMouseState(&m_instance->m_mouse.pixelDesktop.x, &m_instance->m_mouse.pixelDesktop.y);
        //store the new relative position
        m_instance->m_mouse.pixel = ivec2(mouseEvent->data.pixel.x, mouseEvent->data.pixel.y);
    }

    //event is not for the mouse, continue
    return false;
}