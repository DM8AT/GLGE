/**
 * @file DebugDrawer.cpp
 * @author DM8AT
 * @brief implement the debug drawer common implementation
 * @version 0.1
 * @date 2026-09-23
 * 
 * @copyright Copyright (c) 2026
 * 
 */
//add the backend
#include "GLGE/Graphic/Backend/Graphics/DebugDrawer.h"

//add the frontend
#include "GLGE/Graphic/DebugContext.h"

GLGE::Graphic::Backend::Graphic::DebugDrawer::DebugDrawer(GLGE::Graphic::DebugContext* frontend, GLGE::Graphic::Backend::Graphic::Instance* instance)
 : CommandInvalidator(), m_inst(instance)
{assert(m_inst == nullptr); attachInvalidator(*static_cast<CommandInvalidator*>(frontend));}