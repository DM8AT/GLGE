/**
 * @file main.cpp
 * @author DM8AT
 * @brief an example on how to load asset data
 * @version 0.1
 * @date 2025-07-31
 * 
 * @copyright Copyright (c) 2025
 * 
 */

//add GLGE
#include "../../GLGE/GLGE.h"

int main()
{
    Instance inst("Loader instance", API_OPENGL_4_6);
    inst.setLogger(new Logger());
    WindowSettings settings = WINDOW_SETTINGS_DEFAULT;
    settings.resizable = false;
    Window window("Model viewer", 600, 0, settings, inst);
    inst.syncGraphicSetup();

    AssetManager manager("Manager", inst);
    Asset* asset = 0;
    manager.loadAsync<Asset>(&asset, "assets/doc/examples/example.empty");
    manager.waitTillLoaded(&asset);

    Framebuffer fbuff(1, true, false, false, window.getSize(), inst);

    RenderStage stages[] = {
        RenderStage(RENDER_STAGE_CLEAR, ClearStageData(&fbuff), 0,0,0),
        RenderStage(RENDER_STAGE_BLIT_TO_WINDOW, BlitToWindowStageData(&fbuff, &window), 0,0,0),
        RenderStage(RENDER_STAGE_SWAP_WINDOW, SwapWindowData(&window), 0,0,0)
    };
    RenderPipeline pipe(stages, sizeof(stages)/sizeof(*stages), true, inst);

    Limiter lim;
    lim.start();

    while (window.isOpen())
    {
        inst.getLogger()->printAll();
        lim.endTick();
    }
}