/**
 * @file vk_example1.cpp
 * @author DM8AT
 * @brief define the first Vulkan example
 * @version 0.1
 * @date 2026-04-14
 * 
 * @copyright Copyright (c) 2026
 * 
 */
//add the example
#include "vk_example1.h"

//add GLGE
#define GLGE_NO_THANKS_MSG
#include "GLGE.h"

void vk_example1() {
    //initialize GLGE
    GLGE::Instance::init();

    //setup the backends
    GLGE::Graphic::Builtin::Graphics::Vulkan graphicDesc;
    GLGE::Graphic::Builtin::Video::SDL3 videoDesc;
    //store the graphic instance
    GLGE::Graphic::Instance gInst(&graphicDesc, &videoDesc);
    
    std::cout << "Using graphic backend " << gInst.getGraphicBackendName() << "\n";
    std::cout << "Using video backend " << gInst.getVideoBackendName() << "\n";

    //create the instance
    GLGE::Instance inst("Vulkan instance", GLGE::Version(0, 1, 0), std::pair{"Graphic instance", &gInst});
    inst.mainLimiter().setLimit(144);

    GLGE::Graphic::Window win("Hello from SDL3", {600, 600});
    win.setVSyncMode(GLGE::Graphic::VSYNC_ENABLED);

    std::cout << "Selected GPU: "           << gInst.getGPUName()          << "\n";
    std::cout << "    GPU Vendor: "         << gInst.getGPUVendorName()    << "\n";
    std::cout << "    GPU Driver Version: " << gInst.getGPUDriverVersion() << "\n";

    //while (!win.isClosingRequested()) {
    {
        //start the tick
        inst.startMainTick();

        //end the tick
        inst.endMainTick();
    }

}