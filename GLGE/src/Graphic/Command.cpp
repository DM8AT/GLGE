/**
 * @file Command.cpp
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2026-08-27
 * 
 * @copyright Copyright (c) 2026
 * 
 */
//add commands
#include "Graphic/Command.h"

//implement outside of class
GLGE::Graphic::Command::~Command() 
{for (auto& inv : m_invalidator) {inv->removeCmd(this);}}