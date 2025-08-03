/**
 * @file GLGEGraphicAPI_Window.cpp
 * @author DM8AT
 * @brief implement the default functionality for the graphic window API
 * @version 0.1
 * @date 2025-04-15
 * 
 * @copyright Copyright (c) 2025
 * 
 */

//include the graphic window
#include "GLGEGraphicAPI_Window.h"
//include windows
#include "../GLGEWindow.h"
//include pipelines
#include "../GLGERenderPipeline.h"

void GraphicWindow::create(Window* window, GraphicInstance* instance)
{
    //check if the window is set up
    if (m_graphicInstance)
    {
        //log an error
        m_graphicInstance->getInstance()->log("Can not re-create window that was never destroyed", MESSAGE_TYPE_ERROR);
        return;
    }

    //store the inputs
    m_window = window;
    m_graphicInstance = instance;
    //add the element to the instance
    m_graphicInstance->addElement(this);

    //call the create hook
    this->onCreate();
}

void GraphicWindow::destroy()
{
    //check if the window is set up
    if (!m_graphicInstance) {return;}

    //iterate over all render pipelines that reference this window
    for (uint64_t i = 0; i < m_referencedBy.size(); ++i)
    {
        //stop the pipeline
        m_referencedBy[i]->stop();
    }

    //call the destroy hook
    onDestroy();

    //remove from the instance
    m_graphicInstance->removeElement(this);
    m_graphicInstance = 0;
    //reset the variables
    m_window = 0;
}

void GraphicWindow::addReference(RenderPipeline* pipeline)
{
    //check if the reference is contained
    if (std::find(m_referencedBy.begin(), m_referencedBy.end(), pipeline) == m_referencedBy.end())
    {
        //add the element
        m_referencedBy.push_back(pipeline);
    }
}