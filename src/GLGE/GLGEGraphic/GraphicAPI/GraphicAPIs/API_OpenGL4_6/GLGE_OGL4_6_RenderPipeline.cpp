/**
 * @file GLGE_OGL4_6_RenderPipeline.cpp
 * @author DM8AT
 * @brief implement the OpenGL 4.6 render pipeline
 * @version 0.1
 * @date 2025-06-08
 * 
 * @copyright Copyright (c) 2025
 * 
 */

//include the render pipeline
#include "GLGE_OGL4_6_RenderPipeline.h"
//include the render pipeline
#include "../../../GLGERenderPipeline.h"
//include command buffers
#include "GLGE_OGL4_6_CommandBuffer.h"
//include OpenGL
#include <GL/glew.h>

//include OpenGL stuff to read from it
#include "GLGE_OGL4_6_Framebuffer.h"

void OGL4_6_RenderPipeline::onAttatch() noexcept
{
    //create a new command buffer
    m_cmdBuff = new OGL4_6_CommandBuffer(m_renderPipeline->getInstance()->getGraphicInstance());
}

void OGL4_6_RenderPipeline::onDestroy() noexcept
{
    //check if a command buffer exists
    if (m_cmdBuff)
    {
        //delete the command buffer
        delete m_cmdBuff;
        m_cmdBuff = 0;
    }
}

void OGL4_6_RenderPipeline::getSupportedStageTypes(std::vector<StageType>& types) const noexcept
{
    //add all (currently) supported opperations to the types
    types.push_back(RENDER_STAGE_CLEAR);
    types.push_back(RENDER_STAGE_BLIT_TO_WINDOW);
    types.push_back(RENDER_STAGE_SWAP_WINDOW);
}

void OGL4_6_RenderPipeline::onStageExecution(uint64_t stageIndex) noexcept
{
    //querry the requested stage
    RenderStage& stage = m_renderPipeline->getStage(stageIndex);

    //execute the before function if it exists
    if (stage.before) {(*stage.before)(stage.userData);}

    //switch over the stage type
    switch (stage.type)
    {
    //no need to specify stage_none as it is just default behaviour
    
    //handle a clear stage
    case RENDER_STAGE_CLEAR:
        //call the framebuffer's clear function
        ((Framebuffer*)stage.data.clear.framebuffer)->getGraphicFramebuffer()->clear(m_cmdBuff);
        break;

    //handle a blit stage
    case RENDER_STAGE_BLIT:
        /* code */
        break;
    
    //handle blitting to a window
    case RENDER_STAGE_BLIT_TO_WINDOW:
        //handle the blitting to the window
        blitToWindow(stage);
        break;
    
    //handle blitting from a window
    case RENDER_STAGE_BLIT_FROM_WINDOW:
        /* code */
        break;

    //handle the swapping of a window
    case RENDER_STAGE_SWAP_WINDOW:
        //if the window is not open, don't swap it
        if (!((Window*)stage.data.swapWindow.window)->getGraphicWindow()) {break;}
        //get the window and swap it with the own command buffer
        ((Window*)stage.data.swapWindow.window)->getGraphicWindow()->makeCurrent(m_cmdBuff);
        ((Window*)stage.data.swapWindow.window)->getGraphicWindow()->swap(m_cmdBuff);
        break;

    //handle RENDER_STAGE_NONE or a unknown command
    default:
        break;
    }
}

void OGL4_6_RenderPipeline::blitToWindow(RenderStage& stage)
{
    //make sure to activate the window
    ((Window*)stage.data.toWindow.dst)->getGraphicWindow()->makeCurrent(m_cmdBuff);

    //create the data array to store the parameters
    void* data = malloc(sizeof(void*)*2);
    //store the first data element 
    *((Framebuffer**)data) = (Framebuffer*)stage.data.toWindow.src;
    //store the second data element
    *((Window**)data + 1) = (Window*)stage.data.toWindow.dst;

    //queue the blit command
    m_cmdBuff->add(0, (void*)ogl_blitToWindow, data, sizeof(void*)*2);
}

void OGL4_6_RenderPipeline::ogl_blitToWindow(void* data, uint64_t) noexcept
{
    //extract the framebuffer and window
    Framebuffer* fbuff = *((Framebuffer**)data);
    Window* window = *((Window**)data + 1);

    //bind the framebuffer as source and the window as destination
    glBindFramebuffer(GL_READ_FRAMEBUFFER, ((OGL4_6_Framebuffer*)fbuff->getGraphicFramebuffer())->getOpenGLFramebuffer());
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

    //execute the blit command
    glBlitFramebuffer(0,0, fbuff->getSize().x, fbuff->getSize().y, 0,0, window->getSize().x, window->getSize().y, GL_COLOR_BUFFER_BIT, GL_NEAREST);

    //free the data
    free(data);
}