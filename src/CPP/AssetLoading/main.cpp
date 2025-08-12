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
    RenderVertexLayout rLayout(inst);
    RenderMaterial mat("main mat", 0, false, DEPTH_TEST_ALWAYS, false, &rLayout, inst);
    ShaderProcessor proc;
    proc.loadPackage(Path("shader/glgeTextures.gp"), "GLGETextures");
    AssetShader* shader = manager.load<AssetShader>("assets/doc/examples/example.shader", &proc, std::unordered_map<std::string_view, Texture*>({
        {"mainTex", tex->getTexture()}
    }), std::unordered_map<std::string_view, BufferShaderBinding>({
        {"GLGE_TextureBuffer", BufferShaderBinding(0, inst.getTextureBuffer())},
        {"GLGE_ImageBuffer", BufferShaderBinding(1, inst.getImageBuffer())},
        {"GLGE_MaterialBuffer", BufferShaderBinding(2, &mat.getBuffer())},
        {"GLGE_Cameras", BufferShaderBinding(3, inst.getCameraBuffer())},
        {"GLGE_ObjectTransform", BufferShaderBinding(4, inst.getObjectTransformBuffer())}
    }));
    std::cout << shader->getName() << "\n";
    AssetObject* obj = manager.load<AssetObject>("assets/doc/examples/example.gobj", nullptr,0);
    std::cout << obj->getName() << ", " << obj->getObject()->getPos() << "\n";

    Framebuffer fbuff(1, true, false, false, window.getSize(), inst);

    Object camera("Camera", Transform(vec3(0,0,2)), 0,0, inst);
    camera.addAttatchment(new Camera(&fbuff));

    SimpleVertex verts[] = {
        {vec3(0,0,0), vec2(0), vec3(0)},
        {vec3(0,1,0), vec2(0), vec3(0)},
        {vec3(1,0,0), vec2(0), vec3(0)}
    };
    idx_Triangle tris[] = {{0,1,2}};
    Mesh mesh(verts, sizeof(verts)/sizeof(*verts), tris, sizeof(tris)/sizeof(*tris));
    mat.setShader(shader->getShader());
    RenderMesh rMesh(&mesh, &mat);
    obj->getObject()->addAttatchment(new ObjectRenderer({{&rMesh, &mat}}));

    Object* objs[] = {obj->getObject(), &camera};
    World world(objs, sizeof(objs)/sizeof(*objs), "World", inst);

    RenderStage stages[] = {
        RenderStage(RENDER_STAGE_RESIZE_FRAMEBUFFER, ResizeFramebufferData(&fbuff, window.getSize()), 0,0,0),
        RenderStage(RENDER_STAGE_CLEAR, ClearStageData(&fbuff), 0,0,0),
        RenderStage(RENDER_STAGE_RENDER_WORLD, RenderWorldStageData(world, camera, false), 0,0,0),
        RenderStage(RENDER_STAGE_BLIT_TO_WINDOW, BlitToWindowStageData(&fbuff, &window), 0,0,0),
        RenderStage(RENDER_STAGE_SWAP_WINDOW, SwapWindowData(&window), 0,0,0)
    };
    RenderPipeline pipe(stages, sizeof(stages)/sizeof(*stages), true, inst);

    Limiter lim;
    lim.start();

    while (window.isOpen())
    {
        pipe.getStage(0).data.resizeFramebuffer.size = window.getSize();
        inst.getLogger()->printAll();
        lim.endTick();
    }
}