/**
 * @file GUIExample.cpp
 * @author DM8AT
 * @brief a simple example to showcase the GUI functionality
 * @version 0.1
 * @date 2026-09-16
 * 
 * @copyright Copyright (c) 2026
 * 
 */
#define GLGE_NO_THANKS_MSG
#ifndef GLGE_HAS_IMGUI
#define GLGE_HAS_IMGUI //Normally auto-defined, but some IDEs do not track that
#endif
#include "GLGE/GLGE.h"

#include "imgui_demo.cpp"

void DrawMainDockspace() {
    ImGuiIO& io = ImGui::GetIO();

    ImGui::SetNextWindowPos(ImVec2(0.f, 0.f));
    ImGui::SetNextWindowSize(ImVec2(io.DisplaySize.x, io.DisplaySize.y));

    ImGuiWindowFlags flags =
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoBringToFrontOnFocus |
        ImGuiWindowFlags_NoNavFocus;

    ImGui::Begin("MainDockspace", nullptr, flags);
    ImGui::DockSpace(ImGui::GetID("MainDockspace"), ImVec2(0, 0), ImGuiDockNodeFlags_PassthruCentralNode);
    ImGui::End();
}

int main(void) {
    GLGE::Instance::init();

    GLGE::Graphic::Instance gInst(new GLGE::Graphic::Builtin::Graphics::OpenGL(), new GLGE::Graphic::Builtin::Video::SDL3());
    GLGE::Instance inst("Instance", {0,1,0}, std::pair{"Graphic", &gInst});

    GLGE::Graphic::Window win {"Window", {600, 600}};
    GLGE::Graphic::RenderTarget winTarget(&win);

    GLGE::Graphic::GUIContext ctx;
    GLGE::Graphic::CommandStream stream {
        std::pair{"Clear", std::make_unique<GLGE::Graphic::Cmd::Clear>(win, GLGE::vec4{0.5,0.5,0.5,1})},
        std::pair{"GUI",   std::make_unique<GLGE::Graphic::Cmd::DrawGUI>(ctx)}
    };
    GLGE::Graphic::CommandExecutor exec(&win);

    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.IniFilename = nullptr;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    GLGE::Graphic::ImGuiProvider imGuiProvider(winTarget);

    inst.start();

    while (!win.isClosingRequested()) {
        inst.startMainTick();

        imGuiProvider.newFrame();
        ImGui::NewFrame();

        DrawMainDockspace();

        ImGui::Begin("Test Window");
        ImGui::Text("Hello World!");
        ImGui::End();

        ImGui::ShowDemoWindow();

        ImGui::EndFrame();
        ImGui::Render();
        imGuiProvider.render();

        ctx.beginRecording();
        ctx.draw(&imGuiProvider);
        ctx.endRecording();

        exec.dispatch(stream);

        inst.endMainTick();
    }
}