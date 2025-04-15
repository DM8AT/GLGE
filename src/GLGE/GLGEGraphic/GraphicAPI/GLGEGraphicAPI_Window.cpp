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


void GraphicWindow::create(uvec2 size, GraphicInstance* instance)
{
    //check if the texture is set up
    if (m_graphicInstance)
    {
        //log an error
        m_graphicInstance->getInstance()->log("Can not re-create texture that was never destroyed", MESSAGE_TYPE_ERROR);
        return;
    }

    //store the inputs
    m_size = size;
    m_graphicInstance = instance;
    //add the element to the instance
    m_graphicInstance->addElement(this);

    //call the create hook
    onCreate();
}

void GraphicWindow::destroy()
{
    //check if the texture is set up
    if (!m_graphicInstance) {return;}

    //call the destroy hook
    onDestroy();

    //remove from the instance
    m_graphicInstance->removeElement(this);
    m_graphicInstance = 0;
    //reset the variables
    m_size = 0;
}