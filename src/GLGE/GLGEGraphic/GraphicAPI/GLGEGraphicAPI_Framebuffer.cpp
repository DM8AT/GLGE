/**
 * @file GLGEGraphicAPI_Framebuffer.cpp
 * @author DM8AT
 * @brief implement the default graphic framebuffer
 * @version 0.1
 * @date 2025-04-15
 * 
 * @copyright Copyright (c) 2025
 * 
 */

//include the framebuffer
#include "GLGEGraphicAPI_Framebuffer.h"
//include render pipelines
#include "../GLGERenderPipeline.h"

void GraphicFramebuffer::destroy()
{
    //check if the framebuffer is set up
    if (!m_graphicInstance) {return;}

    //call the destroy hook
    onDestroy();

    //iterate over all render pipelines that reference this window
    for (uint64_t i = 0; i < m_referencedBy.size(); ++i)
    {
        //stop the pipeline
        m_referencedBy[i]->stop();
    }

    //remove from the instance
    m_graphicInstance->removeElement(this);
    m_graphicInstance = 0;
    //set the framebuffer to 0
    m_fbuff = 0;
}

void GraphicFramebuffer::addReference(RenderPipeline* pipeline)
{
    //check if the reference is contained
    if (std::find(m_referencedBy.begin(), m_referencedBy.end(), pipeline) == m_referencedBy.end())
    {
        //add the element
        m_referencedBy.push_back(pipeline);
    }
}