/**
 * @file Keyboard.cpp
 * @author DM8AT
 * @brief implement the keyboard state storage
 * @version 0.1
 * @date 2026-04-07
 * 
 * @copyright Copyright (c) 2026
 * 
 */
//include the keyboard
#include "Core/Keyboard.h"

void GLGE::Keyboard::update() noexcept {
    //iterate over all keys and update the state according to the pressed / released state
    //if a key is currently pressed or pressed down then it should be set, if the key is up or moved up it should not be set
    for (u8 key = 0; key < static_cast<u8>(Key::ENUM_MAX); ++key) 
    {m_pressed.set(static_cast<Key>(key), (m_pressed[static_cast<Key>(key)] || m_thisTickDown[static_cast<Key>(key)]) && !m_thisTickUp[static_cast<Key>(key)]);}

    //copy the this tick state
    m_down = m_thisTickDown;
    m_up = m_thisTickUp;
    //fill the up and down states with false
    m_thisTickDown.fill(false);
    m_thisTickUp.fill(false);
}