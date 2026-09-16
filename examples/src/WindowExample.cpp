/**
 * @file WindowExample.cpp
 * @author DM8AT
 * @brief a simple example to open and clear a window
 * @version 0.1
 * @date 2026-09-16
 * 
 * @copyright Copyright (c) 2026
 * 
 */
#define GLGE_NO_THANKS_MSG
#include "GLGE/GLGE.h"

int main(void) {
    GLGE::Instance::init();

    GLGE::Graphic::Instance gInst(new GLGE::Graphic::Builtin::Graphics::OpenGL(), new GLGE::Graphic::Builtin::Video::SDL3());
    GLGE::Instance inst("Instance", {0,1,0}, std::pair{"Graphic", &gInst});

    GLGE::Graphic::Window win {"Window", {600, 600}};

    GLGE::Graphic::CommandStream stream {
        std::pair{"Clear", std::make_unique<GLGE::Graphic::Cmd::Clear>(win, GLGE::vec4{0.5,0.5,0.5,1})}
    };
    GLGE::Graphic::CommandExecutor exec(&win);

    while (!win.isClosingRequested()) {
        inst.startMainTick();

        exec.dispatch(stream);

        inst.endMainTick();
    }
}