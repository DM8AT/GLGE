/**
 * @file Mouse.cpp
 * @author DM8AT
 * @brief 
 * @version 0.1
 * @date 2026-04-07
 * 
 * @copyright Copyright (c) 2026
 * 
 */
//include the mouse header
#include "Core/Mouse.h"

void GLGE::Mouse::update() noexcept {
    //iterate over all buttons and update the state according to the pressed / released state
    //if a button is currently pressed or pressed down then it should be set, if the button is up or moved up it should not be set
    for (u8 button = 0; button < static_cast<u8>(MouseButton::ENUM_MAX); ++button) 
    {m_pressed.set(static_cast<MouseButton>(button), (m_pressed[static_cast<MouseButton>(button)] || m_thisTickDown[static_cast<MouseButton>(button)]) && !m_thisTickUp[static_cast<MouseButton>(button)]);}
    //reset the wheel
    m_wheel = 0;

    //copy the state of this tick over
    m_down = m_thisTickDown;
    m_up = m_thisTickUp;
    //reset the this tick state
    m_thisTickDown.fill(false);
    m_thisTickUp.fill(false);
}