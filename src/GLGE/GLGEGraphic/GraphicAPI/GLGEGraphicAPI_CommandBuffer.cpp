/**
 * @file GLGEGraphicAPI_CommandBuffer.cpp
 * @author DM8AT
 * @brief 
 * @version 0.1
 * @date 2025-04-16
 * 
 * @copyright Copyright (c) 2025
 */

//include the command buffer
#include "GLGEGraphicAPI_CommandBuffer.h"


void GraphicCommandBuffer::create(GraphicInstance* instance)
{
    //check if the command buffer exists
    if (m_graphicInstance)
    {
        //print an error
        m_graphicInstance->getInstance()->log("Can not create command buffer because it is allready created", MESSAGE_TYPE_ERROR);
        return;
    }

    //store the instance
    m_graphicInstance = instance;
    //add this to the instance
    m_graphicInstance->addElement(this);
    //also mark this as a command buffer
    m_graphicInstance->addCommandBuffer(this);

    //clear all the commands
    m_commands.clear();

    //call the attatch hook
    onCreate();
}

void GraphicCommandBuffer::destroy()
{
    //check if the framebuffer is set up
    if (!m_graphicInstance) {return;}

    //lock this command buffer
    begin();

    //call the destroy hook
    onDestroy();

    //remove from the instance
    m_graphicInstance->removeElement(this);
    m_graphicInstance->removeCommandBuffer(this);
    m_graphicInstance = 0;
    //clear the commands
    m_commands.clear();
    //say that no command is queued
    m_filled = false;
    m_inUse.unlock();
}

void GraphicCommandBuffer::play()
{
    //say that the command buffer is in use
    m_inUse.lock();
    //play back the buffer
    onPlay();
    //unlock the mutex again
    m_inUse.unlock();
}