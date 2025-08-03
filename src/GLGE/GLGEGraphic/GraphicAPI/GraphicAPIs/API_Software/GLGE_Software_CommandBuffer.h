/**
 * @file GLGE_Software_CommandBuffer.h
 * @author JuNi4
 * @brief 
 * @version 0.1
 * @date 2025-07-02
 * 
 * @copyright Copyright (c) 2025
 * 
 */

//header guard
#ifndef _GLGE_SOFTWARE_COMMAND_BUFFER_
#define _GLGE_SOFTWARE_COMMAND_BUFFER_

//include the command buffer
#include "../../GLGEGraphicAPI_CommandBuffer.h"

//check if this is C++
#if GLGE_CPP

/**
 * @brief store a command buffer for OpenGL
 */
class Software_CommandBuffer : public GraphicCommandBuffer
{
public:

    /**
     * @brief Construct a new OpenGL 4.6 Command buffer
     */
    Software_CommandBuffer() = default;

    /**
     * @brief Construct a new OpenGL 4.6 Command buffer
     * 
     * @param instance 
     */
    Software_CommandBuffer(GraphicInstance* instance) : GraphicCommandBuffer(instance) {onCreate();}

protected:

    /**
     * @brief override the playing of the command buffer
     */
    void onPlay() override;

};

#endif //C++ section

#endif