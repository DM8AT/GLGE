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

constexpr int TITLE_BAR_HEIGHT = 32;
constexpr int RESIZE_MARGIN = 10;
constexpr int BUTTON_AREA_WIDTH = 86;
constexpr int NON_DRAGGABLE_LEFT_WIDTH = 290;
constexpr int BUTTON_WIDTH = 32;

void DrawMainDockspace() {
    ImGuiIO& io = ImGui::GetIO();

    ImGui::SetNextWindowPos(ImVec2(0.f, TITLE_BAR_HEIGHT));
    ImGui::SetNextWindowSize(ImVec2(io.DisplaySize.x, io.DisplaySize.y - TITLE_BAR_HEIGHT));

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

void DrawTitleBar(GLGE::Graphic::Window* win) {
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize({ImGui::GetIO().DisplaySize.x, TITLE_BAR_HEIGHT});

    ImGui::Begin("TitleBar", nullptr, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBackground);
    auto wp = ImGui::GetWindowPos();
    auto ws = ImGui::GetWindowSize();
    ImGui::GetBackgroundDrawList()->AddRectFilled(wp, {wp.x+ws.x,wp.y+ws.y}, IM_COL32(30,30,30,255));
    ImGui::Text("My Application");
    ImGui::SameLine();
    if (ImGui::Button("File")) {
        win->getInstance()->getExtension<GLGE::Graphic::Instance>()->openFileSelector(
            [] (const std::vector<std::filesystem::path>& files) -> void {
                for (const auto& f : files) {printf("Opened file at %s\n", f.c_str());}
            }, false, {
                std::pair{"PNG Image files", "png"},
                std::pair{"JPG Image files", "jpg;jpeg"},
                std::pair{"Vector image files", "svg"}
            }
        );
    }
    ImGui::SameLine();
    if (ImGui::Button("Fullscreen"))
    {win->setFullscreen(!win->getSettings().fullscreen());}
    ImGui::SameLine();
    if (ImGui::Button("View"))
    {printf("This button does nothing\n");}

    ImGui::SameLine(ImGui::GetWindowWidth() - BUTTON_AREA_WIDTH);
    if (ImGui::Button("__"))
    {win->minimize();}
    ImGui::SameLine();
    if (ImGui::Button("[]")) {
        if (win->getSettings().maximized()) 
        {win->restore();}
        else
        {win->maximize();}
    }
    ImGui::SameLine();
    if (ImGui::Button("X"))
    {win->requestClosing();}

    ImGui::End();
}

int main(void) {
    GLGE::Instance::init();

    GLGE::Graphic::Instance gInst(new GLGE::Graphic::Builtin::Graphics::Vulkan(), new GLGE::Graphic::Builtin::Video::SDL3());
    GLGE::Instance inst("Instance", {0,1,0}, std::pair{"Graphic", &gInst});

    GLGE::Graphic::WindowSettings winSettings;
    winSettings.borderless(true);
    GLGE::Graphic::Window win {"Window", {600, 600}, winSettings};
    win.setMinimumSize({400,300});
    GLGE::Graphic::RenderTarget winTarget(&win);
    win.setHitTestCallback([](GLGE::Graphic::Window* win, GLGE::uvec2 hitPos) -> GLGE::Graphic::Window::HitTest {
        //resizing only allowed when not maximized
        if (!win->getSettings().maximized()) {
            //handle resizing, this takes priority over dragging
            //check in which region the click occurred
            const bool left   = hitPos.x < RESIZE_MARGIN;
            const bool right  = hitPos.x >= win->getSize().x - RESIZE_MARGIN;
            const bool top    = hitPos.y < RESIZE_MARGIN;
            const bool bottom = hitPos.y >= win->getSize().y - RESIZE_MARGIN;
            //handle all combinations of the regions
            if (top && left)
            {return GLGE::Graphic::Window::HitTest::Resize_TopLeft;}
            if (top && right)
            {return GLGE::Graphic::Window::HitTest::Resize_TopRight;}
            if (bottom && left)
            {return GLGE::Graphic::Window::HitTest::Resize_BottomLeft;}
            if (bottom && right)
            {return GLGE::Graphic::Window::HitTest::Resize_BottomRight;}
            if (top)
            {return GLGE::Graphic::Window::HitTest::Resize_Top;}
            if (bottom)
            {return GLGE::Graphic::Window::HitTest::Resize_Bottom;}
            if (left)
            {return GLGE::Graphic::Window::HitTest::Resize_Left;}
            if (right)
            {return GLGE::Graphic::Window::HitTest::Resize_Right;}
        }

        //special exceptions
        if (hitPos.y < TITLE_BAR_HEIGHT) {
            //window controls on the right
            if (hitPos.x >= win->getSize().x - BUTTON_AREA_WIDTH)
            {return GLGE::Graphic::Window::HitTest::Normal;}
            //window controls on the left
            else if (hitPos.x <= NON_DRAGGABLE_LEFT_WIDTH)
            {return GLGE::Graphic::Window::HitTest::Normal;}
        }

        //rest of title bar is draggable
        if (hitPos.y < TITLE_BAR_HEIGHT) 
        {return GLGE::Graphic::Window::HitTest::Dragging;}

        return GLGE::Graphic::Window::HitTest::Normal;
    });

    GLGE::Graphic::GUIContext ctx;
    GLGE::Graphic::CommandStream stream {
        std::pair{"Clear", std::make_unique<GLGE::Graphic::Cmd::Clear>(win, GLGE::vec4{0.5,0.5,0.5,1})},
        std::pair{"GUI",   std::make_unique<GLGE::Graphic::Cmd::DrawGUI>(ctx)}
    };
    GLGE::Graphic::CommandExecutor exec(&win);

    GLGE::u8 imgData[] = {
        0xff,0x00,0x00,0xff, 0x00,0xff,0x00,0xff,
        0x00,0x00,0xff,0xff, 0xff,0xff,0x00,0xff
    };
    GLGE::Graphic::ImageCPU imgCPU(imgData, GLGE::Graphic::PIXEL_FORMAT_RGBA_8_UNORM, {2,2});
    GLGE::Graphic::Image img(imgCPU);

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

        DrawTitleBar(&win);
        DrawMainDockspace();

        ImGui::Begin("Test Window", nullptr, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
        ImGui::Image(reinterpret_cast<ImTextureID>(&img), ImGui::GetWindowSize());
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