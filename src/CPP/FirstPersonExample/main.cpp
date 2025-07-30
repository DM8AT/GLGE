/**
 * @file main.cpp
 * @author DM8AT
 * @brief a simple example for a first person camera
 * @version 0.1
 * @date 2025-07-25
 * 
 * @copyright Copyright (c) 2025
 * 
 */

//include file loggers - simple logging
//this also includes GLGE
#include "FileLogger.hpp"
//include camera controllers
#include "CameraController.h"

class Test final : public ObjectAttatchable
{
public:

    Test() : ObjectAttatchable() {}

    virtual ~Test() {}

    virtual void onUpdate() noexcept override
    {
        m_object->setPos(vec3(0, sin(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - start).count() * 0.01), 0));
    }

protected:

    std::chrono::system_clock::time_point start = std::chrono::system_clock::now();

};

int main()
{
    Instance inst("Instance", API_OPENGL_4_6);
    inst.setLogger(new FileLogger("FP_EXAMPLE.log", true));

    WindowSettings settings = WINDOW_SETTINGS_DEFAULT;
    Window win("Hello GLGE!", uvec2(600, 600), uvec2(0,0), settings, inst);
    inst.syncGraphicSetup();

    Framebuffer fbuff(1, false, false, true, win.getSize(), inst);
    fbuff.setClearDepth(1);

    struct MaterialData
    {
        uint32_t texIdx = 0;
        float roughness = 0.8;
        float metallic = 0;

        MaterialData() = default;
    };

    Texture tex("assets/textures/cubeTexture.png", false, TEXTURE_PURPOSE_IMAGE, inst);

    RenderVertexLayout vLayout(inst);
    RenderMaterial material(sizeof(MaterialData), true, DEPTH_TEST_LESS, true, &vLayout, inst);
    ShaderProcessor proc;
    proc.loadPackage(Path("shader/glgeTextures.gp"), "GLGETextures");
    Shader shader(Path("shader/default.gs"), &proc, {
        {"Cube", &tex}
    }, {
        {"GLGE_TextureBuffer", BufferShaderBinding(0, inst.getTextureBuffer())},
        {"GLGE_ImageBuffer", BufferShaderBinding(1, inst.getImageBuffer())},
        {"GLGE_MaterialBuffer", BufferShaderBinding(2, &material.getBuffer())},
        {"GLGE_Cameras", BufferShaderBinding(3, inst.getCameraBuffer())},
        {"GLGE_ObjectTransform", BufferShaderBinding(4, inst.getObjectTransformBuffer())}
    }, inst);
    material.setShader(&shader);

    float size = 1;
    SimpleVertex vertices[] = {
        //back
        {vec3(-size,-size,-size), vec2(0,0), vec3(0,0,-1)},
        {vec3(-size,-size, size), vec2(0,1), vec3(0,0,-1)},
        {vec3(-size, size,-size), vec2(1,0), vec3(0,0,-1)},
        {vec3(-size, size, size), vec2(1,1), vec3(0,0,-1)},
        //front
        {vec3( size,-size,-size), vec2(0,0), vec3(0,0, 1)},
        {vec3( size,-size, size), vec2(0,1), vec3(0,0, 1)},
        {vec3( size, size,-size), vec2(1,0), vec3(0,0, 1)},
        {vec3( size, size, size), vec2(1,1), vec3(0,0, 1)},
        //top
        {vec3(-size,-size,-size), vec2(0,0), vec3(0,-1,0)},
        {vec3(-size,-size, size), vec2(0,1), vec3(0,-1,0)},
        {vec3( size,-size,-size), vec2(1,0), vec3(0,-1,0)},
        {vec3( size,-size, size), vec2(1,1), vec3(0,-1,0)},
        //bottom
        {vec3(-size, size,-size), vec2(0,0), vec3(0, 1,0)},
        {vec3(-size, size, size), vec2(0,1), vec3(0, 1,0)},
        {vec3( size, size,-size), vec2(1,0), vec3(0, 1,0)},
        {vec3( size, size, size), vec2(1,1), vec3(0, 1,0)},
        //left
        {vec3(-size,-size,-size), vec2(0,0), vec3(-1,0,0)},
        {vec3(-size, size,-size), vec2(0,1), vec3(-1,0,0)},
        {vec3( size,-size,-size), vec2(1,0), vec3(-1,0,0)},
        {vec3( size, size,-size), vec2(1,1), vec3(-1,0,0)},
        //right
        {vec3(-size,-size, size), vec2(0,0), vec3( 1,0,0)},
        {vec3(-size, size, size), vec2(0,1), vec3( 1,0,0)},
        {vec3( size,-size, size), vec2(1,0), vec3( 1,0,0)},
        {vec3( size, size, size), vec2(1,1), vec3( 1,0,0)}
    };
    idx_Triangle triangles[] = {
        {0,1,2}, {2,1,3},
        {5,4,6}, {5,6,7},
        {9,8,10}, {9,10,11},
        {12,13,14}, {14,13,15},
        {16,17,18}, {18,17,19},
        {21,20,22}, {21,22,23}
    };
    Mesh mesh(vertices, sizeof(vertices)/sizeof(*vertices), triangles, sizeof(triangles) / sizeof(*triangles));
    MaterialData cubeMat = {tex.getIdentifyer(), 0.8, 1};
    RenderMesh rMesh(&mesh, &material);
    rMesh.setMaterialData(&cubeMat);

    SimpleVertex triVerts[] = {
        {vec3(-0.5,0,0), vec2(0,0),  vec3(0,0,1)},
        {vec3(0,1,0),    vec2(0.5,1),vec3(0,0,1)},
        {vec3(0.5,0,0),  vec2(1,0),  vec3(0,0,1)}
    };
    idx_Triangle triTires[] = {{1,0,2}};
    Mesh tri(triVerts, sizeof(triVerts) / sizeof(*triVerts), triTires, sizeof(triTires) / sizeof(*triTires));
    MaterialData triMat = {tex.getIdentifyer(), 0.8, 1};
    RenderMesh rTri(&tri, &material);
    rTri.setMaterialData(&triMat);

    Object cube("Cube", Transform(), 0,0, inst);
    cube.addAttatchment(new ObjectRenderer({
        RenderPair(&rMesh, &material)
    }));
    cube.addAttatchment(new Test());

    Object triangle("Triangle", Transform(vec3(0,2,0)), 0,0, inst);
    triangle.addAttatchment(new ObjectRenderer({
        RenderPair(&rTri, &material)
    }));

    Object camera("Camera", Transform(vec3(0,0,3)), 0,0, inst);
    Camera cam(&fbuff);
    camera.addAttatchment(&cam);
    camera.addAttatchment(new CameraController(win));

    Object* worldObjs[] = {&cube, &camera, &triangle};
    World world(worldObjs, sizeof(worldObjs) / sizeof(*worldObjs), "World", inst);

    RenderStage stages[] = {
        RenderStage(RENDER_STAGE_RESIZE_FRAMEBUFFER, ResizeFramebufferData(&fbuff, win.getSize()), 0,0,0),
        RenderStage(RENDER_STAGE_CLEAR, ClearStageData(&fbuff), 0,0,0),
        RenderStage(RENDER_STAGE_RENDER_WORLD, RenderWorldStageData(world, camera, false), 0,0,0),
        RenderStage(RENDER_STAGE_BLIT_TO_WINDOW, BlitToWindowStageData(&fbuff,&win), 0,0,0),
        RenderStage(RENDER_STAGE_SWAP_WINDOW, SwapWindowData(&win), 0,0,0)
    };
    RenderPipeline pipe(stages, sizeof(stages) / sizeof(*stages), false, inst);
    pipe.start();

    //set a custom FPS limit - 120 FPS seems good
    inst.getSDLLimiter().setIterationRate(120);

    Limiter lim;
    lim.start();

    while (win.isOpen())
    {
        pipe.getStage(0).data.resizeFramebuffer.size = win.getSize();
        if (inst.getLogger())
        {
            inst.getLogger()->printAll();
        }

        std::cout << "\rCurrent FPS: " << inst.getSDLLimiter().getIPS() << "                " << std::flush;

        lim.endTick();
    }

    std::cout << "\n";
}