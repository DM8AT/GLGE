#define GLGE_NO_THANKS_MSG
#include "GLGE.h"

struct Params {
    float gamma;      // e.g., 2.2
    float localScale; // e.g., 0.18 (Reinhard key)
    float epsilon;    // avoid division issues, e.g., 1e-4
    float exposure;
};

struct FirstPersonController {GLGE::u8 filler;};

void updateFirstPersonController(GLGE::Transform& transform, GLGE::Graphic::Component::Camera& camera, FirstPersonController) {
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

int main() {
    //call the static library initialization
    GLGE::Instance::init();

    //store the graphic instance descriptions
    GLGE::Graphic::Builtin::Graphics::OpenGL graphicDesc;
    GLGE::Graphic::Builtin::Video::SDL3 videoDesc;
    //store the graphic instance
    GLGE::Graphic::Instance gInst(&graphicDesc, &videoDesc);

    std::cout << "Using graphic backend " << gInst.getGraphicBackendName() << "\n";
    std::cout << "Using video backend " << gInst.getVideoBackendName() << "\n";

    GLGE::Instance inst("Hello World!", GLGE::Version(0, 1, 0), std::pair{"Graphics", &gInst});
    inst.mainLimiter().setLimit(144);

    GLGE::Graphic::Window win("Hello from SDL3", {600, 600});
    win.setVSyncMode(GLGE::Graphic::VSYNC_ENABLED);

    std::cout << "Selected GPU: "           << gInst.getGPUName()          << "\n";
    std::cout << "    GPU Vendor: "         << gInst.getGPUVendorName()    << "\n";
    std::cout << "    GPU Driver Version: " << gInst.getGPUDriverVersion() << "\n";

    GLGE::AssetHandle<GLGE::Graphic::Asset::Mesh> suzanne_mesh = inst.assets().load<GLGE::Graphic::Asset::Mesh>("assets/meshes/Suzanne.glb", GLGE::Graphic::Asset::Mesh::ASSIMP);
    GLGE::AssetHandle<GLGE::Graphic::Asset::Mesh> cube_mesh = inst.assets().load<GLGE::Graphic::Asset::Mesh>("assets/meshes/Cube.fbx", GLGE::Graphic::Asset::Mesh::ASSIMP);

    GLGE::Graphic::VertexLayout layout {{
        GLGE::Graphic::VertexAttribute(GLGE::Graphic::VertexAttribute::Type::Position, GLGE::Graphic::VertexAttribute::Format::vec3, 0,  0),
        GLGE::Graphic::VertexAttribute(GLGE::Graphic::VertexAttribute::Type::UV,       GLGE::Graphic::VertexAttribute::Format::vec2, 12, 1),
        GLGE::Graphic::VertexAttribute(GLGE::Graphic::VertexAttribute::Type::Normal,   GLGE::Graphic::VertexAttribute::Format::vec3, 20, 2),
        GLGE::Graphic::VertexAttribute(GLGE::Graphic::VertexAttribute::Type::Tangent,  GLGE::Graphic::VertexAttribute::Format::vec3, 32, 3),
        GLGE::Graphic::VertexAttribute(GLGE::Graphic::VertexAttribute::Type::Color,    GLGE::Graphic::VertexAttribute::Format::vec4, 44, 4)
    }, 60};

    GLGE::Graphic::Sampler sampler(GLGE::Graphic::SamplerCPU(
        GLGE::Graphic::SamplerCPU::ANISOTROPY_X1, GLGE::Graphic::SamplerCPU::FILTER_LINEAR, GLGE::Graphic::SamplerCPU::FILTER_LINEAR, 
        GLGE::Graphic::SamplerCPU::WRAP_CLAMP_TO_BLACK
    ));

    GLGE::Graphic::Image colBuff(win.getResolution(), GLGE::Graphic::PIXEL_FORMAT_RGBA_16_FLOAT);
    GLGE::Graphic::Image depthBuff(win.getResolution(), GLGE::Graphic::PIXEL_FORMAT_DEPTH_32_FLOAT);
    GLGE::Graphic::Framebuffer fbuff({&colBuff}, {&depthBuff});

    GLGE::Graphic::Image ldrOut(win.getResolution(), GLGE::Graphic::PIXEL_FORMAT_RGBA_8_UNORM);
    GLGE::Graphic::Framebuffer ldrFbuff({&ldrOut});

    float exposure = 1.15f;
    GLGE::Graphic::StructuredBuffer<Params> buff({Params(2.2, 0.18, 1e-4, exposure)}, GLGE::Graphic::Buffer::Type::UNIFORM, GLGE::Graphic::Buffer::Usage::STREAMING_UPLOAD);
    GLGE::Graphic::SampledTexture sampled(colBuff, sampler);

    GLGE::Graphic::Shader finalize({std::pair{"Compute", "assets/shader/finalize.comp.spv"}});
    GLGE::Graphic::ResourceSet ldrSet(finalize.getSet(0), std::pair{"imgInput", &sampled}, std::pair{"imgOutput", &ldrOut}, std::pair{"params", &buff});
    finalize.setResources(0, &ldrSet);

    GLGE::Graphic::RenderTarget window(&win);
    GLGE::Graphic::RenderTarget LDR_Target(&ldrFbuff);
    GLGE::Graphic::RenderTarget HDR_Target(&fbuff);

    GLGE::World world("Scene 1");
    GLGE::Object camera = world.create<GLGE::Graphic::Component::Camera, GLGE::Transform, FirstPersonController>("Camera", 
                                                                                            GLGE::Graphic::Component::Camera{90,0.1,1000, GLGE::vec3(0,0,0)}, 
                                                                                            GLGE::Transform{GLGE::vec3(-2,-2,4), GLGE::Quaternion(GLGE::vec3(0,0,0)), GLGE::vec3(1,1,1)},
                                                                                            FirstPersonController{0}
                                                                                        );
    GLGE::Graphic::Renderer renderer(world, &camera, HDR_Target);

    GLGE::Graphic::Shader rt_comp({std::pair{"Compute", "assets/shader/rt_sphere.comp.spv"}});
    GLGE::Graphic::SampledTexture sampledDepth(depthBuff, sampler);
    GLGE::Graphic::ResourceSet set(rt_comp.getSet(0), std::pair{"imgOutput", &colBuff}, std::pair{"depthIn", &sampledDepth}, std::pair{"cam", renderer.getCameraBuffer()});
    rt_comp.setResources(0, &set);

    GLGE::Graphic::Shader meshShader {
        std::pair{"Vertex", "assets/shader/simple.vert.spv"},
        std::pair{"Fragment", "assets/shader/simple.frag.spv"}
    };
    GLGE::Graphic::ResourceSet renderSet(meshShader.getSet(0), std::pair{"cam", renderer.getCameraBuffer()}, std::pair{"transforms", renderer.getTransformBuffer()});
    meshShader.setResources(0, &renderSet);

    GLGE::Graphic::Shader fancyMeshShader {
        std::pair{"Vertex", "assets/shader/simple.vert.spv"},
        std::pair{"Fragment", "assets/shader/fancy.frag.spv"},
    };
    fancyMeshShader.setResources(0, &renderSet);

    GLGE::Graphic::Material mat(meshShader, layout, GLGE::Graphic::Material::CullMode::BACK, GLGE::Graphic::Material::DepthMode::DEPTH_COMPARE_LESS, true);
    GLGE::Graphic::Material fancyMat(fancyMeshShader, layout, GLGE::Graphic::Material::CullMode::BACK, GLGE::Graphic::Material::DepthMode::DEPTH_COMPARE_LESS, true);

    GLGE::Object suzanne = world.create<GLGE::Graphic::Component::Renderable, GLGE::Transform>("Suzanne", 
                                                                                                GLGE::Graphic::Component::Renderable{&suzanne_mesh.reference()->mesh(), &mat, true}, 
                                                                                                GLGE::Transform{{0,0,0}, {{glm::radians(45.f),glm::radians(45.f),0}}, {1,1,1}}
                                                                                            );
    GLGE::Object suzanne2 = world.create<GLGE::Graphic::Component::Renderable, GLGE::Transform>("Suzanne 2", 
                                                                                                GLGE::Graphic::Component::Renderable{&suzanne_mesh.reference()->mesh(), &fancyMat, true}, 
                                                                                                GLGE::Transform{{0,0,0}, {{0,0,0}}, {1,1,1}}
                                                                                            );
    GLGE::Object cube = world.create<GLGE::Graphic::Component::Renderable, GLGE::Transform>("Cube", 
                                                                                                GLGE::Graphic::Component::Renderable{&cube_mesh.reference()->mesh(), &mat, true}, 
                                                                                                GLGE::Transform{{-5,-1,-2}, {{0,0,0}}, {1,1,1}}
                                                                                            );

    auto pipe = GLGE::Graphic::RenderPipeline::create(
        std::pair{"Clear", GLGE::Graphic::Command(GLGE::Graphic::COMMAND_CLEAR, HDR_Target, GLGE::u8(0), GLGE::vec4(0.18,0.18,0.18,1), GLGE::f32(1), GLGE::u32(0))},
        std::pair{"Draw", GLGE::Graphic::Command(GLGE::Graphic::COMMAND_DRAW_WORLD, &renderer)},
        std::pair{"Compute", GLGE::Graphic::Command(GLGE::Graphic::COMMAND_DISPATCH_COMPUTE, &rt_comp, GLGE::uvec3(glm::ceil(colBuff.getSize().x/16.f), glm::ceil(colBuff.getSize().y/16.f), 1))},
        std::pair{"Finalize", GLGE::Graphic::Command(GLGE::Graphic::COMMAND_DISPATCH_COMPUTE, &finalize, GLGE::uvec3(glm::ceil(colBuff.getSize().x/16.f), glm::ceil(colBuff.getSize().y/16.f), 1))},
        std::pair{"Copy", GLGE::Graphic::Command(GLGE::Graphic::COMMAND_COPY, LDR_Target, GLGE::u8(0), window, GLGE::u8(0))},
        std::pair{"Swap", GLGE::Graphic::Command(GLGE::Graphic::COMMAND_SWAP_WINDOW, &win)}
    );
    pipe.record();

    inst.start();

    inst.registerKeyboard(GLGE::Keyboard("Hi"));

    while (!win.isClosingRequested()) {
        //start the tick
        inst.startMainTick();

        //handle resizing
        if (win.getResolution().x != colBuff.getSize().x || win.getResolution().y != colBuff.getSize().y) {
            fbuff.resize(win.getResolution());
            ldrFbuff.resize(win.getResolution());
            *pipe.getCommand("Compute")->access<GLGE::uvec3>(sizeof(void*)) = GLGE::uvec3(glm::ceil(colBuff.getSize().x/16.f), glm::ceil(colBuff.getSize().y/16.f), 1);
            *pipe.getCommand("Finalize")->access<GLGE::uvec3>(sizeof(void*)) = GLGE::uvec3(glm::ceil(colBuff.getSize().x/16.f), glm::ceil(colBuff.getSize().y/16.f), 1);

            //re-record the pipeline to make it aware of the changes
            pipe.record();
        }

        //play back the pipeline (to render a frame)
        pipe.play();
        //apply some crude animation
        world.each<GLGE::Transform, GLGE::Graphic::Component::Camera, FirstPersonController>(updateFirstPersonController);
        world.get<GLGE::Transform>(suzanne)->pos.y = glm::sin(std::chrono::system_clock::now().time_since_epoch().count() * 1E-9);
        world.get<GLGE::Transform>(suzanne2)->pos.y = 1.f - glm::sin(std::chrono::system_clock::now().time_since_epoch().count() * 1E-9);
        //update the renderer (update transformation state)
        renderer.update();

        //end the tick
        inst.endMainTick();
    }

    inst.shutdown();
}