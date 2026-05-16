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
//include the render pipeline
#include "Graphic/RenderPipeline.h"
//get the window
#include "Graphic/Window.h"
//add the OpenGL contract
#include "Graphic/Backend/Video/APIContracts/OpenGL.h"
//add video windows
#include "Graphic/Backend/Video/Window.h"

void GLGE::Graphic::Backend::Graphic::OpenGL::CommandBuffer::onPlay() {
    //playback the recorded stuff
    playback();

    //if a window is bound, swap it
    if (getRenderPipeline()->getWindow()) {
        getRenderPipeline()->getWindow()->getGraphicInstance()->getVideoBackendInstance()->getContract<GLGE::Graphic::Backend::Video::Contracts::OpenGL>()
            ->swap(getRenderPipeline()->getWindow()->getVideoWindow());
    }
}

void GLGE::Graphic::Backend::Graphic::OpenGL::CommandBuffer::onBegin() {
    //make the correct window current
    auto* win = getRenderPipeline()->getWindow();
    if (win)
    {win->getVideoWindow()->getBackendInstance()->getContract<GLGE::Graphic::Backend::Video::Contracts::OpenGL>()->makeCurrent(win->getVideoWindow());}
}