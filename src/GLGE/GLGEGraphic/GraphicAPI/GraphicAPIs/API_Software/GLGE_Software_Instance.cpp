/**
 * @file GLGE_Software_Instance.cpp
 * @author JuNi4
 * @brief 
 * @version 0.1
 * @date 2025-07-02
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#include "GLGE_Software_Instance.h"

#include "GLGE_Software_Window.h"
#include "../../../GLGEWindow.h"

//check for Dear ImGui usage
#if GLGE_3RD_PARTY_INCLUDE_DEAR_IMGUI

//include Dear ImGui
#include "../../../../GLGE3rdParty/imgui/imgui.h"
#include "../../../../GLGE3rdParty/imgui/backends/imgui_impl_sdl2.h"
#include "../../../../GLGE3rdParty/imgui/backends/imgui_impl_opengl3.h"
#endif

#include <SDL2/SDL.h>

void Software_Instance::onCreate()
{
    //create the data buffer
    m_dataBuffer.create(this);
    //mark setup as finished
    m_isSetupFinished = true;
}

//check if Dear ImGui should be used
#if GLGE_3RD_PARTY_INCLUDE_DEAR_IMGUI
void Software_Instance::initializeImGui(Software_Window* window)
{

    //check the ImGui version
    IMGUI_CHECKVERSION();
    //create the context for Dear ImGui
    ImGui::CreateContext();
    //set the instances
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

    //set the default style to classic
    ImGui::StyleColorsClassic();

    //initalize Dear ImGui
    if (!ImGui_ImplSDL2_InitForOther((SDL_Window*)window->getWindow()->getSDL2Window()))
    {
        //failed to initalize ImGui for SDL2
        m_instance->log("Failed to initalize SDL2 for ImGui", MESSAGE_TYPE_ERROR);
    }

    m_isImGuiInitialized = true;
}
#endif

void Software_Instance::onDestroy()
{
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
}

void Software_Instance::onRender()
{
    //loop over all command buffers
    for (size_t i = 0; i < m_buffers.size(); ++i)
    {
        //execute the command buffer
        m_buffers[i]->play();
    }
}
