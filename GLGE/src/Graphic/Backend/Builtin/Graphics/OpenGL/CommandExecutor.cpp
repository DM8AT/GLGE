/**
 * @file CommandExecutor.cpp
 * @author DM8AT
 * @brief implement the command executor for OpenGL
 * @version 0.1
 * @date 2026-08-28
 * 
 * @copyright Copyright (c) 2026
 * 
 */
//add the backend
#include "Graphic/Backend/Builtin/Graphics/OpenGL/CommandExecutor.h"

//add command streams
#include "Graphic/CommandStream.h"

//get the window frontend
#include "Graphic/Window.h"
//add the OpenGL contract
#include "Graphic/Backend/Video/APIContracts/OpenGL.h"

void GLGE::Graphic::Backend::Graphic::OpenGL::CommandExecutor::dispatch(GLGE::Graphic::CommandStream* stream) {
    //iterate over all commands and dispatch them
    for (const auto& el : stream->getEntries()) {
        if (!el.cmd->isDirty() && el.enabled)
        {el.cmdBuff->play();}
    }

    //if a window is set, flip it
    if (m_window) 
    {m_window->getGraphicInstance()->getVideoBackendInstance()->getContract<GLGE::Graphic::Backend::Video::Contracts::OpenGL>()->swap(m_window->getVideoWindow());}
}