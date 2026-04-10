/**
 * @file DisplaySetup.cpp
 * @author DM8AT
 * @brief implement the functionality for the display setup class
 * @version 0.1
 * @date 2025-12-28
 * 
 * @copyright Copyright (c) 2025
 * 
 */
//access the backend
#define __GLGE_DISPLAY_BACKEND
//include the display setup class
#include "Graphic/DisplaysSetup.h"

//use the namespace
using namespace GLGE::Graphic;

const Display* DisplaySetup::getDisplay(u32 id) const {
    //if the ID is unknown, do an early return
    if (!m_displays.contains(id))
    {return nullptr;}
    //return the display stored by the ID
    return &m_displays.getNamed(id);
}

void DisplaySetup::__backendRegisterDisplay(const Display& display) {
    //store the new display at the correct position
    m_displays.push_back({display.displayID, display});
}

void DisplaySetup::__backendRemoveDisplay(u32 id) {
    //erase the specific element
    m_displays.eraseNamed(id);
}

Display& DisplaySetup::__backendAccessForEditing(u32 id) {
    //return the specific element
    return m_displays.at(id);
}

void DisplaySetup::__backendSetPrimaryDisplayID(u32 id) {
    //just store the inputted index
    m_primaryID = id;
}