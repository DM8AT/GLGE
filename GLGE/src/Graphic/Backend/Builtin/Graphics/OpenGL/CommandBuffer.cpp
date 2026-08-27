/**
 * @file CommandBuffer.cpp
 * @author DM8AT
 * @brief implement the OpenGL command buffer
 * @version 0.1
 * @date 2026-05-08
 * 
 * @copyright Copyright (c) 2026
 * 
 */
//include the command buffer
#include "Graphic/Backend/Builtin/Graphics/OpenGL/CommandBuffer.h"
//include the instance
#include "Graphic/Instance.h"
//get the window
#include "Graphic/Window.h"
//add the OpenGL contract
#include "Graphic/Backend/Video/APIContracts/OpenGL.h"
//add video windows
#include "Graphic/Backend/Video/Window.h"

GLGE::Graphic::Backend::Graphic::OpenGL::CommandBuffer::CommandBuffer(GLGE::Graphic::Instance* instance) 
 : GLGE::Graphic::Backend::Graphic::CommandBuffer(instance)
{}

void GLGE::Graphic::Backend::Graphic::OpenGL::CommandBuffer::onPlay() {
    //playback the recorded stuff
    playback();
}