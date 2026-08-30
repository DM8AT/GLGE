/**
 * @file DefaultExample.cpp
 * @author DM8AT
 * @brief the default example shipped as an general example of GLGE
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
 * @brief a function that invokes the default example
 * 
 * This is just an all-around example that does not test a specific system in-depth.
 * 
 * @param gDescr the graphic description to use
 * @param vDescr the video description to use
 * @return `unsigned char` the return value of the call
 */
unsigned char defaultExample(const char* graphicBackendName, const char* videoBackendName);

/**
 * @brief define the function that is used to register the example
 * 
 * @param ptr a pointer to the example registry
 */
extern "C" GLGE_EXAMPLE_PLUGIN_API void EXAMPLE_SYS_REGISTER_EXAMPLE_PLUGIN(ExampleRegistryPtr ptr) {
    //get the example registry
    auto* reg = reinterpret_cast<ExampleRegistry*>(ptr);

    //add the example
    reg->addExample("Default Example - This is just an all-around example that does not test a specific system in-depth.",
                    &defaultExample);
}

struct Params {
    float gamma;      // e.g., 2.2
    float localScale; // e.g., 0.18 (Reinhard key)
    float epsilon;    // avoid division issues, e.g., 1e-4
    float exposure;
};

struct FirstPersonController {GLGE::u8 filler;};

static void updateFirstPersonController(GLGE::Transform& transform, GLGE::Graphic::Component::Camera& camera, FirstPersonController) {
    GLGE::Instance* inst = GLGE::Instance::getCurrentInstance();

    float speed = 0.0125 * inst->mainLimiter().getCurrentDeltaTime();
    //compute forward, right and up vector
    GLGE::Quaternion quat(camera.eulerAngles);
    GLGE::vec3 forward = quat * GLGE::vec3(0,0,-speed);
    GLGE::vec3 right = quat * GLGE::vec3(speed,0,0);
    GLGE::vec3 up = quat * GLGE::vec3(0,speed,0);

    if (inst->getKeyboard().pressed()['w'])
    {transform.pos += forward;}
    if (inst->getKeyboard().pressed()['s'])
    {transform.pos -= forward;}
    if (inst->getKeyboard().pressed()['a'])
    {transform.pos -= right;}
    if (inst->getKeyboard().pressed()['d'])
    {transform.pos += right;}
    if (inst->getKeyboard().pressed()[GLGE::Key::Space])
    {transform.pos += up;}
    if (inst->getKeyboard().pressed()[GLGE::Key::LeftShift])
    {transform.pos -= up;}

    float rotSpeed = glm::radians(0.125) * inst->mainLimiter().getCurrentDeltaTime();
    if (inst->getKeyboard().pressed()[GLGE::Key::ArrowLeft])
    {camera.eulerAngles.y += rotSpeed;}
    if (inst->getKeyboard().pressed()[GLGE::Key::ArrowRight])
    {camera.eulerAngles.y -= rotSpeed;}
    if (inst->getKeyboard().pressed()[GLGE::Key::ArrowUp])
    {camera.eulerAngles.x += rotSpeed;}
    if (inst->getKeyboard().pressed()[GLGE::Key::ArrowDown])
    {camera.eulerAngles.x -= rotSpeed;}
    if (inst->getKeyboard().pressed()[GLGE::Key::PageUp])
    {camera.eulerAngles.z += rotSpeed;}
    if (inst->getKeyboard().pressed()[GLGE::Key::PageDown])
    {camera.eulerAngles.z -= rotSpeed;}
}

unsigned char defaultExample(const char* graphicBackendName, const char* videoBackendName) {
    //the descriptions need to be created here using the provided names
    auto gDescr = createGraphicBackendDescription(graphicBackendName);
    auto vDescr = createVideoBackendDescription(videoBackendName);

    //call the static library initialization
    GLGE::Instance::init();

    //store the graphic instance
    GLGE::Graphic::Instance gInst(gDescr.get(), vDescr.get());

    std::cout << "Using graphic backend " << gInst.getGraphicBackendName() << "\n";
    std::cout << "Using video backend " << gInst.getVideoBackendName() << "\n";

    GLGE::Instance inst("OpenGL Instance", GLGE::Version(0, 1, 0), std::pair{"Graphics", &gInst});
    inst.mainLimiter().setLimit(144);

    GLGE::Graphic::Window win("Hello from SDL3", {600, 600});
    win.setVSyncMode(GLGE::Graphic::VSYNC_ENABLED);

    std::cout << "Selected GPU: "           << gInst.getGPUName()          << "\n";
    std::cout << "    GPU Vendor: "         << gInst.getGPUVendorName()    << "\n";
    std::cout << "    GPU Driver Version: " << gInst.getGPUDriverVersion() << "\n";

    auto suzanne_mesh = inst.assets().load<GLGE::MeshAsset>("assets/meshes/Suzanne.glb", GLGE::MeshAsset::ASSIMP);
    auto cube_mesh = inst.assets().load<GLGE::MeshAsset>("assets/meshes/Cube.fbx", GLGE::MeshAsset::ASSIMP);
    auto plane_mesh = inst.assets().load<GLGE::MeshAsset>("assets/meshes/Plane.fbx", GLGE::MeshAsset::ASSIMP);

    GLGE::Graphic::VertexLayout layout {suzanne_mesh.reference()->getMesh()->getLayout(), 
        std::pair{GLGE::VertexAttribute::Position {}, GLGE::u64(0)},    //Pos    -> stream 0
        std::pair{GLGE::VertexAttribute::UV {},       GLGE::u64(1)},    //UV     -> stream 1
        std::pair{GLGE::VertexAttribute::Normal {},   GLGE::u64(1)}     //Normal -> stream 1
    };

    auto gpuSuzanneMesh = GLGE::Graphic::Mesh(*suzanne_mesh.reference()->getMesh(), layout);
    auto gpuCubeMesh    = GLGE::Graphic::Mesh(*cube_mesh.reference()->getMesh(), layout);
    auto gpuPlaneMesh   = GLGE::Graphic::Mesh(*plane_mesh.reference()->getMesh(), layout);

    GLGE::Graphic::Sampler sampler(GLGE::Graphic::SamplerCPU(
        GLGE::Graphic::SamplerCPU::ANISOTROPY_X1, GLGE::Graphic::SamplerCPU::FILTER_LINEAR, GLGE::Graphic::SamplerCPU::FILTER_LINEAR, 
        GLGE::Graphic::SamplerCPU::WRAP_CLAMP_TO_BLACK
    ));

    GLGE::Graphic::Image colBuff(win.getResolution(), GLGE::Graphic::PIXEL_FORMAT_RGBA_16_FLOAT);
    GLGE::Graphic::Image depthBuff(win.getResolution(), GLGE::Graphic::PIXEL_FORMAT_R_32_FLOAT); //NOTE: Not an actual depth buffer
    GLGE::Graphic::Framebuffer fbuff({&colBuff, &depthBuff});

    GLGE::Graphic::Image multiSample_colBuff(win.getResolution(), GLGE::Graphic::PIXEL_FORMAT_RGBA_16_FLOAT, 16);
    GLGE::Graphic::Image multiSample_depthBuff(win.getResolution(), GLGE::Graphic::PIXEL_FORMAT_DEPTH_32_FLOAT, 16);
    GLGE::Graphic::Framebuffer multiSample_fbuff({&multiSample_colBuff}, {&multiSample_depthBuff});

    GLGE::Graphic::Image ldrOut(win.getResolution(), GLGE::Graphic::PIXEL_FORMAT_RGBA_8_UNORM);
    GLGE::Graphic::Framebuffer ldrFbuff({&ldrOut});

    float exposure = 1.15f;
    GLGE::Graphic::StructuredBuffer<Params> buff({Params(2.2, 0.18, 1e-4, exposure)}, GLGE::Graphic::Buffer::Type::UNIFORM, GLGE::Graphic::Buffer::Usage::STREAMING_UPLOAD);
    GLGE::Graphic::SampledTexture sampled(colBuff, sampler);

    GLGE::Graphic::Shader finalize({std::pair{"Compute", "assets/shader/finalize.comp.spv"}});
    GLGE::Graphic::ResourceSet ldrSet(finalize.getSet(0), std::pair{"imgInput", &sampled}, std::pair{"imgOutput", &ldrOut}, std::pair{"params", &buff});
    finalize.setResources(0, &ldrSet);

    GLGE::World world("Scene 1");
    GLGE::Object camera = world.create<GLGE::Graphic::Component::Camera, GLGE::Transform, FirstPersonController>(
        "Camera", 
        GLGE::Graphic::Component::Camera{90,0.1,1000, GLGE::vec3(0,0,0)}, 
        GLGE::Transform{GLGE::vec3(-2,0,4), GLGE::Quaternion(GLGE::vec3(0,0,0)), GLGE::vec3(1,1,1)},
        FirstPersonController{0}
    );
    GLGE::Graphic::Renderer renderer(world, &camera, {&multiSample_fbuff});

    GLGE::Graphic::Shader cull({std::pair{"Compute", "assets/shader/culling.comp.spv"}});
    GLGE::Graphic::ResourceSet cullSet(cull.getSet(0), std::pair{"cam", renderer.getCameraBuffer()}, std::pair{"transforms", renderer.getTransformBuffer()},
                                       std::pair{"commandBuffer", renderer.getIndirectDrawBuffer()});
    cull.setResources(0, &cullSet);

    GLGE::Graphic::Shader rt_comp({std::pair{"Compute", "assets/shader/rt_sphere.comp.spv"}});
    GLGE::Graphic::SampledTexture sampledDepth(depthBuff, sampler);
    GLGE::Graphic::SampledTexture sampledImg(colBuff, sampler);
    GLGE::Graphic::ResourceSet set(rt_comp.getSet(0), std::pair{"imgOutput", &colBuff}, std::pair{"depthIn", &sampledDepth}, std::pair{"imgIn", &sampledImg}, std::pair{"cam", renderer.getCameraBuffer()}, 
        std::pair{"pointLights", renderer.getPointLightBuffer()}, std::pair{"spotLights", renderer.getSpotLightBuffer()}, std::pair{"directionalLights", renderer.getDirectionalLightBuffer()}
    );
    rt_comp.setResources(0, &set);

    GLGE::Graphic::Shader meshShader {
        std::pair{"Vertex", "assets/shader/simple.vert.spv"},
        std::pair{"Fragment", "assets/shader/simple.frag.spv"}
    };
    GLGE::Graphic::ResourceSet renderSet(meshShader.getSet(0), std::pair{"cam", renderer.getCameraBuffer()}, std::pair{"transforms", renderer.getTransformBuffer()}, 
        std::pair{"pointLights", renderer.getPointLightBuffer()}, std::pair{"spotLights", renderer.getSpotLightBuffer()}, std::pair{"directionalLights", renderer.getDirectionalLightBuffer()}
    );
    meshShader.setResources(0, &renderSet);

    GLGE::Graphic::Shader resolveDepth {
        std::pair{"Compute", "assets/shader/resolveDepth.comp.spv"}
    };
    GLGE::Graphic::SampledTexture sampledMultiDepth(multiSample_depthBuff, sampler);
    GLGE::Graphic::ResourceSet resolveDepthSet(resolveDepth.getSet(0), std::pair{"sourceDepth", &sampledMultiDepth}, std::pair{"destinationDepth", &depthBuff});
    resolveDepth.setResources(0, &resolveDepthSet);

    GLGE::Graphic::Material mat(meshShader, layout, multiSample_fbuff, GLGE::Graphic::Material::CullMode::BACK, GLGE::Graphic::Material::DepthMode::DEPTH_COMPARE_LESS, true);

    GLGE::Object suzanne = world.create<GLGE::Graphic::Component::Renderable, GLGE::Transform>(
        "Suzanne", 
        GLGE::Graphic::Component::Renderable{&gpuSuzanneMesh, &mat, true}, 
        GLGE::Transform{{0,0,0}, {{glm::radians(45.f),glm::radians(45.f),0}}, {1,1,1}}
    );
    GLGE::Object suzanne2 = world.create<GLGE::Graphic::Component::Renderable, GLGE::Transform>(
        "Suzanne 2", 
        GLGE::Graphic::Component::Renderable{&gpuSuzanneMesh, &mat, true}, 
        GLGE::Transform{{0,0,0}, {{0,0,0}}, {1,1,1}}
    );
    GLGE::Object cube = world.create<GLGE::Graphic::Component::Renderable, GLGE::Transform>(
        "Cube", 
        GLGE::Graphic::Component::Renderable{&gpuCubeMesh, &mat, true}, 
        GLGE::Transform{{-5,-1,-2}, {{0,0,0}}, {1,1,1}}
    );
    world.setParent(cube, suzanne);
    GLGE::Object plane = world.create<GLGE::Graphic::Component::Renderable, GLGE::Transform>(
        "Plane", 
        GLGE::Graphic::Component::Renderable{&gpuPlaneMesh, &mat, true}, 
        GLGE::Transform{{0,-2,0}, {{glm::radians<float>(-90),0,0}}, {100,100,1}}
    );
    GLGE::Object light = world.create<GLGE::Graphic::Component::PointLight, GLGE::Transform>(
        "Light",
        GLGE::Graphic::Component::PointLight{
            .radius = 1.f,
            .color = GLGE::vec3(0.2,0.4,0.9),
            .intensity = 10,
            .fallof_linear = 0.f,
            .fallof_quadratic = 1.f,
            .flags = 0
        },
        GLGE::Transform{
            {-0.75,0,1},
            GLGE::vec3{0,0,0},
            {1,1,1}
        }
    );
    GLGE::Object spot = world.create<GLGE::Graphic::Component::SpotLight, GLGE::Transform>(
        "Spotlight", 
        GLGE::Graphic::Component::SpotLight{
            .color = GLGE::vec3(0,1,0),
            .intensity = 160,
            .fallof_linear = 0.f,
            .fallof_quadratic = 1.f,
            .cone_inner = glm::radians<float>(33.33),
            .cone_outer = glm::radians<float>(45.f),
            .flags = 0
        },
        GLGE::Transform{
            {0,0,-3},
            GLGE::vec3{glm::radians(160.f),glm::radians(-20.f),0},
            {1,1,1}
        }
    );
    GLGE::Object skylight = world.create<GLGE::Graphic::Component::DirectionalLight, GLGE::Transform>(
        "Skylight",
        GLGE::Graphic::Component::DirectionalLight {
            .color = GLGE::vec3(1),
            .intensity = 1.2,
            .flags = 0
        }, 
        GLGE::Transform {
            {0,0,0},
            GLGE::vec3{glm::radians(-12.5f), 0, glm::radians(20.f)},
            {1,1,1}
        }
    );

    //record the commands
    GLGE::uvec3 extent = GLGE::uvec3(glm::ceil(colBuff.getSize().x/16.f), glm::ceil(colBuff.getSize().y/16.f), 1);
    GLGE::Graphic::CommandStream stream(
        std::pair{"Clear", std::make_unique<GLGE::Graphic::Cmd::Clear>(multiSample_fbuff, 0, GLGE::vec4{0.5, 0.5, 0.5, 1})},
        std::pair{"Cull",  std::make_unique<GLGE::Graphic::Cmd::DispatchCompute>(cull, GLGE::uvec3(1))},
        //TODO: Actual rendering
        std::pair{"Flatten multi-sample", std::make_unique<GLGE::Graphic::Cmd::Copy>(multiSample_fbuff, 0, fbuff, 0)},
        std::pair{"Resolve msaa depth", std::make_unique<GLGE::Graphic::Cmd::DispatchCompute>(resolveDepth, extent)},
        std::pair{"Ray trace sphere", std::make_unique<GLGE::Graphic::Cmd::DispatchCompute>(rt_comp, extent)},
        std::pair{"Finalize", std::make_unique<GLGE::Graphic::Cmd::DispatchCompute>(finalize, extent)},
        std::pair{"Present", std::make_unique<GLGE::Graphic::Cmd::Copy>(ldrFbuff, win, 0)}
    );
    //define a structure to execute commands that operate on the main window
    GLGE::Graphic::CommandExecutor exec(&win);

    inst.start();

    while (!win.isClosingRequested()) {
        //start the tick
        inst.startMainTick();

        //handle resizing
        if (win.didResize()) {
            multiSample_fbuff.resize(win.getResolution());
            fbuff.resize(win.getResolution());
            ldrFbuff.resize(win.getResolution());
            extent = GLGE::uvec3(glm::ceil(colBuff.getSize().x/16.f), glm::ceil(colBuff.getSize().y/16.f), 1);
            stream.accessCmd<GLGE::Graphic::Cmd::DispatchCompute>("Resolve msaa depth")->setExtent(extent);
            stream.accessCmd<GLGE::Graphic::Cmd::DispatchCompute>("Ray trace sphere")->setExtent(extent);
            stream.accessCmd<GLGE::Graphic::Cmd::DispatchCompute>("Finalize")->setExtent(extent);
        }

        //play back the pipeline (to render a frame)
        exec.dispatch(stream);
        //apply some crude animation
        world.each<GLGE::Transform, GLGE::Graphic::Component::Camera, FirstPersonController>(updateFirstPersonController);
        world.get<GLGE::Transform>(suzanne)->pos.y = glm::sin(std::chrono::system_clock::now().time_since_epoch().count() * 1E-9);
        world.get<GLGE::Transform>(suzanne2)->pos.y = 1.f - glm::sin(std::chrono::system_clock::now().time_since_epoch().count() * 1E-9);
        world.get<GLGE::Graphic::Component::PointLight>(light)->intensity = (glm::sin(std::chrono::system_clock::now().time_since_epoch().count() * 1E-9 / 2.333) * 0.5 + 0.5) * 50;
        //bake the transforms
        GLGE::System::BakeTransforms(world);
        //update the renderer (update transformation state)
        renderer.update();

        //end the tick
        inst.endMainTick();
    }

    inst.shutdown();

    //success
    return 0;
}