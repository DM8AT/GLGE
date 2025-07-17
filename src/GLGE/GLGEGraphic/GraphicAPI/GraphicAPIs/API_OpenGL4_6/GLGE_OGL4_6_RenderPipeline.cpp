/**
 * @file GLGE_OGL4_6_RenderPipeline.cpp
 * @author DM8AT
 * @brief implement the OpenGL 4.6 render pipeline
 * @version 0.1
 * @date 2025-06-08
 * 
 * @copyright Copyright (c) 2025
 * 
 */

//include the render pipeline
#include "GLGE_OGL4_6_RenderPipeline.h"
//include the render pipeline
#include "../../../GLGERenderPipeline.h"
//include command buffers
#include "GLGE_OGL4_6_CommandBuffer.h"
//include shader
#include "../../../Shader/GLGEShader.h"
//include OpenGL
#include <GL/glew.h>

//include render meshes for rendering worlds
#include "../../../GLGERenderMesh.h"

//include OpenGL stuff to read from it
#include "GLGE_OGL4_6_Framebuffer.h"
#include "GLGE_OGL4_6_Shader.h"
#include "GLGE_OGL4_6_MemoryArena.h"
#include "GLGE_OGL4_6_Instance.h"

//check if ImGui is included
#if GLGE_3RD_PARTY_INCLUDE_DEAR_IMGUI

//Dear ImGui needs SDL2, include SDL2
#include <SDL2/SDL.h>

//include Dear ImGui
#include "../../../../GLGE3rdParty/imgui/imgui.h"
#include "../../../../GLGE3rdParty/imgui/backends/imgui_impl_sdl2.h"
#include "../../../../GLGE3rdParty/imgui/backends/imgui_impl_opengl3.h"

#endif //end of ImGui section

void OGL4_6_RenderPipeline::onAttatch() noexcept
{
    //create a new command buffer
    m_cmdBuff = new OGL4_6_CommandBuffer(m_renderPipeline->getInstance()->getGraphicInstance());
}

void OGL4_6_RenderPipeline::onDestroy() noexcept
{
    //check if a command buffer exists
    if (m_cmdBuff)
    {
        //delete the command buffer
        delete m_cmdBuff;
        m_cmdBuff = 0;
    }
}

void OGL4_6_RenderPipeline::getSupportedStageTypes(std::vector<StageType>& types) const noexcept
{
    //add all (currently) supported opperations to the types
    types.push_back(RENDER_STAGE_CLEAR);
    types.push_back(RENDER_STAGE_BLIT_TO_WINDOW);
    types.push_back(RENDER_STAGE_SWAP_WINDOW);
}

void OGL4_6_RenderPipeline::onStageExecution(uint64_t stageIndex) noexcept
{
    //querry the requested stage
    RenderStage& stage = m_renderPipeline->getStage(stageIndex);

    //execute the before function if it exists
    if (stage.before) {(*stage.before)(stage.userData);}

    //switch over the stage type
    switch (stage.type)
    {
    //no need to specify stage_none as it is just default behaviour
    
    //handle a clear stage
    case RENDER_STAGE_CLEAR:
        //call the framebuffer's clear function
        ((Framebuffer*)stage.data.clear.framebuffer)->getGraphicFramebuffer()->clear(m_cmdBuff);
        break;

    //handle framebuffer resizing
    case RENDER_STAGE_RESIZE_FRAMEBUFFER:
        //queue the size change for the framebuffer
        ((Framebuffer*)stage.data.resizeFramebuffer.framebuffer)->getGraphicFramebuffer()->resize(m_cmdBuff, stage.data.resizeFramebuffer.size);
        break;

    //handle a blit stage
    case RENDER_STAGE_BLIT:
        /* code */
        break;
    
    //handle blitting to a window
    case RENDER_STAGE_BLIT_TO_WINDOW:
        //handle the blitting to the window
        blitToWindow(stage);
        break;
    
    //handle blitting from a window
    case RENDER_STAGE_BLIT_FROM_WINDOW:
        /* code */
        break;

    //handle the swapping of a window
    case RENDER_STAGE_SWAP_WINDOW:
        //if the window is not open, don't swap it
        if (!((Window*)stage.data.swapWindow.window)->getGraphicWindow()) {break;}
        //get the window and swap it with the own command buffer
        ((Window*)stage.data.swapWindow.window)->getGraphicWindow()->makeCurrent(m_cmdBuff);
        ((Window*)stage.data.swapWindow.window)->getGraphicWindow()->swap(m_cmdBuff);
        break;

    //execute a compute shader
    case RENDER_STAGE_COMPUTE:
        //attatch the shader
        ((Shader*)stage.data.compute.shader)->attatch(m_cmdBuff);
        //add the execution of the stage
        m_cmdBuff->add(0, (void*)ogl_executeCompute, &stage.data.compute, sizeof(&stage.data.compute));
        //detatch the shader
        ((Shader*)stage.data.compute.shader)->detatch(m_cmdBuff);
        break;

    //render a world
    case RENDER_STAGE_RENDER_WORLD:
        //queue the render world function
        m_cmdBuff->add(0, (void*)ogl_renderWorld, &stage.data.renderWorld, sizeof(stage.data.renderWorld));
        break;

    //check if ImGui is included
    #if GLGE_3RD_PARTY_INCLUDE_DEAR_IMGUI

    //check for the start of a new ImGui frame
    case RENDER_STAGE_DEAR_IMGUI_START_FRAME:
        //queue the frame start
        m_cmdBuff->add(0, (void*)ogl_ImGui_StartFrame, 0,0);
        break;

    //check for a function that uses ImGui
    case RENDER_STAGE_DEAR_IMGUI_FUNCTION:
        //queue the function execution
        m_cmdBuff->add(0, (void*)ogl_ImGui_Execute, (void*)(&stage.data.imguiFunc), sizeof(void (*)()));
        break;

    //check for the end of an ImGui frame
    case RENDER_STAGE_DEAR_IMGUI_END_FRAME:
        //end the frame
        m_cmdBuff->add(0, (void*)ogl_Imgui_EndFrame, 0,0);
        break;

    #endif

    //handle RENDER_STAGE_NONE or a unknown command
    default:
        break;
    }

    //then, instantly, run the function that runs after the stage executes, if it exists
    if (stage.after) {(*stage.after)(stage.userData);}
}

void OGL4_6_RenderPipeline::blitToWindow(RenderStage& stage)
{
    //make sure to activate the window
    ((Window*)stage.data.toWindow.dst)->getGraphicWindow()->makeCurrent(m_cmdBuff);

    //create the data array to store the parameters
    void* data = malloc(sizeof(void*)*2);
    //store the first data element 
    *((Framebuffer**)data) = (Framebuffer*)stage.data.toWindow.src;
    //store the second data element
    *((Window**)data + 1) = (Window*)stage.data.toWindow.dst;

    //queue the blit command
    m_cmdBuff->add(0, (void*)ogl_blitToWindow, data, sizeof(void*)*2);
}

void OGL4_6_RenderPipeline::ogl_blitToWindow(void* data, uint64_t) noexcept
{
    //extract the framebuffer and window
    Framebuffer* fbuff = *((Framebuffer**)data);
    OGL4_6_Framebuffer* gl_Fb = (OGL4_6_Framebuffer*)((fbuff)->getGraphicFramebuffer());
    Window* window = *((Window**)data + 1);

    //execute the blit command
    glBlitNamedFramebuffer(gl_Fb->getOpenGLFramebuffer(), 0, 0,0, fbuff->getSize().x, fbuff->getSize().y, 0,0, window->getSize().x, window->getSize().y, GL_COLOR_BUFFER_BIT, GL_NEAREST);

    //free the data
    free(data);
}

void OGL4_6_RenderPipeline::ogl_executeCompute(void* data, uint64_t) noexcept
{
    //extract the data
    const uvec3& executions = ((ComputeStageData*)data)->executions;
    //run the shader
    glDispatchCompute(executions.x, executions.y, executions.z);
}

void OGL4_6_RenderPipeline::ogl_renderWorld(void* data, uint64_t) noexcept
{
    //extract the data
    RenderWorldStageData* dat = (RenderWorldStageData*)data;
    //extract the world
    World* world = (World*)dat->world;

    //store a list of all objects in the world
    std::vector<Object*> objs;
    //get all objects from the world
    world->getAllObjects(objs);

    //store all elements to render mapped to the material that they use
    std::unordered_map<RenderMaterial*, std::vector<RenderMesh*>> toRender;

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

                //check if the material allready exists
                if (toRender.find(mesh->getMaterial()) == toRender.end())
                {
                    //add a new element with the material
                    toRender[mesh->getMaterial()] = {mesh};
                }
                //else, add the element to the list of it's material
                else
                {
                    toRender[mesh->getMaterial()].push_back(mesh);
                }
            }
        }
    }

    //bind the framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, ((OGL4_6_Framebuffer*)(((Framebuffer*)dat->target)->getGraphicFramebuffer()))->getOpenGLFramebuffer());

    //set the vertex array object to prevent errors
    //changing a VAO is not recomended (performance cost)
    OGL4_6_Instance* inst = (OGL4_6_Instance*)((Framebuffer*)(dat->target))->getGraphicFramebuffer()->getInstance();
    glBindVertexArray(inst->getVAO());
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    //iterate over all materials
    for (auto it = toRender.begin(); it != toRender.end(); ++it)
    {
        //apply the material's shader
        ((OGL4_6_Shader*)it->first->getShader()->getGraphicShader())->attatchShaderDirect();

        //iterate over all meshes that use this material
        for (uint64_t i = 0; i < it->second.size(); ++i)
        {
            //draw the mesh
            glDrawArrays(GL_TRIANGLES, it->second[i]->getIndexPointer().startIdx, it->second[i]->getMesh()->getIndices().size());
        }
    }
}

//check for Dear ImGui support
#if GLGE_3RD_PARTY_INCLUDE_DEAR_IMGUI

void OGL4_6_RenderPipeline::ogl_ImGui_StartFrame(void*, uint64_t) noexcept
{
    //start the ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();
}

void OGL4_6_RenderPipeline::ogl_Imgui_EndFrame(void*, uint64_t) noexcept
{
    //end the ImGui frame
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void OGL4_6_RenderPipeline::ogl_ImGui_Execute(void* data, uint64_t) noexcept
{
    //extract the data for the stage execution
    s_DearImGuiStageData* stageDat = (s_DearImGuiStageData*)data;
    //call the function
    (*stageDat->imGuiFunction)(stageDat->data, stageDat->size);
}

#endif //end of Dear ImGui section