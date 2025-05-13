/**
 * @file GLGE_OGL3_3_CommandBuffer.h
 * @author DM8AT
 * @brief overload the command buffers for OpenGL 3.3
 * @version 0.1
 * @date 2025-04-16
 * 
 * @copyright Copyright (c) 2025
 * 
 */

//header guard
#ifndef _GLGE_OGL_3_3_COMMAND_BUFFER_
#define _GLGE_OGL_3_3_COMMAND_BUFFER_

//include the command buffer
#include "../../GLGEGraphicAPI_CommandBuffer.h"

//check if this is C++
#if GLGE_CPP

class OGL3_3_CommandBuffer : public GraphicCommandBuffer
{
public:

    /**
     * @brief Construct a new OpenGL 3.3 Command buffer
     */
    OGL3_3_CommandBuffer() = default;

    /**
     * @brief Construct a new OpenGL 3.3 Command buffer
     * 
     * @param instance 
     */
    OGL3_3_CommandBuffer(GraphicInstance* instance) : GraphicCommandBuffer(instance) {onCreate();}

protected:

    /**
     * @brief override the playing of the command buffer
     */
    void onPlay() override;

};

#endif //C++ section

#endif