/**
 * @file GLGERenderPipeline.h
 * @author DM8AT
 * @brief define the layout of a render pipeline for GLGE
 * @version 0.1
 * @date 2025-04-18
 * 
 * @copyright Copyright (c) 2025
 * 
 */

//header guard
#ifndef _GLGE_RENDER_PIPELINE_
#define _GLGE_RENDER_PIPELINE_

//include settings
#include "../GLGECore/GLGESettings.h"

/**
 * @brief specify the type of render stage
 */
typedef enum e_StageType {
    /**
     * @brief specify that the render stage should not 
     */
    RENDER_STAGE_NONE,
    /**
     * @brief specify that the render stage should clear the specified framebuffer
     */
    RENDER_STAGE_CLEAR,
} StageType;

/**
 * @brief define the structure of a render stage
 */
typedef struct s_RenderStage {

} RenderStage;

//check if this is C++
#if GLGE_CPP

class RenderPipeline
{

};

#endif //C++ section

#endif