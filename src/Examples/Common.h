/**
 * @file Common.h
 * @author DM8AT
 * @brief define a common abstraction for all examples to addhere to
 * @version 0.1
 * @date 2026-05-17
 * 
 * @copyright Copyright (c) 2026
 * 
 */
//header guard
#ifndef _EXAMPLE_COMMON_
#define _EXAMPLE_COMMON_

//add GLGE
#define GLGE_NO_THANKS_MSG
#include "GLGE.h"

/**
 * @brief define a type of function pointer to use for all examples
 */
typedef unsigned char (*PFN_ExampleFunc)(GLGE::Graphic::Backend::Graphic::Description*, GLGE::Graphic::Backend::Video::Description*);

#endif