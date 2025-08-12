/**
 * @file main.cpp
 * @author DM8AT
 * @brief implement the main file to run all other examples from
 * @version 0.1
 * @date 2025-07-30
 * 
 * @copyright Copyright (c) 2025
 * 
 */

//incldue GLGE for the UI
#include "GLGE/GLGE.h"

//add a function to start a program
#if GLGE_WINDOWS
#define LAUNCH_PROGRAM(path) system((std::string(path) + ".exe").c_str())
#else
#define LAUNCH_PROGRAM(path) system(("./" + std::string(path)).c_str())
#endif

//a wrapper for the define as a function (required for threading)
inline void launchProgram(std::string path) noexcept {LAUNCH_PROGRAM(path);}

/**
 * @brief execute an executable file in another thread
 * 
 * @param path the path to the file to execute
 */
void launchProgramDet(std::string path)
{
    //launch the program in the other thread
    std::thread prog(launchProgram, path);
    //detatch the thread so other stuff can run
    prog.detach();
}

//do all the SDL stuff
void ImGuiFunc(void*, uint64_t)
{
    //prepare to make the window fullscreen
    ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
    ImGui::SetNextWindowPos(ImVec2(0,0));

    //start the window for SDL2
    bool open = true;
    ImGui::Begin("GLGE Demo Programs", &open, 
        ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoDecoration);

    //draw some text what the purpose of the window is
    ImGui::Text("A list of all examples for GLGE");

    //collapsable section for all C++ examples
    if (ImGui::CollapsingHeader("C++ Examples"))
    {
        //button to start the first person example
        if (ImGui::Button("Start first person demo")) {launchProgramDet("build/FP_EXAMPLE_CPP");}
        //button to start the C++ unit tests
        if (ImGui::Button("Start unit tests")) {launchProgramDet("build/UNIT_TEST_CPP");}
        //button to start the C++ asset loading test
        if (ImGui::Button("Start asset loader")) {launchProgramDet("build/ASSET_LOADING_CPP");}
    }

    //end the window
    ImGui::End();
}

int main()
{
    //setup the instance
    Instance inst("Main Instance", API_OPENGL_4_6);
    //customize the window settings to avoid resizing
    WindowSettings settings = WINDOW_SETTINGS_DEFAULT;
    settings.resizable = false;
    //create the window
    Window win("Example Hub", uvec2(600,800), 0, settings, inst);
    //syncroize the setup
    inst.syncGraphicSetup();

    //select the dark color style for ImGui
    ImGui::StyleColorsDark();

    //background for the window
    Framebuffer target(1, false, false, false, 1, inst);
    //store all stages for the render pipeline - just a simple one for ImGui
    RenderStage stages[] = {
        RenderStage(RENDER_STAGE_CLEAR, ClearStageData(&target), 0,0,0),
        RenderStage(RENDER_STAGE_BLIT_TO_WINDOW, BlitToWindowStageData(&target, &win), 0,0,0),
        RenderStage(RENDER_STAGE_DEAR_IMGUI_START_FRAME, RenderStage::Data(), 0,0,0),
        RenderStage(RENDER_STAGE_DEAR_IMGUI_FUNCTION, DearImGuiStageData(ImGuiFunc, 0,0), 0,0,0),
        RenderStage(RENDER_STAGE_DEAR_IMGUI_END_FRAME, RenderStage::Data(), 0,0,0),
        RenderStage(RENDER_STAGE_SWAP_WINDOW, SwapWindowData(&win), 0,0,0)
    };
    //start the pipeline instantly
    RenderPipeline pipe(stages, sizeof(stages) / sizeof(*stages), true, inst);

    //stop the main thread from using one CPU thread fully - that is not neccecerry
    //because it is just waiting 10 FPS is enough (dosn't correspond to the actual window FPS - that is still 60)
    Limiter lim(10);
    lim.start();

    //wait for the window to close
    while (win.isOpen())
    {
        lim.endTick();
    }
}