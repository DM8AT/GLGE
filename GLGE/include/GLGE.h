/**
 * @file GLGE.h
 * @author DM8AT
 * @brief include the whole GLGE library
 * @version 0.1
 * @date 2025-12-12
 * 
 * @copyright Copyright (c) 2025
 * 
 */
//header guard
#ifndef _GLGE_
#define _GLGE

//print a thanks message during compilation if not disabled
#ifndef GLGE_NO_THANKS_MSG
#pragma message "Thanks for using the GLGE game library"
#endif

//add the core library
#include "Core/Core.h"

//add graphics
#include "Graphic/Graphic.h"

#endif