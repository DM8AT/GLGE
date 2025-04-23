/**
 * @file GLGEGraphic.h
 * @author DM8AT
 * @brief include the graphic core for GLGE
 * @version 0.1
 * @date 2025-04-12
 * 
 * @copyright Copyright (c) 2025
 * 
 */

//header guard
#ifndef _GLGE_GRAPHIC_
#define _GLGE_GRAPHIC_

//include settings
#include "../GLGECore/GLGESettings.h"

//check if the graphic core is enabled
#if GLGE_INCLUDE_GRAPHICS

//include all APIs
#include "GraphicAPI/GraphicAPIs/GLGE_AllAPIs.h"

//check if the math core is included
#if !GLGE_INCLUDE_MATH
//check if compile time errors are enabled
#if GLGE_COMPILER_ERRORS
//throw an error: the graphic core depends on the math core
#error "[ERROR] Including the graphic core without the math core is not possible: the graphic core depends on the math core"
#endif //compile errors
#endif //math include check

//include textures
#include "GLGETexture.h"
//include framebuffers
#include "GLGEFramebuffer.h"
//include windows
#include "GLGEWindow.h"

#endif //graphic including section

#endif