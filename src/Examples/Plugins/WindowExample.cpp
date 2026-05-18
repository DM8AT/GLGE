/**
 * @file WindowExample.cpp
 * @author DM8AT
 * @brief an example that just opens and manages a blank window with a solid gray clear color
 * @version 0.1
 * @date 2026-05-18
 * 
 * @copyright Copyright (c) 2026
 * 
 */
//add the example system
#include "ExamplePluginContract.h"
#include "ExampleBackendFactory.h"

/**
 * @brief a function that invokes the window example
 * 
 * @param gDescr the graphic description to use
 * @param vDescr the video description to use
 * @return `unsigned char` the return value of the call
 */
unsigned char windowExample(const char* graphicBackendName, const char* videoBackendName);

/**
 * @brief define the function that is used to register the example
 * 
 * @param ptr a pointer to the example registry
 */
extern "C" GLGE_EXAMPLE_PLUGIN_API void EXAMPLE_SYS_REGISTER_EXAMPLE_PLUGIN(ExampleRegistryPtr ptr) {
    //get the example registry
    auto* reg = reinterpret_cast<ExampleRegistry*>(ptr);

    //add the example
    reg->addExample("Window Example - This example opens a simple window with a solid gray clear color",
                    &windowExample);
}

unsigned char windowExample(const char* graphicBackendName, const char* videoBackendName) {
    //the descriptions need to be created here using the provided names
    auto gDescr = createGraphicBackendDescription(graphicBackendName);
    auto vDescr = createVideoBackendDescription(videoBackendName);

    //call the static library initialization
    GLGE::Instance::init();

    //store the graphic instance
    GLGE::Graphic::Instance gInst(gDescr.get(), vDescr.get());

    //create an instance
    GLGE::Instance inst("Window Example", GLGE::Version(0,1,0), std::pair("Graphics", &gInst));

    //print some statistics
    std::cout << "Selected GPU: "           << gInst.getGPUName()          << "\n";
    std::cout << "    GPU Vendor: "         << gInst.getGPUVendorName()    << "\n";
    std::cout << "    GPU Driver Version: " << gInst.getGPUDriverVersion() << "\n";

    //create a window
    GLGE::Graphic::Window win("Window Example", {600, 600});
    GLGE::Graphic::RenderTarget window(&win);

    GLGE::Graphic::RenderPipeline pipe = GLGE::Graphic::RenderPipeline::create(&win, 
        std::pair("Clear", GLGE::Graphic::Command(GLGE::Graphic::COMMAND_CLEAR, window, GLGE::u8(0), GLGE::vec4(0.4, 0.4, 0.4, 1), 0.f, GLGE::u32(0)))
    );
    pipe.record();

    //run while the window is open
    while (!win.isClosingRequested()) {
        inst.startMainTick();

        //potential re-record
        if (win.didResize())
        {pipe.record();}

        //playback
        pipe.play();

        inst.endMainTick();
    }

    //success
    return 0;
}