/**
 * @file CommandBuffer.h
 * @author DM8AT
 * @brief define the opengl command buffer
 * @version 0.1
 * @date 2026-05-08
 * 
 * @copyright Copyright (c) 2026
 * 
 */
//header guard
#ifndef _GLGE_GRAPHIC_BUILTIN_GRAPHICS_OGL_COMMAND_BUFFER_
#define _GLGE_GRAPHIC_BUILTIN_GRAPHICS_OGL_COMMAND_BUFFER_

//include the command buffer backend
#include "Graphic/Backend/Graphics/CommandBuffer.h"

//use the namespace
namespace GLGE::Graphic::Backend::Graphic::OpenGL {

    /**
     * @brief store the OpenGL command buffer
     */
    class CommandBuffer : public GLGE::Graphic::Backend::Graphic::CommandBuffer {
    public:

        /**
         * @brief Construct a new Command Buffer
         * 
         * All command buffers are initialized in an empty state
         */
        CommandBuffer(GLGE::Graphic::RenderPipeline* renderPipeline)
         : GLGE::Graphic::Backend::Graphic::CommandBuffer(renderPipeline)
        {}

        /**
         * @brief Destroy the Command Buffer
         */
        virtual ~CommandBuffer() {}

        /**
         * @brief a function used to say that a recording should start
         */
        virtual void onBegin() override;

    protected:

        /**
         * @brief a function used to play back the recorded command buffer
         */
        virtual void onPlay() override;

    };

}

#endif