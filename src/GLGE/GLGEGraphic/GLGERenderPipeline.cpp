/**
 * @file GLGERenderPipeline.cpp
 * @author DM8AT
 * @brief implement the render pipeline class
 * @version 0.1
 * @date 2025-05-11
 * 
 * @copyright Copyright (c) 2025
 * 
 */

//include the render pipeline
#include "GLGERenderPipeline.h"

RenderPipeline::RenderPipeline(RenderStage* stages, uint64_t stageCount, Instance& instance)
 : InstAttachableClass(&instance, "RenderPipeline"), m_stages(stageCount)
{
    //copy the contents of the render stages to the stages
    memcpy(m_stages.data(), stages, m_stages.size() * sizeof(*m_stages.data()));

    //TODO: Implement switch - case for GraphicAPIs
    m_renderPipeline = new GraphicRenderPipeline(this);

    //Initalize the render pipeline
    m_renderPipeline->onAttatch();
}

RenderPipeline::~RenderPipeline()
{
    //check if an API render pipeline exists
    if (m_renderPipeline)
    {
        //clean up the render pipline
        delete m_renderPipeline;
        m_renderPipeline = 0;
    }
}

void RenderPipeline::execute() noexcept
{
    //loop over all render stages
    for (size_t i = 0; i < m_stages.size(); ++i)
    {
        //execute the stage index using the API-Dependend render pipeline
        m_renderPipeline->onStageExecution(i);
    }
}