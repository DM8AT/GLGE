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
    inst.setLogger(new Logger(true));
    WindowSettings settings = WINDOW_SETTINGS_DEFAULT;
    settings.resizable = false;
    Window window("Model viewer", 600, 0, settings, inst);
    inst.syncGraphicSetup();

    Framebuffer fbuff(1, true, false, false, window.getSize(), inst);

    Asset texture("assets/textures/cubeTexture.png", TEXTURE_PURPOSE_IMAGE, true, inst);
    ShaderProcessor proc;
    proc.loadPackage(Path("shader/glgeTextures.gp"), "GLGETextures");
    StructuredBuffer<uint32_t> buff("TexBuffer", MEMORY_USAGE_UNIFORM, inst);
    buff.resize(sizeof(uint32_t) * 2);
    if (!buff.set(fbuff.getColorAttatchment(0)->getIdentifyer(), 0)) {std::cout << "1\n";}
    if (!buff.set(texture.getTexture()->getIdentifyer(), 1)) {std::cout << "2\n";}
    Asset shader("shader/compute.gs", &proc, {
        {"Texture", texture.getTexture()},
        {"Target", fbuff.getColorAttatchment(0)}
    }, {
        {"Data Buffer", BufferShaderBinding(0, &buff)},
        {"Textures", BufferShaderBinding(0, inst.getTextureBuffer())},
        {"Images", BufferShaderBinding(1, inst.getImageBuffer())}
    }, true, inst);

    RenderStage stages[] = {
        RenderStage(RENDER_STAGE_COMPUTE, ComputeStageData(*shader.getShader(), 
                    uvec3(ceil(window.getSize().x / 32.), ceil(window.getSize().y / 32.), 1)
                    ), 0,0,0),
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