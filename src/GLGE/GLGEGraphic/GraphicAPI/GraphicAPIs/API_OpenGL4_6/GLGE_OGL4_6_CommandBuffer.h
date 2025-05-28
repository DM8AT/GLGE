/**
 * @file GLGE_OGL4_6_CommandBuffer.h
 * @author DM8AT
 * @brief overload the command buffers for OpenGL 4.6
 * @version 0.1
 * @date 2025-04-16
 * 
 * @copyright Copyright (c) 2025
 * 
 */

//header guard
#ifndef _GLGE_OGL_4_6_COMMAND_BUFFER_
#define _GLGE_OGL_4_6_COMMAND_BUFFER_

//include the command buffer
#include "../../GLGEGraphicAPI_CommandBuffer.h"

//check if this is C++
#if GLGE_CPP

class OGL4_6_CommandBuffer : public GraphicCommandBuffer
{
public:

    /**
     * @brief Construct a new OpenGL 4.6 Command buffer
     */
    OGL4_6_CommandBuffer() = default;

    /**
     * @brief Construct a new OpenGL 4.6 Command buffer
     * 
     * @param instance 
     */
    OGL4_6_CommandBuffer(GraphicInstance* instance) : GraphicCommandBuffer(instance) {onCreate();}

protected:

    /**
     * @brief override the playing of the command buffer
     */
    void onPlay() override;

};

#endif //C++ section

#endif