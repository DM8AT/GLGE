/**
 * @file Window.cpp
 * @author DM8AT
 * @brief implement the not overloadable functionality for the video backend window
 * @version 0.1
 * @date 2025-12-30
 * 
 * @copyright Copyright (c) 2025
 * 
 */
//include the video backend window
#include "Graphic/Backend/Video/Window.h"

//add backend instances
#include "Graphic/Backend/Video/Instance.h"
//add frontend classes
#include "Graphic/Window.h"
#include "Graphic/Instance.h"

//use the namespace
using namespace GLGE::Graphic::Backend::Video;

Window::Window(GLGE::Graphic::Window* window)
 : m_window(window), m_displayID(0), m_instance(window->getInstance()->getExtension<GLGE::Graphic::Instance>()->getVideoBackendInstance())
{}