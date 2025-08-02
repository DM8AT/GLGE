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
//include all APIs
#include "GraphicAPI/GraphicAPIs/GLGE_AllAPIs.h"

//include the shared stuff to get access to SDL handeling
#include "GLGEGraphicShared.h"

RenderPipeline::RenderPipeline(RenderStage* stages, uint64_t stageCount, bool _start, Instance& instance)
 : InstAttachableClass(&instance, ATTACHMENT_TYPE_RENDER_PIPELINE, "RenderPipeline"), m_stages(stageCount)
{
    //copy the contents of the render stages to the stages
    memcpy(m_stages.data(), stages, m_stages.size() * sizeof(*m_stages.data()));

    //loop over all stages to extract all referenced elements
    for (uint64_t i = 0; i < m_stages.size(); ++i)
    {
        //switch over the stage's type
        switch (m_stages[i].type)
        {
        case RENDER_STAGE_CLEAR:
            //extract and store the framebuffer refernece
            ((Framebuffer*)m_stages[i].data.clear.framebuffer)->getGraphicFramebuffer()->addReference(this);
            break;
        case RENDER_STAGE_BLIT_TO_WINDOW:
            //extract and store the window reference
            ((Window*)m_stages[i].data.toWindow.dst)->getGraphicWindow()->addReference(this);
            //extract and store the framebuffer refernece
            ((Framebuffer*)m_stages[i].data.toWindow.src)->getGraphicFramebuffer()->addReference(this);
            break;

        case RENDER_STAGE_BLIT_FROM_WINDOW:
            //extract and store the window reference
            ((Window*)m_stages[i].data.fromWindow.src)->getGraphicWindow()->addReference(this);
            //extract and store the framebuffer refernece
            ((Framebuffer*)m_stages[i].data.fromWindow.dst)->getGraphicFramebuffer()->addReference(this);
            break;
        
        case RENDER_STAGE_SWAP_WINDOW:
            //extract and store the window reference
            ((Window*)m_stages[i].data.swapWindow.window)->getGraphicWindow()->addReference(this);
            break;

        default:
            break;
        }
    }

    //Create a new graphic render pipeline
    switch (m_instance->getAPI())
    {
    //use OpenGL 4.6
    case API_OPENGL_4_6:
        m_renderPipeline = new OGL4_6_RenderPipeline(this);
        break;
    
    //default: use the base class
    default:
    {
        //log the missing render pipeline implementaion as a cricial warning
        std::stringstream stream;
        stream << "No overload for API " << m_instance->getAPI() << " for a render pipeline was implemented";
        m_instance->log(stream, MESSAGE_TYPE_CRITICAL_WARNING);

        //fall back to the base class
        m_renderPipeline = new GraphicRenderPipeline(this);
        break;
    }
    
    }

    //Initalize the render pipeline
    m_renderPipeline->onAttatch();

    //start the render pipeline if requested
    if (_start) {start();}
}

RenderPipeline::~RenderPipeline()
{
    //stop the render thread
    stop();
    //check if an API render pipeline exists
    if (m_renderPipeline)
    {
        //clean up the render pipline
        delete m_renderPipeline;
        m_renderPipeline = 0;
    }
}

void RenderPipeline::start()
{
    //stop if the render pipeline is running
    if (m_run) {return;}

    //if not, store that the pipeline is now running
    m_run = true;
    //start the render thread
    m_thread = std::thread(execute, this);
}

void RenderPipeline::stop()
{
    //if the render pipeline isn't running, stop
    if (!m_run) {return;}

    //say that the render pipeline is no longer running
    m_run = false;
    //join the render thread
    m_thread.join();
}

void RenderPipeline::execute(RenderPipeline* pipeline) noexcept
{
    //start the limiter
    pipeline->m_limiter.start();
    //loop while the render pipeline is running
    while (pipeline->m_run)
    {
        //execute all the stages of the render pipeline
        pipeline->m_renderPipeline->onExecute();
        //check if SDL2 stopped
        if (!__glge_sdl_2_thread)
        {
            //disable the loop
            pipeline->m_run = false;
        }
        //end the current limiter's tick
        pipeline->m_limiter.endTick();
    }
}