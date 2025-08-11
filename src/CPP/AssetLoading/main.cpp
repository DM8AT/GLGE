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
    Window window("Model viewer", 600, 0, settings, inst);
    inst.syncGraphicSetup();

    AssetManager manager("Manager", inst);
    AssetTexture* tex = manager.load<AssetTexture>("assets/doc/examples/example.tex", TEXTURE_PURPOSE_IMAGE);
    std::cout << tex->getName() << "\n";
    ShaderProcessor proc;
    proc.loadPackage(Path("shader/glgeTextures.gp"), "GLGETextures");
    AssetShader* shader = manager.load<AssetShader>("assets/doc/examples/example.shader", &proc);
    std::cout << shader->getName() << "\n";

    Framebuffer fbuff(1, true, false, false, window.getSize(), inst);

    uvec2 fbuffResizeSize = window.getSize();
    RenderStage stages[] = {
        RenderStage(RENDER_STAGE_RESIZE_FRAMEBUFFER, ResizeFramebufferData(&fbuff, fbuffResizeSize), 0,0,0),
        RenderStage(RENDER_STAGE_CLEAR, ClearStageData(&fbuff), 0,0,0),
        RenderStage(RENDER_STAGE_BLIT_TO_WINDOW, BlitToWindowStageData(&fbuff, &window), 0,0,0),
        RenderStage(RENDER_STAGE_SWAP_WINDOW, SwapWindowData(&window), 0,0,0)
    };
    RenderPipeline pipe(stages, sizeof(stages)/sizeof(*stages), true, inst);

    Limiter lim;
    lim.start();

    while (window.isOpen())
    {
        fbuffResizeSize = window.getSize();
        inst.getLogger()->printAll();
        lim.endTick();
    }
}