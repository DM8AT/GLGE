/**
 * @file GLGE_OGL4_6_CommandBuffer.cpp
 * @author DM8AT
 * @brief implement the command buffer
 * @version 0.1
 * @date 2025-04-16
 * 
 * @copyright Copyright (c) 2025
 * 
 */

//include the command buffer
#include "GLGE_OGL4_6_CommandBuffer.h"

void OGL4_6_CommandBuffer::onPlay()
{
    //iterate over all commands
    while (m_commands.size() > 0)
    {
        //get the top command
        Command& cmd = m_commands[0];
        //execute the command
        (*((void (*)(void*, uint64_t))cmd.commandPtr))(cmd.commandData, cmd.commandDataSize);
        //delete the command
        m_commands.erase(m_commands.begin());
    }
    //say that the command buffer is empty
    m_filled = false;
}