/**
 * @file GLGEGraphicAPI_RenderPipeline.cpp
 * @author DM8AT
 * @brief define the default behaviour of a graphic render pipeline
 * @version 0.1
 * @date 2025-06-08
 * 
 * @copyright Copyright (c) 2025
 * 
 */

//include the base render pipeline
#include "GLGEGraphicAPI_RenderPipeline.h"
//include render pipelines
#include "../GLGERenderPipeline.h"

//include the shared stuff to get access to SDL handeling
#include "../GLGEGraphicShared.h"

void GraphicRenderPipeline::onExecute() noexcept
{
    //lock the command buffer
    m_cmdBuff->begin();

    //reset the currently active window (might change due to other render pipeline)
    m_cmdBuff->setCurrentWindow(0);

    //iterate over all stages
    for (uint64_t i = 0; i < m_renderPipeline->getStageCount(); ++i)
    {
        //execute a single stage
        onStageExecution(i);
    }

    //unlock the command buffer
    m_cmdBuff->end();

    //wait till the command buffer is empty
    while (m_cmdBuff->isFilled() && __glge_sdl_2_thread) {std::this_thread::sleep_for(std::chrono::nanoseconds(10));}
}

void GraphicRenderPipeline::onStageExecution(uint64_t stageIndex) noexcept
{
    //get a reference to the stage
    RenderStage& stage = (RenderStage&)m_renderPipeline->getStage(stageIndex);
    //run the function before the execution if it exists
    if (stage.before) {(*stage.before)(stage.userData);}
    //then, instantly, run the function that runs after the stage executes, if it exists
    if (stage.after) {(*stage.after)(stage.userData);}
}