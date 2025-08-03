/**
 * @file GLGE_Software_CommandBuffer.cpp
 * @author JuNi4
 * @brief 
 * @version 0.1
 * @date 2025-07-02
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#include "GLGE_Software_CommandBuffer.h"

void Software_CommandBuffer::onPlay()
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