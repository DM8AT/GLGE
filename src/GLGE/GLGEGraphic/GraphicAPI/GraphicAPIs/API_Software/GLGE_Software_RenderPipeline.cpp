/**
 * @file GLGE_Software_RenderPipeline.cpp
 * @author JuNi4
 * @brief 
 * @version 0.1
 * @date 2025-07-02
 * 
 * @copyright Copyright (c) 2025
 * 
 */

//include the render pipeline
#include "GLGE_Software_RenderPipeline.h"
//include the render pipeline
#include "../../../GLGERenderPipeline.h"
//include command buffers
#include "GLGE_Software_CommandBuffer.h"
//include software rasterizer functions
#include "GLGE_Software_Rasterizer.h"

//include render meshes for rendering worlds
#include "../../../GLGERenderMesh.h"

#include "../../GLGEGraphicAPI_MemoryArena.h"

//include OpenGL stuff to read from it
#include "GLGE_Software_Framebuffer.h"
#include "../../../GLGEFramebuffer.h"

#include "GLGE_Software_Window.h"

#include <SDL2/SDL.h>

//check if ImGui is included
#if GLGE_3RD_PARTY_INCLUDE_DEAR_IMGUI

//include Dear ImGui
#include "../../../../GLGE3rdParty/imgui/imgui.h"
#include "../../../../GLGE3rdParty/imgui/backends/imgui_impl_sdl2.h"
#include "../../../../GLGE3rdParty/imgui/backends/imgui_impl_opengl3.h"

#endif //end of ImGui section

void Software_RenderPipeline::onAttatch() noexcept
{
    //create command buffer
    m_cmdBuff = new Software_CommandBuffer(m_renderPipeline->getInstance()->getGraphicInstance());
}

void Software_RenderPipeline::onDestroy() noexcept
{
    //check if a command buffer exists
    if (m_cmdBuff)
    {
        //delete the command buffer
        delete m_cmdBuff;
        m_cmdBuff = 0;
    }
}

void Software_RenderPipeline::getSupportedStageTypes(std::vector<StageType>& types) const noexcept
{
}

void Software_RenderPipeline::onStageExecution(uint64_t stageIndex) noexcept
{
    //querry the requested stage
    RenderStage& stage = m_renderPipeline->getStage(stageIndex);

    //execute the before function if it exists
    if (stage.before) {(*stage.before)(stage.userData);}

    //switch over the stage type
    switch (stage.type)
    {
    case RENDER_STAGE_CLEAR:
        ((Software_Framebuffer*)((Framebuffer*)(stage.data.clear.framebuffer))->getGraphicFramebuffer())->clear(m_cmdBuff);
        break;

    case RENDER_STAGE_SWAP_WINDOW:
        ((Software_Window*)(((Window*)stage.data.swapWindow.window)->getGraphicWindow()))->swap(m_cmdBuff);
        break;

    case RENDER_STAGE_BLIT_TO_WINDOW:
        ((Software_Window*)(((Window*)stage.data.blit.dst)->getGraphicWindow()))->makeCurrent(m_cmdBuff);
        blitToWindow(stage);
        break;

    case RENDER_STAGE_RESIZE_FRAMEBUFFER:
        ((Framebuffer*)stage.data.resizeFramebuffer.framebuffer)->getGraphicFramebuffer()->resize(m_cmdBuff, stage.data.resizeFramebuffer.size);
        break;

    case RENDER_STAGE_RENDER_WORLD:
        //queue the render world function
        m_cmdBuff->add(0, (void*)software_renderWorld, &stage.data.renderWorld, sizeof(stage.data.renderWorld));
        break;

    //check if ImGui is included
    #if GLGE_3RD_PARTY_INCLUDE_DEAR_IMGUI

     //check for the start of a new ImGui frame
    case RENDER_STAGE_DEAR_IMGUI_START_FRAME:
        //queue the frame start
        m_cmdBuff->add(0, (void*)software_ImGui_StartFrame, 0,0);
        break;

    //check for a function that uses ImGui
    case RENDER_STAGE_DEAR_IMGUI_FUNCTION:
        //queue the function execution
        m_cmdBuff->add(0, (void*)software_ImGui_Execute, (void*)(&stage.data.imguiFunc), sizeof(void (*)()));
        break;

    //check for the end of an ImGui frame
    case RENDER_STAGE_DEAR_IMGUI_END_FRAME:
        //end the frame
        m_cmdBuff->add(0, (void*)software_Imgui_EndFrame, 0,0);
        break;

    #endif

    default:
        break;
    }

    //then, instantly, run the function that runs after the stage executes, if it exists
    if (stage.after) {(*stage.after)(stage.userData);}
}

void Software_RenderPipeline::blitToWindow(RenderStage& stage)
{
    m_cmdBuff->add(0, (void*)software_blitToWindow, &stage.data.toWindow, sizeof(stage.data.toWindow));
}

void Software_RenderPipeline::software_blitToWindow(void* data, uint64_t) noexcept
{
    BlitToWindowStageData* sd = (BlitToWindowStageData*)data;

    SDL_Surface* destination = (SDL_Surface*)((Software_Window*)((Window*)sd->dst)->getGraphicWindow())->getWindowSurface();
    SDL_Surface* source = (SDL_Surface*)(((Software_Texture*)(((Framebuffer*)(sd->src))->getColorAttatchment(0)->getGraphicTexture()))->getData());

    SDL_BlitSurface(source, 0, destination, 0);
}

void Software_RenderPipeline::software_executeCompute(void* data, uint64_t) noexcept
{
}

void Software_RenderPipeline::software_renderWorld(void* data, uint64_t)
{
    //extract the data
    RenderWorldStageData* dat = (RenderWorldStageData*)data;
    //extract the world
    World* world = (World*)dat->world;

    //store a list of all objects in the world
    std::vector<Object*> objs;
    //get all objects from the world
    world->getAllObjects(objs);

    //iterate over all objects and get the materials
    for (Object* obj : objs)
    {
        //get all render meshes of the object
        for (uint64_t i = 0; i <  obj->getAttatchments().size(); ++i)
        {
            //check if this is a render mesh
            if (std::string(obj->getAttatchment(i)->getTypeName()) == "RENDER_MESH")
            {
                //get the render mesh
                RenderMesh* mesh = ((RenderMesh*)obj->getAttatchment(i));

                renderMesh((Software_Framebuffer*)((Framebuffer*)dat->target)->getGraphicFramebuffer(), mesh);
            }
        }
    }
}

//check for Dear ImGui support
#if GLGE_3RD_PARTY_INCLUDE_DEAR_IMGUI

void Software_RenderPipeline::software_ImGui_StartFrame(void*, uint64_t) noexcept
{
    //start the ImGui frame
    // ImGui_ImplSDL2_NewFrame();
    // ImGui::NewFrame();
}

void Software_RenderPipeline::software_Imgui_EndFrame(void*, uint64_t) noexcept
{
    //end the ImGui frame
    // ImGui::Render();
}

void Software_RenderPipeline::software_ImGui_Execute(void* data, uint64_t) noexcept
{
    //extract the data for the stage execution
    // s_DearImGuiStageData* stageDat = (s_DearImGuiStageData*)data;
    //call the function
    // (*stageDat->imGuiFunction)(stageDat->data, stageDat->size);
}

#endif //end of Dear ImGui section